#include "Function.h"
#include <list>


#include <vector>
#include "Type.h"
#include "Unit.h"


#include <queue>


using namespace std;

extern FILE *yyout;

Function::Function(Unit *u_zhouyanlin, SymbolEntry *s_zhouyanlin)
{
    entry = new BasicBlock(this);
    sym_ptr = s_zhouyanlin;
    parent = u_zhouyanlin;
    ((IdentifierSymbolEntry *)s_zhouyanlin)->setFunction(this);
    call = false;
    recur = false;
    u_zhouyanlin->insertFunc(this);
}

int TreeNode::Num = 0;

Function::~Function()
{
   
}

void Function::remove(BasicBlock *bb_zhouyanlin)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb_zhouyanlin));
}

void Function::dfs1(BasicBlock *block_zhouyanlin, std::set<BasicBlock *> &v_zhouyanlin)
{
    v_zhouyanlin.insert(block_zhouyanlin);
    for (auto it_zhouyanlin = block_zhouyanlin->succ_begin(); it_zhouyanlin != block_zhouyanlin->succ_end(); it_zhouyanlin++)
    {
        if (v_zhouyanlin.find(*it_zhouyanlin) == v_zhouyanlin.end())
        {
            (*it_zhouyanlin)->output();
            dfs1(*it_zhouyanlin, v_zhouyanlin);
        }
    }
}

void Function::output()
{
    FunctionType *funcType_zhouyanlin = dynamic_cast<FunctionType *>(sym_ptr->getType());
    Type *retType_zhouyanlin = funcType_zhouyanlin->getRetType();
    std::vector<SymbolEntry *> paramsSe_zhouyanlin = funcType_zhouyanlin->getParamsSe();
    if (!paramsSe_zhouyanlin.size())
        fprintf(yyout, "define %s %s() {\n", retType_zhouyanlin->toStr().c_str(),
                sym_ptr->toStr().c_str());
    else
    {
        fprintf(yyout, "define %s %s(", retType_zhouyanlin->toStr().c_str(),
                sym_ptr->toStr().c_str());
        for (long unsigned int i = 0; i < paramsSe_zhouyanlin.size(); i++)
        {
            if (i)
                fprintf(yyout, ", ");
            fprintf(yyout, "%s %s", paramsSe_zhouyanlin[i]->getType()->toStr().c_str(),
                    paramsSe_zhouyanlin[i]->toStr().c_str());
        }
        fprintf(yyout, ") {\n");
    }
    std::set<BasicBlock *> v;
    entry->output();
    dfs1(entry, v);
    fprintf(yyout, "}\n");
    fflush(yyout);
}


void Function::computeDFSTree()
{
    TreeNode::Num = 0;
    int blockNum = block_list.size();
    preOrder2DFS.resize(blockNum);
    bool *visited = new bool[blockNum]{};
    DFSTreeRoot = new TreeNode(entry);
    preOrder2DFS[DFSTreeRoot->num] = DFSTreeRoot;
    search(DFSTreeRoot, visited);
    delete[] visited;
}

void Function::search(TreeNode *node, bool *visited)
{
    int index = getIndex(node->block);
    visited[index] = true;
    auto block = block_list[index];
    for (auto it = block->succ_begin(); it != block->succ_end(); it++)
    {
        int itIndex = getIndex(*it);
        if (!visited[itIndex])
        {
            TreeNode *child = new TreeNode(*it);
            preOrder2DFS[child->num] = child;
            child->parent = node;
            node->addChild(child);
            search(child, visited);
        }
    }
}

int Function::eval(int v, int *ancestors)
{
    int a_zhouyanlin = ancestors[v];
    while (a_zhouyanlin != -1 && ancestors[a_zhouyanlin] != -1)
    {
        if (sdoms[v] > sdoms[a_zhouyanlin])
            v = a_zhouyanlin;
        a_zhouyanlin = ancestors[a_zhouyanlin];
    }
    return v;
}

void Function::computeSdom()
{
    int blockNum = block_list.size();
    sdoms.resize(blockNum);
    int *ancestors = new int[blockNum];
    for (int i = 0; i < blockNum; i++){
        sdoms[i] = i;
        ancestors[i] = -1;
    }
    for (auto it = preOrder2DFS.rbegin(); (*it)->block != entry; it++){
        auto block = (*it)->block;
        int s = block->order;
        for (auto it1 = block->pred_begin(); it1 != block->pred_end(); it1++)
        {
            int z = eval((*it1)->order, ancestors);
            if (sdoms[z] < sdoms[s])
                sdoms[s] = sdoms[z];
        }
        ancestors[s] = (*it)->parent->num;
    }
    delete[] ancestors;
}


