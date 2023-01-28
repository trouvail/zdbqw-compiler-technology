#ifndef __GRAPHCOLOR_H__
#define __GRAPHCOLOR_H__

#include <map>
#include "Unit.h"

struct Web
{
    std::set<MachineOperand *> defs;
    std::set<MachineOperand *> uses;
    bool spill;
    double spillCost;
    int sreg;
    int disp;
    int rreg;
    bool fpu;
};

class GraphColor
{
private:
    MachineUnit *unit;
    int regNum, fRegNum;
    double defWeight, useWeight, copyWeight;
    MachineFunction *func;
    std::map<MachineOperand *, std::set<MachineOperand *> > du_chains;
    std::vector<Web *> webs;
    std::map<MachineOperand *, int> operand2web;
    std::vector<std::vector<bool> > matrix;
    std::vector<std::vector<int> > list;
    std::vector<std::vector<int> > removeList;
    std::stack<int> stk;
    void makeDuChains();
    void constructWebs();
    void constructMatrix();
    void constructList();
    bool coalesceRegs();
    void computeSpillCost();
    void simplifyGraph();
    void delOneNode(int i);
    bool assignRegs();
    int allocColor(int r);
    void modifyCode();
    void genSpillCode();

public:
    GraphColor(MachineUnit *unit);
    void allocateRegisters();
};
#endif