#include "GraphColor.h"
#include <cmath>
#include <vector>
#include "ControlFlowAnalysis.h"
#include "LiveVariableAnalysis.h"
#include "ReachDefAnalysis.h"

using namespace std;

GraphColor::GraphColor(MachineUnit *unit)
{
    this->unit = unit;
    regNum = 11;
    fRegNum = 32;
    defWeight = 2;
    useWeight = 4;
    copyWeight = 1;
}

void GraphColor::allocateRegisters()
{
    for (auto &func : unit->getFuncs()){
        this->func = func;
        bool success = false;
        while (!success)
        {
            bool coalesce = true;
            while (coalesce)
            {
                constructWebs();
                constructMatrix();
                coalesce = coalesceRegs();
            }
            constructList();
            computeSpillCost();
            simplifyGraph();
            success = assignRegs();
            if (success)
                modifyCode();
            else
                genSpillCode();
        }
    }
}

void GraphColor::makeDuChains()
{
    ReachDefAnalysis rd;
    rd.pass(func);
    std::map<MachineOperand, std::set<MachineOperand *> > reachingDef;
    du_chains.clear();
    for (auto &block : func->getBlocks())
    {
        for (auto &inst : block->getInsts())
        {
            auto defs = inst->getDef();
            for (auto &def : defs)
                if (def->needColor())
                    du_chains[def].insert({});
        }
    }
    for (auto &block : func->getBlocks())
    {
        reachingDef.clear();
        for (auto &t : block->getDefIn())
            reachingDef[*t].insert(t);
        for (auto &inst : block->getInsts())
        {
            for (auto &use : inst->getUse())
            {
                if (use->needColor())
                {
                    if (reachingDef[*use].empty())
                        du_chains[use].insert(use);
                    for (auto &def : reachingDef[*use])
                        du_chains[def].insert(use);
                }
            }
            auto defs = inst->getDef();
            for (auto &def : defs)
            {
                auto &t = reachingDef[*def];
                auto &s = rd.getAllDefs()[*def];
                std::set<MachineOperand *> res;
                set_difference(t.begin(), t.end(), s.begin(), s.end(),
                               inserter(res, res.end()));
                reachingDef[*def] = res;
                reachingDef[*def].insert(def);
            }
        }
    }
}

void GraphColor::constructWebs()
{
    makeDuChains();
    webs.clear();
    operand2web.clear();
    for (auto &du_chain : du_chains)
    {
        Web *web = new Web({{du_chain.first},du_chain.second,false,du_chain.first->isReg() ? __DBL_MAX__ / 2 : 0,
                            0,-1,-1,du_chain.first->isFloat()});
        webs.push_back(web);
    }
    bool continueLoop = true;
    while (continueLoop)
    {
        continueLoop = false;
        vector<Web *> tempWebs(webs.begin(), webs.end());
        for (size_t i = 0; i < tempWebs.size(); i++)
            for (size_t j = i + 1; j < tempWebs.size(); j++)
            {
                Web *web1 = tempWebs[i];
                Web *web2 = tempWebs[j];
                if (**web1->defs.begin() == **web2->defs.begin())
                {
                    std::set<MachineOperand *> temp;
                    set_intersection(web1->uses.begin(), web1->uses.end(),web2->uses.begin(), web2->uses.end(),inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        continueLoop = true;
                        web1->defs.insert(web2->defs.begin(), web2->defs.end());
                        web2->uses.insert(web2->uses.begin(), web2->uses.end());
                        auto it = std::find(webs.begin(), webs.end(), web2);
                        if (it != webs.end())
                            webs.erase(it);
                    }
                }
            }
    }
    vector<Web *> symreg;
    int regIndex = 0;
    for (; regIndex < regNum; regIndex++)
    {
        Web *web = new Web({set<MachineOperand *>(), set<MachineOperand *>(), false, __DBL_MAX__ / 2, -1, -1, regIndex, false});
        symreg.push_back(web);
    }
    for (; regIndex < regNum + fRegNum; regIndex++)
    {
        Web *web = new Web({set<MachineOperand *>(), set<MachineOperand *>(), false, __DBL_MAX__ / 2, -1, -1, regIndex, true});
        symreg.push_back(web);
    }
    for (auto &web : webs)
    {
        web->sreg = regIndex;
        for (auto &def : web->defs)
            operand2web[def] = regIndex;
        for (auto &use : web->uses)
            operand2web[use] = regIndex;
        regIndex++;
    }
    webs.insert(webs.begin(), symreg.begin(), symreg.end());
}