void Function::genSSAGraph()
{
    vector<Instruction *> temp;
    for (auto block_zhouyanlin : block_list)
    {
        for (auto in_zhouyanlin = block_zhouyanlin->begin(); in_zhouyanlin != block_zhouyanlin->end(); in_zhouyanlin = in_zhouyanlin->getNext())
        {
            bool flag = in_zhouyanlin->genNode();
            if (!flag)
            {
                temp.push_back(in_zhouyanlin);
                nodes.push_back(in_zhouyanlin->getNode());
            }
            else
            {
                auto node = in_zhouyanlin->getNode();
                if (node &&
                    find(nodes.begin(), nodes.end(), node) == nodes.end())
                    nodes.push_back(node);
            }
        }
    }
    std::vector<Instruction *> falseIns;

    while (temp.size() != 0)
    {
        for (auto in : temp)
        {
            bool flag;
            flag = in->reGenNode();
            if (!flag)
            {
                falseIns.push_back(in);
            }
        }
        temp.clear();
        temp.assign(falseIns.begin(), falseIns.end());
        falseIns.clear();
    }
}


int Function::LCA(int i_zhouyanlin, int j_zhouyanlin)
{
    TreeNode *n1 = preOrder2dom[i_zhouyanlin];
    TreeNode *n2 = preOrder2dom[j_zhouyanlin];
    int h1 = n1->getHeight();
    int h2 = n2->getHeight();
    if (h1 > h2)
    {
        swap(h1, h2);
        swap(n1, n2);
    }
    int h = h2 - h1;
    for (int i = 0; i < h; i++)
        n2 = n2->parent;
    while (n1 && n2)
    {
        if (n1 == n2)
            return n1->num;
        n1 = n1->parent;
        n2 = n2->parent;
    }

    return -1;
}

void Function::computeIdom()
{
    int blockNum = block_list.size();
    idoms.resize(blockNum);
    domTreeRoot = new TreeNode(entry, 0);
    preOrder2dom.resize(blockNum);
    preOrder2dom[entry->order] = domTreeRoot;
    idoms[entry->order] = 0;
    for (auto it = preOrder2DFS.begin() + 1; it != preOrder2DFS.end(); it++){
        int p = LCA((*it)->parent->num, sdoms[(*it)->num]);
        idoms[(*it)->num] = p;
        auto parent = preOrder2dom[p];
        TreeNode *node = new TreeNode((*it)->block, 0);
        node->parent = parent;
        parent->addChild(node);
        preOrder2dom[(*it)->num] = node;
    }
}

void Function::computeDomFrontier()
{
    for (auto block : block_list)
    {
        if (block->getNumOfPred() >= 2)
        {
            for (auto it = block->pred_begin(); it != block->pred_end(); it++)
            {
                int runner = (*it)->order;
                while (runner != idoms[block->order])
                {
                    preOrder2DFS[runner]->block->domFrontier.insert(block);
                    runner = idoms[runner];
                }
            }
        }
    }

}

void Function::domTest()
{
#define a2b(a, b)                          \
    block_list[a]->addSucc(block_list[b]); \
    block_list[b]->addPred(block_list[a])

    for (int i = 0; i < 8; i++)
        new BasicBlock(this);
    entry = block_list[0];
    int links[8][2] = {{1}, {2}, {3, 6}, {}, {2}, {4, 7}, {5}, {5}};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 2 && links[i][j]; j++)
        {
            a2b(i, links[i][j]);
        }
    computeDFSTree();
    computeSdom();
    computeIdom();
    computeDomFrontier();
}

void Function::computeReverseDFSTree(BasicBlock *exit)
{
    TreeNode::Num = 0;
    int len_zhouyanlin = block_list.size();
    preOrder2DFS.resize(len_zhouyanlin);
    bool *visited = new bool[len_zhouyanlin]{};
    DFSTreeRoot = new TreeNode(exit);
    preOrder2DFS[DFSTreeRoot->num] = DFSTreeRoot;
    reverseSearch(DFSTreeRoot, visited);
    delete[] visited;
}

void Function::reverseSearch(TreeNode *node, bool *visited)
{
    int n_zhouyanlin = getIndex(node->block);
    visited[n_zhouyanlin] = true;
    auto block = block_list[n_zhouyanlin];
    for (auto it = block->pred_begin(); it != block->pred_end(); it++)
    {
        int idx = getIndex(*it);
        if (!visited[idx])
        {
            TreeNode *child = new TreeNode(*it);
            preOrder2DFS[child->num] = child;
            child->parent = node;
            node->addChild(child);
            reverseSearch(child, visited);
        }
    }
}


