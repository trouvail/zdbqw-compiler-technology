%{
#include <stdio.h>
#include <stdlib.h>
#include "Symbol.h"
#ifndef YYSTYPE
#define YYSTYPE double
#endif
char idStr[50];
static int curSize = 0;
int yylex();
extern int yyparse();
extern int lookup(char idStr[50],struct Symbol* Sym[1024]);
extern void insert(struct Symbol* s,struct Symbol* Sym[1024]);
FILE* yyin;
void yyerror(const char* s);
%}

%token ADD SUB MUL DIV LEFTKUOHAO RIGHTKUOHAO NUMBER ID EQUAL
%right EQUAL
%left ADD SUB
%left MUL DIV
%right UMINUS

%%

lines	:	lines statement ';'	{ printf("%f\n",$2); }
	|	lines ';'
	|
	;

statement   : ID EQUAL expr   { $$ = $3; int pos = lookup(idStr,Sym);
                            Sym[pos]->idValue = $3; }
    |   expr    { $$ = $1; }

expr	:	expr ADD expr	{ $$ = $1 + $3;}
	|	expr SUB expr	{ $$ = $1 - $3; }
	|	expr MUL expr	{ $$ = $1 * $3; }
	|	expr DIV expr	{ $$ = $1 / $3; }
	|	LEFTKUOHAO expr RIGHTKUOHAO	{ $$ = $2; }
	|	SUB expr %prec UMINUS	{ $$ = -$2; }
	|	NUMBER  { $$ = $1; }
    |   ID     
	;

%%

int yylex()
{
	int t;
    while(1){
        t = getchar();
        if(t==' '||t=='\t'||t=='\n'){}
        else if((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t == '_')){
            int ti = 0;
            while((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z' || (t == '_')) || (t >= '0' && t <= '9')){
                idStr[ti] = t;
                ti ++;
                t = getchar();
            }
            idStr[ti] = '\0';
            int pos = lookup(idStr,Sym);
            if (pos != -1){
                yylval = Sym[pos]->idValue;
            } else {
                struct Symbol *s = (struct Symbol*)malloc(sizeof(struct Symbol));
                strcpy(s->idName,idStr);
                s->idValue = 0;
                insert(s,Sym);
                yylval = 0;
            }
            ungetc(t,stdin);
            return ID;
        }
        else {
                if(isdigit(t)){
                yylval=0;
                while(isdigit(t)){
                    yylval = yylval * 10 + t - '0';
                    t = getchar();
                }
                ungetc(t,stdin);
                return NUMBER;
                }
                else {
                    switch(t){
                        case '+': return ADD;
                        case '-': return SUB;
                        case '*': return MUL;
                        case '/': return DIV;
                        case '(': return LEFTKUOHAO;
                        case ')': return RIGHTKUOHAO;
                        case '=': return EQUAL;
                        default: return t;
                    }
                }
        }
    }
}

int main(void)
{
    yyin = stdin;
    do{
        yyparse();
    } while(!feof(yyin));
	return 0;
}

void yyerror(const char* s){
    fprintf(stderr,"Parse error : %s\n",s);
    exit(1);
}

int lookup(char idStr[50],struct Symbol* Sym[1024]){
    for(int i=0;i<curSize;i++){
        if(strcmp(idStr,Sym[i]->idName)==0)
            return i;
    }
    return -1;
}

void insert(struct Symbol* s,struct Symbol* Sym[1024]){
    Sym[curSize] = s;
    curSize ++;
}