void GraphColor::constructMatrix()
{
    LiveVariableAnalysis lva;
    int totalRegNum = regNum + fRegNum;
    lva.pass(func);
    matrix.resize(webs.size());
    for (auto &secondMatrix : matrix)
    {
        secondMatrix.resize(webs.size());
        for (auto element : secondMatrix)
            element = false;
    }
    for (int i = 0; i < totalRegNum; i++){
        for (int j = 0; j < totalRegNum; j++)
        {
            if(i == j){
                continue ;
            }
            matrix[i][j] = true;
        }
    }
    for (auto i = totalRegNum; i < (int)webs.size(); i++)
    {
        auto def = *(webs[i]->defs.begin());
        if (!def->isReg()){
            continue ;
        }
        webs[i]->rreg = def->getReg();
        if (def->isFloat()){
            webs[i]->rreg -= 5;
        }
        auto rreg = webs[i]->rreg;
        int k = operand2web[def];
        for (int i = 0; i < totalRegNum; i++)
        {
            if (i == rreg)
                continue;
            matrix[k][i] = true;
            matrix[i][k] = true;
        }
    }
    for (auto &block : func->getBlocks())
    {
        auto liveOutSet = block->getLiveOut();
        auto insts = block->getInsts();
        for (auto inst = insts.rbegin(); inst != insts.rend(); inst++){
            auto defs = (*inst)->getDef();
            for (auto &def : defs){
                if (operand2web.find(def) != operand2web.end()){
                    int k = operand2web[def];
                    for (auto &liveOut : liveOutSet){
                        if (operand2web.find(liveOut) == operand2web.end())
                            continue ;
                        int j = operand2web[liveOut];
                        matrix[k][j] = true;
                        matrix[j][k] = true;
                    }
                }
                auto &uses = lva.getAllUses()[*def];
                for (auto &use : uses)
                    if (liveOutSet.find(use) != liveOutSet.end()){
                        liveOutSet.erase(use);
                    }
            }
            for (auto &use : (*inst)->getUse()){
                liveOutSet.insert(use);
            }
        }
    }
    for (auto it = webs.begin() + regNum + fRegNum; it != webs.end(); it++)
    {
        auto web = *it;
        auto def = *(web->defs.begin());
        auto j = operand2web[def];
        if (web->fpu)
        {
            for (int i = regNum + 12; i <= regNum + 15; i++)
                matrix[i][j] = matrix[j][i] = true;
        }
    }
}

void GraphColor::constructList()
{
    int maxtrixSize = int(matrix.size());
    list.resize(matrix.size(), vector<int>());
    for (auto i = 0; i < maxtrixSize; i++){
        for (auto j = 0; j < maxtrixSize; j++){
            if(i == j){
                continue ;
            }
            if (matrix[i][j]){
                list[i].push_back(j);
            }
        }
    }
    removeList = list;
}

