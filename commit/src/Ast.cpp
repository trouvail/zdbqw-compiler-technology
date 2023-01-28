#include "Ast.h"
#include <stack>
#include <string>

#include "Instruction.h"
#include "SymbolTable.h"
#include "Type.h"
#include "Unit.h"




#include "IRBuilder.h"

extern Unit unit;
extern MachineUnit mUnit;

#include <iostream>

extern FILE *yyout;
int Node::counter = 0;
IRBuilder *Node::builder;

Node::Node()
{
    seq = counter++;
    next = nullptr;
}

void Node::setNext(Node *node_zhouyanlin)
{
    Node *n = this;
    while (n->getNext())
    {
        n = n->getNext();
    }
    if (n == this)
    {
        this->next = node_zhouyanlin;
    }
    else
    {
        n->setNext(node_zhouyanlin);
    }
}

void Node::backPatch(std::vector<Instruction *> &list_zhouyanlin, BasicBlock *bb_zhouyanlin)
{
    for (auto &inst_zhouyanlin : list_zhouyanlin)
    {
        if (inst_zhouyanlin->isCond())
            dynamic_cast<CondBrInstruction *>(inst_zhouyanlin)->setTrueBranch(bb_zhouyanlin);
        else if (inst_zhouyanlin->isUncond())
            dynamic_cast<UncondBrInstruction *>(inst_zhouyanlin)->setBranch(bb_zhouyanlin);
    }
}

std::vector<Instruction *> Node::merge(std::vector<Instruction *> &list_zhouyanlin,
                                       std::vector<Instruction *> &list2_zhouyanlin)
{
    std::vector<Instruction *> res_zhouyanlin(list_zhouyanlin);
    res_zhouyanlin.insert(res_zhouyanlin.end(), list2_zhouyanlin.begin(), list2_zhouyanlin.end());
    return res_zhouyanlin;
}

void Ast::genCode(Unit *unit_zhouyanlin)
{
    IRBuilder *builder_zhouyanlin = new IRBuilder(unit_zhouyanlin);
    Node::setIRBuilder(builder_zhouyanlin);
    root->genCode();
}