void Function::addPred(Instruction *in)
{
    assert(in->isCall());
    auto func_zhouyanlin = in->getParent()->getParent();
    if (func_zhouyanlin == this)
        recur = true;
    if (preds.count(func_zhouyanlin))
        preds[func_zhouyanlin].push_back(in);
    else
        preds[func_zhouyanlin] = {in};
}

void Function::removePred(Instruction *in)
{
    assert(in->isCall());
    auto func_zhouyanlin = in->getParent()->getParent();
    auto it = find(preds[func_zhouyanlin].begin(), preds[func_zhouyanlin].end(), in);
    assert(it != preds[func_zhouyanlin].end());
    preds[func_zhouyanlin].erase(it);
}

void Function::computeReverseSdom(BasicBlock *exit)
{
    int len_zhouyanlin = block_list.size();
    sdoms.resize(len_zhouyanlin);
    int *ancestors = new int[len_zhouyanlin];
    for (int i = 0; i < len_zhouyanlin; i++)
    {
        sdoms[i] = i;
        ancestors[i] = -1;
    }
    for (auto it = preOrder2DFS.rbegin(); (*it)->block != exit; it++)
    {
        auto block = (*it)->block;
        int s = block->order;
        for (auto it1 = block->succ_begin(); it1 != block->succ_end(); it1++)
        {
            int z = eval((*it1)->order, ancestors);
            if (sdoms[z] < sdoms[s])
                sdoms[s] = sdoms[z];
        }
        ancestors[s] = (*it)->parent->num;
    }
    delete[] ancestors;
}


void Function::computeReverseDomFrontier()
{
    BasicBlock *exit = new BasicBlock(this);
    for (auto b_zhouyanlin : block_list)
    {
        if (b_zhouyanlin->rbegin()->isRet())
        {
            b_zhouyanlin->addSucc(exit);
            exit->addPred(b_zhouyanlin);
        }
    }
    computeReverseDFSTree(exit);
    computeReverseSdom(exit);
    computeReverseIdom(exit);
    for (auto block_zhouyanlin : block_list)
    {
        if (block_zhouyanlin->getNumOfSucc() >= 2)
        {
            for (auto it = block_zhouyanlin->succ_begin(); it != block_zhouyanlin->succ_end(); it++)
            {
                int runner = (*it)->order;
                while (runner != idoms[block_zhouyanlin->order])
                {
                    preOrder2DFS[runner]->block->domFrontier.insert(block_zhouyanlin);
                    runner = idoms[runner];
                }
            }
        }
    }
    delete exit;
}

int Function::getEssential()
{
    if (essential != -1)
        return essential;
    FunctionType *type = (FunctionType *)(sym_ptr->getType());
    auto paramsType = type->getParamsType();
    for (auto type_zhouyanlin : paramsType)
        if (type_zhouyanlin->isArray())
        {
            essential = 1;
            return essential;
        }
    for (auto block_zhouyanlin : block_list)
    {
        for (auto it_zhouyanlin = block_zhouyanlin->begin(); it_zhouyanlin != block_zhouyanlin->end(); it_zhouyanlin = it_zhouyanlin->getNext())
        {
            if (it_zhouyanlin->isCall())
            {
                IdentifierSymbolEntry *funcSE =
                    (IdentifierSymbolEntry *)(((CallInstruction *)it_zhouyanlin)
                                                  ->getFuncSE());
                if (funcSE->isSysy() ||
                    funcSE->getName() == "llvm.memset.p0.i32")
                {
                    essential = 1;
                    return essential;
                }
                else
                {
                    auto func = funcSE->getFunction();
                    if (func == this)
                        continue;
                    if (func->getEssential() == 1)
                    {
                        essential = 1;
                        return essential;
                    }
                }
            }
            else
            {
                auto def = it_zhouyanlin->getDef();
                if (def && def->getEntry()->isVariable())
                {
                    auto se = (IdentifierSymbolEntry *)(def->getEntry());
                    if (se->isGlobal())
                    {
                        essential = 1;
                        return essential;
                    }
                }
                auto uses = it_zhouyanlin->getUse();
                for (auto use : uses)
                {
                    if (use && use->getEntry()->isVariable())
                    {
                        auto se = (IdentifierSymbolEntry *)(use->getEntry());
                        if (se->isGlobal())
                        {
                            essential = 1;
                            return essential;
                        }
                    }
                }
            }
        }
    }
    essential = 0;
    return essential;
}


