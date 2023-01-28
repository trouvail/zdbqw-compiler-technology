#include "Unit.h"
#include <algorithm>
#include "Ast.h"
#include "SymbolTable.h"
#include "Type.h"
#include <stack>
#include <string>

// #include "MachineCode.h"

extern FILE *yyout;

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
    if (f->getSymPtr()->toStr() == "@main")
        main = f;
}

void Unit::genMachineCode(MachineUnit *munit_zhouyanlin)
{
    AsmBuilder *builder_zhouyanlin = new AsmBuilder();
    builder_zhouyanlin->setUnit(munit_zhouyanlin);
    for (auto &func_zhouyanlin : func_list)
        func_zhouyanlin->genMachineCode(builder_zhouyanlin);
}

Unit::~Unit()
{
    for (auto &func_zhouyanlin : func_list)
        delete func_zhouyanlin;
}

void Unit::insertDeclare(SymbolEntry *se_zhouyanlin)
{
    auto it = std::find(declare_list.begin(), declare_list.end(), se_zhouyanlin);
    if (it == declare_list.end())
    {
        declare_list.push_back(se_zhouyanlin);
    }
}

void Unit::output() const
{
    for (auto se_zhouyanlin : global_list)
    {
        if (se_zhouyanlin->getType()->isInt())
        {
            fprintf(yyout, "@%s = global %s %d, align 4\n", se_zhouyanlin->toStr().c_str(),
                    se_zhouyanlin->getType()->toStr().c_str(),
                    (int)((IdentifierSymbolEntry *)se_zhouyanlin)->getValue());
        }
        else if (se_zhouyanlin->getType()->isFloat())
        {
            double temp = (float)(((IdentifierSymbolEntry *)se_zhouyanlin)->getValue());
            uint64_t val = reinterpret_cast<uint64_t &>(temp);

            fprintf(yyout, "@%s = global %s 0x%lX, align 4\n",
                    se_zhouyanlin->toStr().c_str(), se_zhouyanlin->getType()->toStr().c_str(), val);
        }
        else if (se_zhouyanlin->getType()->isArray())
        {
            ArrayType *type = (ArrayType *)(se_zhouyanlin->getType());

            double *val = ((IdentifierSymbolEntry *)se_zhouyanlin)->getArrayValue();
            int i = 0;
            fprintf(yyout, "@%s = global ", se_zhouyanlin->toStr().c_str());
            if (((IdentifierSymbolEntry *)se_zhouyanlin)->isAllZero())
            {
                fprintf(yyout, "%s zeroinitializer", type->toStr().c_str());
            }
            else
            {
                std::stack<ArrayType *> stk;
                std::stack<int> stk1;
                stk.push(type);
                stk1.push(0);
                ArrayType *temp;
                while (!stk.empty())
                {
                    temp = stk.top();
                    Type *elemType = temp->getElementType();
                    if (elemType->isInt() || elemType->isFloat())
                    {
                        fprintf(yyout, "%s [", temp->toStr().c_str());
                        for (int j = 0; j < temp->getLength(); j++)
                        {
                            if (j != 0)
                                fprintf(yyout, ", ");
                            if (elemType->isInt())
                            {
                                fprintf(yyout, "i32 %d", (int)val[i++]);
                            }
                            else if (elemType->isFloat())
                            {
                                fprintf(yyout, "float %f", (float)val[i++]);
                            }
                        }
                        fprintf(yyout, "]");
                        stk1.pop();
                        stk.pop();
                        if (stk.empty())
                            break;
                        stk1.top()++;
                        continue;
                    }

                    if (stk1.top() != temp->getLength())
                    {
                        stk.push((ArrayType *)(temp->getElementType()));
                        if (stk1.top() == 0)
                            fprintf(yyout, "%s [", temp->toStr().c_str());
                        if (stk1.top() != 0)
                            fprintf(yyout, ", ");
                        stk1.push(0);
                    }
                    else
                    {
                        fprintf(yyout, "]");
                        stk.pop();
                        stk1.pop();
                        if (stk.empty())
                            break;
                        stk1.top()++;
                        continue;
                    }
                }
            }
            fprintf(yyout, ", align 4\n");
        }
    }

    for (auto &func : func_list)
        func->output();
    for (auto se : declare_list)
    {
        FunctionType *type = (FunctionType *)(se->getType());
        std::string str = type->toStr();
        std::string name = str.substr(0, str.find('('));
        std::string param = str.substr(str.find('('));
        fprintf(yyout, "declare %s %s%s\n", type->getRetType()->toStr().c_str(),
                se->toStr().c_str(), param.c_str());
    }
}

void Unit::removeFunc(Function *func)
{
    auto it_zhouyanlin = std::find(func_list.begin(), func_list.end(), func);
    if (it_zhouyanlin != func_list.end())
        func_list.erase(it_zhouyanlin);
}

void Unit::insertGlobal(SymbolEntry *se_zhouyanlin)
{
    global_list.push_back(se_zhouyanlin);
}