BinaryExpr::BinaryExpr(SymbolEntry *se,
                       int op,
                       ExprNode *expr1,
                       ExprNode *expr2)
    : ExprNode(se, BINARYEXPR), op(op), expr1(expr1), expr2(expr2)
{
    dst = new Operand(se);
    std::string op_str;
    switch (op)
    {
    case ADD:
        op_str = "+";
        break;
    case SUB:
        op_str = "-";
        break;
    case MUL:
        op_str = "*";
        break;
    case DIV:
        op_str = "/";
        break;
    case MOD:
        op_str = "%";
        break;
    case AND:
        op_str = "&&";
        break;
    case OR:
        op_str = "||";
        break;
    case LESS:
        op_str = "<";
        break;
    case LESSEQUAL:
        op_str = "<=";
        break;
    case GREATER:
        op_str = ">";
        break;
    case GREATEREQUAL:
        op_str = ">=";
        break;
    case EQUAL:
        op_str = "==";
        break;
    case NOTEQUAL:
        op_str = "!=";
        break;
    }
    if (expr1->getType()->isVoid() || expr2->getType()->isVoid())
    {
        fprintf(stderr,
                "invalid operand of type \'void\' to binary \'opeartor%s\'\n",
                op_str.c_str());
    }
    if (op >= BinaryExpr::AND && op <= BinaryExpr::NOTEQUAL)
    {
        type = TypeSystem::boolType;
        if (op == BinaryExpr::AND || op == BinaryExpr::OR)
        {
            if (expr1->getType()->isInt() &&
                expr1->getType()->getSize() == 32)
            {
                ImplicitCastExpr *temp_zhouyanlin = new ImplicitCastExpr(expr1);
                this->expr1 = temp_zhouyanlin;
            }
            else if (expr1->getType()->isFloat())
            {
                SymbolEntry *zero_zhouyanlin =
                    new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry *temp_zhouyanlin = new TemporarySymbolEntry(
                    TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr *cmpZero_zhouyanlin = new BinaryExpr(temp_zhouyanlin, BinaryExpr::NOTEQUAL,
                                                     expr1, new Constant(zero_zhouyanlin));
                this->expr1 = cmpZero_zhouyanlin;
            }

            if (expr2->getType()->isInt() &&
                expr2->getType()->getSize() == 32)
            {
                ImplicitCastExpr *temp_zhouyanlin = new ImplicitCastExpr(expr2);
                this->expr2 = temp_zhouyanlin;
            }
            else if (expr2->getType()->isFloat())
            {
                SymbolEntry *zero_zhouyanlin =
                    new ConstantSymbolEntry(TypeSystem::floatType, 0);
                SymbolEntry *temp_zhouyanlin = new TemporarySymbolEntry(
                    TypeSystem::boolType, SymbolTable::getLabel());
                BinaryExpr *cmpZero_zhouyanlin = new BinaryExpr(temp_zhouyanlin, BinaryExpr::NOTEQUAL,
                                                     expr2, new Constant(zero_zhouyanlin));
                this->expr2 = cmpZero_zhouyanlin;
            }
        }
        // TODO optimize literal number
        if (op == BinaryExpr::LESS || op == BinaryExpr::LESSEQUAL ||
            op == BinaryExpr::GREATER || op == BinaryExpr::GREATEREQUAL ||
            op == BinaryExpr::EQUAL || op == BinaryExpr::NOTEQUAL)
        {
            if (expr1->getType()->isFloat() && expr2->getType()->isInt())
            {
                ImplicitCastExpr *temp_zhouyanlin =
                    new ImplicitCastExpr(expr2, TypeSystem::floatType);
                this->expr2 = temp_zhouyanlin;
            }
            else if (expr1->getType()->isInt() &&
                     expr2->getType()->isFloat())
            {
                ImplicitCastExpr *temp_zhouyanlin =
                    new ImplicitCastExpr(expr1, TypeSystem::floatType);
                this->expr1 = temp_zhouyanlin;
                type = TypeSystem::floatType;
            }
        }
    }
    else if (expr1->getType()->isFloat() && expr2->getType()->isInt())
    {
        if (op == BinaryExpr::MOD)
        {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr *temp_zhouyanlin =
            new ImplicitCastExpr(expr2, TypeSystem::floatType);
        this->expr2 = temp_zhouyanlin;
        type = TypeSystem::floatType;
    }
    else if (expr1->getType()->isInt() && expr2->getType()->isFloat())
    {
        if (op == BinaryExpr::MOD)
        {
            fprintf(stderr, "Operands of `mod` must be both integers");
        }
        ImplicitCastExpr *temp_zhouyanlin =
            new ImplicitCastExpr(expr1, TypeSystem::floatType);
        this->expr1 = temp_zhouyanlin;
        type = TypeSystem::floatType;
    }
    else if (expr1->getType()->isFloat() && expr2->getType()->isFloat())
    {
        type = TypeSystem::floatType;
    }
    else
    {
        type = TypeSystem::intType;
    }
};

BinaryExpr::BinaryExpr(const BinaryExpr &b) : ExprNode(b)
{
    op = b.op;
    expr1 = b.expr1->copy();
    expr2 = b.expr2->copy();
    symbolEntry = new TemporarySymbolEntry(b.symbolEntry->getType(),
                                           SymbolTable::getLabel());
    dst = new Operand(symbolEntry);
}

ExprNode *BinaryExpr::getLeft()
{
    return this->expr1;
}

ExprNode *BinaryExpr::getRight()
{
    return this->expr2;
}

ExprNode *UnaryExpr::getSubExpr()
{
    return this->expr;
}
void Constant::genCode()
{
    
}

void Id::genCode()
{
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    Operand *addr_zhouyanlin = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getAddr();
    if (type->isInt() || type->isFloat())
    {
        new LoadInstruction(dst, addr_zhouyanlin, bb_zhouyanlin);
    }
    else if (type->isArray())
    {
        if (arrIdx)
        {
            Type *type = ((ArrayType *)(this->type))->getElementType();
            Type *type1 = this->type;
            Operand *tempSrc_zhouyanlin = addr_zhouyanlin;
            Operand *tempDst_zhouyanlin = dst;
            ExprNode *idx_zhouyanlin = arrIdx;
            bool flag_zhouyanlin = false;
            bool pointer_zhouyanlin = false;
            bool firstFlag_zhouyanlin = true;
            while (true)
            {
                if (((ArrayType *)type1)->getLength() == -1)
                {
                    Operand *dst1 = new Operand(new TemporarySymbolEntry(
                        new PointerType(type), SymbolTable::getLabel()));
                    tempSrc_zhouyanlin = dst1;
                    new LoadInstruction(dst1, addr_zhouyanlin, bb_zhouyanlin);
                    flag_zhouyanlin = true;
                    firstFlag_zhouyanlin = false;
                }
                if (!idx_zhouyanlin)
                {
                    Operand *dst1 = new Operand(new TemporarySymbolEntry(
                        new PointerType(type), SymbolTable::getLabel()));
                    Operand *idx = new Operand(
                        new ConstantSymbolEntry(TypeSystem::intType, 0));
                    new GepInstruction(dst1, tempSrc_zhouyanlin, idx, bb_zhouyanlin);
                    tempDst_zhouyanlin = dst1;
                    pointer_zhouyanlin = true;
                    break;
                }

                idx_zhouyanlin->genCode();
                auto gep_zhouyanlin = new GepInstruction(tempDst_zhouyanlin, tempSrc_zhouyanlin,
                                              idx_zhouyanlin->getOperand(), bb_zhouyanlin, flag_zhouyanlin);
                if (!flag_zhouyanlin && firstFlag_zhouyanlin)
                {
                    gep_zhouyanlin->setFirst();
                    firstFlag_zhouyanlin = false;
                }
                if (flag_zhouyanlin)
                    flag_zhouyanlin = false;
                if (type == TypeSystem::intType ||
                    type == TypeSystem::constIntType ||
                    type == TypeSystem::floatType ||
                    type == TypeSystem::constFloatType)
                {
                    break;
                }
                type = ((ArrayType *)type)->getElementType();
                type1 = ((ArrayType *)type1)->getElementType();
                tempSrc_zhouyanlin = tempDst_zhouyanlin;
                tempDst_zhouyanlin = new Operand(new TemporarySymbolEntry(
                    new PointerType(type), SymbolTable::getLabel()));
                idx_zhouyanlin = (ExprNode *)(idx_zhouyanlin->getNext());
            }
            dst = tempDst_zhouyanlin;
            // 如果是右值还需要加载指令
            if (!left && !pointer_zhouyanlin)
            {
                Operand *dst1 = new Operand(
                    new TemporarySymbolEntry(type, SymbolTable::getLabel()));
                new LoadInstruction(dst1, dst, bb_zhouyanlin);
                dst = dst1;
            }
        }
        else
        {
            if (((ArrayType *)(this->type))->getLength() == -1)
            {
                Operand *dst1 = new Operand(new TemporarySymbolEntry(
                    new PointerType(
                        ((ArrayType *)(this->type))->getElementType()),
                    SymbolTable::getLabel()));
                new LoadInstruction(dst1, addr_zhouyanlin, bb_zhouyanlin);
                dst = dst1;
            }
            else
            {
                Operand *idx = new Operand(
                    new ConstantSymbolEntry(TypeSystem::intType, 0));
                auto gep = new GepInstruction(dst, addr_zhouyanlin, idx, bb_zhouyanlin);
                gep->setFirst();
            }
        }
    }
}

void IfStmt::genCode()
{
    Function *func_zhouyanlin;
    BasicBlock *then_bb_zhouyanlin, *end_bb_zhouyanlin;

    func_zhouyanlin = builder->getInsertBB()->getParent();
    then_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    end_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);

    cond->genCode();

    backPatch(cond->trueList(), then_bb_zhouyanlin);
    backPatch(cond->falseList(), end_bb_zhouyanlin);

    builder->setInsertBB(then_bb_zhouyanlin);
    thenStmt->genCode();
    then_bb_zhouyanlin = builder->getInsertBB();
    new UncondBrInstruction(end_bb_zhouyanlin, then_bb_zhouyanlin);

    builder->setInsertBB(end_bb_zhouyanlin);
}


void FunctionDef::genCode()
{
    Unit *unit_zhouyanlin = builder->getUnit();
    Function *func_zhouyanlin = new Function(unit_zhouyanlin, se);
    BasicBlock *entry_zhouyanlin = func_zhouyanlin->getEntry();
    // set the insert point to the entry basicblock of this function.

    if (func_zhouyanlin->getSymPtr()->toStr() == "@fib")
    {
        builder->setInsertBB(entry_zhouyanlin);
        auto type_zhouyanlin = (FunctionType *)(func_zhouyanlin->getSymPtr()->getType());
        auto cmpDst_zhouyanlin = new Operand(new TemporarySymbolEntry(
            TypeSystem::boolType, SymbolTable::getLabel()));

        auto zero_zhouyanlin = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
        
        auto two_zhouyanlin = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 2));


        auto one_zhouyanlin = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 1));
        
        auto param_zhouyanlin = new Operand(type_zhouyanlin->getParamsSe()[0]);
        auto newParam_zhouyanlin = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::ADD, newParam_zhouyanlin, param_zhouyanlin, zero_zhouyanlin, entry_zhouyanlin);
        new CmpInstruction(CmpInstruction::LE, cmpDst_zhouyanlin, newParam_zhouyanlin, two_zhouyanlin, entry_zhouyanlin);
        BasicBlock *trueBranch = new BasicBlock(func_zhouyanlin);
        BasicBlock *falseBranch = new BasicBlock(func_zhouyanlin);
        entry_zhouyanlin->addSucc(trueBranch);
        entry_zhouyanlin->addSucc(falseBranch);
        trueBranch->addPred(entry_zhouyanlin);
        falseBranch->addPred(entry_zhouyanlin);
        new CondBrInstruction(trueBranch, falseBranch, cmpDst_zhouyanlin, entry_zhouyanlin);
        new RetInstruction(one_zhouyanlin, trueBranch);
        auto dst_zhouyanlin = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::SUB, dst_zhouyanlin, newParam_zhouyanlin, one_zhouyanlin,
                              falseBranch);
        auto res1_zhouyanlin = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new CallInstruction(res1_zhouyanlin, func_zhouyanlin->getSymPtr(), {dst_zhouyanlin}, falseBranch);
        dst_zhouyanlin = new Operand(new TemporarySymbolEntry(TypeSystem::intType,
                                                   SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::SUB, dst_zhouyanlin, newParam_zhouyanlin, two_zhouyanlin,
                              falseBranch);
        auto res2_zhouyanlin = new Operand(new TemporarySymbolEntry(
            TypeSystem::intType, SymbolTable::getLabel()));
        new CallInstruction(res2_zhouyanlin, func_zhouyanlin->getSymPtr(), {dst_zhouyanlin}, falseBranch);
        dst_zhouyanlin = new Operand(new TemporarySymbolEntry(TypeSystem::intType,
                                                   SymbolTable::getLabel()));
        new BinaryInstruction(BinaryInstruction::ADD, dst_zhouyanlin, res1_zhouyanlin, res2_zhouyanlin,
                              falseBranch);
        new RetInstruction(dst_zhouyanlin, falseBranch);
        return;
    }

    builder->setInsertBB(entry_zhouyanlin);
    if (decl)
        decl->genCode();

    if (stmt)
        stmt->genCode();

    for (auto block_zhouyanlin = func_zhouyanlin->begin(); block_zhouyanlin != func_zhouyanlin->end(); block_zhouyanlin++)
    {
        //获取该块的最后一条指令
        Instruction *i = (*block_zhouyanlin)->begin();
        Instruction *last = (*block_zhouyanlin)->rbegin();
        while (i != last)
        {
            if (i->isCond() || i->isUncond())
            {
                (*block_zhouyanlin)->remove(i);
            }
            i = i->getNext();
        }
        if (last->isCond())
        {
            BasicBlock *truebranch, *falsebranch;
            truebranch =
                dynamic_cast<CondBrInstruction *>(last)->getTrueBranch();
            falsebranch =
                dynamic_cast<CondBrInstruction *>(last)->getFalseBranch();
            if (truebranch->empty())
            {
                new RetInstruction(nullptr, truebranch);
            }
            else if (falsebranch->empty())
            {
                new RetInstruction(nullptr, falsebranch);
            }
            (*block_zhouyanlin)->addSucc(truebranch);
            (*block_zhouyanlin)->addSucc(falsebranch);
            truebranch->addPred(*block_zhouyanlin);
            falsebranch->addPred(*block_zhouyanlin);
        }
        else if (last->isUncond())
        { //无条件跳转指令可获取跳转的目标块
            BasicBlock *dst =
                dynamic_cast<UncondBrInstruction *>(last)->getBranch();
            (*block_zhouyanlin)->addSucc(dst);
            dst->addPred(*block_zhouyanlin);
            if (dst->empty())
            {
                if (((FunctionType *)(se->getType()))->getRetType() ==
                    TypeSystem::intType)
                {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(
                                           TypeSystem::intType, 0)),
                                       dst);
                }
                else if (((FunctionType *)(se->getType()))->getRetType() ==
                         TypeSystem::floatType)
                {
                    new RetInstruction(new Operand(new ConstantSymbolEntry(
                                           TypeSystem::floatType, 0)),
                                       dst);
                }
                else if (((FunctionType *)(se->getType()))->getRetType() ==
                         TypeSystem::voidType)
                {
                    new RetInstruction(nullptr, dst);
                }
            }
        }
        //最后一条语句不是返回以及跳转
        else if (!last->isRet())
        {
            if (((FunctionType *)(se->getType()))->getRetType() ==
                TypeSystem::voidType)
            {
                new RetInstruction(nullptr, *block_zhouyanlin);
            }
        }
    }
    // 如果已经有ret了，删除后面的指令
    for (auto it_zhouyanlin = func_zhouyanlin->begin(); it_zhouyanlin != func_zhouyanlin->end(); it_zhouyanlin++)
    {
        auto block = *it_zhouyanlin;
        bool flag = false;
        for (auto i = block->begin(); i != block->end(); i = i->getNext())
        {
            if (flag)
            {
                block->remove(i);
                delete i;
                continue;
            }
            if (i->isRet())
                flag = true;
        }
        if (flag)
        {
            while (block->succ_begin() != block->succ_end())
            {
                auto b = *(block->succ_begin());
                block->removeSucc(b);
                b->removePred(block);
            }
        }
    }
    while (true)
    {
        bool flag = false;
        for (auto it_zhouyanlin = func_zhouyanlin->begin(); it_zhouyanlin != func_zhouyanlin->end(); it_zhouyanlin++)
        {
            auto block = *it_zhouyanlin;
            if (block == func_zhouyanlin->getEntry())
                continue;
            if (block->getNumOfPred() == 0)
            {
                delete block;
                flag = true;
                break;
            }
        }
        if (!flag)
            break;
    }
}


