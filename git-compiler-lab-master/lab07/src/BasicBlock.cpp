#include "BasicBlock.h"
#include <algorithm>
#include <vector>
#include "Function.h"

using namespace std;
extern FILE *yyout;

// insert the instruction to the front of the basicblock.
void BasicBlock::insertFront(Instruction *inst_zhouyanlin)
{
    insertBefore(inst_zhouyanlin, head->getNext());
}


void BasicBlock::replaceIns(Instruction *old_zhouyanlin, Instruction *new_)
{
    old_zhouyanlin->getPrev()->setNext(new_);
    new_->setPrev(old_zhouyanlin->getPrev());
    old_zhouyanlin->getNext()->setPrev(new_);
    new_->setNext(old_zhouyanlin->getNext());
}

void BasicBlock::addAlloc(Instruction *alloc_zhouyanlin)
{
    assert(alloc_zhouyanlin->isAlloc());
    if (!lastAlloc)
    {
        insertFront(alloc_zhouyanlin);
        lastAlloc = alloc_zhouyanlin;
    }
    else
    {
        insertAfter(alloc_zhouyanlin, lastAlloc);
        lastAlloc = alloc_zhouyanlin;
    }
}


// insert the instruction to the back of the basicblock.
void BasicBlock::insertBack(Instruction *inst_zhouyanlin)
{
    insertBefore(inst_zhouyanlin, head);
}


// remove the instruction from intruction list.
void BasicBlock::remove(Instruction *inst_zhouyanlin)
{
    inst_zhouyanlin->getPrev()->setNext(inst_zhouyanlin->getNext());
    inst_zhouyanlin->getNext()->setPrev(inst_zhouyanlin->getPrev());
}

void BasicBlock::output() const
{
    fprintf(yyout, "B%d:", no);

    if (!pred.empty())
    {
        fprintf(yyout, "%*c; preds = %%B%d", 32, '\t', pred[0]->getNo());
        for (auto i = pred.begin() + 1; i != pred.end(); i++)
            fprintf(yyout, ", %%B%d", (*i)->getNo());
    }
    fprintf(yyout, "\n");
    for (auto i_zhouyanlin = head->getNext(); i_zhouyanlin != head; i_zhouyanlin = i_zhouyanlin->getNext())
        i_zhouyanlin->output();
}

BasicBlock::~BasicBlock()
{
    Instruction *inst_zhouyanlin;
    inst_zhouyanlin = head->getNext();
    while (inst_zhouyanlin != head)
    {
        Instruction *t;
        t = inst_zhouyanlin;
        inst_zhouyanlin = inst_zhouyanlin->getNext();
        delete t;
    }
    for (auto &bb : pred)
        bb->removeSucc(this);
    for (auto &bb : succ)
        bb->removePred(this);
    parent->remove(this);
}

void BasicBlock::cleanSucc()
{
    for (auto i : succ)
        i->removePred(this);
    vector<BasicBlock *>().swap(succ);
}




void BasicBlock::addSucc(BasicBlock *bb_zhouyanlin, bool first_zhouyanlin)
{
    if (first_zhouyanlin)
        succ.insert(succ.begin(), bb_zhouyanlin);
    else
        succ.push_back(bb_zhouyanlin);
}

// remove the successor basicclock bb.
void BasicBlock::removeSucc(BasicBlock *bb_zhouyanlin)
{
    auto iter = std::find(succ.begin(), succ.end(), bb_zhouyanlin);
    if (iter != succ.end())
        succ.erase(iter);
}

void BasicBlock::removeSuccFromEnd(BasicBlock *bb_zhouyanlin)
{
    succ.erase((std::find(succ.rbegin(), succ.rend(), bb_zhouyanlin) + 1).base());
}

void BasicBlock::addPred(BasicBlock *bb_zhouyanlin)
{
    pred.push_back(bb_zhouyanlin);
}

// remove the predecessor basicblock bb.
void BasicBlock::removePred(BasicBlock *bb_zhouyanlin)
{
    auto iter = std::find(pred.begin(), pred.end(), bb_zhouyanlin);
    if (iter != pred.end())
        pred.erase(iter);
}

void BasicBlock::removePredFromEnd(BasicBlock *bb_zhouyanlin)
{
    auto iter = std::find(pred.rbegin(), pred.rend(), bb_zhouyanlin);
    if (iter != pred.rend())
        pred.erase((iter + 1).base());
}

void BasicBlock::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_func = builder_zhouyanlin->getFunction();
    auto cur_block = new MachineBlock(cur_func, no);
    builder_zhouyanlin->setBlock(cur_block);
    for (auto i = head->getNext(); i != head; i = i->getNext())
    {
        i->genMachineCode(builder_zhouyanlin);
    }
    cur_func->InsertBlock(cur_block);
}

BasicBlock::BasicBlock(Function *f_zhouyanlin)
{
    this->no = SymbolTable::getLabel();
    f_zhouyanlin->insertBlock(this);
    parent = f_zhouyanlin;
    head = new DummyInstruction();
    head->setParent(this);
    mark = false;
    lastAlloc = nullptr;
}



// insert the instruction dst before src.
void BasicBlock::insertBefore(Instruction *dst_zhouyanlin, Instruction *src_zhouyanlin)
{
    // Todo
    src_zhouyanlin->getPrev()->setNext(dst_zhouyanlin);
    dst_zhouyanlin->setPrev(src_zhouyanlin->getPrev());

    dst_zhouyanlin->setNext(src_zhouyanlin);
    src_zhouyanlin->setPrev(dst_zhouyanlin);

    dst_zhouyanlin->setParent(this);
}

// insert the instruction dst after src.
void BasicBlock::insertAfter(Instruction *dst_zhouyanlin, Instruction *src_zhouyanlin)
{
    dst_zhouyanlin->setNext(src_zhouyanlin->getNext());
    src_zhouyanlin->getNext()->setPrev(dst_zhouyanlin);

    dst_zhouyanlin->setPrev(src_zhouyanlin);
    src_zhouyanlin->setNext(dst_zhouyanlin);

    dst_zhouyanlin->setParent(this);
}


void BasicBlock::cleanMark()
{
    auto inst_zhouyanlin = head->getNext();
    while (inst_zhouyanlin != head)
    {
        inst_zhouyanlin->unsetMark();
        inst_zhouyanlin = inst_zhouyanlin->getNext();
    }
}
void BasicBlock::insertPhiInstruction(Operand *dst_zhouyanlin)
{
    Instruction *i_zhouyanlin = new PhiInstruction(dst_zhouyanlin);
    insertFront(i_zhouyanlin);
}

void BasicBlock::deleteBack(int num_zhouyanlin)
{
    while (num_zhouyanlin--)
    {
        remove(head->getPrev());
    }
}

bool BasicBlock::isBefore(Instruction *a_zhouyanlin, Instruction *b_zhouyanlin)
{
    if (a_zhouyanlin->getParent() != this)
        return true;
    assert(b_zhouyanlin->getParent() == this);
    auto temp = a_zhouyanlin;
    while (temp != head)
    {
        if (temp == b_zhouyanlin)
            return true;
        temp = temp->getNext();
    }
    return false;
}

