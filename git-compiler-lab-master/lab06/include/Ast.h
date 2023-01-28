#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
public:
    std::vector<Instruction*> true_list;
    std::vector<Instruction*> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<Instruction*> &list, BasicBlock*bb);
    void backPatchFalse(std::vector<Instruction*> &list, BasicBlock*bb);
    std::vector<Instruction*> merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2);

public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual bool typeCheck(Type* retType = nullptr) = 0;
    virtual void genCode() = 0;
    std::vector<Instruction*>& trueList() {return true_list;}
    std::vector<Instruction*>& falseList() {return false_list;}
};

class ExprNode : public Node
{
public:
    SymbolEntry *symbolEntry;
    Operand *dst;   // The result of the subtree is stored into dst.
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry),dst(new Operand(symbolEntry)){};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymPtr() {return symbolEntry;};
};

class BinaryExpr : public ExprNode
{
public:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, MUL, DIV, MOD, XOR, AND, OR, LESS, GREATER, LESSEQUAL, GREATEREQUAL, EQUAL, NOTEQUAL};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){dst = new Operand(se);};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {ADD,SUB,NEGATION};
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr) : ExprNode(se), op(op), expr(expr){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class StmtNode : public Node
{
public:
    Type *type = nullptr;
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    WhileStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;
public:
    ExprStmt(ExprNode *expr) : expr(expr) {};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class AssignStmt : public StmtNode
{
public:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ListNode : public Node
{};

class FuncParam : public ExprNode
{
public:
    FuncParam(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class FuncParamList : public ListNode
{
public:
    std::vector<FuncParam*> FPs;
    std::vector<AssignStmt*> Assigns;
    FuncParamList(std::vector<FuncParam*> FPs,std::vector<AssignStmt*> Assigns) : FPs(FPs),Assigns(Assigns){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    FuncParamList *FPlist;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, FuncParamList *FPlist, StmtNode *stmt) : se(se), FPlist(FPlist), stmt(stmt){};
    // FunctionDef(SymbolEntry *se, StmtNode *stmt) : se(se), stmt(stmt){}
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class EmptyStmt : public StmtNode
{
public:
    EmptyStmt(){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

// 无用类
// class FuncCallParam : public ExprNode
// {
// public:
//     FuncCallParam(SymbolEntry *se) : ExprNode(se){};
//     void output(int level);
// };

class FuncCallParamList : public ListNode
{
public:
    std::vector<ExprNode*> FCPs;
    FuncCallParamList(std::vector<ExprNode*> FCPs) : FCPs(FCPs){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class FuncExpr : public ExprNode
{
private:
    FuncCallParamList *FCPlist;
public:
    FuncExpr(SymbolEntry *se, FuncCallParamList *FCPlist) : ExprNode(se), FCPlist(FCPlist){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class IdList : public ListNode
{
public:
    std::vector<Id*> Ids;
    std::vector<AssignStmt*> Assigns;
    IdList(std::vector<Id*> Ids,std::vector<AssignStmt*> Assigns) : Ids(Ids),Assigns(Assigns){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class DeclStmt : public StmtNode
{
private:
    IdList *ids;
public:
    DeclStmt(IdList *ids) : ids(ids){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ConstId : public ExprNode
{
public:
    ConstId(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ConstIdList : public ListNode
{
public:
    std::vector<ConstId*> CIds;
    std::vector<AssignStmt*> CAssigns;
    ConstIdList(std::vector<ConstId*> CIds,std::vector<AssignStmt*> CAssigns) : CIds(CIds),CAssigns(CAssigns){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class ConstDeclStmt : public StmtNode
{
private:
    ConstIdList *cids;
public:
    ConstDeclStmt(ConstIdList *cids) : cids(cids){};
    void output(int level);
    bool typeCheck(Type* retType);
    void genCode();
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    bool typeCheck(Type* retType);
    void genCode(Unit *unit);
};

#endif
