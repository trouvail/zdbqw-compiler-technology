%option noyywrap
%top{
    #include <vector>
    #include <string>
    #include <map>
}
%{
    /*
    * You will need to comment this line in lab5.
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>

    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }

    void AddLineNo(char* content,int lineno){
        char noline[100];
        memset(noline,0,100);
        sprintf(noline,"%d",lineno);
        sprintf(content,"%-10s %-10s",content,noline);
    }

    void AddOffset(char* content,int offset){
        AddLineNo(content,offset);
    }

    #endif
    int extraLScope = 0;
    int totalCompletedScope = 0;
    struct Symbol{
        char identifier[100];
        // 当时多余的左括号（不修改）
        int curExtraLScope;
        // 当时完成的总大括号个数（不修改）
        int curCompletedScope;
        // 剩余的多余的左括号（随时修改）
        int restExtraLScope;
        int id;
    };
    int id = 0;
    map<string,vector<struct Symbol> > SymbolTable;

    void InsertSymbol(int extraLScope,int totalCompletedScope,int restExtraLScope,int& id,vector<struct Symbol>& symbolList){
        struct Symbol symbol;
        memset(symbol.identifier,0,100);
        strcpy(symbol.identifier,yytext);
        symbol.curExtraLScope = extraLScope;
        symbol.curCompletedScope = totalCompletedScope;
        symbol.restExtraLScope = 1;
        id ++;
        symbol.id = id;
        // 将该符号加入符号列表中，然后加入映射中
        symbolList.push_back(symbol);
    }
    int offset = 0;
    void AppendChar(char* content,char* oth){
        sprintf(content,"%-10s %-10s",content,oth);
    }
%}

SINGLECOMMENT "//"
COMMENTBEGIN ("/*")
COMMENTEND ("*/")
COMMENTCONTENT .|\n
OCTDECIMIAL (0([1-7][0-7]*|0))
HEXDECIMIAL (0x([1-9a-fA-F][0-9a-fA-F]*|0))
DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
%x BLOCKCOMMENT
%x LINECOMMENT

%%

{SINGLECOMMENT} {
    #ifdef ONLY_FOR_LEX
        BEGIN LINECOMMENT;
        char content[100];
        memset(content,0,100);
        strcpy(content,"LINECOMMENT\t//");
        AddLineNo(content,yylineno);
        // DEBUG_FOR_LAB4(content);
    #else
        return SINGLECOMMENT;
    #endif
}

<LINECOMMENT>{COMMENTCONTENT} {
    #ifdef ONLY_FOR_LEX
        if(strcmp(yytext,"\n")==0){
            BEGIN INITIAL;
            yylineno ++;
        }else{
            char content[100];
            memset(content,0,100);
            strcpy(content,"COMMENT\t");
            strcat(content,yytext);
            AddLineNo(content,yylineno);
            // DEBUG_FOR_LAB4(content);
        }
    #else
        return COMMENTCONTENT;
    #endif
}

{COMMENTBEGIN} {
    #ifdef ONLY_FOR_LEX
        BEGIN BLOCKCOMMENT;
        char content[100];
        memset(content,0,100);
        strcpy(content,"COMMENTBEGIN\t/*");
        AddLineNo(content,yylineno);
        // DEBUG_FOR_LAB4(content);
    #else
        return COMMENTBEGIN;
    #endif
}

<BLOCKCOMMENT>{COMMENTEND} {
    #ifdef ONLY_FOR_LEX
        BEGIN INITIAL;
        char content[100];
        memset(content,0,100);
        strcpy(content,"COMMENTEND\t*/");
        AddLineNo(content,yylineno);
        // DEBUG_FOR_LAB4(content);
    #else
        return COMMENTEND;
    #endif
}

<BLOCKCOMMENT>{COMMENTCONTENT} {
    #ifdef ONLY_FOR_LEX
        if(strcmp(yytext,"\n")==0){
            yylineno ++;
        }else{
            char content[100];
            memset(content,0,100);
            strcpy(content,"COMMENT\t");
            strcat(content,yytext);
            AddLineNo(content,yylineno);
            // DEBUG_FOR_LAB4(content);
        }
    #else
        return COMMENTCONTENT;
    #endif
}


"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","INT");
        AppendChar(content,"int");
        // strcpy(content,"INT\tint");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 3;
        DEBUG_FOR_LAB4(content);
    #else
        return INT;
    #endif
}
"char" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","CHAR");
        AppendChar(content,"char");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 4;
        DEBUG_FOR_LAB4(content);
    #else
        return CHAR;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","VOID");
        AppendChar(content,"void");
        // strcpy(content,"VOID\tvoid");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 4;
        DEBUG_FOR_LAB4(content);
    #else
        return VOID;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","IF");
        AppendChar(content,"if");
        // strcpy(content,"IF\tif");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 2;
        DEBUG_FOR_LAB4(content);
    #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","ELSE");
        AppendChar(content,"else");
        // strcpy(content,"ELSE\telse");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 4;
        DEBUG_FOR_LAB4(content);
    #else
        return ELSE;
    #endif
};
"return" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","RETURN");
        AppendChar(content,"return");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 6;
        DEBUG_FOR_LAB4(content);
    #else
        return RETURN;
    #endif
}
"for" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","FOR");
        AppendChar(content,"for");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 3;
        DEBUG_FOR_LAB4(content);
    #else
        return FOR;
    #endif
}
"while" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","WHILE");
        AppendChar(content,"while");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 5;
        DEBUG_FOR_LAB4(content);
    #else
        return WHILE;
    #endif
}
"continue" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","CONTINUE");
        AppendChar(content,"continue");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 8;
        DEBUG_FOR_LAB4(content);
    #else
        return CONTINUE;
    #endif
}
"break" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","BREAK");
        AppendChar(content,"break");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 5;
        DEBUG_FOR_LAB4(content);
    #else
        return BREAK;
    #endif
}
"const" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","CONST");
        AppendChar(content,"const");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 5;
        DEBUG_FOR_LAB4(content);
    #else
        return CONST;
    #endif
}
"new" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","NEW");
        AppendChar(content,"new");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 3;
        DEBUG_FOR_LAB4(content);
    #else
        return NEW;
    #endif
}
"delete" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","DELETE");
        AppendChar(content,"delete");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 6;
        DEBUG_FOR_LAB4(content);
    #else
        return DELETE;
    #endif
}
"using" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","USING");
        AppendChar(content,"using");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 5;
        DEBUG_FOR_LAB4(content);
    #else
        return USING;
    #endif
}
"namespace" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","NAMESPACE");
        AppendChar(content,"namespace");
        // strcpy(content,"RETURN\treturn");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 9;
        DEBUG_FOR_LAB4(content);
    #else
        return NAMESPACE;
    #endif
}
"=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","ASSIGN");
        AppendChar(content,"=");
        // strcpy(content,"ASSIGN\t=");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return ASSIGN;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","LESS");
        AppendChar(content,"<");
        // strcpy(content,"LESS\t<");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MORE");
        AppendChar(content,">");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MORE;
    #endif
}
"<=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","LESSEQUAL");
        AppendChar(content,"<=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return LESSEQUAL;
    #endif
}
"==" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","EQUAL");
        AppendChar(content,"==");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return EQUAL;
    #endif
}
">=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MOREQUAL");
        AppendChar(content,">=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MOREQUAL;
    #endif
}
"!=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","NOTEQUAL");
        AppendChar(content,"!=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return NOTEQUAL;
    #endif
}
"&&" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","AND");
        AppendChar(content,"&&");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return AND;
    #endif
}
"||" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","OR");
        AppendChar(content,"||");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return OR;
    #endif
}
"+=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","ADDASSIGN");
        AppendChar(content,"+=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return ADDASSIGN;
    #endif
}
"-=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","SUBASSIGN");
        AppendChar(content,"-=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return SUBASSIGN;
    #endif
}
"*=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MULASSIGN");
        AppendChar(content,"*=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MULASSIGN;
    #endif
}
"/=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","DIVASSIGN");
        AppendChar(content,"/=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return DIVASSIGN;
    #endif
}
"%=" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MODASSIGN");
        AppendChar(content,"%%=");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MODASSIGN;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","ADD");
        AppendChar(content,"+");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","SUB");
        AppendChar(content,"-");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return SUB;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MUL");
        AppendChar(content,"*");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MUL;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","DIV");
        AppendChar(content,"/");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","MOD");
        AppendChar(content,"%%");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return MOD;
    #endif
}
"++" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","SELFADD");
        AppendChar(content,"++");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return SELFADD;
    #endif
}
"--" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","SELFSUB");
        AppendChar(content,"--");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return SELFSUB;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","SEMICOLON");
        AppendChar(content,";");
        // strcpy(content,"SEMICOLON\t;");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return SEMICOLON;
    #endif
}
"~" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","OPPOSITE");
        AppendChar(content,"~");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return OPPOSITE;
    #endif
}
"!" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","NOT");
        AppendChar(content,"+");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return ADD;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","LPAREN");
        AppendChar(content,"(");
        // strcpy(content,"LPAREN\t(");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","RPAREN");
        AppendChar(content,")");
        // strcpy(content,"RPAREN\t)");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
    return RPAREN;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","LBRACE");
        AppendChar(content,"{");
        // strcpy(content,"LBRACE\t{");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
        extraLScope ++;
        map<string,vector<struct Symbol> >::iterator itMap;
        for(itMap=SymbolTable.begin();itMap!=SymbolTable.end();itMap++){
            vector<struct Symbol>::iterator itVec;
            for(itVec=(itMap->second).begin();itVec!=(itMap->second).end();itVec++){
                itVec->restExtraLScope ++;
            }
        }
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
         char content[100];
         sprintf(content,"%-10s","RBRACE");
        AppendChar(content,"}");
        // strcpy(content,"RBRACE\t}");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
        extraLScope --;
        if(extraLScope == 0){
            totalCompletedScope ++;
        }
        map<string,vector<struct Symbol> >::iterator itMap;
        for(itMap=SymbolTable.begin();itMap!=SymbolTable.end();itMap++){
            vector<struct Symbol>::iterator itVec;
            vector<struct Symbol> newVec;
            for(itVec=(itMap->second).begin();itVec!=(itMap->second).end();itVec++){
                if(itVec->restExtraLScope>1){
                    struct Symbol newSymbol = *itVec;
                    newSymbol.restExtraLScope --;
                    newVec.push_back(newSymbol);
                }
            }
            SymbolTable[itMap->first] = newVec;
        }
    #else
        return RBRACE;
    #endif
}
"[" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","LSQUARE");
        AppendChar(content,"[");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return LSQUARE;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","RSQUARE");
        AppendChar(content,"]");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return RSQUARE;
    #endif
}
"#" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","POUND");
        AppendChar(content,"#");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return POUND;
    #endif
}
"," {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","COMMA");
        AppendChar(content,",");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return COMMA;
    #endif
}
"'" {
    #ifdef ONLY_FOR_LEX
        char content[100];
        sprintf(content,"%-10s","QUOTATION");
        AppendChar(content,"'");
        // strcpy(content,"ADD\t+");
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += 1;
        DEBUG_FOR_LAB4(content);
    #else
        return QUOTATION;
    #endif
}
{EOL} {
    yylineno++;
    offset = 0;
    }

