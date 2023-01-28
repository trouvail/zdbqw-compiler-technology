%option noyywrap
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
    #include <vector>
    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    #include <iostream>
    #include <math.h>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #endif
%}
%{
    
    class Node{
    public:
        string name_of_id;
        int global_count_of_id;
    };
    int leftNum = 0;
    int rightNum = 0;
    int id_count = 0;
    int global_count_of_id = 0;
    vector<int> a;
    
    Node table[1000];
    
%}
HEX (0x[1-9|A-F|a-f][0-9|A-F|a-f]*|0x0)
OCT (0[1-7][0-7]*|00)
DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
char ('.')
EOL (\r\n|\n|\r)
WHITE [\t ]
linecomment \/\/.*
commentbegin "/*"
commentelement .|\n
commentend "*/"

%x BLOCKCOMMENT

%%


{linecomment} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "linecomment\t" + a + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return linecomment;
    #endif
}

{commentbegin} {
    #ifdef ONLY_FOR_LEX
        BEGIN BLOCKCOMMENT;
        string a = yytext;

        while(a.find('\n') != a.npos)
        {
            yylineno++;
            a.replace(a.find('\n'),1,"");
        }
        string temp = "commentbegin\t/*\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return commentbegin;
    #endif
}

<BLOCKCOMMENT>{commentend} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        if(a.find("\n") != a.npos)
        {
            yylineno++;
            a.replace(a.find("\n"),1,"");
        }
        string temp = "commentend\t" + a + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
        BEGIN INITIAL;
    #else 
        return commentend;
    #endif
}

<BLOCKCOMMENT>{commentelement} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        if(a.find('\n') != a.npos)
        {
            yylineno++;
            a.replace(a.find('\n'),1,"");
        }
        string temp = "commentelement\t" + a + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return commentelement;
    #endif
}

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        string temp = "INT\tint\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return INT;
    #endif
}
"char" {
    #ifdef ONLY_FOR_LEX        
        string temp = "CHAR\tchar\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CHAR;
    #endif
}
"while" {
    #ifdef ONLY_FOR_LEX
        string temp = "WHILE\twhile\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return WHILE;
    #endif
}
"break" {
    #ifdef ONLY_FOR_LEX
        string temp = "BREAK\tbreak\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return BREAK;
    #endif
}
"continue" {
    #ifdef ONLY_FOR_LEX
        string temp = "CONTINUE\tcontinue\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CONTINUE;
    #endif
}
"const" {
    #ifdef ONLY_FOR_LEX
        string temp = "CONST\tconst\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return CONST;
    #endif
}
"new" {
    #ifdef ONLY_FOR_LEX
        string temp = "NEW\tnew\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return NEW;
    #endif
}
"delete" {
    #ifdef ONLY_FOR_LEX
        string temp = "DELETE\tdelete\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return DELETE;
    #endif
}
"using" {
    #ifdef ONLY_FOR_LEX
        string temp = "USING\tusing\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return USING;
    #endif
}
"namespace" {
    #ifdef ONLY_FOR_LEX
        string temp = "NAMESPACE\tnamespace\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return namespace;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        string temp = "VOID\tvoid\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return VOID;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        string temp = "IF\tif\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        string temp = "ELSE\telse\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ELSE;
    #endif
};
"return" {
    #ifdef ONLY_FOR_LEX
        string temp = "RETURN\treturn\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RETURN;
    #endif
}

