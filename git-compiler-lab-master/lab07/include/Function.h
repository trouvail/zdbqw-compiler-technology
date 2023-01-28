#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <algorithm>
#include <iostream>

#include <map>
#include <set>


#include <vector>
#include "AsmBuilder.h"
#include "Ast.h"
#include "BasicBlock.h"
#include "SSAGraph.h"
#include "SymbolTable.h"



#include <list>

class Unit;

struct TreeNode
{
    
    static int Num;
    int num;


    std::vector<TreeNode *> children;
    TreeNode *parent = nullptr;


    BasicBlock *block;
    
    int getHeight()
    {
        int height = 0;
        TreeNode *temp = this;
        while (temp)
        {
            height++;
            temp = temp->parent;
        }
        return height;
    }
    
    TreeNode(BasicBlock *block) : block(block)
    {
        num = Num++;
        block->order = num;
    }
    
    TreeNode(BasicBlock *block, int num) : block(block)
    {
        this->num = block->order;
    }
    void addChild(TreeNode *child_zyl) { children.push_back(child_zyl); }
    
    
};

class Function
{
    typedef std::vector<BasicBlock *>::iterator iterator;
    typedef std::vector<BasicBlock *>::reverse_iterator reverse_iterator;

private:
    std::vector<BasicBlock *> block_list;
    SymbolEntry *sym_ptr;
    BasicBlock *entry;
    Unit *parent;
    TreeNode *DFSTreeRoot;
    TreeNode *domTreeRoot;
    
    std::vector<TreeNode *> preOrder2DFS;


    int essential = -1;
    std::vector<SSAGraphNode *> nodes;
    std::set<Operand *> stores;
    
    bool call;
    std::map<Function *, std::vector<Instruction *>> preds;
    bool recur;
    
    int instNum;
    std::set<Operand *> storedGlobals;
    
    std::vector<TreeNode *> preOrder2dom;
    
    std::vector<int> sdoms;
    std::vector<int> idoms;
    
    

public:
    Function() {}
    Function(Unit *, SymbolEntry *);
    ~Function();
    void insertBlock(BasicBlock *bb_zyl) { block_list.push_back(bb_zyl); };
    BasicBlock *getEntry() { return entry; };
    void setEntry(BasicBlock *entry_zyl) { this->entry = entry_zyl; };
    void remove(BasicBlock *bb);
    
    void computeDFSTree();
    void search(TreeNode *node, bool *visited);


    void dfs1(BasicBlock *block, std::set<BasicBlock *> &v);
    void computeReverseDFSTree(BasicBlock *exit);
    void reverseSearch(TreeNode *node, bool *visited);

    void computeReverseSdom(BasicBlock *exit);
    void computeReverseIdom(BasicBlock *exit);
    void computeReverseDomFrontier();


    int getEssential();
    BasicBlock *getMarkBranch(BasicBlock *block);
    void computeStores();





    
    std::set<Operand *> &getStores() { return stores; }
    int getIndex(BasicBlock *block)
    {
        return std::find(block_list.begin(), block_list.end(), block) -
               block_list.begin();
    }
    int eval(int i, int *ancestors);
    void computeSdom();
    int LCA(int i, int j);
    void computeIdom();
    void domTest();
    void computeDomFrontier();
    TreeNode *getDomNode(BasicBlock *b) { return preOrder2dom[b->order]; }
    void dfs(AsmBuilder *builder,
             BasicBlock *block,
             std::set<BasicBlock *> &v,
             std::map<BasicBlock *, MachineBlock *> &map);


    void addPred(Instruction *in);
    void removePred(Instruction *in);
    bool hasRecur() { return recur; }






    void setInstNum(int num_zyl) { instNum = num_zyl; }


    int getInstNum() { return instNum; }
    void addStoredGlobal(Operand *ope_zyl) { storedGlobals.insert(ope_zyl); }
    std::set<Operand *> getStoredGlobals() { return storedGlobals; }
    void genSSAGraph();



    void output();
    std::vector<BasicBlock *> &getBlockList() { return block_list; };
    iterator begin() { return block_list.begin(); };
    iterator end() { return block_list.end(); };


    reverse_iterator rbegin() { return block_list.rbegin(); };
    reverse_iterator rend() { return block_list.rend(); };




    SymbolEntry *getSymPtr() { return sym_ptr; };
    void genMachineCode(AsmBuilder *);
    std::vector<std::vector<int>> getBlockMap();




    std::vector<SSAGraphNode *> &getSSAGraph() { return nodes; };


    void addNodeToSSAGraph(SSAGraphNode *node_zyl) { nodes.push_back(node_zyl); };
    
    bool hasCall() { return call; }
    void setHasCall() { call = true; }
    std::map<Function *, std::vector<Instruction *>> &getPreds()
    {
        return preds;
    };
    
};

#endif