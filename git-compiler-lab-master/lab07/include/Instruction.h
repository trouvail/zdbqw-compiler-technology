#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <map>



#include "AsmBuilder.h"
#include "Operand.h"
#include "Type.h"



#include <sstream>
#include <vector>


class SSAGraphNode;
class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    virtual std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &) { return {-1, 0}; };
    
    
    
    void remove();
    Instruction *getNext();
    Instruction *getPrev();
    virtual void output() const = 0;




    BasicBlock *getParent();
    bool isUncond() const { return instType == UNCOND; };
    bool isCond() const { return instType == COND; };
    bool isAlloc() const { return instType == ALLOCA; };




    MachineOperand *genMachineOperand(Operand *);
    MachineOperand *genMachineFloatOperand(Operand *);
    MachineOperand *genMachineReg(int reg);


    virtual std::vector<Operand *> getUse() { return std::vector<Operand *>(); }
    virtual Operand *getDef() { return nullptr; }
    virtual void replaceUse(Operand *old, Operand *new_) {}
    virtual void replaceDef(Operand *new_) {}






    bool isRet() const { return instType == RET; };
    bool isCall() const { return instType == CALL; }
    bool isStore() const { return instType == STORE; }
    bool isPhi() const { return instType == PHI; };





    bool isBin() const { return instType == BINARY; };
    bool isLoad() const { return instType == LOAD; };



    std::vector<Operand *> getOperands() { return operands; }
    virtual bool genNode() { return true; }
    SSAGraphNode *getNode() { return node; }



    MachineOperand *genMachineFReg(int freg);
    MachineOperand *genMachineVReg(bool fpu = false);
    MachineOperand *genMachineImm(int val);
    MachineOperand *genMachineLabel(int block_no);
    virtual void genMachineCode(AsmBuilder *) = 0;
    int getInstType() { return instType; }
    bool isEssential();
    void setMark() { mark = true; }
    void unsetMark() { mark = false; }



    bool isCmp() const { return instType == CMP; };
    bool isGep() const { return instType == GEP; };
    bool isXor() const { return instType == XOR; };
    bool isBitcast() const { return instType == BITCAST; };
    bool isShl() const { return instType == SHL; };
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);




    bool isMark() const { return mark; }
    
    void setNode(SSAGraphNode *node_zyl) { this->node = node_zyl; }
    bool reGenNode();




    bool hasEqualOp(Instruction *in_zyl) const
    {
        if ((int)instType == in_zyl->getInstType())
            if ((int)opcode == in_zyl->getOpcode())
                return true;
        return false;
    }



    virtual std::string getHash() { return ""; }
    bool isIntMul();
    bool isIntDiv();
    virtual bool isConstExp() { return false; }
    double getConstVal() { return constVal; }
    bool isComAndAsso()
    {
        if (isBin() && opcode >= 1 && opcode <= 4)
            return true;
        return false;
    }




    bool isAdd() { return isBin() && opcode == 1; }
    bool isSub() { return isBin() && opcode == 0; }
    bool isAddZero();
    // shallow copy
    virtual Instruction *copy() = 0;
    virtual void setDef(Operand *def) {}


    int getOpcode() const { return opcode; }
    
    
    enum
    {
        BINARY,
        COND,
        UNCOND,
        RET,
        LOAD,
        STORE,
        CMP,
        ALLOCA,
        CALL,
        ZEXT,
        XOR,
        GEP,
        PHI,
        FPTOSI, // floating point to signed int
        SITOFP, // signed int to floating point
        BITCAST,
        SHL,
        ASHR,
    };

protected:
    unsigned instType;


    BasicBlock *parent;
    std::vector<Operand *> operands;
    bool mark;
    SSAGraphNode *node;
    double constVal;



    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr){};
    void output() const {};
    void genMachineCode(AsmBuilder *){};
    Instruction *copy() { return nullptr; }
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst,
                      SymbolEntry *se,
                      BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    void genMachineCode(AsmBuilder *);



    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }
    SymbolEntry *getEntry() { return se; }


    Operand *getDef() { return operands[0]; }
    void replaceDef(Operand *new_);
    bool isArray() { return se->getType()->isArray(); }
    bool genNode();
    Instruction *copy();
    

private:
    SymbolEntry *se;
};

class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst,
                    Operand *src_addr,
                    BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    void genMachineCode(AsmBuilder *);
    Operand *getDef() { return operands[0]; }


    bool genNode();
    std::string getHash();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }



    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr,
                     Operand *src,
                     BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;



    Operand *getSrc() { return operands[1]; }
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[0], operands[1]});
    }

    void genMachineCode(AsmBuilder *);
    void replaceUse(Operand *old, Operand *new_);
    Operand *getDstAddr() { return operands[0]; }
    
    Instruction *copy();
};