"==" {
    #ifdef ONLY_FOR_LEX
        string temp = "EQUAL\t==\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return EQUAL;
    #endif
}
"=" {
    #ifdef ONLY_FOR_LEX
        string temp = "ASSIGN\t=\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ASSIGN;
    #endif
}
"!=" {
    #ifdef ONLY_FOR_LEX
        string temp = "UNEQUAL\t!=\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return UNEQUAL;
    #endif
}
"!" {
    #ifdef ONLY_FOR_LEX
        string temp = "AGAINST\t!\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return AGAINST;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        string temp = "STAR\t*\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return STAR;
    #endif
}
">=" {
    #ifdef ONLY_FOR_LEX
        string temp = "GTE\t>=\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return GTE;
    #endif
}
"<=" {
    #ifdef ONLY_FOR_LEX
        string temp = "LTE\t<=\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LTE;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        string temp = "LESS\t<\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        string temp = "MORE\t>\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return MORE;
    #endif
}
"," {
    #ifdef ONLY_FOR_LEX
        string temp = "COMMA\t,\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return COMMA;
    #endif
}
":" {
    #ifdef ONLY_FOR_LEX
        string temp = "COLON\t:\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return COLON;
    #endif
}
"'" {
    #ifdef ONLY_FOR_LEX
        string temp = "QUOTATION\t'\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return QUOTATION;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        string temp = "ADD\t+\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        string temp = "SUB\t-\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return SUB;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        string temp = "DIV\t/\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return DIV;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        string temp = "MOD\t%\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return MOD;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        string temp = "SEMICOLON\t;\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        string temp = "LPAREN\t(\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return LPAREN; 
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        string temp = "RPAREN\t)\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RPAREN;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        global_count_of_id++;
        a.push_back(global_count_of_id);
        //cout << "{" << a.back() << endl;
        leftNum++;
        string temp = "LBRACE\t{\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        a.pop_back();
        //cout << "}" << a.back() << endl;
        rightNum++;
        string temp = "RBRACE\t}\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RBRACE;
    #endif
}
"[" {
    #ifdef ONLY_FOR_LEX
        string temp = "LSQUARE\t[\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return LSQUARE;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX
        string temp = "RSQUARE\t]\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return RSQUARE;
    #endif
}
"#" {
    #ifdef ONLY_FOR_LEX
        string temp = "POUND\t#\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else
        return POUND;
    #endif
}
{ID} {
    #ifdef ONLY_FOR_LEX
        string m = yytext;
        //cout << yytext << ":" << a.size() << endl;
        int idnum = 0;
        bool flag = 0;
        for(int i = 0; i < id_count; i++)
        {
            if(table[i].name_of_id == yytext && table[i].global_count_of_id == a.back())
            {
                idnum = i + 1;
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            table[id_count].name_of_id = yytext;
            if(a.size() != 0){
                table[id_count].global_count_of_id = a.back();
            }
            id_count++;
            idnum = id_count;
        }
        string temp = "ID\t" + m + "\tlinecount:" + to_string(yylineno) + "\tIDcount:" + to_string(idnum);
        DEBUG_FOR_LAB4(temp);
    #else 
        return ID;
    #endif
}

{OCT} {
    #ifdef ONLY_FOR_LEX
        string a  = yytext;
        int len = a.length() - 1;
        int all = 0;
        for(int i = 0; i < len; i++)
        {
            //cout << i << ":" << (pow(8, (len - i - 1))) << ' ' << (a[i + 1] - '0') << endl;
            int temp1 = (pow(8, (len - i - 1)));
            int temp2 = (a[i + 1] - '0');
            all = all + temp1 * temp2;         
        }
        string temp = "OCT\t" + to_string(all) + "(DEC)" + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return OCT;
    #endif
} 

{DECIMIAL} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "DECIMIAL\t" + a + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return DECIMIAL;
    #endif
}

{HEX} {
    #ifdef ONLY_FOR_LEX
        string a  = yytext;
        int len = a.length() - 2;
        int all = 0;
        for(int i = 0; i < len; i++)
        {
            int temp1 = (pow(16, (len - i - 1)));
            int temp2;
            if(a[i + 2] >= '0' && a[i + 2] <= '9'){
                temp2 = (a[i + 2] - '0');
            }
            else if(a[i + 2] >= 'A' && a[i + 2] <= 'F'){
                temp2 = a[i + 2] - 'A' + 10;
            }
            else if(a[i + 2] >= 'a' && a[i + 2] <= 'f'){
                temp2 = a[i + 2] - 'a' + 10;
            }
            all = all + temp1 * temp2;         
        }
        string temp = "HEX\t" + to_string(all) + "(DEC)" + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return HEX;
    #endif
}

{char} {
    #ifdef ONLY_FOR_LEX
        string a = yytext;
        string temp = "CHARACTER\t" + a + "\tlinecount:" + to_string(yylineno);
        DEBUG_FOR_LAB4(temp);
    #else 
        return CHARACTER;
    #endif
}


{EOL} yylineno++;
{WHITE}
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