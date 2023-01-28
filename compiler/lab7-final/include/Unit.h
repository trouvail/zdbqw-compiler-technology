#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "Function.h"
#include "SymbolTable.h"

class Unit {
    typedef std::vector<Function*>::iterator iterator;
    typedef std::vector<Function*>::reverse_iterator reverse_iterator;

   private:
    std::vector<SymbolEntry*> global_list;
    std::vector<Function*> func_list;
    std::vector<SymbolEntry*> declare_list;
    Function* main;

   public:
    Unit() = default;
    ~Unit();
    void insertFunc(Function*);
    void removeFunc(Function*);
    void insertGlobal(SymbolEntry*);
    void insertDeclare(SymbolEntry*);
    void output() const;
    iterator begin() { return func_list.begin(); };
    iterator end() { return func_list.end(); };
    reverse_iterator rbegin() { return func_list.rbegin(); };
    reverse_iterator rend() { return func_list.rend(); };
    void genMachineCode(MachineUnit* munit);
    std::vector<SymbolEntry*> getGlobals() { return global_list; }
    Function* getMain() { return main; }
};

#endif
