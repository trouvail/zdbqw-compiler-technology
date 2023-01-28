#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <set>
#include <vector>
#include "AsmBuilder.h"
#include "Instruction.h"

class Function;

class BasicBlock
{
    typedef std::vector<BasicBlock *>::iterator bb_iterator;

private:
    std::vector<BasicBlock *> pred, succ;
    Instruction *head;


    std::set<Operand *> stores;
    
    std::set<Operand *> stores1;
    
    Instruction *lastAlloc;
    Function *parent;
    int no;
    
    std::map<BasicBlock *, BasicBlock *> phiBlocks;
    bool mark;
    
    

public:
    int order;
    std::set<BasicBlock *> domFrontier;
    BasicBlock(Function *);
    ~BasicBlock();


    void addSucc(BasicBlock *, bool first = false);
    void removeSucc(BasicBlock *);
    void removeSuccFromEnd(BasicBlock *);
    void addPred(BasicBlock *);
    void removePred(BasicBlock *);
    void insertFront(Instruction *);
    void insertBack(Instruction *);


    
    void insertBefore(Instruction *, Instruction *);
    void insertAfter(Instruction *, Instruction *);
    void remove(Instruction *);
    bool empty() const { return head->getNext() == head; }
    void output() const;
    bool succEmpty() const { return succ.empty(); };


    Instruction *rend() { return head; };
    bb_iterator succ_begin() { return succ.begin(); };
    bb_iterator succ_end() { return succ.end(); };
    bool isBefore(Instruction *a, Instruction *b);
    std::set<Operand *> &getStores() { return stores; }
    std::set<Operand *> &getStores1() { return stores1; }
    void addStore(Operand *ope_zyl) { stores.insert(ope_zyl); }
    void addStore1(Operand *ope_zyl) { stores1.insert(ope_zyl); }
    void removeStore(Operand *ope_zyl) { stores.erase(ope_zyl); }
    bool inStore(Operand *ope_zyl) { return stores.count(ope_zyl); }
    void replaceIns(Instruction *old, Instruction *new_);
    void addAlloc(Instruction *alloc);
    bb_iterator pred_begin() { return pred.begin(); };
    bb_iterator pred_end() { return pred.end(); };
    int getNumOfPred() const { return pred.size(); };
    int getNumOfSucc() const { return succ.size(); };
    bool predEmpty() const { return pred.empty(); };
    
    void removePredFromEnd(BasicBlock *);
    int getNo() { return no; };
    Function *getParent() { return parent; };
    Instruction *begin() { return head->getNext(); };
    Instruction *end() { return head; };
    Instruction *rbegin() { return head->getPrev(); };


    void insertPhiInstruction(Operand *operand);
    void cleanPhiBlocks() { phiBlocks.clear(); }
    std::map<BasicBlock *, BasicBlock *> &getPhiBlocks() { return phiBlocks; }
    std::vector<BasicBlock *> getSucc() { return succ; }
    std::vector<BasicBlock *> getPred() { return pred; }
    void deleteBack(int num = 1);
    void setMark() { mark = true; }
    
    void genMachineCode(AsmBuilder *);
    void cleanSucc();
    void cleanMark();
    
    void unsetMark() { mark = false; }
    bool isMark() { return mark; }
    
};

#endif
