#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    fprintf(yyout,"I am removing function name:");
    fprintf(yyout,"%s \n",(dynamic_cast<IdentifierSymbolEntry*>(sym_ptr))->name.c_str());
    fprintf(yyout,"----------Before Remove---------- \n");
    for(auto &func:parent->func_list)
    {
        fprintf(yyout,"%s ",(dynamic_cast<IdentifierSymbolEntry*>(func->sym_ptr))->name.c_str());
    }
    fprintf(yyout,"\n");
    auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);
    fprintf(yyout,"----------After Remove---------- \n");
    for(auto &func:parent->func_list)
    {
        fprintf(yyout,"%s ",(dynamic_cast<IdentifierSymbolEntry*>(func->sym_ptr))->name.c_str());
    }
    fprintf(yyout,"\n");
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    // printf(yyout,"}\n");
    fprintf(yyout, "}\n");
}
