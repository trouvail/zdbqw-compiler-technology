#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond()){
            bb->addPred(dynamic_cast<CondBrInstruction*>(inst)->getParent());
            dynamic_cast<CondBrInstruction*>(inst)->getParent()->addSucc(bb);
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        }
        else if(inst->isUncond()){
            bb->addPred(dynamic_cast<CondBrInstruction*>(inst)->getParent());
            dynamic_cast<CondBrInstruction*>(inst)->getParent()->addSucc(bb);
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
        }
    }
}

void Node::backPatchFalse(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
        {
            bb->addPred(dynamic_cast<CondBrInstruction*>(inst)->getParent());
            dynamic_cast<CondBrInstruction*>(inst)->getParent()->addSucc(bb);
            dynamic_cast<CondBrInstruction*>(inst)->setFalseBranch(bb);
        }
        else if(inst->isUncond())
        {
            bb->addPred(dynamic_cast<CondBrInstruction*>(inst)->getParent());
            dynamic_cast<CondBrInstruction*>(inst)->getParent()->addSucc(bb);
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
        }
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    fprintf(yyout, "declare i32 @getint()\n");
    fprintf(yyout,"declare void @putint(i32)\n");
    fprintf(yyout,"declare i32 @getch()\n");
    fprintf(yyout,"declare void @putch(i32)\n");
    root->genCode();
}

void FuncParam::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void FuncParamList::genCode()
{
    for(int i=0;i<FPs.size();i++){
        // just copied from DeclStmt, which is different from Mr.Zhu
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(FPs[i]->getSymPtr());
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        StoreInstruction *store;
        Operand *addr,*addr2;
        SymbolEntry *addr_se,*addr_se2;
        Type *type,*type2;
        type = new PointerType(se->getType());
        type2 = new IntType(32);
        // I don't know why where exists counter ++
        // SymbolTable :: counter++;
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        addr_se2 = new TemporarySymbolEntry(type2, SymbolTable::getLabel());
        addr2 = new Operand(addr_se2);
        alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
        entry->insertBack(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
        store = new StoreInstruction(addr,addr2);
        entry->insertBack(store);
        se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        func->params.push_back(addr);
    }
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    //printf("I am genCode of FunctionDef the name: ");
    //printf("%s \n",(dynamic_cast<IdentifierSymbolEntry*>(se))->name.c_str());
    // printf(se->name);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    if(FPlist != nullptr){
        FPlist -> genCode();
    }
    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
//    for (auto block = func->begin(); block != func->end(); block++) {
//         //获取该块的最后一条指令
//         Instruction* i = (*block)->begin();
//         Instruction* last = (*block)->rbegin();
//         while (i != last) {
//             if (i->isCond() || i->isUncond()) {
//                 (*block)->remove(i);
//             }
//             i = i->getNext();
//         }
//         if (last->isCond()) {
//             BasicBlock *truebranch, *falsebranch;
//             truebranch =
//                 dynamic_cast<CondBrInstruction*>(last)->getTrueBranch();
//             falsebranch =
//                 dynamic_cast<CondBrInstruction*>(last)->getFalseBranch();
//             if (truebranch->empty()) {
//                 new RetInstruction(nullptr, truebranch);

//             } else if (falsebranch->empty()) {
//                 new RetInstruction(nullptr, falsebranch);
//             }
//             (*block)->addSucc(truebranch);
//             (*block)->addSucc(falsebranch);
//             truebranch->addPred(*block);
//             falsebranch->addPred(*block);
//         } else if (last->isUncond())  //无条件跳转指令可获取跳转的目标块
//         {
//             BasicBlock* dst =
//                 dynamic_cast<UncondBrInstruction*>(last)->getBranch();
//             (*block)->addSucc(dst);
//             dst->addPred(*block);
//             if (dst->empty()) {
//                 if (((FunctionType*)(se->getType()))->getRetType() ==
//                     TypeSystem::intType)
//                     new RetInstruction(new Operand(new ConstantSymbolEntry(
//                                            TypeSystem::intType, 0)),
//                                        dst);
//                 else if (((FunctionType*)(se->getType()))->getRetType() ==
//                          TypeSystem::voidType)
//                     new RetInstruction(nullptr, dst);
//             }

//         }
//         //最后一条语句不是返回以及跳转
//         else if (!last->isRet()) {
//             if (((FunctionType*)(se->getType()))->getRetType() ==
//                 TypeSystem::voidType) {
//                 new RetInstruction(nullptr, *block);
//             }
//         }
//     }
}

