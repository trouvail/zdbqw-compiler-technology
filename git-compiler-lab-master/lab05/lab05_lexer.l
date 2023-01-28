%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include <fstream>
    #include <string>

    #include <math.h>

    #include "parser.h"
    #include <ostream>
    

    using namespace std;

    extern bool the_ins_of_zyl;

    extern FILE *yyin; 
    extern FILE *yyout;
    

    void cout_the_information(std::string s)
    {
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";

        fputs(DEBUG_INFO.c_str(), yyout);
    }
%}

DECIMIAL ([1-9][0-9]*|0)
HEX (0x[1-9|A-F|a-f][0-9|A-F|a-f]*|0x0)
OCT (0[1-7][0-7]*|00)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
char ('.')
EOL (\r\n|\n|\r)
WHITE [\t ]

LINECOMMENT \/\/[^\n]*
COMMENTBEIGN "/*"
COMMENTELEMENT .
COMMENTLINE (\r\n|\n|\r)
COMMENTEND "*/"
%x BLOCKCOMMENT

%%

{LINECOMMENT}

{COMMENTBEIGN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{COMMENTELEMENT} {}
<BLOCKCOMMENT>{COMMENTLINE} {yylineno++;}
<BLOCKCOMMENT>{COMMENTEND} {BEGIN INITIAL;}

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    if(the_ins_of_zyl)
        cout_the_information("INT\tint");
    return INT;
}
"void" {
    if(the_ins_of_zyl)
        cout_the_information("VOID\tvoid");
    return VOID;
}
"char" {
    if(the_ins_of_zyl)
        cout_the_information("CHAR\tchar");
    return CHAR;
}
"if" {
    if(the_ins_of_zyl)
        cout_the_information("IF\tif");
    return IF;
};
"else" {
    if(the_ins_of_zyl)
        cout_the_information("ELSE\telse");
    return ELSE;
};
"return" {
    if(the_ins_of_zyl)
        cout_the_information("RETURN\treturn");
    return RETURN;
}

"while" {
    if(the_ins_of_zyl)
        cout_the_information("WHILE\twhile");
    return WHILE;
}

"break" {
    if(the_ins_of_zyl)
        cout_the_information("BREAK\tbreak");
    return BREAK;
}

"continue" {
    if(the_ins_of_zyl)
        cout_the_information("CONTINUE\tcontinue");
    return CONTINUE;
}

"const" {
    if(the_ins_of_zyl)
        cout_the_information("CONST\tconst");
    return CONST;
}
"==" {
    if(the_ins_of_zyl)
        cout_the_information("EQUAL\t==");
    return EQUAL;
}
"=" {
    if(the_ins_of_zyl)
        cout_the_information("ASSIGN\t=");
    return ASSIGN;
}
">=" {
    if(the_ins_of_zyl)
        cout_the_information("MOREEQUAL\t>=");
    return MOREEQUAL;
}
"<=" {
    if(the_ins_of_zyl)
        cout_the_information("LESSEQUAL\t<=");
    return LESSEQUAL;
}
"!=" {
    if(the_ins_of_zyl)
        cout_the_information("NOEQUAL\t!=");
    return NOEQUAL;
}
"<" {
    if(the_ins_of_zyl)
        cout_the_information("LESS\t<");
    return LESS;
}
">" {
    if(the_ins_of_zyl)
        cout_the_information("MORE\t>");
    return MORE;
}
"!" {
    if(the_ins_of_zyl)
        cout_the_information("EXCLAMATION\t!");
    return EXCLAMATION;
}
"+" {
    if(the_ins_of_zyl)
        cout_the_information("ADD\t+");
    return ADD;
}
"-" {
    if(the_ins_of_zyl)
        cout_the_information("SUB\t-");
    return SUB;
}
"*" {
    if(the_ins_of_zyl)
        cout_the_information("MUL\t*");
    return MUL;
}
"/" {
    if(the_ins_of_zyl)
        cout_the_information("DIV\t/");
    return DIV;
}
"%" {
    if(the_ins_of_zyl)
        cout_the_information("PERC\t%");
    return PERC;
}
";" {
    if(the_ins_of_zyl)
        cout_the_information("SEMICOLON\t;");
    return SEMICOLON;
}
"&&" {
    if(the_ins_of_zyl)
        cout_the_information("AND\t&&");
    return AND;
}
"||" {
    if(the_ins_of_zyl)
        cout_the_information("OR\t||");
    return OR;
}
"(" {
    if(the_ins_of_zyl)
        cout_the_information("LPAREN\t(");
    return LPAREN;
}
")" {
    if(the_ins_of_zyl)
        cout_the_information("RPAREN\t)");
    return RPAREN;
}
"{" {
    if(the_ins_of_zyl)
        cout_the_information("LBRACE\t{");
    return LBRACE;
}
"}" {
    if(the_ins_of_zyl)
        cout_the_information("RBRACE\t}");
    return RBRACE;
}

"," {
    if(the_ins_of_zyl)
        cout_the_information("COMMA\tcomma");
    return COMMA;
}

{DECIMIAL} {
    if(the_ins_of_zyl)
        cout_the_information(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}

{HEX} {
    if(the_ins_of_zyl)  
        cout_the_information(yytext);
    int wxnNum = 0;
    string getZyl  = yytext;
    int len = getZyl.length() - 2;
    for(int i = 0; i < len; i++)
    {
        int mid1 = (pow(16, (len - i - 1)));
        int mid2;
        if(getZyl[i + 2] >= '0' && getZyl[i + 2] <= '9'){
            mid2 = (getZyl[i + 2] - '0');
        }
        else if(getZyl[i + 2] >= 'A' && getZyl[i + 2] <= 'F'){
            mid2 = getZyl[i + 2] - 'A' + 10;
        }
        else if(getZyl[i + 2] >= 'a' && getZyl[i + 2] <= 'f'){
            mid2 = getZyl[i + 2] - 'a' + 10;
        }
        wxnNum = wxnNum + mid1 * mid2;         
    }
    yylval.itype = wxnNum;
    return INTEGER;
}

{OCT} {
    if(the_ins_of_zyl)
        cout_the_information(yytext);
    string getZyl  = yytext;
    int len = getZyl.length() - 1;
    int wxnNum = 0;
    for(int i = 0; i < len; i++)
    {
        int mid1 = (pow(8, (len - i - 1)));
        int mid2 = (getZyl[i + 1] - '0');
        wxnNum = wxnNum + mid1 * mid2;         
    }
    yylval.itype = wxnNum;
    return INTEGER;
} 

{ID} {
    if(the_ins_of_zyl)
        cout_the_information(yytext);
    char *getZyl;
    getZyl = new char[strlen(yytext) + 1];
    strcpy(getZyl, yytext);
    yylval.strtype = getZyl;
    return ID;
}
{EOL} yylineno++;
{WHITE}
%%