void IfElseStmt::genCode()
{
    Function *func_zhouyanlin;
    BasicBlock *then_bb_zhouyanlin, *else_bb_zhouyanlin, *end_bb_zhouyanlin;
    
    func_zhouyanlin = builder->getInsertBB()->getParent();
    then_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    else_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    end_bb_zhouyanlin= new BasicBlock(func_zhouyanlin);

    cond->genCode();
    
    backPatch(cond->trueList(), then_bb_zhouyanlin);
    backPatch(cond->falseList(), else_bb_zhouyanlin);

    

    builder->setInsertBB(then_bb_zhouyanlin);
    thenStmt->genCode();
    then_bb_zhouyanlin = builder->getInsertBB();
    new UncondBrInstruction(end_bb_zhouyanlin, then_bb_zhouyanlin);

    builder->setInsertBB(else_bb_zhouyanlin);
    elseStmt->genCode();
    else_bb_zhouyanlin = builder->getInsertBB();
    new UncondBrInstruction(end_bb_zhouyanlin, else_bb_zhouyanlin);

    builder->setInsertBB(end_bb_zhouyanlin);
}

void CompoundStmt::genCode()
{
    if (stmt)
        stmt->genCode();
}

void SeqNode::genCode()
{
    stmt1->genCode();
    stmt2->genCode();
}

bool DeclStmt::hasMemset = false;

void DeclStmt::genCode()
{
    IdentifierSymbolEntry *se_zhouyanlin =
        dynamic_cast<IdentifierSymbolEntry *>(id->getSymbolEntry());
    if (se_zhouyanlin->isGlobal())
    {
        Operand *addr_zhouyanlin;
        SymbolEntry *addr_se_zhouyanlin;
        addr_se_zhouyanlin = new IdentifierSymbolEntry(*se_zhouyanlin);
        addr_se_zhouyanlin->setType(new PointerType(se_zhouyanlin->getType()));
        addr_zhouyanlin = new Operand(addr_se_zhouyanlin);
        se_zhouyanlin->setAddr(addr_zhouyanlin);
        unit.insertGlobal(se_zhouyanlin);
        mUnit.insertGlobal(se_zhouyanlin);
    }
    else if (se_zhouyanlin->isLocal() || se_zhouyanlin->isParam())
    {
        Function *func_zhouyanlin = builder->getInsertBB()->getParent();
        BasicBlock *entry_zhouyanlin = func_zhouyanlin->getEntry();
        Instruction *alloca_zhouyanlin;
        Operand *addr_zhouyanlin;
        SymbolEntry *addr_se_zhouyanlin;
        Type *type_zhouyanlin;
        
        type_zhouyanlin = new PointerType(se_zhouyanlin->getType());                            
        addr_se_zhouyanlin= new TemporarySymbolEntry(type_zhouyanlin, SymbolTable::getLabel()); 
       
        addr_zhouyanlin = new Operand(addr_se_zhouyanlin);
        alloca_zhouyanlin = new AllocaInstruction(addr_zhouyanlin, se_zhouyanlin);
       
        entry_zhouyanlin->addAlloc(alloca_zhouyanlin); 
        Operand *temp_zhouyanlin = nullptr;
        if (se_zhouyanlin->isParam())
            temp_zhouyanlin = se_zhouyanlin->getAddr();
        se_zhouyanlin->setAddr(addr_zhouyanlin); 
        bool useMemset_zhouyanlin = false;
        if (expr && se_zhouyanlin->getType()->isArray())
        {
            int notZeroNum_zhouyanlin = se_zhouyanlin->getNotZeroNum();
            int size_zhouyanlin = se_zhouyanlin->getType()->getSize() / 8;
            int length_zhouyanlin = size_zhouyanlin / 4;
            if (notZeroNum_zhouyanlin < length_zhouyanlin / 2)
            {
                useMemset_zhouyanlin = true;
                auto int8PtrType = new PointerType(TypeSystem::int8Type);
                Operand *int8Ptr = new Operand(new TemporarySymbolEntry(
                    int8PtrType, SymbolTable::getLabel()));
                auto bb = builder->getInsertBB();
                new BitcastInstruction(int8Ptr, addr_zhouyanlin, bb);
                std::string name = "llvm.memset.p0.i32";
                SymbolEntry *funcSE_zhouyanlin;
                if (!hasMemset)
                {
                    hasMemset = true;
                    std::vector<Type *> vec;
                    vec.push_back(int8PtrType);
                    vec.push_back(TypeSystem::int8Type);
                    vec.push_back(TypeSystem::intType);
                    vec.push_back(TypeSystem::boolType);
                    std::vector<SymbolEntry *> vec1;
                    auto funcType =
                        new FunctionType(TypeSystem::voidType, vec, vec1);
                    SymbolTable *st = identifiers;
                    while (st->getPrev())
                        st = st->getPrev();
                    funcSE_zhouyanlin = new IdentifierSymbolEntry(funcType, name,
                                                       st->getLevel());
                    st->install(name, funcSE_zhouyanlin);
                    unit.insertDeclare(funcSE_zhouyanlin);
                }
                else
                {
                    funcSE_zhouyanlin = identifiers->lookup(name);
                    assert(funcSE_zhouyanlin);
                }
                std::vector<Operand *> params_zhouyanlin;
                params_zhouyanlin.push_back(int8Ptr);
                params_zhouyanlin.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::int8Type, 0)));
                params_zhouyanlin.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::intType, size_zhouyanlin)));
                params_zhouyanlin.push_back(new Operand(
                    new ConstantSymbolEntry(TypeSystem::boolType, 0)));
                new CallInstruction(nullptr, funcSE_zhouyanlin, params_zhouyanlin, bb);
            }
        }
        if (expr)
        {
            if (expr->isInitValueListExpr())
            {
                Operand *init_zhouyanlin = nullptr;
                int off_zhouyanlin = 0;
                BasicBlock *bb_zhouyanlin = builder->getInsertBB();
                ExprNode *temp_zhouyanlin = expr;
                std::stack<ExprNode *> stk_zhouyanlin;
                std::vector<int> idx_zhouyanlin;
                idx_zhouyanlin.push_back(0);
                while (temp_zhouyanlin)
                {
                    if (temp_zhouyanlin->isInitValueListExpr())
                    {
                        stk_zhouyanlin.push(temp_zhouyanlin);
                        idx_zhouyanlin.push_back(0);
                        temp_zhouyanlin = ((InitValueListExpr *)temp_zhouyanlin)->getExpr();
                        continue;
                    }
                    else
                    {
                        temp_zhouyanlin->genCode();
                        Type *type =
                            ((ArrayType *)(se_zhouyanlin->getType()))->getElementType();
                        Operand *tempSrc = addr_zhouyanlin;
                        Operand *tempDst;
                        Operand *index;
                        bool flag = true;
                        int i = 1;
                        while (true)
                        {
                            tempDst = new Operand(new TemporarySymbolEntry(
                                new PointerType(type),
                                SymbolTable::getLabel()));
                            index = (new Constant(new ConstantSymbolEntry(
                                         TypeSystem::intType, idx_zhouyanlin[i++])))
                                        ->getOperand();
                            auto gep =
                                new GepInstruction(tempDst, tempSrc, index, bb_zhouyanlin);
                            
                            gep->setInit(init_zhouyanlin, off_zhouyanlin);
                            if (flag)
                            {
                                gep->setFirst();
                                flag = false;
                            }
                            if (type == TypeSystem::intType ||
                                type == TypeSystem::constIntType ||
                                type == TypeSystem::floatType ||
                                type == TypeSystem::constFloatType)
                            {
                                gep->setLast();
                                break;
                            }
                            type = ((ArrayType *)type)->getElementType();
                            tempSrc = tempDst;
                        }
                        if (useMemset_zhouyanlin &&
                            temp_zhouyanlin->getOperand()->getEntry()->isConstant() &&
                            ((ConstantSymbolEntry *)(temp_zhouyanlin->getOperand()
                                                         ->getEntry()))
                                    ->getValue() == 0)
                        {
                            bb_zhouyanlin->deleteBack(idx_zhouyanlin.size() - 1);
                            off_zhouyanlin += 4;
                        }
                        else
                        {
                            init_zhouyanlin = tempDst;
                            off_zhouyanlin = 0;
                            new StoreInstruction(tempDst, temp_zhouyanlin->getOperand(),
                                                 bb_zhouyanlin);
                        }
                    }
                    while (true)
                    {
                        if (temp_zhouyanlin->getNext())
                        {
                            temp_zhouyanlin = (ExprNode *)(temp_zhouyanlin->getNext());
                            idx_zhouyanlin[idx_zhouyanlin.size() - 1]++;
                            break;
                        }
                        else
                        {
                            temp_zhouyanlin = stk_zhouyanlin.top();
                            stk_zhouyanlin.pop();
                            idx_zhouyanlin.pop_back();
                            if (stk_zhouyanlin.empty())
                                break;
                        }
                    }
                    if (stk_zhouyanlin.empty())
                        break;
                }
            }
            else
            {
                BasicBlock *bb = builder->getInsertBB();
                expr->genCode();
                Operand *src = expr->getOperand();
                new StoreInstruction(addr_zhouyanlin, src, bb);
            }
        }
        if (se_zhouyanlin->isParam())
        {
            BasicBlock *bb = builder->getInsertBB();
            new StoreInstruction(addr_zhouyanlin, temp_zhouyanlin, bb);
        }
    }
    if (this->getNext())
    {
        this->getNext()->genCode();
    }
}