{WHITE} offset ++;

{ID} {
    #ifdef ONLY_FOR_LEX
        char content[100];
        memset(content,0,100);
        sprintf(content,"%-10s","ID");
        AppendChar(content,yytext);
        // strcpy(content,"ID");
        // strcat(content,yytext);
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += strlen(yytext);
        if(SymbolTable.find(string(yytext))==SymbolTable.end()){
            vector<struct Symbol> symbolList;
            // 创建该符号
            // struct Symbol symbol;
            // memset(symbol.identifier,0,100);
            // strcpy(symbol.identifier,yytext);
            // symbol.curExtraLScope = extraLScope;
            // symbol.curCompletedScope = totalCompletedScope;
            // symbol.restExtraLScope = 1;
            // id ++;
            // symbol.id = id;
            // // 将该符号加入符号列表中，然后加入映射中
            // symbolList.push_back(symbol);
            // int extraLScope,int totalCompletedScope,int restExtraLScope,int& id,vector<struct Symbol>& symbolList
            InsertSymbol(extraLScope,totalCompletedScope,1,id,symbolList);
            SymbolTable[string(yytext)] = symbolList;
            char idChar[10];
            memset(idChar,0,10);
            sprintf(idChar,"%d",id);
            // strcat(content,"\t ");
            // strcat(content,idChar);
            AppendChar(content,idChar);
            DEBUG_FOR_LAB4(content);
        }else {
            bool flag = 0;
            int symbolid;
            vector<struct Symbol> symbolList = SymbolTable[string(yytext)];
            for(int i=0;i<symbolList.size();i++){
                if(symbolList[i].curExtraLScope==extraLScope && symbolList[i].curCompletedScope==totalCompletedScope){
                    flag = 1;
                    symbolid = symbolList[i].id;
                    break;
                }
            }
            if(flag){
                // 符号表中存在
                char idChar[100];
                memset(idChar,0,10);
                sprintf(idChar,"%d",symbolid);
                // strcat(content,"\t ");
                // strcat(content,idChar);
                AppendChar(content,idChar);
                DEBUG_FOR_LAB4(content);
            }else{
                // // 符号表中不存在
                // struct Symbol symbol;
                // memset(symbol.identifier,0,100);
                // strcpy(symbol.identifier,yytext);
                // symbol.curExtraLScope = extraLScope;
                // symbol.curCompletedScope = totalCompletedScope;
                // symbol.restExtraLScope = 1;
                // id ++;
                // symbol.id = id;
                // symbolList.push_back(symbol);
                InsertSymbol(extraLScope,totalCompletedScope,1,id,symbolList);
                SymbolTable[string(yytext)] = symbolList;
                char idChar[100];
                memset(idChar,0,100);
                sprintf(idChar,"%d",id);
                // strcat(content,"\t ");
                // strcat(content,idChar);
                AppendChar(content,idChar);
                DEBUG_FOR_LAB4(content);
            }
        }

    #else
        return ID;
    #endif
}

