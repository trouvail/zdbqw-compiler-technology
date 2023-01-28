%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
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
    stmtNode* stmttype;
    exprNode* exprtype;
    Type* type;
    IdList* Idlisttype;
    FuncFParams* Fstype;
    FuncRParams* FRtype;
    ConstIdList* CIdstype;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE BREAK CONTINUE
%token WHILE
%token INT VOID CHAR
%token CONST 
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token ADD SUB MUL DIV EXCLAMATION MORE OR AND LESS ASSIGN EQUAL NOEQUAL LESSEQUAL MOREEQUAL PERC
%token RETURN
%token LINECOMMENT COMMENTBEIGN COMMENTELEMENT COMMENTLINE COMMENTEND

%nterm <stmttype> stmts stmt the_stmt_of_assign the_stmt_of_block the_stmt_of_if the_stmt_of_return the_stmt_of_decline the_define_of_fun the_stmt_of_whi the_stmt_of_condec the_stmt_of_sin
%nterm <exprtype> exp the_exp_of_unar the_exp_of_add the_exp_of_mul Cond the_expr_of_or the_expr_of_pri value the_expr_of_rel the_expr_of_and 
%nterm <type> Type 
%nterm <Idlisttype> Idlist 
%nterm <Fstype> FuncFParams
%nterm <FRtype> FuncRParams
%nterm <CIdstype> ConstIdList

%precedence THEN
%precedence ELSE
%%



Program
    : stmts 
    {
        ast.setRoot($1);
    }
    ;

stmts
    : stmt {$$=$1;}
    | stmts stmt
    {
        $$ = new SeqNode($1, $2);
    }
    ;
    
stmt
    : the_stmt_of_assign {$$=$1;}
    | the_stmt_of_block {$$=$1;}
    | the_stmt_of_if {$$=$1;}
    | the_stmt_of_return {$$=$1;}
    | the_stmt_of_decline {$$=$1;}
    | the_stmt_of_condec {$$ = $1;}
    | the_define_of_fun {$$=$1;}
    | the_stmt_of_whi {$$ = $1;}
    | SEMICOLON {$$ = new Empty();}
    | BREAK SEMICOLON {$$ = new Breakstmt();}
    | CONTINUE SEMICOLON {$$ = new Continuestmt();}
    | the_stmt_of_sin {$$ = $1;}
    ;

the_stmt_of_block
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        stmts RBRACE 
        {
            $$ = new Compoundstmt($3);
            SymbolTable *zyl_ins = identifiers;
            identifiers = identifiers->getPrev();
            delete zyl_ins;
        }
    ;

the_stmt_of_sin
    :
    exp SEMICOLON
    {
        $$ = new the_stmt_of_sin($1);
    }
    ;

the_stmt_of_assign
    :
    value ASSIGN exp SEMICOLON 
    {
        $$ = new the_stmt_of_assign($1, $3);
    }
    ;

the_stmt_of_if
    : IF LPAREN Cond RPAREN stmt %prec THEN 
    {
        $$ = new the_stmt_of_if($3, $5);
    }
    | IF LPAREN Cond RPAREN LBRACE RBRACE
    {
        $$ = new the_stmt_of_if($3, new Empty());
    } 
    | IF LPAREN Cond RPAREN stmt ELSE stmt 
    {
        $$ = new IfElsestmt($3, $5, $7);
    }
    ;

the_stmt_of_return
    :
    RETURN exp SEMICOLON
    {
        $$ = new the_stmt_of_return($2);
    }
    ;

the_stmt_of_whi
    : WHILE LPAREN Cond RPAREN stmt 
    {
        $$ = new the_stmt_of_whi($3, $5);
    }
    ;

Cond
    :
    the_expr_of_or {$$ = $1;}
    ;

exp
    :
    the_exp_of_add {$$ = $1;}
    ;

value
    : ID 
    {
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
the_expr_of_pri
    :
    value 
    {
        $$ = $1;
    }
    | INTEGER 
    {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    |
    LPAREN exp RPAREN{$$ = $2;}
    ;
the_exp_of_unar
    :
    the_expr_of_pri {$$ = $1;}
    |
    ID LPAREN RPAREN
    {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "Function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new FunctionCall(se, nullptr);
        delete []$1;
    }
    |
    ID LPAREN FuncRParams RPAREN
    {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "Function \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new FunctionCall(se, $3);
        delete []$1;
    }
    |
    SUB the_exp_of_unar 
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_sin(se, the_exp_of_sin::SUB, $2);
    }
    |
    EXCLAMATION the_exp_of_unar
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_sin(se, the_exp_of_sin::EXCLAMATION, $2);
    }
    |
    ADD the_exp_of_unar
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_sin(se, the_exp_of_sin::ADD, $2);
    }
    ;

the_exp_of_add
    :
    the_exp_of_mul {$$ = $1;}
    |
    the_exp_of_add ADD the_exp_of_mul
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::ADD, $1, $3);
    }
    |
    the_exp_of_add SUB the_exp_of_mul
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::SUB, $1, $3);
    }
    ;

the_exp_of_mul
    :
    the_exp_of_unar {$$ = $1;}
    |
    the_exp_of_mul MUL the_exp_of_unar
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::MUL, $1, $3);
    }
    |
    the_exp_of_mul DIV the_exp_of_unar
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::DIV, $1, $3);
    }
    |
    the_exp_of_mul PERC the_exp_of_unar
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::PERC, $1, $3);
    }
    ;    