void ReturnStmt::genCode()
{
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    Operand *src_zhouyanlin = nullptr;
    if (retValue)
    {
        retValue->genCode();
        src_zhouyanlin = retValue->getOperand();
    }
    new RetInstruction(src_zhouyanlin, bb_zhouyanlin);
}
void ExprStmt::genCode()
{
    expr->genCode();
}
void ContinueStmt::genCode()
{
    Function *func_zhouyanlin = builder->getInsertBB()->getParent();
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt *)whileStmt)->get_cond_bb(), bb_zhouyanlin);
    BasicBlock *continue_next_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    builder->setInsertBB(continue_next_bb_zhouyanlin);
}
void BreakStmt::genCode()
{
    Function *func_zhouyanlin = builder->getInsertBB()->getParent();
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    new UncondBrInstruction(((WhileStmt *)whileStmt)->get_end_bb(), bb_zhouyanlin);
    BasicBlock *break_next_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    builder->setInsertBB(break_next_bb_zhouyanlin);
}
void WhileStmt::genCode()
{
    Function *func_zhouyanlin;
    BasicBlock *cond_bb_zhouyanlin, *while_bb_zhouyanlin, *end_bb_zhouyanlin, *bb_zhouyanlin;
    bb_zhouyanlin = builder->getInsertBB();
    func_zhouyanlin = builder->getInsertBB()->getParent();
    cond_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    while_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);
    end_bb_zhouyanlin = new BasicBlock(func_zhouyanlin);

    this->cond_bb = cond_bb_zhouyanlin;
    this->end_bb = end_bb_zhouyanlin;

    new UncondBrInstruction(cond_bb_zhouyanlin, bb_zhouyanlin);

    builder->setInsertBB(cond_bb_zhouyanlin);
    cond->genCode();
    backPatch(cond->trueList(), while_bb_zhouyanlin);
    backPatch(cond->falseList(), end_bb_zhouyanlin);

    builder->setInsertBB(while_bb_zhouyanlin);
    stmt->genCode();
    ExprNode *cond1 = cond->copy();

    cond1->genCode();
    backPatch(cond1->trueList(), while_bb_zhouyanlin);
    backPatch(cond1->falseList(), end_bb_zhouyanlin);

    builder->setInsertBB(end_bb_zhouyanlin);
}
void BlankStmt::genCode()
{
    
}
void InitValueListExpr::genCode()
{
    
}
void CallExpr::genCode()
{
    std::vector<Operand *> operands_zhouyanlin;
    ExprNode *temp_zhouyanlin = param;
    while (temp_zhouyanlin)
    {
        temp_zhouyanlin->genCode();
        operands_zhouyanlin.push_back(temp_zhouyanlin->getOperand());
        temp_zhouyanlin = ((ExprNode *)temp_zhouyanlin->getNext());
    }
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    new CallInstruction(dst, symbolEntry, operands_zhouyanlin, bb_zhouyanlin);
}
void UnaryExpr::genCode()
{
    // Todo
    expr->genCode();
    if (op == NOT)
    {
        BasicBlock *bb_zhouyanlin = builder->getInsertBB();
        Operand *src_zhouyanlin = expr->getOperand();
        if (expr->getType()->isFloat() ||
            expr->getType()->isInt())
        { // FIXME: not i1
            Operand *temp_zhouyanlin = new Operand(new TemporarySymbolEntry(
                TypeSystem::boolType, SymbolTable::getLabel()));
            new CmpInstruction(
                CmpInstruction::NE, temp_zhouyanlin, src_zhouyanlin,
                new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)),
                bb_zhouyanlin);
            src_zhouyanlin = temp_zhouyanlin;
        }
        new XorInstruction(dst, src_zhouyanlin, bb_zhouyanlin);
    }
    else if (op == SUB)
    {
        Operand *src2_zhouyanlin;
        BasicBlock *bb_zhouyanlin = builder->getInsertBB();
        Operand *src1_zhouyanlin = new Operand(new ConstantSymbolEntry(dst->getType(), 0));
        if (expr->getType()->getSize() == 1)
        {
            src2_zhouyanlin = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(src2_zhouyanlin, expr->getOperand(), bb_zhouyanlin);
        }
        else
            src2_zhouyanlin = expr->getOperand();
        new BinaryInstruction(BinaryInstruction::SUB, dst, src1_zhouyanlin, src2_zhouyanlin, bb_zhouyanlin);
    }
}
void ExprNode::genCode()
{

}

ExprNode *ExprNode::alge_simple(int depth)
{
    int op_zhouyanlin;
    ExprNode *res_zhouyanlin = this;
    if (this->isBinaryExpr())
    {
        enum
        {
            ADD,
            SUB,
            MUL,
            DIV,
            MOD,
            AND,
            OR,
            LESS,
            LESSEQUAL,
            GREATER,
            GREATEREQUAL,
            EQUAL,
            NOTEQUAL
        };
        op_zhouyanlin= ((BinaryExpr *)this)->getOp();
        ExprNode *lhs_zhouyanlin = ((BinaryExpr *)this)->getLeft(),
                 *rhs = ((BinaryExpr *)this)->getRight();
        if (depth && lhs_zhouyanlin->isBinaryExpr())
        {
            lhs_zhouyanlin = lhs_zhouyanlin->alge_simple(depth - 1);
        }
        if (depth && rhs->isBinaryExpr())
        {
            rhs = rhs->alge_simple(depth - 1);
        }
        switch (op_zhouyanlin)
        {
        case ADD:
            if (lhs_zhouyanlin->getSymbolEntry()->isConstant() &&
                ((ConstantSymbolEntry *)(lhs_zhouyanlin->getSymbolEntry()))
                        ->getValue() == 0)
            {
                res_zhouyanlin = rhs;
            }
            else if (rhs->getSymbolEntry()->isConstant() &&
                     ((ConstantSymbolEntry *)(rhs->getSymbolEntry()))
                             ->getValue() == 0)
            {
                res_zhouyanlin = lhs_zhouyanlin;
            }
            else
            {
                SymbolEntry *se =
                    new TemporarySymbolEntry(type, SymbolTable::getLabel());
                res_zhouyanlin = new BinaryExpr(se, ADD, lhs_zhouyanlin, rhs);
            }
            break;
        case SUB:
            if (rhs->getSymbolEntry()->isConstant() &&
                ((ConstantSymbolEntry *)(rhs->getSymbolEntry()))
                        ->getValue() == 0)
            {
                res_zhouyanlin = lhs_zhouyanlin;
            }
            else
            {
                SymbolEntry *se =
                    new TemporarySymbolEntry(type, SymbolTable::getLabel());
                res_zhouyanlin = new BinaryExpr(se, SUB, lhs_zhouyanlin, rhs);
            }
            break;
        case MUL:
            if (lhs_zhouyanlin->getSymbolEntry()->isConstant())
            {
                if (((ConstantSymbolEntry *)(lhs_zhouyanlin->getSymbolEntry()))
                        ->getValue() == 0)
                {
                    SymbolEntry *se = new ConstantSymbolEntry(type, 0);
                    res_zhouyanlin = new Constant(se);
                }
                else if (((ConstantSymbolEntry *)(lhs_zhouyanlin->getSymbolEntry()))
                             ->getValue() == 1)
                {
                    res_zhouyanlin = rhs;
                }
            }
            else if (rhs->getSymbolEntry()->isConstant())
            {
                if (((ConstantSymbolEntry *)(rhs->getSymbolEntry()))
                        ->getValue() == 0)
                {
                    SymbolEntry *se = new ConstantSymbolEntry(type, 0);
                    res_zhouyanlin = new Constant(se);
                }
                else if (((ConstantSymbolEntry *)(rhs->getSymbolEntry()))
                             ->getValue() == 1)
                {
                    res_zhouyanlin = lhs_zhouyanlin;
                }
            }
            else
            {
                SymbolEntry *se =
                    new TemporarySymbolEntry(type, SymbolTable::getLabel());
                res_zhouyanlin = new BinaryExpr(se, MUL, lhs_zhouyanlin, rhs);
            }
            break;
        case DIV:
            if (rhs->getSymbolEntry()->isConstant() &&
                ((ConstantSymbolEntry *)(lhs_zhouyanlin->getSymbolEntry()))
                        ->getValue() == 1)
            {
                res_zhouyanlin = lhs_zhouyanlin;
            }
            else
            {
                SymbolEntry *se =
                    new TemporarySymbolEntry(type, SymbolTable::getLabel());
                res_zhouyanlin = new BinaryExpr(se, DIV, lhs_zhouyanlin, rhs);
            }
            break;
        }
    }
    return res_zhouyanlin;
}

