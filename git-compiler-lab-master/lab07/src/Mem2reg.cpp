#include "Mem2reg.h"
#include <map>
#include <set>
#include <vector>
#include "BasicBlock.h"
#include "Instruction.h"
using namespace std;

void Mem2reg::pass()
{
    auto iter = unit->begin();
    while (iter != unit->end())
        pass(*iter++);
}

void Mem2reg::pass(Function *function)
{
    checkCondBranch(function);
    function->computeDFSTree();
    function->computeSdom();
    function->computeIdom();
    function->computeDomFrontier();
    insertPhiInstruction(function);
    rename(function);
    cleanAddZeroIns(function);
}

void Mem2reg::insertPhiInstruction(Function *function)
{
    vector<BinaryInstruction *>().swap(addZeroIns);
    vector<AllocaInstruction *>().swap(allocaIns);
    BasicBlock *funcEntry = function->getEntry();
    for (auto i = funcEntry->begin(); i != funcEntry->end(); i = i->getNext()){
        if (!i->isAlloc()){
            break;
        }
        auto alloca = (AllocaInstruction *)i;
        if (!alloca->isArray()){
            allocaIns.push_back(alloca);
        }
    }
    vector<BasicBlock *> worklist;
    set<BasicBlock *> inWorklist, inserted, assigns;
    for (auto inst : allocaIns)
    {
        vector<BasicBlock *>().swap(worklist);
        inWorklist.clear();
        inserted.clear();
        assigns.clear();
        auto block = inst->getParent();
        auto operand = inst->getDef();
        Operand *newOperand = new Operand(new TemporarySymbolEntry(((PointerType *)(operand->getType()))->getType(), SymbolTable::getLabel()));
        block->remove(inst);
        operand->setDef(nullptr);
        inst->replaceDef(newOperand);
        while (operand->use_begin() != operand->use_end())
        {
            auto useInst = operand->use_begin();
            if ((*useInst)->isStore())
            {
                if (newOperand != (*useInst)->getUse()[1])
                {
                    auto assignIns = new BinaryInstruction(BinaryInstruction::ADD, newOperand, (*useInst)->getUse()[1], new Operand(new ConstantSymbolEntry(newOperand->getType(), 0)));
                    addZeroIns.push_back(assignIns);
                    (*useInst)->getParent()->insertBefore(assignIns, *useInst);
                    assigns.insert((*useInst)->getParent());
                    (*useInst)->getUse()[1]->removeUse(*useInst);
                }
            }
            auto defOperand = (*useInst)->getDef();
            (*useInst)->getParent()->remove(*useInst);
            if (defOperand && defOperand != newOperand)
                while (defOperand->use_begin() != defOperand->use_end())
                {
                    auto useOperand = *(defOperand->use_begin());
                    useOperand->replaceUse(defOperand, newOperand);
                }
            operand->removeUse(*useInst);
        }
        for (auto block : assigns)
        {
            worklist.push_back(block);
            inWorklist.insert(block);
            while (!worklist.empty())
            {
                BasicBlock *block = worklist[0];
                worklist.erase(worklist.begin());
                for (auto domFrontierBlock : block->domFrontier)
                {
                    if (inserted.find(domFrontierBlock) == inserted.end())
                    {
                        domFrontierBlock->insertPhiInstruction(newOperand);
                        inserted.insert(domFrontierBlock);
                        if (inWorklist.find(domFrontierBlock) == inWorklist.end())
                        {
                            inWorklist.insert(domFrontierBlock);
                            worklist.push_back(domFrontierBlock);
                        }
                    }
                }
            }
        }
    }
}

void Mem2reg::rename(Function *function)
{
    stacks.clear();
    for (auto inst : allocaIns)
    {
        auto operand = inst->getDef();
        stacks[operand] = stack<Operand *>();
    }
    rename(function->getEntry());
}

void Mem2reg::rename(BasicBlock *block)
{
    std::map<Operand *, int> counter;
    for (auto inst = block->begin(); inst != block->end(); inst = inst->getNext())
    {
        Operand *def = inst->getDef();
        if (def && stacks.find(def) != stacks.end())
        {
            counter[def]++;
            Operand *newOperand = newName(def);
            inst->replaceDef(newOperand);
        }
        if (!inst->isPhi())
            for (auto u : inst->getUse())
                if (stacks.find(u) != stacks.end() && !stacks[u].empty())
                    inst->replaceUse(u, stacks[u].top());
    }
    for (auto it = block->succ_begin(); it != block->succ_end(); it++)
    {
        for (auto inst = (*it)->begin(); inst != (*it)->end(); inst = inst->getNext())
        {
            if (inst->isPhi())
            {
                PhiInstruction *phi = (PhiInstruction *)inst;
                Operand *operand = phi->getOriginDef();
                if (!stacks[operand].empty())
                    phi->addSrc(block, stacks[operand].top());
                else
                    phi->addSrc(block, new Operand(new ConstantSymbolEntry(operand->getType(), 0)));
            }
            else
                break;
        }
    }
    auto func = block->getParent();
    auto node = func->getDomNode(block);
    for (auto child : node->children)
        rename(child->block);
    for (auto it : counter)
        for (int i = 0; i < it.second; i++)
            stacks[it.first].pop();
}

Operand *Mem2reg::newName(Operand *old)
{
    Operand *ret = new Operand(*old);
    stacks[old].push(ret);
    return ret;
}

void Mem2reg::cleanAddZeroIns(Function *func)
{
    auto type = (FunctionType *)(func->getSymPtr()->getType());
    int paramNo = type->getParamsType().size() - 1;
    int regNum = 4;
    if (paramNo > 3)
        regNum--;
    for (auto i : addZeroIns)
    {
        auto use = i->getUse()[0];
        // if (use->getEntry()->isConstant())
        //     continue;
        if (i->getParent()->begin() == i && i->getNext()->isUncond())
            continue;
        if (use->getEntry()->isVariable())
        {
            continue;
            // if (func->hasCall())
            //     if (paramNo < regNum) {
            //         paramNo--;
            //         continue;
            //     }
            // if (paramNo >= regNum) {
            //     paramNo--;
            //     continue;
            // }
            // paramNo--;
        }
        auto def = i->getDef();
        // if (def != use)
        while (def->use_begin() != def->use_end())
        {
            auto u = *(def->use_begin());
            u->replaceUse(def, use);
        }
        i->getParent()->remove(i);
        use->removeUse(i);
        delete i;
    }
}

void Mem2reg::checkCondBranch(Function *func)
{
    for (auto block : func->getBlockList())
    {
        auto lastInst = block->rbegin();
        if (lastInst->isCond()){
            auto cond = (CondBrInstruction *)lastInst;
            auto condTrueBranch = cond->getTrueBranch();
            auto condFalseBranch = cond->getFalseBranch();
            if (condTrueBranch == condFalseBranch)
            {
                block->removeSucc(condTrueBranch);
                condTrueBranch->removePred(block);
                new UncondBrInstruction(condTrueBranch, block);
                block->remove(lastInst);
            }
        }
    }
}