{OCTDECIMIAL} {
    #ifdef ONLY_FOR_LEX
        char content[100];
        char numStr[100];
        int n;
        memset(content,0,100);
        memset(numStr,0,100);
        // 八进制字符串转为十进制数到n
        sscanf(yytext,"%o",&n);
        // 十进制数转为字符串存到numStr中
        sprintf(numStr,"%d",n);
        sprintf(content,"%-10s","OCTDECIMAL");
        AppendChar(content,numStr);
        // strcpy(content,"EIGHTDECIMAL \t");
        // strcat(content,numStr);
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += strlen(yytext);
        DEBUG_FOR_LAB4(content);
    #else
        return EIGHTDECIMAL;
    #endif
}

{HEXDECIMIAL} {
    #ifdef ONLY_FOR_LEX
        char content[100];
        char numStr[100];
        int n;
        memset(content,0,100);
        memset(numStr,0,100);
        // 十六进制字符串转为十进制数到n
        sscanf(yytext,"%x",&n);
        // 十进制数转为字符串存到numStr中
        sprintf(numStr,"%d",n);
        sprintf(content,"%-10s","HEXDECIMAL");
        AppendChar(content,numStr);
        // strcpy(content,"SIXTEENDECIMIAL \t");
        // strcat(content,numStr);
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += strlen(yytext);
        DEBUG_FOR_LAB4(content);
    #else
        return SIXTEENDECIMIAL;
    #endif
}

{DECIMIAL} {
    #ifdef ONLY_FOR_LEX
        char content[100];
        memset(content,0,100);
        sprintf(content,"%-10s","DECIMAL");
        AppendChar(content,yytext);
        // strcpy(content,"DECIMAL \t");
        // strcat(content,yytext);
        AddLineNo(content,yylineno);
        AddOffset(content,offset);
        offset += strlen(yytext);
        DEBUG_FOR_LAB4(content);
    #else
        return DECIMAL;
    #endif

}

%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[3], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }

    yylex();
    return 0;
}
#endif