ExprNode *ExprNode::const_fold()
{
    ExprNode *res_zhouyanlin = this;
    res_zhouyanlin = this->alge_simple(5); 
    bool flag_zhouyanlin = true;
    double fconst_zhouyanlin = res_zhouyanlin->fold_const(flag_zhouyanlin);
    if (flag_zhouyanlin)
    {
        if (dst->getType()->isInt())
            fconst_zhouyanlin = int(fconst_zhouyanlin);
        SymbolEntry *se_zhouyanlin = new ConstantSymbolEntry(type, fconst_zhouyanlin);
        res_zhouyanlin = new Constant(se_zhouyanlin);
    }
    return res_zhouyanlin;
}


void BinaryExpr::genCode()
{
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    Function *func_zhouyanlin = bb_zhouyanlin->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func_zhouyanlin); // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB); 
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if (op == OR)
    {
        // Todo
        BasicBlock *trueBB = new BasicBlock(func_zhouyanlin);
        expr1->genCode();
        backPatch(expr1->falseList(), trueBB);
        builder->setInsertBB(trueBB);
        expr2->genCode();
        true_list = merge(expr1->trueList(), expr2->trueList());
        false_list = expr2->falseList();
    }
    else if (op >= LESS && op <= NOTEQUAL)
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1_zhouyanlin = expr1->getOperand();
        Operand *src2_zhouyanlin = expr2->getOperand();
        if (src1_zhouyanlin->getType()->getSize() == 1)
        {
            Operand *dst_zhouyanlin = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst_zhouyanlin, src1_zhouyanlin, bb_zhouyanlin);
            src1_zhouyanlin = dst_zhouyanlin;
        }
        if (src2_zhouyanlin->getType()->getSize() == 1)
        {
            Operand *dst_zhouyanlin = new Operand(new TemporarySymbolEntry(
                TypeSystem::intType, SymbolTable::getLabel()));
            new ZextInstruction(dst_zhouyanlin, src2_zhouyanlin, bb_zhouyanlin);
            src2_zhouyanlin = dst_zhouyanlin;
        }
        int cmpopcode_zhouyanlin = -1;
        switch (op)
        {
        case LESS:
            cmpopcode_zhouyanlin = CmpInstruction::L;
            break;
        case LESSEQUAL:
            cmpopcode_zhouyanlin = CmpInstruction::LE;
            break;
        case GREATER:
            cmpopcode_zhouyanlin = CmpInstruction::G;
            break;
        case GREATEREQUAL:
            cmpopcode_zhouyanlin = CmpInstruction::GE;
            break;
        case EQUAL:
            cmpopcode_zhouyanlin = CmpInstruction::E;
            break;
        case NOTEQUAL:
            cmpopcode_zhouyanlin = CmpInstruction::NE;
            break;
        }
        new CmpInstruction(cmpopcode_zhouyanlin, dst, src1_zhouyanlin, src2_zhouyanlin, bb_zhouyanlin);
        //
        BasicBlock *truebb, *falsebb, *tempbb;
        //临时假块
        truebb = new BasicBlock(func_zhouyanlin);
        falsebb = new BasicBlock(func_zhouyanlin);
        tempbb = new BasicBlock(func_zhouyanlin);

        true_list.push_back(new CondBrInstruction(truebb, tempbb, dst, bb_zhouyanlin));

        false_list.push_back(new UncondBrInstruction(falsebb, tempbb));
    }
    else if (op >= ADD && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1_zhouyanlin = expr1->getOperand();
        Operand *src2_zhouyanlin = expr2->getOperand();
        int opcode_zhouyanlin = -1;
        switch (op)
        {
        case ADD:
            opcode_zhouyanlin = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode_zhouyanlin = BinaryInstruction::SUB;
            break;
        case MUL:
            opcode_zhouyanlin = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode_zhouyanlin = BinaryInstruction::DIV;
            break;
        case MOD:
            opcode_zhouyanlin = BinaryInstruction::MOD;
            break;
        }

        new BinaryInstruction(opcode_zhouyanlin, dst, src1_zhouyanlin, src2_zhouyanlin, bb_zhouyanlin);
    }
}



double ExprNode::fold_const(bool &flag_zhouyanlin)
{
    if (this->isBinaryExpr())
    {
        ExprNode *lhs = ((BinaryExpr *)this)->getLeft(),
                 *rhs = ((BinaryExpr *)this)->getRight();
        lhs->fold_const(flag_zhouyanlin);
        if (flag_zhouyanlin)
            rhs->fold_const(flag_zhouyanlin);
        if (flag_zhouyanlin)
        {
            return ((BinaryExpr *)this)->getValue();
        }
        else
            return 0;
    }
    else if (this->isUnaryExpr())
    {
        ExprNode *hs = ((UnaryExpr *)this)->getSubExpr();
        hs->fold_const(flag_zhouyanlin);
        if (flag_zhouyanlin)
        {
            return ((UnaryExpr *)this)->getValue();
        }
        else
            return 0;
    }
    else if (this->isExpr() && this->getSymbolEntry()->isConstant())
    {
        return ((ConstantSymbolEntry *)(this->getSymbolEntry()))->getValue();
    }
    flag_zhouyanlin = 0;
    return 0;
}
void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if (retValue != nullptr)
        retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

bool ContinueStmt::typeCheck(Type *retType)
{
    return false;
}
bool BreakStmt::typeCheck(Type *retType)
{
    return false;
}
bool WhileStmt::typeCheck(Type *retType)
{
    if (stmt)
        return stmt->typeCheck(retType);
    return false;
}
bool BlankStmt::typeCheck(Type *retType)
{
    return false;
}
bool InitValueListExpr::typeCheck(Type *retType)
{
    return false;
}
bool CallExpr::typeCheck(Type *retType)
{
    return false;
}
bool UnaryExpr::typeCheck(Type *retType)
{
    return false;
}

bool ExprStmt::typeCheck(Type *retType)
{
    return false;
}

void AssignStmt::genCode()
{
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    expr->genCode();
    Operand *addr_zhouyanlin = nullptr;
    if (lval->getOriginType()->isInt() || lval->getOriginType()->isFloat())
        addr_zhouyanlin = dynamic_cast<IdentifierSymbolEntry *>(lval->getSymbolEntry())
                   ->getAddr();
    else if (lval->getOriginType()->isArray())
    {
        ((Id *)lval)->setLeft();
        lval->genCode();
        addr_zhouyanlin = lval->getOperand();
    }
    Operand *src = expr->getOperand();

    new StoreInstruction(addr_zhouyanlin, src, bb_zhouyanlin);
}

bool Ast::typeCheck(Type *retType)
{
    if (root != nullptr)
        return root->typeCheck();
    return false;
}