void Function::computeReverseIdom(BasicBlock *exit)
{
    int len = block_list.size();
    idoms.resize(len);
    domTreeRoot = new TreeNode(exit, 0);
    preOrder2dom.resize(len);
    preOrder2dom[exit->order] = domTreeRoot;
    idoms[exit->order] = 0;
    for (auto it_zhouyanlin = preOrder2DFS.begin() + 1; it_zhouyanlin != preOrder2DFS.end(); it_zhouyanlin++)
    {
        int p = LCA((*it_zhouyanlin)->parent->num, sdoms[(*it_zhouyanlin)->num]);
        idoms[(*it_zhouyanlin)->num] = p;
        auto parent = preOrder2dom[p];
        TreeNode *node = new TreeNode((*it_zhouyanlin)->block, 0);
        node->parent = parent;
        parent->addChild(node);
        preOrder2dom[(*it_zhouyanlin)->num] = node;
    }
}


BasicBlock *Function::getMarkBranch(BasicBlock *block_zhouyanlin)
{
    set<BasicBlock *> blocks;
    while (true)
    {
        auto order = idoms[block_zhouyanlin->order];
        block_zhouyanlin = preOrder2dom[order]->block;
        if (blocks.count(block_zhouyanlin))
            return nullptr;
        blocks.insert(block_zhouyanlin);
        if (block_zhouyanlin->isMark())
            return block_zhouyanlin;
    }
}

void Function::computeStores()
{
    for (auto node_zhouyanlin : preOrder2dom)
    {
        auto block = node_zhouyanlin->block;
        for (auto it_zhouyanlin = block->pred_begin(); it_zhouyanlin != block->pred_end(); it_zhouyanlin++)
            for (auto ope : (*it_zhouyanlin)->getStores1())
            {
                block->addStore(ope);
                stores.insert(ope);
            }
        for (auto ope : block->getStores())
            block->addStore1(ope);
        for (auto in = block->begin(); in != block->end(); in = in->getNext())
            if (in->isStore())
            {
                block->addStore1(in->getUse()[0]);
                stores.insert(in->getUse()[0]);
            }
    }
    for (auto block : block_list)
    {
        for (auto it = block->pred_begin(); it != block->pred_end(); it++)
            for (auto ope : (*it)->getStores1())
            {
                block->addStore1(ope);
                stores.insert(ope);
            }
    }
}

void Function::dfs(AsmBuilder *builder,
                   BasicBlock *block_zhouyanlin,
                   std::set<BasicBlock *> &v,
                   std::map<BasicBlock *, MachineBlock *> &map)
{
    v.insert(block_zhouyanlin);
    for (auto it = block_zhouyanlin->succ_begin(); it != block_zhouyanlin->succ_end(); it++)
    {
        if (v.find(*it) == v.end())
        {
            (*it)->genMachineCode(builder);
            map[*it] = builder->getBlock();
            dfs(builder, *it, v, map);
        }
    }
}

void Function::genMachineCode(AsmBuilder *builder)
{
    auto cur_unit_zhouyanlin = builder->getUnit();
    auto cur_func_zhouyanlin = new MachineFunction(cur_unit_zhouyanlin, this->sym_ptr);
    builder->setFunction(cur_func_zhouyanlin);
    std::map<BasicBlock *, MachineBlock *> map;

    std::set<BasicBlock *> v;
    entry->genMachineCode(builder);
    map[entry] = builder->getBlock();
    dfs(builder, entry, v, map);

    for (auto block : block_list)
    {
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
            mblock->addPred(map[*pred]);
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            mblock->addSucc(map[*succ]);
    }
    cur_func_zhouyanlin->setEntry(map[entry]);
    cur_unit_zhouyanlin->InsertFunc(cur_func_zhouyanlin);
}

vector<vector<int>> Function::getBlockMap()
{
    int len_zhouyanlin = block_list.size();
    vector<vector<int>> m(len_zhouyanlin, vector<int>(len_zhouyanlin, 0));
    for (int i = 0; i < len_zhouyanlin; i++)
    {
        auto iter = block_list[i]->succ_begin();
        auto end = block_list[i]->succ_end();
        while (iter != end)
        {
            int j = find(block_list.begin(), block_list.end(), *iter) -
                    block_list.begin();
            m[i][j] = 1;
            iter++;
        }
    }
    return m;
}

