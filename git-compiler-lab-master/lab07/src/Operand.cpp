#include "Operand.h"
#include <string.h>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include "Instruction.h"

std::string Operand::toStr() const
{
    std::string res_zhouyanlin = se->toStr(); // llvm ir global variable with `@0`
    if (typeid(*se) == typeid(IdentifierSymbolEntry))
    {
        if (((IdentifierSymbolEntry *)se)->isGlobal())
        {
            res_zhouyanlin = "@" + res_zhouyanlin;
        }
    }
    return res_zhouyanlin;
}

void Operand::removeUse(Instruction *inst)
{
    auto i_zhouyanlin = std::find(uses.begin(), uses.end(), inst);
    if (i_zhouyanlin != uses.end())
        uses.erase(i_zhouyanlin);
}

void Operand::removeDef(Instruction *inst)
{
    def = nullptr;
}

Operand::Operand(const Operand &o)
{
    if (o.se->isTemporary())
    {
        se = new TemporarySymbolEntry(o.se->getType(), SymbolTable::getLabel());
    }
}

std::pair<int, int> Operand::getInitLatticeValue()
{
    if (this->isConst())
    {
        return {0, ((ConstantSymbolEntry *)se)->getValue()};
    }
    else if (se->isTemporary())
    {
        if (this->isParam())
            return {-1, 0};
        return {1, 0};
    }
    else if (se->isVariable())
    {
        if (((IdentifierSymbolEntry *)se)->isConstant())
        {
            return {0, ((IdentifierSymbolEntry *)se)->getValue()};
        }
        if (this->isGlobal())
        {
            PointerType *type_zhouyanlin = (PointerType *)(this->getType());
            if (type_zhouyanlin->getType()->isInt() && ((IntType *)type_zhouyanlin->getType())->isConst())
            {
                return {0, ((IdentifierSymbolEntry *)se)->getValue()};
            }
            else if (type_zhouyanlin->getType()->isFloat() && ((FloatType *)type_zhouyanlin->getType())->isConst())
            {
                return {0, ((IdentifierSymbolEntry *)se)->getValue()};
            }
        }
    }
    return {-1, 0};
}

bool Operand::isSSAName()
{
    if (se->isTemporary())
    {
        return true;
    }
    else if (se->isVariable() || this->isGlobal())
    {
        if (se->getType()->isArray())
        {
            return false;
        }
        return true;
    }
    return false;
}

bool Operand::isConArray()
{
    if (this->getDef() && this->getDef()->isGep())
    {
        return ((ArrayType *)(((PointerType *)(this->getDef()->getUse()[0]->getEntry()->getType()))->getType()))->isConst();
    }
    return false;
}
