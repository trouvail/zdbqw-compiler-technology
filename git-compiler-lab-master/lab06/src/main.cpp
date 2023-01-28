#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Ast.h"
#include "Unit.h"
#include "Type.h"
#include "SymbolTable.h"
using namespace std;

Ast ast;
Unit unit;
extern FILE *yyin;
extern FILE *yyout;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "iato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_ast = true;
            break;
        case 't':
            dump_tokens = true;
            break;
        case 'i':
            dump_ir = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    FunctionType *intType = new FunctionType(TypeSystem::intType, {});
    FunctionType *voidType = new FunctionType(TypeSystem::voidType, {});
    SymbolEntry *getint = new IdentifierSymbolEntry(intType, "getint", identifiers->getLevel());
    SymbolEntry *getch = new IdentifierSymbolEntry(intType, "getch", identifiers->getLevel());
    SymbolEntry *putint = new IdentifierSymbolEntry(voidType, "putint", identifiers->getLevel());
    SymbolEntry *putch = new IdentifierSymbolEntry(voidType, "putch", identifiers->getLevel());
    identifiers->install("getint", getint);
    identifiers->install("getch", getch);
    identifiers->install("putint", putint);
    identifiers->install("putch", putch);
    yyparse();
    if(dump_ast)
    {
        ast.output();
    }
    //printf("Hello2");
    ast.typeCheck(nullptr);
    ast.genCode(&unit);
    //printf("In main to verify the link \n");;
    
    // for (auto &func : unit.func_list)
    // {
    //     // printf(func-)
    //     printf("%s \n",(dynamic_cast<IdentifierSymbolEntry*>(func->sym_ptr))->name.c_str());
    // }
    if(dump_ir)
        unit.output();
    // for (auto &func : unit.func_list)
    // {
    //     // printf(func-)
    //     //printf("%s \n",(dynamic_cast<IdentifierSymbolEntry*>(func->sym_ptr))->name.c_str());
    // }
    return 0;
}