bool FunctionDef::typeCheck(Type *retType)
{
    SymbolEntry *se_zhouyanlin = this->getSymbolEntry();
    Type *ret_zhouyanlin = ((FunctionType *)(se_zhouyanlin->getType()))->getRetType();
    StmtNode *stmt_zhouyanlin = this->stmt;
    if (stmt_zhouyanlin == nullptr)
    {
        if (ret_zhouyanlin != TypeSystem::voidType)
            fprintf(stderr, "non-void function does not return a value.\n");
        return false;
    }
    if (!stmt->typeCheck(ret_zhouyanlin))
    {
        return false;
    }
    return false;
}

bool BinaryExpr::typeCheck(Type *retType)
{
    return false;
}

bool Constant::typeCheck(Type *retType)
{
    return false;
}

bool Id::typeCheck(Type *retType)
{
    return false;
}

bool IfStmt::typeCheck(Type *retType_zhouyanlin)
{
    if (thenStmt)
        return thenStmt->typeCheck(retType_zhouyanlin);
    return false;
}


bool ReturnStmt::typeCheck(Type *retType_zhouyanlin)
{
    if (!retType_zhouyanlin)
    {
        fprintf(stderr, "expected unqualified-id\n");
        return true;
    }
    if (!retValue && !retType_zhouyanlin->isVoid())
    {
        fprintf(
            stderr,
            "return-statement with no value, in function returning \'%s\'\n",
            retType_zhouyanlin->toStr().c_str());
        return true;
    }
    if (retValue && retType_zhouyanlin->isVoid())
    {
        fprintf(
            stderr,
            "return-statement with a value, in function returning \'void\'\n");
        return true;
    }

    if (!retType_zhouyanlin->isVoid())
    {
        Type *type = retValue->getType();
        if (type != retType_zhouyanlin)
        {
            fprintf(
                stderr,
                "cannot initialize return object of type \'%s\' with an rvalue "
                "of type \'%s\'\n",
                retType_zhouyanlin->toStr().c_str(), type->toStr().c_str());
            return true;
        }
    }
    return false;
}

bool AssignStmt::typeCheck(Type *retType)
{
    return false;
}


CallExpr::CallExpr(SymbolEntry* se, ExprNode* param)
    : ExprNode(se, CALLEXPR), param(param) {
    dst = nullptr;
    SymbolEntry* s = se;
    int paramCnt = 0;
    ExprNode* temp = param;
    while (temp) {
        paramCnt++;
        temp = (ExprNode*)(temp->getNext());
    }
    while (s) {
        Type* type = s->getType();
        std::vector<Type*> params = ((FunctionType*)type)->getParamsType();
        if ((long unsigned int)paramCnt == params.size()) {
            this->symbolEntry = s;
            break;
        }
        s = s->getNext();
    }
    if (symbolEntry) {
        Type* type = symbolEntry->getType();
        this->type = ((FunctionType*)type)->getRetType();
        if (this->type != TypeSystem::voidType) {
            SymbolEntry* se =
                new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se);
        }
        std::vector<Type*> params = ((FunctionType*)type)->getParamsType();
        ExprNode* temp = param;
        ExprNode* temp1 = nullptr;

        for (auto it = params.begin(); it != params.end(); it++) {
            if (temp == nullptr) {
                fprintf(stderr, "too few arguments to function %s %s\n",
                        symbolEntry->toStr().c_str(), type->toStr().c_str());
                break;
            } else if ((*it)->getKind() != temp->getType()->getKind()) {
                if (((*it)->isFloat() && temp->getType()->isInt()) ||
                    ((*it)->isInt() && temp->getType()->isFloat())) {
                    ImplicitCastExpr* implicitCastExpr =
                        new ImplicitCastExpr(temp, (*it));
                    implicitCastExpr->setNext((ExprNode*)(temp->getNext()));

                    if (it != params.begin()) {
                        temp1->setAdjNext(implicitCastExpr);
                        temp = implicitCastExpr;
                    } else {
                        temp = implicitCastExpr;
                        this->param = implicitCastExpr;
                    }

                } else {
                    fprintf(stderr, "parameter's type %s can't convert to %s\n",
                            temp->getType()->toStr().c_str(),
                            (*it)->toStr().c_str());
                }
            }
            temp1 = temp;
            temp = (ExprNode*)(temp->getNext());
        }
        if (temp != nullptr) {
            fprintf(stderr, "too many arguments to function %s %s\n",
                    symbolEntry->toStr().c_str(), type->toStr().c_str());
        }
    }
    if (((IdentifierSymbolEntry*)se)->isSysy()) {
        unit.insertDeclare(se);
    }
}


CallExpr::CallExpr(const CallExpr &c) : ExprNode(c)
{
    if (c.param)
        param = c.param->copy();
    symbolEntry = c.symbolEntry;
    if (symbolEntry)
    {
        Type *type_zhouyanlin = symbolEntry->getType();
        this->type = ((FunctionType *)type_zhouyanlin)->getRetType();
        if (this->type != TypeSystem::voidType)
        {
            SymbolEntry *se_zhouyanlin =
                new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se_zhouyanlin);
        }
    }
}

AssignStmt::AssignStmt(ExprNode *lval, ExprNode *expr)
    : lval(lval), expr(expr)
{
    Type *type_zhouyanlin = ((Id *)lval)->getType();
    Type *exprType_zhouyanlin = expr->getType();
    SymbolEntry *se = lval->getSymbolEntry();
    bool flag = true;
    if (type_zhouyanlin->isInt())
    {
        if (((IntType *)type_zhouyanlin)->isConst())
        {
            fprintf(stderr,
                    "cannot assign to variable \'%s\' with const-qualified "
                    "type \'%s\'\n",
                    ((IdentifierSymbolEntry *)se)->toStr().c_str(),
                    type_zhouyanlin->toStr().c_str());
            flag = false;
        }
    }
    else if (type_zhouyanlin->isFloat())
    {
        if (((FloatType *)type_zhouyanlin)->isConst())
        {
            fprintf(stderr,
                    "cannot assign to variable \'%s\' with const-qualified "
                    "type \'%s\'\n",
                    ((IdentifierSymbolEntry *)se)->toStr().c_str(),
                    type_zhouyanlin->toStr().c_str());
            flag = false;
        }
    }
    else if (type_zhouyanlin->isArray())
    {
        fprintf(stderr, "array type \'%s\' is not assignable\n",
                type_zhouyanlin->toStr().c_str());
        flag = false;
    }
    if (flag)
    {
        if (type_zhouyanlin != exprType_zhouyanlin)
        { // comparing ptr, how about const?
            if (type_zhouyanlin->isInt() && exprType_zhouyanlin->isFloat())
            {
                ImplicitCastExpr *temp =
                    new ImplicitCastExpr(expr, TypeSystem::intType);
                this->expr = temp;
            }
            else if (type_zhouyanlin->isFloat() && exprType_zhouyanlin->isInt())
            {
                ImplicitCastExpr *temp =
                    new ImplicitCastExpr(expr, TypeSystem::floatType);
                this->expr = temp;
            }
            else
            {
                fprintf(stderr,
                        "cannot initialize a variable of type \'%s\' with an "
                        "rvalue "
                        "of type \'%s\'\n",
                        type_zhouyanlin->toStr().c_str(), exprType_zhouyanlin->toStr().c_str());
            }
        }
    }
}

Type *Id::getType()
{
    SymbolEntry *se_zhouyanlin = this->getSymbolEntry();
    if (!se_zhouyanlin)
        return TypeSystem::voidType;
    Type *type_zhouyanlin = se_zhouyanlin->getType();
    if (!arrIdx)
        return type_zhouyanlin;
    else if (!type_zhouyanlin->isArray())
    {
        fprintf(stderr, "subscripted value is not an array\n");
        return TypeSystem::voidType;
    }
    else
    {
        ArrayType *temp1_zhouyanlin = (ArrayType *)type_zhouyanlin; // whole type of array
        ExprNode *temp2_zhouyanlin = arrIdx;             // current index
        while (!temp1_zhouyanlin->getElementType()->isInt() &&
               !temp1_zhouyanlin->getElementType()->isFloat())
        {
            if (!temp2_zhouyanlin)
            {
                return temp1_zhouyanlin; // return array
            }
            temp2_zhouyanlin = (ExprNode *)(temp2_zhouyanlin->getNext());
            temp1_zhouyanlin = (ArrayType *)(temp1_zhouyanlin->getElementType());
        }
        if (!temp2_zhouyanlin)
        {
            fprintf(stderr, "subscripted value is not an array\n");
            return temp1_zhouyanlin;
        }
        else if (temp2_zhouyanlin->getNext())
        {
            fprintf(stderr, "subscripted value is not an array\n");
            return TypeSystem::voidType;
        }
        
        return temp1_zhouyanlin->getElementType();
    }
}

