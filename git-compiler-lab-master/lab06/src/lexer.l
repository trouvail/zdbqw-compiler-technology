%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include "parser.h"
    #include <ostream>
    #include <fstream>
    using namespace std;

    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
%}

SINGLECOMMENT "//"
COMMENTBEGIN ("/*")
COMMENTEND  ("*/")
COMMENTCONTENT .|\n
OCTDECIMIAL (0([1-7][0-7]*|0))
HEXDECIMIAL (0(x|X)([1-9a-fA-F][0-9a-fA-F]*|0))
DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
%x BLOCKCOMMENT
%x LINECOMMENT

%%

{SINGLECOMMENT} {
    BEGIN LINECOMMENT;
}

<LINECOMMENT>{COMMENTCONTENT} {
    if(strcmp(yytext,"\n")==0){
        BEGIN INITIAL;
    }
}

{COMMENTBEGIN} {
    BEGIN BLOCKCOMMENT;
}

<BLOCKCOMMENT>{COMMENTCONTENT} {}

<BLOCKCOMMENT>{COMMENTEND} {
    BEGIN INITIAL;
}

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    if(dump_tokens)
        DEBUG_FOR_LAB4("INT\tint");
    return INT;
}
"void" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("VOID\tvoid");
    return VOID;
}
"if" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("IF\tif");
    return IF;
};
"else" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ELSE\telse");
    return ELSE;
};
"return" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RETURN\treturn");
    return RETURN;
};
"while" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("WHILE\twhile");
    return WHILE;
}

"!" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NEGATION\t}");
    return NEGATION;
}
"&&" { 
    if(dump_tokens)
        DEBUG_FOR_LAB4("AND\t}");
    return AND;
}
"||" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("OR\t}");
    return OR;
}
"!" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NEGATION\t}");
    return NEGATION;
}
"=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ASSIGN\t=");
    return ASSIGN;
}
"<" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESS\t<");
    return LESS;
}
">" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GREATER\t<");
    return GREATER;
}
">=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GREATEREQUAL\t}");
    return GREATEREQUAL;
}
"<=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESSEQUAL\t}");
    return LESSEQUAL;
}
"==" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("EQUAL\t}");
    return EQUAL;
}
"!=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NOTEQUAL\t}");
    return NOTEQUAL;
}
"+" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ADD\t+");
    return ADD;
}
"-" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SUB\t-");
    return SUB;
}
"*" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MUL\t-");
    return MUL;
}
"/" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("DIV\t/");
    return DIV;
}
"%" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MOD\t%%");
    return MOD;
}
";" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SEMICOLON\t;");
    return SEMICOLON;
}
"(" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LPAREN\t(");
    return LPAREN;
}
")" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RPAREN\t)");
    return RPAREN;
}
"{" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LBRACE\t{");
    return LBRACE;
}
"}" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RBRACE\t}");
    return RBRACE;
}
"," {
    if(dump_tokens)
        DEBUG_FOR_LAB4("COMMA\t}");
    return COMMA;
}
"const" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("CONST\t}");
    return CONST;
}

{OCTDECIMIAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    int n;
    // 将八进制数转为十进制数
    sscanf(yytext,"%o",&n);
    char decimal[100];
    sprintf(decimal,"%d",n);
    yylval.itype = atoi(decimal);
    return INTEGER;
}
{HEXDECIMIAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    int n;
    // 将十六进制数转为十进制数
    sscanf(yytext,"%x",&n);
    char decimal[100];
    sprintf(decimal,"%d",n);
    yylval.itype = atoi(decimal);
    return INTEGER;
}
{DECIMIAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}

{ID} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{EOL} yylineno++;
{WHITE}
%%