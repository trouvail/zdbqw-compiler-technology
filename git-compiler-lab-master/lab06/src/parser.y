%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    #include <vector>
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    IdList* Idlisttype;
    ConstIdList* CIdlisttype;
    FuncParamList* FuncParamlisttype;
    FuncCallParamList* FuncCallParamlisttype;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE WHILE
%token INT VOID
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON
%token ADD SUB MUL DIV MOD OR AND NEGATION GREATER LESS EQUAL NOTEQUAL GREATEREQUAL LESSEQUAL ASSIGN
%token RETURN
%token COMMA
%token CONST

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt WhileStmt ReturnStmt DeclStmt ConstDeclStmt FuncDef ExprStmt EmptyStmt
%nterm <exprtype> Exp AddExp MulExp UnaryExp Cond LOrExp PrimaryExp LVal RelExp EqExp LAndExp
%nterm <type> Type
%nterm <Idlisttype> Idlist
%nterm <CIdlisttype> CIdlist
%nterm <FuncParamlisttype> FuncParamlist
%nterm <FuncCallParamlisttype> FuncCallParamlist

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | ConstDeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | ExprStmt {$$=$1;}
    | EmptyStmt {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        //printf("I am AssignStmt\n");
        //printf("I am parse.y");
        //printf("%d",dynamic_cast<BinaryExpr*>($3)->expr1->getSymPtr()->getType()->isFunc());
        $$ = new AssignStmt($1, $3);
    }
    ;
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            //printf("BlockStmt\n");
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        //printf("If Kind 0\n");
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        //printf("If Kind 1\n");
        $$ = new IfElseStmt($3, $5, $7);
    }
    | IF LPAREN Cond RPAREN LBRACE RBRACE{
        //printf("If Kind 2\n");
        $$ = new IfStmt($3, new EmptyStmt());
    }
    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt{
        $$ = new WhileStmt($3, $5);
    }
    | WHILE LPAREN Cond RPAREN LBRACE RBRACE{
        $$ = new WhileStmt($3, new EmptyStmt());
    }
    ;
ReturnStmt
    :
    RETURN SEMICOLON{
        //printf("Return Stmt\n");
        $$ = new ReturnStmt(nullptr);
    }
    |
    RETURN Exp SEMICOLON{
        //printf("Return Stmt\n");
        $$ = new ReturnStmt($2);
    }
    ;
ExprStmt
    :
    Exp SEMICOLON{
        $$ = new ExprStmt($1);
    }
    ;
EmptyStmt
    :
    SEMICOLON{
        $$ = new EmptyStmt();
    }
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | LPAREN Exp RPAREN {
        $$ = $2;
    }
    ;
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //printf("%d\n",$1->getSymPtr()->getType()->isFunc());
        Type* type1 = $1->getSymPtr()->getType();
        Type* type2 = $3->getSymPtr()->getType();
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
        if(type1->isFunc()){
            if(type2->isFunc()){
                if(dynamic_cast<FunctionType*>(type1)->getRetType()->isVoid()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"The first operand is void \n");
                }else if(dynamic_cast<FunctionType*>(type2)->getRetType()->isVoid()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"The second operand is void \n");
                }else if(dynamic_cast<FunctionType*>(type1)->getRetType() != dynamic_cast<FunctionType*>(type2)->getRetType()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"type %s and %s mismatch \n",dynamic_cast<FunctionType*>(type1)->getRetType()->toStr().c_str(),dynamic_cast<FunctionType*>(type2)->getRetType()->toStr().c_str());
                }
            }else{
                if(dynamic_cast<FunctionType*>(type1)->getRetType()->isVoid()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"The first operand is void \n");
                }else if(dynamic_cast<FunctionType*>(type1)->getRetType() != type2){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"type %s and %s mismatch \n",dynamic_cast<FunctionType*>(type1)->getRetType()->toStr().c_str(),type2->toStr().c_str());
                }
            }
            $$ -> symbolEntry -> setType(dynamic_cast<FunctionType*>(type1)->getRetType());
        }else{
            if(type2->isFunc()){
                if(type2->isVoid()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"The second operand is void \n");
                }else if(type1 != dynamic_cast<FunctionType*>(type2)->getRetType()){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),dynamic_cast<FunctionType*>(type2)->getRetType()->toStr().c_str());
                }
            }else{
                if(type1 != type2){
                    fprintf(stderr,"BianryExpr Error:");
                    fprintf(stderr,"type %s and %s mismatch \n",type1->toStr().c_str(),type2->toStr().c_str());
                }
            }
            $$ -> symbolEntry -> setType(type2);
        }
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
MulExp
    :
    // modified unary later
    UnaryExp {$$ = $1;}
    |
    MulExp MUL UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    |
    MulExp DIV UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    |
    MulExp MOD UnaryExp    
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    ;
UnaryExp
    :
    PrimaryExp {$$ = $1;}
    |
    ID LPAREN RPAREN 
    {
        //printf("Hello2\n");
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == NULL)
        {
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != NULL);
        }
        $$ = new FuncExpr(se,NULL);
    }
    |
    ID LPAREN FuncCallParamlist RPAREN
    {
        //printf("Hello\n");
        SymbolEntry *se;
        se = identifiers->lookup($1);
        //printf("Hello\n");
        if(se == NULL)
        {
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != NULL);
        }
        $$ = new FuncExpr(se,$3);
    }
    |
    LPAREN UnaryExp RBRACE 
    {
        $$ = $2;
    }
    |
    ADD UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    |
    SUB UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    |
    NEGATION UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NEGATION, $2);
    }
    ;
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    |
    RelExp LESSEQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSEQUAL, $1, $3);
    }
    |
    RelExp GREATEREQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, $1, $3);
    }
    ;