void ExprNode::output(int level)
{
    std::string name_zhouyanlin, type_zhouyanlin;
    name_zhouyanlin = symbolEntry->toStr();
    type_zhouyanlin = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cconst string\ttype:%s\t%s\n", level, ' ', type_zhouyanlin.c_str(),
            name_zhouyanlin.c_str());
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if (root != nullptr)
        root->output(4);
}

bool IfElseStmt::typeCheck(Type *retType_zhouyanlin)
{
    bool flag1_zhouyanlin = false, flag2_zhouyanlin = false;
    if (thenStmt)
        flag1_zhouyanlin = thenStmt->typeCheck(retType_zhouyanlin);
    if (elseStmt)
        flag2_zhouyanlin = elseStmt->typeCheck(retType_zhouyanlin);
    return flag1_zhouyanlin || flag2_zhouyanlin;
}

bool CompoundStmt::typeCheck(Type *retType_zhouyanlin)
{
    if (stmt)
        return stmt->typeCheck(retType_zhouyanlin);
    return false;
}

bool SeqNode::typeCheck(Type *retType_zhouyanlin)
{
    bool flag1_zhouyanlin = false, flag2_zhouyanlin = false;
    if (stmt1)
        flag1_zhouyanlin = stmt1->typeCheck(retType_zhouyanlin);
    if (stmt2)
        flag2_zhouyanlin = stmt2->typeCheck(retType_zhouyanlin);
    return flag1_zhouyanlin || flag2_zhouyanlin;
}

bool DeclStmt::typeCheck(Type *retType)
{
    return false;
}


void BinaryExpr::output(int level)
{
    std::string op_str_zhouyanlin;
    switch (op)
    {
    case ADD:
        op_str_zhouyanlin = "add";
        break;
    case SUB:
        op_str_zhouyanlin = "sub";
        break;
    case MUL:
        op_str_zhouyanlin = "mul";
        break;
    case DIV:
        op_str_zhouyanlin = "div";
        break;
    case MOD:
        op_str_zhouyanlin = "mod";
        break;
    case AND:
        op_str_zhouyanlin = "and";
        break;
    case OR:
        op_str_zhouyanlin = "or";
        break;
    case LESS:
        op_str_zhouyanlin = "less";
        break;
    case LESSEQUAL:
        op_str_zhouyanlin = "lessequal";
        break;
    case GREATER:
        op_str_zhouyanlin = "greater";
        break;
    case GREATEREQUAL:
        op_str_zhouyanlin = "greaterequal";
        break;
    case EQUAL:
        op_str_zhouyanlin = "equal";
        break;
    case NOTEQUAL:
        op_str_zhouyanlin = "notequal";
        break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str_zhouyanlin.c_str(), type->toStr().c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

UnaryExpr::UnaryExpr(SymbolEntry *se, int op, ExprNode *expr)
    : ExprNode(se, UNARYEXPR), op(op), expr(expr)
{
    std::string op_str_zhouyanlin = op == UnaryExpr::NOT ? "!" : "-";
    if (expr->getType()->isVoid())
    {
        fprintf(stderr,
                "invalid operand of type \'void\' to unary \'opeartor%s\'\n",
                op_str_zhouyanlin.c_str());
    }
    if (op == UnaryExpr::NOT)
    {
        type = TypeSystem::intType;
        dst = new Operand(se);
        if (expr->isUnaryExpr())
        {
            UnaryExpr *ue = (UnaryExpr *)expr;
            if (ue->getOp() == UnaryExpr::NOT)
            {
                if (ue->getType() == TypeSystem::intType)
                    ue->setType(TypeSystem::boolType); 
                
            }
        }

    }
    else if (op == UnaryExpr::SUB)
    {
        type = expr->getType();
        dst = new Operand(se);
        if (expr->isUnaryExpr())
        {
            UnaryExpr *ue = (UnaryExpr *)expr;
            if (ue->getOp() == UnaryExpr::NOT)
                if (ue->getType() == TypeSystem::intType)
                    ue->setType(TypeSystem::boolType);
        }
    }
};

UnaryExpr::UnaryExpr(const UnaryExpr &u) : ExprNode(u)
{
    op = u.op;
    expr = u.expr->copy();
    symbolEntry = new TemporarySymbolEntry(u.symbolEntry->getType(),
                                           SymbolTable::getLabel());
    dst = new Operand(symbolEntry);
}

void UnaryExpr::output(int level)
{
    std::string op_str_zhouyanlin;
    switch (op)
    {
    case NOT:
        op_str_zhouyanlin = "not";
        break;
    case SUB:
        op_str_zhouyanlin = "minus";
        break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str_zhouyanlin.c_str(), type->toStr().c_str());
    expr->output(level + 4);
}

double UnaryExpr::getValue()
{
    double value_zhouyanlin = 0;
    switch (op)
    {
    case NOT:
        value_zhouyanlin = !(expr->getValue());
        break;
    case SUB:
        value_zhouyanlin = -(expr->getValue());
        break;
    }
    return value_zhouyanlin;
}

void CallExpr::output(int level)
{
    std::string name_zhouyanlin, type_zhouyanlin;
    int scope;
    if (symbolEntry)
    {
        name_zhouyanlin = symbolEntry->toStr();
        type_zhouyanlin = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
        fprintf(yyout, "%*cCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n",
                level, ' ', name_zhouyanlin.c_str(), scope, type_zhouyanlin.c_str());
        Node *temp = param;
        while (temp)
        {
            temp->output(level + 4);
            temp = temp->getNext();
        }
    }
}

void Constant::output(int level)
{
    std::string type_zhouyanlin, value_zhouyanlin;
    type_zhouyanlin = symbolEntry->getType()->toStr();
    value_zhouyanlin = symbolEntry->toStr();
    if (symbolEntry->getType()->isInt())
    {
        fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value_zhouyanlin.c_str(), type_zhouyanlin.c_str());
    }
    else if (symbolEntry->getType()->isFloat())
    {
        fprintf(yyout, "%*cFloatLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value_zhouyanlin.c_str(), type_zhouyanlin.c_str());
    }
}

double Constant::getValue()
{
    return ((ConstantSymbolEntry *)symbolEntry)->getValue();
}

double Id::getValue()
{
    return ((IdentifierSymbolEntry *)symbolEntry)->getValue();
}

void Id::output(int level)
{
    std::string name_zhouyanlin, type_zhouyanlin;
    int scope;
    if (symbolEntry)
    {
        name_zhouyanlin = symbolEntry->toStr();
        type_zhouyanlin = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
        fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
                name_zhouyanlin.c_str(), scope, type_zhouyanlin.c_str());
        if (arrIdx)
        {
            ExprNode *temp = arrIdx;
            int i = 0;
            while (temp)
            {
                temp->output(level + 4 + 4 * i++);
                temp = (ExprNode *)(temp->getNext());
            }
        }
    }
}


double BinaryExpr::getValue()
{
    double value = 0;
    if (type->isFloat())
    {
        float val_zhouyanlin;
        float val1_zhouyanlin = (float)(expr1->getValue());
        float val2_zhouyanlin = (float)(expr2->getValue());

        switch (op)
        {
        case ADD:
            val_zhouyanlin = val1_zhouyanlin + val2_zhouyanlin;
            break;
        case SUB:
            val_zhouyanlin = val1_zhouyanlin - val2_zhouyanlin;
            break;
        case MUL:
            val_zhouyanlin = val1_zhouyanlin * val2_zhouyanlin;
            break;
        case DIV:
            if (val2_zhouyanlin != 0)
            {
                val_zhouyanlin = val1_zhouyanlin / val2_zhouyanlin;
                if (dst->getType()->isInt())
                {
                    val_zhouyanlin = (int)(val_zhouyanlin);
                }
            }
            break;
        case MOD:
            val_zhouyanlin = (int)(val1_zhouyanlin) % (int)(val2_zhouyanlin);
            break;
        case AND: // FIXME
            val_zhouyanlin = val1_zhouyanlin && val2_zhouyanlin;
            break;
        case OR: // FIXME
            val_zhouyanlin = val1_zhouyanlin || val2_zhouyanlin;
            break;
        case LESS:
            val_zhouyanlin = val1_zhouyanlin < val2_zhouyanlin;
            break;
        case LESSEQUAL:
            val_zhouyanlin = val1_zhouyanlin <= val2_zhouyanlin;
            break;
        case GREATER:
            val_zhouyanlin = val1_zhouyanlin > val2_zhouyanlin;
            break;
        case GREATEREQUAL:
            val_zhouyanlin = val1_zhouyanlin >= val2_zhouyanlin;
            break;
        case EQUAL:
            val_zhouyanlin = val1_zhouyanlin == val2_zhouyanlin;
            break;
        case NOTEQUAL:
            val_zhouyanlin = val1_zhouyanlin != val2_zhouyanlin;
            break;
        }
        value = (double)val_zhouyanlin;
        return value;
    }

    switch (op)
    {
    case ADD:
        value = expr1->getValue() + expr2->getValue();
        break;
    case SUB:
        value = expr1->getValue() - expr2->getValue();
        break;
    case MUL:
        value = expr1->getValue() * expr2->getValue();
        break;
    case DIV:
        if (expr2->getValue())
            value = expr1->getValue() / expr2->getValue();
        break;
    case MOD:
        value = (int)(expr1->getValue()) % (int)(expr2->getValue());
        break;
    case AND:
        value = expr1->getValue() && expr2->getValue();
        break;
    case OR:
        value = expr1->getValue() || expr2->getValue();
        break;
    case LESS:
        value = expr1->getValue() < expr2->getValue();
        break;
    case LESSEQUAL:
        value = expr1->getValue() <= expr2->getValue();
        break;
    case GREATER:
        value = expr1->getValue() > expr2->getValue();
        break;
    case GREATEREQUAL:
        value = expr1->getValue() >= expr2->getValue();
        break;
    case EQUAL:
        value = expr1->getValue() == expr2->getValue();
        break;
    case NOTEQUAL:
        value = expr1->getValue() != expr2->getValue();
        break;
    } // double should be ok here, probably...
    return value;
}



void InitValueListExpr::output(int level)
{
    std::string type_zhouyanlin;
    if (symbolEntry->getType())
        type_zhouyanlin = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cInitValueListExpr\ttype: %s\n", level, ' ',
            type_zhouyanlin.c_str());
    Node *temp_zhouyanlin = expr;
    while (temp_zhouyanlin)
    {
        temp_zhouyanlin->output(level + 4);
        temp_zhouyanlin = temp_zhouyanlin->getNext();
    }
}