the_expr_of_rel
    :
    the_exp_of_add {$$ = $1;}
    |
    the_expr_of_rel LESS the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::LESS, $1, $3);
    }
    |
    the_expr_of_rel MORE the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::MORE, $1, $3);
    }
    |
    the_expr_of_rel MOREEQUAL the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::MOREEQUAL, $1, $3);
    }
    |
    the_expr_of_rel LESSEQUAL the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::LESSEQUAL, $1, $3);
    }
    |
    the_expr_of_rel EQUAL the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::EQUAL, $1, $3);
    }
    |
    the_expr_of_rel NOEQUAL the_exp_of_add
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::NOEQUAL, $1, $3);
    }
    ;

the_expr_of_or
    :
    the_expr_of_and {$$ = $1;}
    |
    the_expr_of_or OR the_expr_of_and
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::OR, $1, $3);
    }
    ;

the_expr_of_and
    :
    the_expr_of_rel {$$ = $1;}
    |
    the_expr_of_and AND the_expr_of_rel
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new the_exp_of_bin(se, the_exp_of_bin::AND, $1, $3);
    }
    ;

Type
    : INT 
    {
        $$ = TypeSystem::intType;
    }
    | VOID 
    {
        $$ = TypeSystem::voidType;
    }
    | CHAR
    {
        $$ = TypeSystem::charType;
    }
    ;
the_stmt_of_decline
    :
    Type Idlist SEMICOLON 
    {
        $$ = new the_stmt_of_decline($2);
    }
    ;
the_stmt_of_condec
    :
    CONST Type ConstIdList SEMICOLON
    {
        $$ = new the_stmt_of_condec($3);
    }
    ;

Idlist
    :
    ID 
    {
        std::vector<Id*> Ids;
        std::vector<the_stmt_of_assign*> Assigns;
        IdList *wxnMid = new IdList(Ids, Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        wxnMid -> Ids.push_back(new Id(se));
        $$ = wxnMid;
        delete []$1;
    } 
    |
    Idlist COMMA ID
    {
        IdList *wxnMid = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);
        wxnMid -> Ids.push_back(new Id(se));
        $$ = wxnMid;
        delete []$3;
    }
    |
    ID ASSIGN exp 
    {
        std::vector<Id*> Ids;
        std::vector<the_stmt_of_assign*> Assigns;
        IdList *wxnMid = new IdList(Ids, Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        Id *t = new Id(se);
        wxnMid -> Ids.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $3));
        $$ = wxnMid;
        delete []$1;
    }
    |
    Idlist COMMA ID ASSIGN exp 
    {
        IdList *wxnMid = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);
        Id *t = new Id(se);
        wxnMid -> Ids.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $5));
        $$ = wxnMid;
        delete []$3;
    }
    ;

ConstIdList
    :
    ID ASSIGN exp 
    {
        std::vector<ConstId*> ConstIds;
        std::vector<the_stmt_of_assign*> Assigns;
        ConstIdList* wxnMid = new ConstIdList(ConstIds, Assigns);
        SymbolEntry* se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers -> getLevel());
        identifiers->install($1, se);
        ConstId *t = new ConstId(se);
        wxnMid -> CIds.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $3));
        $$ = wxnMid;
        delete []$1;
    }
    |
    ConstIdList COMMA ID ASSIGN exp 
    {
        ConstIdList *wxnMid = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $3, identifiers->getLevel());
        identifiers->install($3, se);
        ConstId *t = new ConstId(se);
        wxnMid -> CIds.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $5));
        $$ = wxnMid;
        delete []$3;
    }
    ;    

FuncFParams
    :
    Type ID
    {
        std::vector<FuncFParam*> FPs;
        std::vector<the_stmt_of_assign*> Assigns;
        FuncFParams *wxnMid = new FuncFParams(FPs, Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        wxnMid -> FPs.push_back(new FuncFParam(se));
        $$ = wxnMid;
        delete []$2;
    }
    |
    FuncFParams COMMA Type ID
    {
        FuncFParams *wxnMid = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);
        wxnMid -> FPs.push_back(new FuncFParam(se));
        $$ = wxnMid;
        delete []$4;
    }
    |
    Type ID ASSIGN exp
    {
        std::vector<FuncFParam*> FPs;
        std::vector<the_stmt_of_assign*> Assigns;
        FuncFParams *wxnMid = new FuncFParams(FPs, Assigns);
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        FuncFParam* t = new FuncFParam(se);
        wxnMid -> FPs.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $4));
        $$ = wxnMid;
        delete []$2;
    }
    |
    FuncFParams COMMA Type ID ASSIGN exp
    {
        FuncFParams *wxnMid = $1;
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);
        FuncFParam* t = new FuncFParam(se);
        wxnMid -> FPs.push_back(t);
        wxnMid -> Assigns.push_back(new the_stmt_of_assign(t, $6));
        $$ = wxnMid;
        delete []$4;
    }
    ;

FuncRParams
    :
    exp
    {
        std::vector<exprNode*> t;
        t.push_back($1);
        FuncRParams *wxnMid = new FuncRParams(t);
        $$ = wxnMid;
    }
    |
    FuncRParams COMMA exp
    {
        FuncRParams *wxnMid = $1;
        wxnMid -> exprs.push_back($3);
        $$ = wxnMid;
    }
    ;

the_define_of_fun
    :
    Type ID LPAREN 
    {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    RPAREN
    the_stmt_of_block
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, nullptr,$6);
        SymbolTable *zyl_ins = identifiers;
        identifiers = identifiers->getPrev();
        delete zyl_ins;
        delete []$2;
    }
    |
    Type ID LPAREN 
    {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    FuncFParams RPAREN
    the_stmt_of_block
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $5 ,$7);
        SymbolTable *zyl_ins = identifiers;
        identifiers = identifiers->getPrev();
        delete zyl_ins;
        delete []$2;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}