EqExp
    :
    RelExp {$$ = $1;}
    |
    EqExp EQUAL RelExp 
    {
        //printf("I am equal\n");
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL, $1, $3);
    }
    |
    EqExp NOTEQUAL RelExp
    {
        //printf("I am not equal\n");
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQUAL, $1, $3);
    }
    ;
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    ;
DeclStmt
    :
    Type Idlist SEMICOLON {
        $$ = new DeclStmt($2);
    }
    ;
Idlist
    :
    ID {
        std::vector<Id*> Ids;
        std::vector<AssignStmt*> Assigns;
        IdList *temp = new IdList(Ids,Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        // identifiers->install($1, se);
        if(!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        temp -> Ids.push_back(new Id(se));
        $$ = temp;
        delete []$1;
    }
    |
    Idlist COMMA ID{
        IdList *temp = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        // identifiers->install($3, se);
        if(!identifiers->install($3, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$3);
        temp -> Ids.push_back(new Id(se));
        $$ = temp;
        delete []$3;
    }
    |
    ID ASSIGN Exp {
        std::vector<Id*> Ids;
        std::vector<AssignStmt*> Assigns;
        IdList *temp = new IdList(Ids,Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        // identifiers->install($1, se);
        if(!identifiers->install($1, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$1);
        Id *t = new Id(se);
        temp -> Ids.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t,$3));
        $$ = temp;
        delete []$1;
    }
    |
    Idlist COMMA ID ASSIGN Exp {
        IdList *temp = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        // identifiers->install($3, se);
        if(!identifiers->install($3, se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)$3);
        Id *t = new Id(se);
        temp -> Ids.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t,$5));
        $$ = temp;
        delete []$3;
    }
    ;
ConstDeclStmt
    :
    CONST Type CIdlist SEMICOLON {
        $$ = new ConstDeclStmt($3);
    }
    ;
CIdlist
    :
    ID ASSIGN Exp {
        std::vector<ConstId*> CIds;
        std::vector<AssignStmt*> CAssigns;
        ConstIdList *temp = new ConstIdList(CIds,CAssigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        ConstId *t = new ConstId(se);
        temp -> CIds.push_back(t);
        temp -> CAssigns.push_back(new AssignStmt(t,$3));
        $$ = temp;
        delete []$1;
    }
    |
    CIdlist COMMA ID ASSIGN Exp {
        ConstIdList *temp = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);
        ConstId *t = new ConstId(se);
        temp -> CIds.push_back(t);
        temp -> CAssigns.push_back(new AssignStmt(t,$5));
        $$ = temp;
        delete []$3;
    }
    ;
FuncDef
    :
    Type ID LPAREN {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != NULL);
        FuncParamList* FPlist = NULL;
        $$ = new FunctionDef(se, FPlist, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    |
    Type ID LPAREN 
    // 原先RPARAM下面的代码在这个位置
    FuncParamlist RPAREN {
        Type *funcType;
        std::vector<Type*> funcParamTypes;
        for(auto &funcParam:$4->FPs){
            funcParamTypes.push_back(dynamic_cast<ExprNode*>(funcParam)->getSymPtr()->getType());
        }
        funcType = new FunctionType($1,funcParamTypes);
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != NULL);
        $$ = new FunctionDef(se, $4, $7);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
FuncParamlist
    :
    Type ID {
        std::vector<FuncParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncParamList *temp = new FuncParamList(FPs,Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        temp -> FPs.push_back(new FuncParam(se));
        $$ = temp;
        delete []$2;
    }
    |
    FuncParamlist COMMA Type ID{
        FuncParamList *temp = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $4, identifiers->getLevel());
        identifiers->install($4, se);
        temp -> FPs.push_back(new FuncParam(se));
        $$ = temp;
        delete []$4;
    }
    |
    Type ID ASSIGN Exp {
        std::vector<FuncParam*> FPs;
        std::vector<AssignStmt*> Assigns;
        FuncParamList *temp = new FuncParamList(FPs,Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        FuncParam *t = new FuncParam(se);
        temp -> FPs.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t,$4));
        $$ = temp;
        delete []$2;
    }
    |
    FuncParamlist COMMA Type ID ASSIGN Exp {
        FuncParamList *temp = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $4, identifiers->getLevel());
        identifiers->install($4, se);
        FuncParam *t = new FuncParam(se);
        temp -> FPs.push_back(t);
        temp -> Assigns.push_back(new AssignStmt(t,$6));
        $$ = temp;
        delete []$4;
    }
    ;
FuncCallParamlist
    :
    Exp
    {
        std::vector<ExprNode*> params;
        params.push_back($1);
        FuncCallParamList *temp = new FuncCallParamList(params);
        $$ = temp;
    };
    |
    FuncCallParamlist COMMA Exp
    {
        FuncCallParamList *temp = $1;
        temp -> FCPs.push_back($3);
        $$ = temp;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}