#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;

class IRBuilder
{
private:
    Unit *unit;
    BasicBlock *insertBB;

public:
    IRBuilder(Unit *unit) : unit(unit){};
    Unit *getUnit() { return unit; };
    BasicBlock *getInsertBB() { return insertBB; };
    void setInsertBB(BasicBlock *bb_zyl) { insertBB = bb_zyl; };
    
};

#endif