void InitValueListExpr::addExpr(ExprNode *expr)
{
    if (this->expr == nullptr)
    {
        assert(childCnt == 0);
        childCnt++;
        this->expr = expr;
    }
    else
    {
        childCnt++;
        this->expr->setNext(expr);
    }
}

bool InitValueListExpr::isFull()
{
    ArrayType *type_zhouyanlin = (ArrayType *)(this->symbolEntry->getType());
    return childCnt == type_zhouyanlin->getLength();
}

void InitValueListExpr::fill()
{
    if (allZero)
    {
        return;
    }
    Type *type_zhouyanlin = ((ArrayType *)(this->getType()))->getElementType();
    if (type_zhouyanlin->isArray())
    {
        while (!isFull())
        {
            this->addExpr(new InitValueListExpr(new ConstantSymbolEntry(type_zhouyanlin)));
        }
        ExprNode *temp_zhouyanlin = expr;
        while (temp_zhouyanlin)
        {
            ((InitValueListExpr *)temp_zhouyanlin)->fill();
            temp_zhouyanlin = (ExprNode *)(temp_zhouyanlin->getNext());
        }
    }
    if (type_zhouyanlin->isInt() || type_zhouyanlin->isFloat())
    {
        while (!isFull())
        {
            this->addExpr(new Constant(new ConstantSymbolEntry(type_zhouyanlin, 0)));
        }
        return;
    }
}

void ImplicitCastExpr::output(int level)
{
    fprintf(yyout, "%*cImplicitCastExpr\ttype: %s to %s\n", level, ' ',
            expr->getType()->toStr().c_str(), type->toStr().c_str());
    this->expr->output(level + 4);
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if (stmt)
        stmt->output(level + 4);
}

void SeqNode::output(int level)
{

    stmt1->output(level);
    stmt2->output(level);
}

DeclStmt::DeclStmt(Id *id, ExprNode *expr) : id(id)
{
    if (expr)
    {
        this->expr = expr;
        if (expr->isInitValueListExpr())
        {
            ((InitValueListExpr *)(this->expr))->fill();
        }
        else
        {
            Type *idType_zhouyanlin = id->getType();
            Type *exprType_zhouyanlin = expr->getType();
            if ((idType_zhouyanlin->isFloat() && exprType_zhouyanlin->isInt()) ||
                (idType_zhouyanlin->isInt() && exprType_zhouyanlin->isFloat()))
            {
                ImplicitCastExpr *temp_zhouyanlin = new ImplicitCastExpr(expr, idType_zhouyanlin);
                this->expr = temp_zhouyanlin;
            }
        }
    }
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
    if (expr)
        expr->output(level + 4);
    if (this->getNext())
    {
        this->getNext()->output(level);
    }
}

void BlankStmt::output(int level)
{
    fprintf(yyout, "%*cBlankStmt\n", level, ' ');
}

IfStmt::IfStmt(ExprNode *cond, StmtNode *thenStmt)
    : cond(cond), thenStmt(thenStmt)
{
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32)
    {
        ImplicitCastExpr *temp = new ImplicitCastExpr(cond);
        this->cond = temp;
    }
    else if (cond->getType()->isFloat())
    {
        SymbolEntry *zero_zhouyanlin = new ConstantSymbolEntry(TypeSystem::floatType, 0);
        SymbolEntry *temp_zhouyanlin = new TemporarySymbolEntry(TypeSystem::boolType,
                                                     SymbolTable::getLabel());
        BinaryExpr *cmpZero_zhouyanlin = new BinaryExpr(temp_zhouyanlin, BinaryExpr::NOTEQUAL, cond,
                                             new Constant(zero_zhouyanlin));
        this->cond = cmpZero_zhouyanlin;
    }
};

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    stmt->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name_zhouyanlin, type_zhouyanlin;
    name_zhouyanlin = se->toStr();
    type_zhouyanlin = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine\tfunction name: %s\ttype: %s\n", level,
            ' ', name_zhouyanlin.c_str(), type_zhouyanlin.c_str());
    if (decl)
    {
        decl->output(level + 4);
    }
    stmt->output(level + 4);
}

void ImplicitCastExpr::genCode()
{
    expr->genCode();
    BasicBlock *bb_zhouyanlin = builder->getInsertBB();
    if (type == TypeSystem::boolType)
    { 
        Function *func = bb_zhouyanlin->getParent();
        BasicBlock *trueBB = new BasicBlock(func);
        BasicBlock *tempbb = new BasicBlock(func);
        BasicBlock *falseBB = new BasicBlock(func);

        new CmpInstruction(
            CmpInstruction::NE, this->dst, this->expr->getOperand(),
            new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), bb_zhouyanlin);
        this->trueList().push_back(
            new CondBrInstruction(trueBB, tempbb, this->dst, bb_zhouyanlin));
        this->falseList().push_back(new UncondBrInstruction(falseBB, tempbb));
    }
    else if (type->isInt())
    {
        new FptosiInstruction(dst, this->expr->getOperand(), bb_zhouyanlin);
    }
    else if (type->isFloat())
    {
        new SitofpInstruction(dst, this->expr->getOperand(), bb_zhouyanlin);
    }
    else
    {
        assert(false);
    }
}

double ImplicitCastExpr::getValue()
{
    if (type == TypeSystem::boolType)
    {
        return -1;
    }
    double temp_zhouyanlin = expr->getValue();

    Type *srcType = expr->getType();

    if (type->isInt() && srcType->isFloat())
    {
        float temp1_zhouyanlin = (float)temp_zhouyanlin;
        int res_zhouyanlin = (int)temp1_zhouyanlin;
        temp_zhouyanlin = (double)res_zhouyanlin;
        return temp_zhouyanlin;
    }
    else if (type->isFloat() && srcType->isInt())
    {
        int temp1_zhouyanlin = (int)temp_zhouyanlin;
        float res_zhouyanlin = (float)temp1_zhouyanlin;
        temp_zhouyanlin = (double)res_zhouyanlin;
        return temp_zhouyanlin;
    }
    else
    {
        return -1;
    }
}

ExprNode *ExprNode::copy()
{
    ExprNode *ret_zhouyanlin;
    switch (kind)
    {
    case BINARYEXPR:
        ret_zhouyanlin = new BinaryExpr(*(BinaryExpr *)this);
        break;
    case UNARYEXPR:
        ret_zhouyanlin = new UnaryExpr(*(UnaryExpr *)this);
        break;
    case CALLEXPR:
        ret_zhouyanlin = new CallExpr(*(CallExpr *)this);
        break;
    case CONSTANT:
        ret_zhouyanlin = new Constant(*(Constant *)this);
        break;
    case ID:
        ret_zhouyanlin = new Id(*(Id *)this);
        break;
    case IMPLICITCASTEXPR:
        ret_zhouyanlin = new ImplicitCastExpr(*(ImplicitCastExpr *)this);
        break;
    }
    ExprNode *temp = this;
    if (temp->getNext())
    {
        ret_zhouyanlin->cleanNext();
        temp = (ExprNode *)(temp->getNext());
        ret_zhouyanlin->setNext(temp->copy());
    }
    return ret_zhouyanlin;
}