#include "SymbolTable.h"


#include <iostream>
#include <sstream>
#include "Type.h"




#include <iomanip>


bool SymbolEntry::setNext(SymbolEntry *se_zhouyanlin)
{
    SymbolEntry *s_zhouyanlin = this;
    long unsigned int cnt =
        ((FunctionType *)(se_zhouyanlin->getType()))->getParamsType().size();
    if (cnt == ((FunctionType *)(s_zhouyanlin->getType()))->getParamsType().size())
        return false;
    while (s_zhouyanlin->getNext())
    {
        if (cnt == ((FunctionType *)(s_zhouyanlin->getType()))->getParamsType().size())
            return false;
        s_zhouyanlin = s_zhouyanlin->getNext();
    }
    if (s_zhouyanlin == this)
    {
        this->next = se_zhouyanlin;
    }
    else
    {
        s_zhouyanlin->setNext(se_zhouyanlin);
    }
    return true;
}

SymbolEntry::SymbolEntry(Type *type_zhouyanlin, int kind_zhouyanlin)
{
    this->type = type_zhouyanlin;
    this->kind = kind_zhouyanlin;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, double value)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isInt() || type->isFloat());
    this->value = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, std::string value)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isString());
    this->strValue = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    
}

double ConstantSymbolEntry::getValue() const
{
    assert(type->isInt() || type->isFloat());
    return value;
}

std::string ConstantSymbolEntry::getStrValue() const
{
    assert(type->isString());
    return strValue;
}





bool SymbolTable::install(std::string name_zhouyanlin, SymbolEntry *entry_zhouyanlin)
{
    if (this->symbolTable.find(name_zhouyanlin) != this->symbolTable.end())
    {
        SymbolEntry *se_zhouyanlin = this->symbolTable[name_zhouyanlin];
        if (se_zhouyanlin->getType()->isFunc())
            return se_zhouyanlin->setNext(entry_zhouyanlin);
        return false;
    }
    else
    {
        symbolTable[name_zhouyanlin] = entry_zhouyanlin;
        return true;
    }
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;

void IdentifierSymbolEntry::setValue(double value)
{
    if (((IntType *)(this->getType()))->isConst())
    { 
        if (!initial)
        {
            this->value = value;
            initial = true;
        }
        else
        {
            fprintf(stderr, "trying to set value for constant.\n");
        }
    }
    else
    {
        this->value = value;
    }
}

void IdentifierSymbolEntry::setArrayValue(double *arrayValue)
{
    if (((ArrayType *)(this->getType()))->isConst())
    {
        if (!initial)
        {
            this->arrayValue = arrayValue;
            initial = true;
        }
        else
        {
            fprintf(stderr, "trying to set value for constant.\n");
        }
    }
    else
    {
        this->arrayValue = arrayValue;
    }
}

std::string IdentifierSymbolEntry::toStr()
{
    std::ostringstream buffer_zhouyanlin;
    if (label < 0)
    {
        if (type->isFunc())
            buffer_zhouyanlin << '@';
        buffer_zhouyanlin << name;
    }
    else
        buffer_zhouyanlin << "%t" << label;
    return buffer_zhouyanlin.str();
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label)
    : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer_zhouyanlin;
    buffer_zhouyanlin << "%t" << label;
    return buffer_zhouyanlin.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer_zhouyanlin;
    if (type->isInt())
    {
        if (value == 2147483648)
            buffer_zhouyanlin << "2147483648";
        else
            buffer_zhouyanlin << (int)value;
    }
    else if (type->isFloat())
        buffer_zhouyanlin << std::fixed << value;
    else if (type->isString())
        buffer_zhouyanlin << strValue;
    return buffer_zhouyanlin.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type,
                                             std::string name,
                                             int scope,
                                             int paramNo,
                                             bool sysy)
    : SymbolEntry(type, SymbolEntry::VARIABLE),
      name(name),
      sysy(sysy),
      paramNo(paramNo)
{
    this->scope = scope;
    this->initial = false;
    this->label = -1;
    this->allZero = false;
    this->constant = false;
    this->notZeroNum = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

SymbolEntry *SymbolTable::lookup(std::string name)
{
    SymbolTable *table_zhouyanlin = this;
    while (table_zhouyanlin != nullptr)
        if (table_zhouyanlin->symbolTable.find(name) != table_zhouyanlin->symbolTable.end())
        {
            return table_zhouyanlin->symbolTable[name];
        }
        else
        {
            table_zhouyanlin = table_zhouyanlin->prev;
        }
    return nullptr;
}