class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode,
                      Operand *dst,
                      Operand *src1,
                      Operand *src2,
                      BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();




    bool isDivConst() { return opcode == DIV && operands[2]->isConst(); }
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1], operands[2]});
    }
    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    std::string getHash();
    bool isConstExp();




    void output() const;
    void genMachineCode(AsmBuilder *);
    enum
    {
        SUB,
        ADD,
        AND,
        OR,
        MUL,
        DIV,
        MOD
    };
    Operand *getDef() { return operands[0]; }
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);




    bool isAdd() { return this->opcode == ADD; };
    bool isSub() { return this->opcode == SUB; };
    bool isMul() { return this->opcode == MUL; };


    
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }
    
    bool isDIV() { return this->opcode == DIV; };
    bool genNode();
    Instruction *copy();
};

class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode,
                   Operand *dst,
                   Operand *src1,
                   Operand *src2,
                   BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();




    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1], operands[2]});
    }
    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    bool genNode();
    bool reGenNode();
    std::string getHash();
    bool isConstExp();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }




    void output() const;
    void genMachineCode(AsmBuilder *);
    enum
    {
        E,
        NE,
        L,
        LE,
        G,
        GE
    };
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    Operand *getDef() { return operands[0]; }
    
    void swapSrc()
    {
        switch (opcode)
        {
        case L:
            opcode = G;
            break;
        case LE:
            opcode = GE;
            break;
        case G:
            opcode = L;
            break;
        case GE:
            opcode = LE;
            break;
        }
        std::swap(operands[1], operands[2]);
    }
};

class UncondBrInstruction : public Instruction
{
private:
    bool noStraighten;

public:
    UncondBrInstruction(BasicBlock *, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    void genMachineCode(AsmBuilder *);
    Instruction *copy();
    void setNoStraighten() { noStraighten = true; }
    bool isNoStraighten() { return noStraighten; }

protected:
    BasicBlock *branch;
};

// conditional branch
class CondBrInstruction : public Instruction
{
private:
    BasicBlock *originTrue;
    BasicBlock *originFalse;

public:
    CondBrInstruction(BasicBlock *,
                      BasicBlock *,
                      Operand *,
                      BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;




    BasicBlock *getOriginTrue() { return originTrue; }
    BasicBlock *getOriginFalse() { return originFalse; }
    void cleanOriginTrue() { originTrue = nullptr; }
    void cleanOriginFalse() { originFalse = nullptr; }
    void setOriginTrue(BasicBlock *block) { originTrue = block; }
    void setOriginFalse(BasicBlock *block) { originFalse = block; }




    void setTrueBranch(BasicBlock *);
    BasicBlock *getTrueBranch();
    void setFalseBranch(BasicBlock *);
    BasicBlock *getFalseBranch();
    void genMachineCode(AsmBuilder *);
    void replaceUse(Operand *old, Operand *new_);
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[0]});
    }
    Instruction *copy();
    

protected:
    BasicBlock *true_branch;
    BasicBlock *false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    void output() const;




    std::vector<Operand *> getUse()
    {
        if (operands.size())
            return std::vector<Operand *>({operands[0]});
        return std::vector<Operand *>();
    }
    Instruction *copy();

    void genMachineCode(AsmBuilder *);
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    
};

class CallInstruction : public Instruction
{
private:
    SymbolEntry *func;
    Operand *dst;

public:
    CallInstruction(Operand *dst,
                    SymbolEntry *func,
                    std::vector<Operand *> params,
                    BasicBlock *insert_bb = nullptr);
    ~CallInstruction();
    void output() const;


    SymbolEntry *getFuncSE() { return func; }
    bool genNode();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        dst = def_zyl;
        def_zyl->setDef(this);
    }
    // used for auto inline
    std::string getHash();
    void addPred();


    void genMachineCode(AsmBuilder *);
    Operand *getDef() { return operands[0]; }
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    std::vector<Operand *> getUse()
    {
        std::vector<Operand *> vec;
        for (auto it = operands.begin() + 1; it != operands.end(); it++)
            vec.push_back(*it);
        return vec;
    }
    
};

class ZextInstruction : public Instruction
{
public:
    ZextInstruction(Operand *dst,
                    Operand *src,
                    BasicBlock *insert_bb = nullptr);
    ~ZextInstruction();




    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    bool genNode();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }


    void output() const;
    void genMachineCode(AsmBuilder *);
    Operand *getDef() { return operands[0]; }
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    
};

class XorInstruction : public Instruction
{
public:
    XorInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~XorInstruction();


    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    bool genNode();


    void output() const;

    std::string getHash();
    bool isConstExp();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }


    void genMachineCode(AsmBuilder *);
    Operand *getDef() { return operands[0]; }
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    
    
};

class GepInstruction : public Instruction
{
private:
    bool paramFirst;
    bool first;
    bool last;
    Operand *init;
    int off;
    
    bool noAsm;

public:
    GepInstruction(Operand *dst,
                   Operand *arr,
                   Operand *idx,
                   BasicBlock *insert_bb = nullptr,
                   bool paramFirst = false);
    ~GepInstruction();