void EmptyStmt::genCode()
{

}

void FuncCallParamList::genCode()
{
    // Maybe I will do later
}

void FuncExpr::genCode()
{
    std::vector<Operand*> params;
    if(FCPlist != nullptr){
        for(int i=0;i<FCPlist->FCPs.size();i++){
            if(FCPlist->FCPs[i] != nullptr){
                FCPlist->FCPs[i] -> genCode();
            }
            params.push_back(FCPlist->FCPs[i]->getOperand());
        }
    }
    // Be similar to DeclStmt
    BasicBlock *entry = builder -> getInsertBB();
    FunctionCallInstruction *funcCall;
    Operand *addr;
    SymbolEntry *addr_se;
    Type *type;
    type = new IntType(32);
    addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
    addr = new Operand(addr_se);
    funcCall = new FunctionCallInstruction(addr, symbolEntry, params);                   // allocate space for local id in function stack.
    entry->insertBack(funcCall);                                                         // allocate instructions should be inserted into the begin of the entry block.
    // se->setAddr(addr);
}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        trueBB -> addPred(bb);
        bb -> addSucc(trueBB);
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
        // dst -> getType() -> kind = BOOL;
    }
    else if(op == OR)
    {
        // Todo
        BasicBlock *falseBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatchFalse(expr1->falseList(), falseBB);
        builder->setInsertBB(falseBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());
        // dst -> getType() -> kind = BOOL;
    }
    else if(op >= LESS && op <= NOTEQUAL)
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case GREATER:
            opcode = CmpInstruction::G;
            break;
        case LESSEQUAL:
            opcode = CmpInstruction::LE;
            break;
        case GREATEREQUAL:
            opcode = CmpInstruction::GE;
            break;
        case EQUAL:
            opcode = CmpInstruction::E;
            break;
        case NOTEQUAL:
            opcode = CmpInstruction::NE;
            break;
        
        }
        new CmpInstruction(opcode, dst, src1, src2, bb);

        true_list = merge(expr1->true_list,expr2->true_list);
        false_list = merge(expr1->false_list,expr2->false_list);
        Instruction *temp = new CondBrInstruction(nullptr,nullptr,dst,bb);
        true_list.push_back(temp);
        false_list.push_back(temp);
    }
    else if(op >= ADD && op <= XOR)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void UnaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if(op == NEGATION)
    {
        Operand *src2 = expr->getOperand();
        // I am confused, because it the const should be set to 1, while Mr.Zhu set 0.
        Operand* src1 = (new Constant(new ConstantSymbolEntry(TypeSystem::intType, 1))) -> getOperand();
        expr -> genCode();
        new XorInstruction(dst,src2,bb);
    }
    else if(op >= ADD && op <= SUB)
    {
        expr -> genCode();
        Operand *src2 = expr->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        }
        Operand* src1 = (new Constant(new ConstantSymbolEntry(TypeSystem::intType, 0))) -> getOperand();
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void ConstId::genCode()
{
    // done in the ConstDeclStmt
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    then_bb -> addPred(builder->getInsertBB());//设置其前驱
    builder -> getInsertBB() -> addSucc(then_bb);//设置后继
    end_bb -> addPred(then_bb);
    then_bb -> addSucc(end_bb);
    end_bb -> addPred(builder -> getInsertBB());
    builder -> getInsertBB() -> addSucc(end_bb);
    
    cond->genCode();
    if(!cond -> getOperand() -> getType() -> isBool())
    {
        // printf("I think come in\n");
        BasicBlock* bb=cond->builder->getInsertBB();
        Operand *src = cond->getOperand();
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
        Constant* digit = new Constant(se);
        Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
        CmpInstruction* temp = new CmpInstruction(CmpInstruction::EXCLAMATION, t, src, digit->getOperand(), bb);
        src=t;
        cond->trueList().push_back(temp);
        cond->falseList().push_back(temp);
        Instruction* m = new CondBrInstruction(nullptr,nullptr,t,bb);
        cond->trueList().push_back(m);
        cond->falseList().push_back(m);
    }
    backPatch(cond->trueList(), then_bb);
    backPatchFalse(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);

    // Function* func;
    // BasicBlock *then_bb, *end_bb;

    // func = builder->getInsertBB()->getParent();
    // then_bb = new BasicBlock(func);
    // end_bb = new BasicBlock(func);

    // cond->genCode();

    // backPatch(cond->trueList(), then_bb);
    // backPatch(cond->falseList(), end_bb);

    // builder->setInsertBB(then_bb);
    // thenStmt->genCode();
    // then_bb = builder->getInsertBB();
    // new UncondBrInstruction(end_bb, then_bb);

    // builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    // Todo
    // printf("Hello4");
    Function *func;
    BasicBlock *then_bb, *end_bb, *else_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);

    then_bb -> addPred(builder->getInsertBB());
    then_bb -> addSucc(end_bb);

    else_bb -> addPred(builder->getInsertBB());
    else_bb -> addSucc(end_bb);

    end_bb -> addPred(then_bb);
    end_bb -> addPred(else_bb);

    builder -> getInsertBB() -> addSucc(then_bb);
    builder -> getInsertBB() -> addSucc(else_bb);

    // then_bb -> addPred(builder -> getInsertBB());
    // builder -> getInsertBB() -> addSucc(then_bb);

    // else_bb -> addPred(builder -> getInsertBB());
    // builder -> getInsertBB() -> addSucc(else_bb);

    // end_bb -> addPred(then_bb);
    // then_bb -> addSucc(end_bb);
    // end_bb -> addPred(else_bb);
    // else_bb -> addSucc(end_bb);

    cond->genCode();
    // if(!cond -> getOperand() -> getType() -> isBool())
    // {
    //     BasicBlock* bb=cond->builder->getInsertBB();
    //     Operand *src = cond->getOperand();
    //     SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, 0);
    //     Constant* digit = new Constant(se);
    //     Operand* t = new Operand(new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel()));
    //     CmpInstruction* temp = new CmpInstruction(CmpInstruction::EXCLAMATION, t, src, digit->getOperand(), bb);
    //     src=t;
    //     cond->trueList().push_back(temp);
    //     cond->falseList().push_back(temp);
    //     Instruction* m = new CondBrInstruction(nullptr,nullptr,t,bb);
    //     cond->trueList().push_back(m);
    //     cond->falseList().push_back(m);
    // }
    backPatch(cond->trueList(), then_bb);
    backPatchFalse(cond->falseList(), else_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void WhileStmt::genCode()
{
    // Function *func;
    // BasicBlock *then_bb, *end_bb, *cond_bb;

    // func = builder->getInsertBB()->getParent();
    // then_bb = new BasicBlock(func);
    // end_bb = new BasicBlock(func);
    // cond_bb = new BasicBlock(func);

    // new UncondBrInstruction(cond_bb, builder -> getInsertBB());

    // cond_bb -> addPred(builder->getInsertBB());
    // cond_bb -> addPred(end_bb);
    // cond_bb -> addSucc(then_bb);
    // cond_bb -> addSucc(end_bb);
    // then_bb -> addPred(cond_bb);
    // then_bb -> addSucc(cond_bb);
    // end_bb -> addPred(then_bb);
    // end_bb -> addPred(cond_bb);
    // builder -> getInsertBB() -> addSucc(cond_bb);
    
    // cond->genCode();

    // backPatch(cond->trueList(), then_bb);
    // backPatchFalse(cond->falseList(), end_bb);

    // builder->setInsertBB(then_bb);
    // thenStmt->genCode();
    // then_bb = builder->getInsertBB();
    // new UncondBrInstruction(cond_bb, then_bb);

    // builder->setInsertBB(end_bb);
    Function *func;
    BasicBlock *then_bb, *end_bb, *cond_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    cond_bb = new BasicBlock(func);

    new UncondBrInstruction(cond_bb, builder->getInsertBB());

    cond_bb -> addPred(builder->getInsertBB());
    builder -> getInsertBB() -> addSucc(cond_bb);
    then_bb -> addPred(cond_bb);
    cond_bb -> addSucc(then_bb);
    end_bb -> addPred(then_bb);
    then_bb -> addSucc(end_bb);
    end_bb -> addPred(cond_bb);
    cond_bb -> addSucc(end_bb);
    
    builder -> setInsertBB(cond_bb);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatchFalse(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void CompoundStmt::genCode()
{
    // Todo
    stmt -> genCode();
}

void SeqNode::genCode()
{
    // Todo
    stmt1 -> genCode();
    stmt2 -> genCode();
}

void IdList::genCode()
{
    // Done in the DeclStmt
}

void ConstIdList::genCode()
{
    // Done in the ConstDeclStmt
}

void DeclStmt::genCode()
{
    for(int i=0; i<ids->Ids.size(); i++){
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(ids->Ids[i]->getSymPtr());
        if(se->isGlobal())
        {
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
            Instruction *alloca = new AllocaInstruction(addr, se);
            alloca -> output();
        }
        else if(se->isLocal())
        {
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Instruction *alloca;
            Operand *addr;
            SymbolEntry *addr_se;
            Type *type;
            type = new PointerType(se->getType());
            addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
            addr = new Operand(addr_se);
            alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        }
    }
    for(int i=0;i<ids->Assigns.size();i++){
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(ids -> Assigns[i] -> lval -> getSymPtr());
        // I will handler the isGlobal later
        if(se -> isGlobal())
        { 
            continue;                   
        }
        else if(se -> isLocal())
        {
            // From my point of view, it doesn't be neccessary of se->setAddr.
            // Because it has been done before.
            // Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(ids -> Assigns[i] -> lval ->getSymPtr())->getAddr();
            // se->setAddr(addr); 
            ids -> Assigns[i] -> genCode();
        }
    }
}

void ConstDeclStmt::genCode()
{
    for(int i=0; i<cids->CIds.size(); i++){
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(cids->CIds[i]->getSymPtr());
        if(se->isGlobal())
        {
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
            cids -> CAssigns[i] -> genCode();
            Instruction *alloca = new AllocaInstruction(addr, se);
            alloca -> output();
        }
        else if(se->isLocal())
        {
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Instruction *alloca;
            Operand *addr;
            SymbolEntry *addr_se;
            Type *type;
            type = new PointerType(se->getType());
            addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
            addr = new Operand(addr_se);
            alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
            
            cids -> CAssigns[i] -> expr -> genCode();
            Operand *addr1 = dynamic_cast<IdentifierSymbolEntry*>(cids -> CAssigns[i] -> lval ->getSymPtr())->getAddr();
            se->setAddr(addr1); 
            Operand *src = cids -> CAssigns[i] -> expr -> getOperand();
            BasicBlock *ttt = builder -> getInsertBB();
            new StoreInstruction(addr1, src, ttt);  
        }
    }
}

void ReturnStmt::genCode()
{
    // Todo
    BasicBlock *bb = builder -> getInsertBB();
    if(retValue){
        retValue -> genCode();
        Operand* src = retValue -> getOperand();
        new RetInstruction(src,bb);
    }
}

void ExprStmt::genCode()
{
    expr -> genCode();
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

bool Ast::typeCheck(Type* retType)
{
    if(root != nullptr)
        return root->typeCheck(retType);
    return false;
}

bool FuncParam::typeCheck(Type* retType)
{
    // do later
    // do nothing
    return false;
}

bool FuncParamList::typeCheck(Type* retType)
{
    // do later
    // 检查赋值是否正确
    for(auto &assign:Assigns)
    {
        return assign -> typeCheck(retType);
    }
    return false;
}

bool FunctionDef::typeCheck(Type* retType)
{
    // Todo
    Type* retType2 = (dynamic_cast<FunctionType*>(se->getType()))->getRetType();
    StmtNode* stmtSet = this->stmt;
    if (stmtSet == nullptr) {
        if (retType2 != TypeSystem::voidType){
            fprintf(stderr,"FuncExpr Error: %s ",se->toStr().c_str());
            fprintf(stderr, "non-void function does not return a value\n");
        }
        return false;
    }
    if (!stmtSet->typeCheck(retType2)) {
        fprintf(stderr,"FuncExpr Error: %s ",se->toStr().c_str());
        fprintf(stderr, "function does not have a return statement\n");
        return false;
    }
    return false;
    if(FPlist){
        FPlist -> typeCheck(retType2);
    }
    // Type* retType = (dynamic_cast<FunctionType*>(se->getType()))->getRetType();
    // if(stmt -> type == nullptr)
    // {
    //     fprintf(stderr, "Missing Return Statement!\n");
    //     return ;
    // }
    // if(se -> getType() != stmt -> type)
    // {
    //     fprintf(stderr, "FunctionDef %s typecheck Dismatch!\n",
    //             se -> getType() -> toStr().c_str());
    //     return ;
    // }
    // type = se -> getType();
}

bool EmptyStmt::typeCheck(Type* retType)
{
    return false;
}

bool FuncCallParamList::typeCheck(Type* retType)
{
    // do later
    for(auto &fcp:FCPs)
    {
        return fcp -> typeCheck(retType);
    }
    return false;
}

bool FuncExpr::typeCheck(Type* retType)
{
    // do later
    if(FCPlist){
        FCPlist -> typeCheck(retType);
    }
    // 函数参数匹配
    Type *type1 = getSymPtr() -> getType();
    SymbolEntry *sym1 = getSymPtr();
    std::vector<Type*> paramsType = dynamic_cast<FunctionType*>(type1) -> getParamsType();
    // printf("I am here;");
    // printf("%d",FCPlist->FCPs.size());
    // paramsType.size();
    // FCPlist->FCPs.size();
    // printf("%d",paramsType.size());
    // printf("%d",FCPlist->FCPs.size());
    if(!FCPlist){
        if(paramsType.size() != 0){
            fprintf(stderr,"FuncExpr Error: %s",getSymPtr()->toStr().c_str());
            fprintf(stderr," param num mismatch \n");
        }
        return false;
    }
    if(paramsType.size() != FCPlist->FCPs.size()){
        fprintf(stderr,"FuncExpr Error: %s",getSymPtr()->toStr().c_str());
        fprintf(stderr," param num mismatch \n");
        return false;
    }
    for(int i=0;i<paramsType.size();i++){
        if(FCPlist->FCPs[i]->getSymPtr()->getType()!=paramsType[i]){
            // fprintf(stderr,"FuncExpr Error:");
            fprintf(stderr,"FuncExpr Error: %s",getSymPtr()->toStr().c_str());
            fprintf(stderr,"%d param mismatch needed %s provided %s \n",paramsType[i]->toStr().c_str(),FCPlist->FCPs[i]->getSymPtr()->getType()->toStr().c_str());
        }
    }
    // SymbolEntry *sym2 = expr -> getSymPtr();
    return false;
}

bool BinaryExpr::typeCheck(Type* retType)
{
    // Todo
    //printf("BinaryExpr::typeCheck");
    Type *type1 = expr1 -> getSymPtr() -> getType();
    Type *type2 = expr2 -> getSymPtr() -> getType();
    // printf("Hello");
    // printf("%d",type1->isBool());
    // printf("%d",type1->isChar());
    // printf("%d",type1->isFunc());
    // printf("%d",type1->isInt());
    // printf("%d",type1->isVoid());
    // printf("Hello");
    // if(type1->isFunc()){
    //     if(type2->isFunc()){
    //         if(dynamic_cast<FunctionType*>(type1)->getRetType()->isVoid()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"The first operand is void \n");
    //         }else if(dynamic_cast<FunctionType*>(type2)->getRetType()->isVoid()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"The second operand is void \n");
    //         }else if(dynamic_cast<FunctionType*>(type1)->getRetType() != dynamic_cast<FunctionType*>(type2)->getRetType()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"type %s and %s mismatch \n",dynamic_cast<FunctionType*>(type1)->getRetType()->toStr().c_str(),dynamic_cast<FunctionType*>(type2)->getRetType()->toStr().c_str());
    //         }
    //     }else{
    //         if(dynamic_cast<FunctionType*>(type1)->getRetType()->isVoid()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"The first operand is void \n");
    //         }else if(dynamic_cast<FunctionType*>(type1)->getRetType() != type2){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"type %s and %s mismatch \n",dynamic_cast<FunctionType*>(type1)->getRetType()->toStr().c_str(),type2->toStr().c_str());
    //         }
    //     }
    //     symbolEntry -> setType(dynamic_cast<FunctionType*>(type1)->getRetType());
    // }else{
    //     if(type2->isFunc()){
    //         if(type2->isVoid()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"The second operand is void \n");
    //         }else if(type1 != dynamic_cast<FunctionType*>(type2)->getRetType()){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),dynamic_cast<FunctionType*>(type2)->getRetType()->toStr().c_str());
    //         }
    //     }else{
    //         if(type1 != type2){
    //             fprintf(stderr,"BianryExpr Error:");
    //             fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),type2->toStr().c_str());
    //         }
    //     }
    //     symbolEntry -> setType(type2);
    // }
    // if(type1 != type2){
    //     fprintf(stderr,"BinaryExpr Error:");
    //     fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),type2->toStr().c_str());
    //     // exit(EXIT_FAILURE);
    // }
    // symbolEntry -> setType(type1);
    expr1 -> typeCheck(retType);
    expr2 -> typeCheck(retType);
    return false;
}

bool UnaryExpr::typeCheck(Type* retType)
{
    Type *type = expr -> getSymPtr() -> getType();
    if(type -> isVoid()){
        fprintf(stderr,"UnaryExpr Error:");
        fprintf(stderr, "type can't be void \n");
        // exit(EXIT_FAILURE);
    }
    if(type -> isFunc()){
        //printf("UnaryExpr:%d",dynamic_cast<FunctionType*>(type)->getRetType()->isVoid());
        symbolEntry -> setType(dynamic_cast<FunctionType*>(type)->getRetType());
    }else{
        symbolEntry -> setType(type);
    }
    expr -> typeCheck(retType);
    return false;
}

bool Constant::typeCheck(Type* retType)
{
    // Todo
    return false;
}

bool Id::typeCheck(Type* retType)
{
    // Todo
    return false;
}

bool ConstId::typeCheck(Type* retType)
{
    // do later
    return false;
}

bool IfStmt::typeCheck(Type* retType)
{
    // Todo
    if(!cond->getSymPtr()->getType()->isBool()){
        fprintf(stderr,"IfStmt: convert cond to bool\n");
    }
    cond -> typeCheck(retType);

    if(thenStmt){
        return thenStmt -> typeCheck(retType);
    }
    return false;
}

bool IfElseStmt::typeCheck(Type* retType)
{
    // Todo
    bool flagThenStmt,flagElseStmt;
    if(!cond->getSymPtr()->getType()->isBool()){
        fprintf(stderr,"IfElseStmt: convert cond to bool\n");
    }
    cond -> typeCheck(retType);
    flagThenStmt = thenStmt -> typeCheck(retType);
    flagThenStmt = elseStmt -> typeCheck(retType);
    return flagThenStmt && flagElseStmt;
}

bool WhileStmt::typeCheck(Type* retType)
{
    // do later
    if(!cond->getSymPtr()->getType()->isBool()){
        fprintf(stderr,"WhileStmt: convert cond to bool\n");
    }
    cond -> typeCheck(retType);
    if(thenStmt){
        return thenStmt -> typeCheck(retType);
    }
    return false;
}

bool CompoundStmt::typeCheck(Type* retType)
{
    // Todo
    if(stmt){
        return stmt -> typeCheck(retType);
    }
    return false;
}

bool SeqNode::typeCheck(Type* retType)
{
    // Todo
    bool flag1,flag2;
    if(stmt1){
        flag1 = stmt1 -> typeCheck(retType);
    }
    if(stmt2){
        flag2 = stmt2 -> typeCheck(retType);
    }
    return flag1 || flag2;
}

bool IdList::typeCheck(Type* retType)
{
    // do later
    return false;
}

bool ConstIdList::typeCheck(Type* retType)
{
    // do later
    return false;
}

bool DeclStmt::typeCheck(Type* retType)
{
    // Todo
    for(auto &id:ids->Ids)
    {
        id -> typeCheck(retType);
    }
    return false;
}

bool ConstDeclStmt::typeCheck(Type* retType)
{
    // do later
    return false;
}

bool ReturnStmt::typeCheck(Type* retType)
{
    // Todo
    if(retValue){
        retValue->typeCheck(retType);
    }
    if(retValue){
        if(retType->isVoid()){
            // fprintf(stderr,"FuncExpr Error: %s",getSymPtr()->toStr().c_str());
            fprintf(stderr,"void function return a value\n");
        }else if(retValue->getSymPtr()->getType()->isFunc()){
            if(dynamic_cast<FunctionType*>(retValue->getSymPtr()->getType())->getRetType() != retType){
                fprintf(stderr,"function return mismathch\n");
            }
        }
        else if(retValue->getSymPtr()->getType() != retType){
            fprintf(stderr,"function return mismatch\n");
        }
        return true;
    }else{
        if(!retType->isVoid()){
            fprintf(stderr,"non-void function return nothing\n");
        }
        return true;
    }
    // if(retValue != nullptr)
    // {
    //     retValue -> typeCheck(retType);
    //     type = retValue -> getOperand() -> getType();
    // }
    // else{
    //     type = new VoidType();
    // }
}

bool ExprStmt::typeCheck(Type* retType)
{
    // do later
    expr -> typeCheck(retType);
    return false;
}

bool AssignStmt::typeCheck(Type* retType)
{
    // Todo
    //printf("AssignStmt::typeCheck");
    //printf("%d",dynamic_cast<BinaryExpr*>(expr)->expr1->getSymPtr()->getType()->isFunc());
    lval -> typeCheck(retType);
    expr -> typeCheck(retType);
    Type *type1 = lval -> getSymPtr() -> getType();
    Type *type2 = expr -> getSymPtr() -> getType();
    SymbolEntry *sym1 = lval -> getSymPtr();
    SymbolEntry *sym2 = expr -> getSymPtr();
    if(type2->isFunc()){
        if(type1 != dynamic_cast<FunctionType*>(type2)->getRetType()){
            fprintf(stderr,"AssignStmt Error:");
            fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),dynamic_cast<FunctionType*>(type2)->getRetType()->toStr().c_str());
        }
    }else if(type1 != type2){
        fprintf(stderr,"AssignStmt Error:");
        fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),type2->toStr().c_str());
    }
    return false;
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case MUL:
            op_str = "mul";
            break;
        case DIV:
            op_str = "div";
            break;
        case MOD:
            op_str = "mod";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
        case GREATER:
            op_str = "greater";
        case XOR:
            op_str = "xor";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case NEGATION:
            op_str = "negation";
            break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void ConstId::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cConstId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

void IdList::output(int level)
{
    fprintf(yyout, "%*cIdList\n", level, ' ');
    for(long unsigned int i=0;i<Ids.size();i++){
        Ids[i] -> output(level+4);
    }
    for(long unsigned int i=0;i<Assigns.size();i++){
        Assigns[i] -> output(level+4);
    }
}

void ConstIdList::output(int level)
{
    fprintf(yyout, "%*cConstIdList\n", level, ' ');
    for(long unsigned int i=0;i<CIds.size();i++){
        CIds[i] -> output(level+4);
    }
    for(long unsigned int i=0;i<CAssigns.size();i++){
        CAssigns[i] -> output(level+4);
    }
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    ids->output(level + 4);
}

void ConstDeclStmt::output(int level)
{
    fprintf(yyout, "%*cConstDeclStmt\n", level, ' ');
    cids->output(level + 4);
}

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
    thenStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FuncParam::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cFuncParam\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void FuncParamList::output(int level)
{
    fprintf(yyout, "%*cFuncParamList\n", level, ' ');
    for(long unsigned int i=0;i<FPs.size();i++){
        FPs[i] -> output(level+4);
    }
    for(long unsigned int i=0;i<Assigns.size();i++){
        Assigns[i] -> output(level+4);
    }
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    stmt->output(level + 4);
}

void EmptyStmt::output(int level)
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}

void FuncCallParamList::output(int level)
{
    fprintf(yyout, "%*cFuncCallParamList\n", level, ' ');
    for(long unsigned int i=0;i<FCPs.size();i++){
        FCPs[i] -> output(level+4);
    }
}

void FuncExpr::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cFunctionCall function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(FCPlist != NULL){
        FCPlist -> output(level+4);
    }
}