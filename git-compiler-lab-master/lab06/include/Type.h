#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>
#include <string.h>

class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC, PTR, BOOL,CHAR};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isBool() const {return kind == BOOL;};
    bool isChar() const {return kind == CHAR;}
};

class BoolType : public Type
{
public:
    BoolType() : Type(Type::BOOL){};
    std::string toStr();
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    std::string toStr();
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    Type* getRetType() {return returnType;};
    std::vector<Type*> getParamsType() {return paramsType;}
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class CharType : public Type
{
private:
    // 此处设置大小为100
    char content[100];
public:
    CharType(char content[100]) : Type(Type::CHAR) {strcpy(this->content,content);};
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
    static CharType commonChar;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
    static Type *charType;
};

#endif
