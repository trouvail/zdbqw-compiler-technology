#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <assert.h>
#include <iostream>
#include <map>
#include <string>
class Type;
class Operand;
class Function;

class SymbolEntry
{
private:
    int kind;
    SymbolEntry *next;

protected:
    enum
    {
        CONSTANT,
        VARIABLE,
        TEMPORARY
    };
    Type *type;
    int label;

public:
    SymbolEntry(Type *type, int kind);
    virtual ~SymbolEntry(){};
    void setType(Type *type_zyl) { this->type = type_zyl; };
    virtual std::string toStr() = 0;
    bool setNext(SymbolEntry *se);
    SymbolEntry *getNext() const { return next; };
    bool isTemporary() const { return kind == TEMPORARY; };
    bool isVariable() const { return kind == VARIABLE; };
    Type *getType() { return type; };
    bool isConstant() const { return kind == CONSTANT; };
    int getLabel() const { return label; }
    
};

class SymbolTable
{
private:
    std::map<std::string, SymbolEntry *> symbolTable;
    SymbolTable *prev;
    int level;
    static int counter;

public:
    SymbolTable();
    SymbolTable(SymbolTable *prev);
    int getLevel() { return level; };
    bool install(std::string name, SymbolEntry *entry);
    SymbolEntry *lookup(std::string name);
    SymbolTable *getPrev() { return prev; };
    static int getLabel() { return counter++; };
    static void resetLabel() { counter = 0; };
    
};

class ConstantSymbolEntry : public SymbolEntry
{
private:
    double value;
    std::string strValue;

public:
    ConstantSymbolEntry(Type *type, double value);
    virtual ~ConstantSymbolEntry(){};
    double getValue() const;
    std::string getStrValue() const;
    std::string toStr();
    ConstantSymbolEntry(Type *type, std::string strValue);
    ConstantSymbolEntry(Type *type);
  
};

class IdentifierSymbolEntry : public SymbolEntry
{
private:
    enum
    {
        GLOBAL,
        PARAM,
        LOCAL
    };
    std::string name;
    int notZeroNum;
    Function *func = nullptr;
    int scope;
    double value;
    bool allZero;
    bool initial;
    bool sysy;
    double *arrayValue;
    int paramNo;
    bool constant;
    Operand *addr;
    


    int allParamNo;
    int stackParamNo;
    int intParamNo;
    int floatParamNo;

public:
    IdentifierSymbolEntry(Type *type,
                          std::string name,
                          int scope,
                          int paramNo = -1,
                          bool sysy = false);
    virtual ~IdentifierSymbolEntry(){};
    std::string toStr();
    bool isGlobal() const { return scope == GLOBAL; };

    bool isParam() const { return scope == PARAM; };
    bool isLocal() const { return scope >= LOCAL; };
    void setAddr(Operand *addr_zyl) { this->addr = addr_zyl; };
    Operand *getAddr() { return addr; };
    void setValue(double value);


    bool getConst() const { return constant; };
    int getNotZeroNum() const { return notZeroNum; }
    void setNotZeroNum(int num_zyl) { notZeroNum = num_zyl; }
    int getFloatParamNo() const { return floatParamNo; };


    int getStackParamNo() const { return stackParamNo; };
    void setAllParamNo(int no_zyl) { allParamNo = no_zyl; }
    void setIntParamNo(int no_zyl) { intParamNo = no_zyl; }
    void setFloatParamNo(int no_zyl) { floatParamNo = no_zyl; }
    void setStackParamNo(int no_zyl) { stackParamNo = no_zyl; }

    bool isSysy() const { return sysy; };
    double getValue() const { return value; };
    void setArrayValue(double *arrayValue);
    
    std::string getName() const { return name; }
    Function *getFunction() { return func; }
    void setFunction(Function *func_zyl) { this->func = func_zyl; }

    double *getArrayValue() const { return arrayValue; };
    int getLabel() const { return label; };
    void setLabel() { label = SymbolTable::getLabel(); };
    void setAllZero() { allZero = true; };



    bool isAllZero() const { return allZero; };
    int getParamNo() const { return paramNo; };
    int getAllParamNo() const { return allParamNo; };

    int getIntParamNo() const { return intParamNo; };
    int getScope() const { return scope; };
    
    
    
    
    void setConst() { constant = true; };
    

};

class TemporarySymbolEntry : public SymbolEntry
{
private:
    int stack_offset;

public:
    TemporarySymbolEntry(Type *type, int label);
    virtual ~TemporarySymbolEntry(){};
    int getOffset() { return this->stack_offset; };
    int getLabel() const { return label; };
    void setOffset(int offset_zyl) { this->stack_offset = offset_zyl; };
    std::string toStr();
    
    
    
};

extern SymbolTable *identifiers;
extern SymbolTable *globals;

#endif