    void setInit(Operand *init_zyl, int off_zyl = 0)
    {
        this->init = init_zyl;
        this->off = off_zyl;
    };
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1], operands[2]});
    }



    void output() const;



    bool genNode();
    std::string getHash();
    bool hasNoAsm() { return noAsm; }
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }


    void genMachineCode(AsmBuilder *);
    void setFirst() { first = true; };
    void setLast() { last = true; };
    Operand *getInit() const { return init; };
    
    Operand *getDef() { return operands[0]; }
    void replaceDef(Operand *new_);
    void replaceUse(Operand *old, Operand *new_);
    
};

class PhiInstruction : public Instruction
{
private:
    Operand *originDef;
    Operand *dst;
    std::map<BasicBlock *, Operand *> srcs;

public:
    PhiInstruction(Operand *dst, BasicBlock *insert_bb = nullptr);
    ~PhiInstruction();
    void output() const;


    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    Operand *getOriginDef() { return originDef; }
    void replaceOriginDef(Operand *new_);
    void changeSrcBlock(std::map<BasicBlock *, std::vector<BasicBlock *>> changes,
                        bool flag = false);
    std::vector<Operand *> getUse()
    {
        std::vector<Operand *> ret;
        for (auto ope : operands)
            if (ope != operands[0])
                ret.push_back(ope);
        return ret;
    }



    void addSrc(BasicBlock *block, Operand *src);
    Operand *getSrc(BasicBlock *block);
    Operand *getDef() { return dst; }
    void genMachineCode(AsmBuilder *) {}



    void setDef(Operand *def_zyl)
    {
        dst = def_zyl;
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }
    void removeSrc(BasicBlock *block);
    bool findSrc(BasicBlock *block);
    // only remove use in operands
    // used for starighten::checkphi
    void removeUse(Operand *use);
    // remove all use operands
    // used for auto inline
    void cleanUseInOperands();



    
    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    bool genNode();
    bool reGenNode();
    std::string getHash();
    std::map<BasicBlock *, Operand *> &getSrcs() { return srcs; }
    void cleanUse();
    Instruction *copy();
    
};

class FptosiInstruction : public Instruction
{
private:
    Operand *dst;
    Operand *src;

public:
    FptosiInstruction(Operand *dst,
                      Operand *src,
                      BasicBlock *insert_bb = nullptr);
    ~FptosiInstruction();

    Instruction *copy();

    void output() const;

    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    Operand *getDef() { return operands[0]; }
    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);



    void genMachineCode(AsmBuilder *);
    
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        dst = def_zyl;
        def_zyl->setDef(this);
    }
    
};

class SitofpInstruction : public Instruction
{
private:
    Operand *dst;
    Operand *src;

public:
    SitofpInstruction(Operand *dst,
                      Operand *src,
                      BasicBlock *insert_bb = nullptr);
    ~SitofpInstruction();
    void output() const;
    
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    Operand *getDef() { return operands[0]; }



    void genMachineCode(AsmBuilder *);
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        dst = def_zyl;
        def_zyl->setDef(this);
    }



    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
};

class BitcastInstruction : public Instruction
{
private:
    Operand *dst;
    Operand *src;
    bool flag;

public:
    BitcastInstruction(Operand *dst,
                       Operand *src,
                       BasicBlock *insert_bb = nullptr);
    ~BitcastInstruction();




    Instruction *copy();
    Operand *getDef() { return operands[0]; }
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        dst = def_zyl;
        def_zyl->setDef(this);
    }
    void replaceUse(Operand *old, Operand *new_);
    void setFlag() { flag = true; }
    bool getFlag() { return flag; }



    void output() const;
    void genMachineCode(AsmBuilder *);
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1]});
    }
    bool genNode();
    
};

class ShlInstruction : public Instruction
{
public:
    ShlInstruction(Operand *dst,
                   Operand *src,
                   Operand *num,
                   BasicBlock *insert_bb = nullptr);
    ~ShlInstruction();
    void output() const;

    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    bool isConstExp();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }


    void genMachineCode(AsmBuilder *);
    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1], operands[2]});
    }
    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    Operand *getDef() { return operands[0]; }
    bool genNode();
    std::string getHash();
    
};

class AshrInstruction : public Instruction
{
public:
    AshrInstruction(Operand *dst,
                    Operand *src,
                    Operand *num,
                    BasicBlock *insert_bb = nullptr);
    ~AshrInstruction();
    void output() const;
    void genMachineCode(AsmBuilder *);




    void replaceUse(Operand *old, Operand *new_);
    void replaceDef(Operand *new_);
    bool isConstExp();
    Instruction *copy();
    void setDef(Operand *def_zyl)
    {
        operands[0] = def_zyl;
        def_zyl->setDef(this);
    }


    std::vector<Operand *> getUse()
    {
        return std::vector<Operand *>({operands[1], operands[2]});
    }
    std::pair<int, int> getLatticeValue(std::map<Operand *, std::pair<int, int>> &);
    Operand *getDef() { return operands[0]; }
    bool genNode();
    std::string getHash();
    
};

#endif