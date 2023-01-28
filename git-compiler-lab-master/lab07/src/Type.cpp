#include "Type.h"
#include <assert.h>
#include <sstream>

IntType TypeSystem::commonBool = IntType(1);
VoidType TypeSystem::commonVoid = VoidType();
FloatType TypeSystem::commonFloat = FloatType(32);
FloatType TypeSystem::commonConstFloat = FloatType(32, true);

IntType TypeSystem::commonConstInt = IntType(32, true);
IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::commonInt8 = IntType(8);


Type *TypeSystem::floatType = &commonFloat;
Type *TypeSystem::constFloatType = &commonConstFloat;





Type *TypeSystem::constIntType = &commonConstInt;
Type *TypeSystem::intType = &commonInt;
Type *TypeSystem::int8Type = &commonInt8;
Type *TypeSystem::voidType = &commonVoid;
Type *TypeSystem::boolType = &commonBool;

std::string IntType::toStr()
{
    std::ostringstream buffer_zyl;
    if (constant)
        buffer_zyl << "i";
    else
        buffer_zyl << "i";
    buffer_zyl << size;
    return buffer_zyl.str();
}

std::string StringType::toStr()
{
    std::ostringstream buffer_zyl;
    buffer_zyl << "const char[" << length << "]";
    return buffer_zyl.str();
}

std::string PointerType::toStr()
{
    std::ostringstream buffer_zyl;
    buffer_zyl << valueType->toStr() << "*";
    return buffer_zyl.str();
}

bool Type::isPtr2Array()
{
    if (isPtr())
        return ((PointerType *)this)->getType()->isArray();
    return false;
}

std::string ArrayType::toStr()
{
    std::vector<std::string> vec_zyl;
    Type *temp_zyl = this;
    int count = 0;
    bool flag = false;
    while (temp_zyl && temp_zyl->isArray())
    {
        std::ostringstream buffer;
        if (((ArrayType *)temp_zyl)->getLength() == -1)
        {
            flag = true;
        }
        else
        {
            buffer << "[" << ((ArrayType *)temp_zyl)->getLength() << " x ";
            count++;
            vec_zyl.push_back(buffer.str());
        }
        temp_zyl = ((ArrayType *)temp_zyl)->getElementType();
    }
    std::ostringstream buffer;
    for (auto it = vec_zyl.begin(); it != vec_zyl.end(); it++)
        buffer << *it;
    if (temp_zyl->isInt())
    {
        buffer << "i32";
    }
    else if (temp_zyl->isFloat())
    {
        buffer << "float";
    }
    else
    {
        assert(false); 
    }
    while (count--)
        buffer << ']';
    if (flag)
        buffer << '*';
    return buffer.str();
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer_zyl;
    buffer_zyl << returnType->toStr() << "(";
    for (auto it_zyl = paramsType.begin(); it_zyl != paramsType.end(); it_zyl++)
    {
        buffer_zyl << (*it_zyl)->toStr();
        if (it_zyl + 1 != paramsType.end())
            buffer_zyl << ", ";
    }
    buffer_zyl << ')';
    return buffer_zyl.str();
}


std::string FloatType::toStr()
{
    return "float";
}

std::string VoidType::toStr()
{
    return "void";
}
