#ifndef __ASMBUILDER_H__
#define __ASMBUILDER_H__

#include "MachineCode.h"

class AsmBuilder
{
private:

    MachineFunction *mFunction; 

    MachineBlock *mBlock;     



    MachineUnit *mUnit;        
    
    int cmpOpcode;              
public:
    void setUnit(MachineUnit *unit_zyl) { this->mUnit = unit_zyl; };
    
    void setFunction(MachineFunction *func_zyl) { this->mFunction = func_zyl; };
    void setBlock(MachineBlock *block_zyl) { this->mBlock = block_zyl; };
    void setCmpOpcode(int opcode_zyl) { this->cmpOpcode = opcode_zyl; };
    MachineUnit *getUnit() { return this->mUnit; };



    
    MachineFunction *getFunction() { return this->mFunction; };
    MachineBlock *getBlock() { return this->mBlock; };
    int getCmpOpcode() { return this->cmpOpcode; };

    
    
};

#endif