bool GraphColor::coalesceRegs()
{
    bool flag = false;
    vector<MachineInstruction *> tempInstVec;
    for (auto &block : func->getBlocks()){
        for (auto &ins : block->getInsts()){
            bool flag1 = false;
            if (ins->isMov()){
                flag1 = true;
            }else if (ins->isVMov()){
                auto def = ins->getDef()[0];
                auto use = ins->getUse()[0];
                if (def->isFloat() && use->isFloat()){
                    flag1 = true;
                }
            }else if (ins->isAdd() || ins->isVAdd()){
                auto def = ins->getDef()[0];
                if (def->isReg() && def->getReg() == 13){
                    continue;
                }
                auto uses = ins->getUse();
                for (auto use : uses){
                    if (use->isImm() && use->getVal() == 0){
                        flag1 = true;
                    }
                }
            }
            if (!flag1){
                continue;
            }
            auto def = *(ins->getDef().begin());
            auto uses = ins->getUse();
            if (uses.empty() || uses[0]->isImm()){
                continue;
            }
            auto use = uses[0];
            if (operand2web.find(def) == operand2web.end()){
                continue ;
            }
            if (def->isReg()){
                continue ;
            }
            if (operand2web.find(use) == operand2web.end()){
                continue ;
            }
            if (use->isReg()){
                continue ;
            }
            int defOperand = operand2web[def];
            int useOperand = operand2web[use];
            if (webs[defOperand]->defs.size() != 1){
                continue;
            }
            if (!matrix[defOperand][useOperand]){
                flag = true;
                for (auto i = 0; i < (int)matrix.size(); i++)
                    matrix[i][defOperand] = matrix[i][useOperand] = matrix[defOperand][i] = matrix[useOperand][i] = matrix[defOperand][i] || matrix[useOperand][i];
                for (auto &use : webs[defOperand]->uses){
                    auto in = use->getParent();
                    MachineOperand *newOperand = new MachineOperand(*use);
                    operand2web[newOperand] = defOperand;
                    webs[useOperand]->uses.insert(newOperand);
                    in->replaceUse(use, newOperand);
                }
                for (auto &def : webs[defOperand]->defs){
                    auto in = def->getParent();
                    MachineOperand *newOperand = new MachineOperand(*use);
                    operand2web[newOperand] = useOperand;
                    webs[useOperand]->defs.insert(newOperand);
                    in->replaceDef(def, newOperand);
                }
                tempInstVec.push_back(ins);
            }
        }
    }
    for (auto &ins : tempInstVec)
        ins->getParent()->remove(ins);
    return flag;
}

void GraphColor::computeSpillCost()
{
    ControlFlowAnalysis cfa;
    cfa.pass(func);
    for (auto &block : func->getBlocks()){
        auto factor = pow(10, cfa.getLoopDepth(block));
        for (auto &ins : block->getInsts()){
            auto defs = ins->getDef();
            auto uses = ins->getUse();
            bool reduceInstFlag = false;
            if (ins->isMov()){
                reduceInstFlag = true;
            }
            else if (ins->isVMov()){
                auto def = ins->getDef()[0];
                auto use = ins->getUse()[0];
                if (def->isFloat() && use->isFloat())
                    reduceInstFlag = true;
            }
            else if (ins->isAdd() || ins->isVAdd()){
                auto def = ins->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
                auto uses = ins->getUse();
                for (auto &use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        reduceInstFlag = true;
            }
            for (auto &def : defs)
            {
                if (def->isReg()){
                    continue ;
                }
                int defOperand = operand2web[def];
                webs[defOperand]->spillCost += factor * defWeight;
                if (reduceInstFlag)
                    webs[defOperand]->spillCost -= factor * copyWeight;
            }
            for (auto &use : uses)
            {
                if (use->isReg())
                    continue;
                int useOperand = operand2web[use];
                webs[useOperand]->spillCost += factor * useWeight;
                if (reduceInstFlag)
                    webs[useOperand]->spillCost -= factor * copyWeight;
            }
        }
    }
}

void GraphColor::simplifyGraph()
{
    while (!stk.empty()){
        stk.pop();
    }
    for (int i = 0; i < (int)list.size(); i++){
        if (list[i].size() == 0){
            stk.push(i);
        }
    }
    bool allocRegFlag = true;
    while (allocRegFlag){
        allocRegFlag = false;
        for (int i = 0; i < (int)list.size(); i++)
            if (list[i].size() > 0 && webs[i]->rreg == -1){
                bool enoughRegFlag = false;
                if (webs[i]->fpu){
                    enoughRegFlag = (int)list[i].size() < fRegNum;
                }
                else{
                    enoughRegFlag = (int)list[i].size() < regNum;
                }
                if (enoughRegFlag)
                {
                    allocRegFlag = true;
                    stk.push(i);
                    delOneNode(i);
                }
            }
    }
    while (stk.size() < list.size())
    {
        double curSpillCost = __DBL_MAX__;
        int curSpillNode = -1;
        for (int i = 0; i < (int)list.size(); i++){
            int nInts = list[i].size();
            if (nInts > 0 && webs[i]->spillCost / nInts < curSpillCost){
                curSpillNode = i;
                curSpillCost = webs[i]->spillCost / nInts;
            }
        }
        stk.push(curSpillNode);
        delOneNode(curSpillNode);
    }
}

void GraphColor::delOneNode(int i)
{
    for (auto neighbor : list[i]){
        auto it = find(list[neighbor].begin(), list[neighbor].end(), i);
        if (it != list[neighbor].end()){
            list[neighbor].erase(it);
        }
        if (list[neighbor].empty()){
            stk.push(neighbor);
        }
    }
    list[i].clear();
}

bool GraphColor::assignRegs()
{
    bool success = true;
    while (!stk.empty()){
        auto r = stk.top();
        stk.pop();
        int color = allocColor(r);
        if (color > -1){
            webs[r]->rreg = color;
        }
        else{
            webs[r]->spill = true;
            success = false;
        }
    }
    return success;
}

int GraphColor::allocColor(int r)
{
    int totalRegNum = regNum + fRegNum;
    vector<bool> use(totalRegNum, false);
    bool fpu = webs[r]->fpu;
    for (auto &i : removeList[r]){
        if (webs[i]->rreg != -1){
            use[webs[i]->rreg] = true;
        }
    }
    if (!fpu){
        for (int i = 0; i < regNum; i++){
            if (!use[i]){
                return i;
            }
        }
    }
    else{
        for (int i = regNum; i < regNum + fRegNum; i++){
            if (!use[i]){
                return i;
            }
        }
    }
    return -1;
}

void GraphColor::modifyCode()
{
    for (int i = regNum; i < (int)webs.size(); i++){
        auto web = webs[i];
        auto rreg = web->rreg;
        if (web->fpu){
            rreg = rreg - regNum + 16;
        }
        if (web->defs.size() || web->uses.size()){
            if (rreg > 3 || rreg > 3 + 16){
                func->addSavedRegs(rreg);
            }
        }
        for (auto use : web->uses){
            use->setReg(rreg);
        }
        for (auto def : web->defs){
            def->setReg(rreg);
        }
    }
    vector<MachineInstruction *> reduceInstVec;
    for (auto &block : func->getBlocks()){
        for (auto &inst : block->getInsts()){
            bool reduceInstFlag = false;
            if (inst->isMov()){
                reduceInstFlag = true;
            }else if (inst->isVMov())
            {
                auto def = inst->getDef()[0];
                auto use = inst->getUse()[0];
                if (def->isFloat() && use->isFloat())
                    reduceInstFlag = true;
            }else if (inst->isAdd()){
                auto def = inst->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
                auto uses = inst->getUse();
                for (auto use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        reduceInstFlag = true;
                if (uses[0]->isParam())
                    reduceInstFlag = false;
            }else if(inst->isVAdd()){
                auto def = inst->getDef()[0];
                if (def->isReg() && def->getReg() == 13)
                    continue;
                auto uses = inst->getUse();
                for (auto use : uses)
                    if (use->isImm() && use->getVal() == 0)
                        reduceInstFlag = true;
                if (uses[0]->isParam())
                    reduceInstFlag = false;
            }
            if (!reduceInstFlag)
                continue ;
            auto def = *(inst->getDef().begin());
            auto uses = inst->getUse();
            if (uses.empty() || uses[0]->isImm())
                continue ;
            auto use = *(uses.begin());
            if (def->getReg() == use->getReg())
                reduceInstVec.push_back(inst);
        }
    }
    for (auto &ins : reduceInstVec)
        ins->getParent()->remove(ins);
}

void GraphColor::genSpillCode()
{
    for (auto web : webs)
    {
        if (!web->spill)
            continue;
        web->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, web->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto def : web->defs){
            if (!def->getParent())
                continue ;
            auto tempOperand = new MachineOperand(*def);
            MachineOperand *operand = nullptr;
            MachineInstruction *regOffsetInst = nullptr, *inst = nullptr;
            if (web->disp > 255 || web->disp < -255){
                operand = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
                regOffsetInst = new LoadMInstruction(def->getParent()->getParent(), LoadMInstruction::LDR, operand, off);
                def->getParent()->insertAfter(regOffsetInst);
            }
            if (operand){
                if (!def->isFloat()){
                    inst = new StoreMInstruction(def->getParent()->getParent(), StoreMInstruction::STR, tempOperand, fp, new MachineOperand(*operand));
                }
                else
                {
                    auto reg = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
                    MachineInstruction *tmp_inst = new BinaryMInstruction(def->getParent()->getParent(), BinaryMInstruction::ADD, reg, fp, new MachineOperand(*operand));
                    regOffsetInst->insertAfter(tmp_inst);
                    regOffsetInst = tmp_inst;
                    inst = new StoreMInstruction(def->getParent()->getParent(), StoreMInstruction::VSTR, tempOperand, new MachineOperand(*reg));
                }
            }
            else{
                if (!def->isFloat()){
                    inst = new StoreMInstruction(def->getParent()->getParent(), StoreMInstruction::STR, tempOperand, fp, off);
                }
                else{
                    inst = new StoreMInstruction(def->getParent()->getParent(),StoreMInstruction::VSTR, tempOperand, fp, off);
                }
            }
            if (regOffsetInst)
                regOffsetInst->insertAfter(inst);
            else
                def->getParent()->insertAfter(inst);
        }
        for (auto use : web->uses){
            auto tempOperand = new MachineOperand(*use);
            MachineOperand *operand = nullptr;
            if (web->disp > 255 || web->disp < -255){
                operand = new MachineOperand(MachineOperand::VREG,SymbolTable::getLabel());
                auto regOffsetInst = new LoadMInstruction(use->getParent()->getParent(), LoadMInstruction::LDR, operand, off);
                use->getParent()->insertBefore(regOffsetInst);
                if (!use->isFloat()){
                    auto inst = new LoadMInstruction(use->getParent()->getParent(), LoadMInstruction::LDR, tempOperand, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);
                }
                else{
                    auto reg = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel()); 
                    MachineInstruction *inst = new BinaryMInstruction(use->getParent()->getParent(), BinaryMInstruction::ADD, reg, fp, new MachineOperand(*operand));
                    use->getParent()->insertBefore(inst);
                    inst = new LoadMInstruction(use->getParent()->getParent(), LoadMInstruction::VLDR, tempOperand, new MachineOperand(*reg));
                    use->getParent()->insertBefore(inst);
                }
            }
            else{
                if (!use->isFloat()){
                    auto inst = new LoadMInstruction(use->getParent()->getParent(), LoadMInstruction::LDR, tempOperand, fp, off);
                    use->getParent()->insertBefore(inst);
                }
                else{
                    auto inst = new LoadMInstruction(use->getParent()->getParent(), LoadMInstruction::VLDR, tempOperand, fp, off);
                    use->getParent()->insertBefore(inst);
                }
            }
        }
    }
}