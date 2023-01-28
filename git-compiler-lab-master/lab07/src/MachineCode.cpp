#include "MachineCode.h"
#include <iostream>
#include <sstream>
#include "Type.h"
extern FILE *yyout;

using std::string;
using std::stringstream;

int MachineBlock::label = 0;

MachineOperand::MachineOperand(int tp_zhouyanlin, LL val, bool fpu)
{
    this->type = tp_zhouyanlin;
    this->fpu = fpu;
    if (tp_zhouyanlin == MachineOperand::IMM)
    {
        this->val = val;
    }
    else
    {
        this->reg_no = val; 
    }
}

MachineOperand::MachineOperand(int tp_zhouyanlin, int val, bool fpu)
{
    this->type = tp_zhouyanlin;
    this->fpu = fpu;
    if (tp_zhouyanlin == MachineOperand::IMM)
    {
        this->val = val;
    }
    else
    {
        this->reg_no = val; 
    }
}

MachineOperand::MachineOperand(std::string label_zhouyanlin)
{
    this->type = MachineOperand::LABEL;
    this->label = label_zhouyanlin;
}

MachineOperand::MachineOperand(int tp_zhouyanlin, float fval)
{
    this->type = tp_zhouyanlin;
    if (tp_zhouyanlin == MachineOperand::IMM)
    {
        this->fval = fval;
        this->fpu = true;
    }
}

uint32_t MachineOperand::getBinVal()
{
    uint32_t bin_val;
    if (fpu)
    {
        bin_val = reinterpret_cast<uint32_t &>(fval);
    }
    else
    {
        bin_val = reinterpret_cast<uint32_t &>(val);
    }
    return bin_val;
}

bool MachineOperand::operator==(const MachineOperand &a_zhouyanlin) const
{
    if (this->type != a_zhouyanlin.type)
        return false;
    if (this->fpu != a_zhouyanlin.fpu)
    {
        return false;
    }
    if (this->type == IMM)
    {
        if (this->fpu)
        {
            return this->fval == a_zhouyanlin.fval;
        }
        else
        {
            return this->val == a_zhouyanlin.val;
        }
    }
    else if (this->type == LABEL)
        return this->label == a_zhouyanlin.label;
    return this->reg_no == a_zhouyanlin.reg_no;
}

bool MachineOperand::operator<(const MachineOperand &a_zhouyanlin) const
{
    if (this->type == a_zhouyanlin.type)
    {
        if (this->type == IMM)
        {
            if (this->fpu && a_zhouyanlin.fpu)
            {
                return this->fval < a_zhouyanlin.fval;
            }
            else if (!this->fpu && !a_zhouyanlin.fpu)
            {
                return this->val < a_zhouyanlin.val;
            }
            else if (this->fpu && !a_zhouyanlin.fpu)
            {
                return this->fval < a_zhouyanlin.val;
            }
            else if (!this->fpu && a_zhouyanlin.fpu)
            {
                return this->val < a_zhouyanlin.fval;
            }
        }
        else if (this->type == LABEL)
        {
            return this->label < a_zhouyanlin.label;
        }
        return this->reg_no < a_zhouyanlin.reg_no;
    }
    return this->type < a_zhouyanlin.type;

    if (this->type != a_zhouyanlin.type)
        return false;
    if (this->type == IMM)
        return this->val == a_zhouyanlin.val;
    return this->reg_no == a_zhouyanlin.reg_no;
}

void MachineOperand::PrintReg()
{
    if (reg_no >= 16)
    {
        int sreg_no_zhouyanlin = reg_no - 16;
        if (sreg_no_zhouyanlin <= 31)
        {
            fprintf(yyout, "s%d", sreg_no_zhouyanlin);
        }
        else if (sreg_no_zhouyanlin == 32)
        {
            fprintf(yyout, "FPSCR");
        }
    }
    else if (reg_no == 11)
    {
        fprintf(yyout, "fp");
    }
    else if (reg_no == 13)
    {
        fprintf(yyout, "sp");
    }
    else if (reg_no == 14)
    {
        fprintf(yyout, "lr");
    }
    else if (reg_no == 15)
    {
        fprintf(yyout, "pc");
    }
    else
    {
        fprintf(yyout, "r%d", reg_no);
    }
}

void MachineOperand::output()
{
    switch (this->type)
    {
    case IMM:
        if (!fpu)
        {
            fprintf(yyout, "#%lld", this->val);
        }
        else
        {
            uint32_t temp = reinterpret_cast<uint32_t &>(this->fval);
            fprintf(yyout, "#%u", temp);
        }
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->label.c_str());
        else if (this->label.substr(0, 1) == "@")
            fprintf(yyout, "%s", this->label.c_str() + 1);
        else
            fprintf(yyout, "addr_%s%d", this->label.c_str(),
                    parent->getParent()->getParent()->getParent()->getN());
    default:
        break;
    }
}

int VNegMInstruction::latency()
{
    return 3;
}

FuseMInstruction::FuseMInstruction(MachineBlock *p,
                                   int op,
                                   MachineOperand *dst,
                                   MachineOperand *src1,
                                   MachineOperand *src2,
                                   MachineOperand *src3)
{
    this->parent = p;
    this->type = MachineInstruction::FUSE;
    this->op = op;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    this->use_list.push_back(src3);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
    src3->setParent(this);
    dst->setDef(this);
}
void FuseMInstruction::output()
{
    switch (this->op)
    {
    case FuseMInstruction::MLA:
        fprintf(yyout, "\tmla ");
        break;
    case FuseMInstruction::MLS:
        fprintf(yyout, "\tmls ");
        break;
    case FuseMInstruction::VMLA:
        fprintf(yyout, "\tvmla.f32 ");
        break;
    case FuseMInstruction::VMLS:
        fprintf(yyout, "\tvmls.f32 ");
        break;
    default:
        break;
    }

    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    if (this->op != FuseMInstruction::VMLA &&
        this->op != FuseMInstruction::VMLS)
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }
    fprintf(yyout, "\n");
}

BinaryMInstruction::BinaryMInstruction(MachineBlock *p,
                                       int op,
                                       MachineOperand *dst,
                                       MachineOperand *src1,
                                       MachineOperand *src2,
                                       int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
    dst->setDef(this);
}

void BinaryMInstruction::output()
{

    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        break;
    case BinaryMInstruction::VADD:
        fprintf(yyout, "\tvadd.f32 ");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        break;
    case BinaryMInstruction::VSUB:
        fprintf(yyout, "\tvsub.f32 ");
        break;
    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        break;
    case BinaryMInstruction::VMUL:
        fprintf(yyout, "\tvmul.f32 ");
        break;
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tsdiv ");
        break;
    case BinaryMInstruction::VDIV:
        fprintf(yyout, "\tvdiv.f32 ");
        break;
    default:
        break;
    }

    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

SmullMInstruction::SmullMInstruction(MachineBlock *p,
                                     MachineOperand *dst,
                                     MachineOperand *dst1,
                                     MachineOperand *src1,
                                     MachineOperand *src2,
                                     int cond)
{
    this->parent = p;
    this->type = MachineInstruction::SMULL;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->def_list.push_back(dst1);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    dst1->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
    dst->setDef(this);
    dst1->setDef(this);
}

void SmullMInstruction::output()
{
    fprintf(yyout, "\tumull ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->def_list[1]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

LoadMInstruction::LoadMInstruction(MachineBlock *p,
                                   int op,
                                   MachineOperand *dst,
                                   MachineOperand *src1,
                                   MachineOperand *src2,
                                   int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = op;
    this->cond = cond;
    this->needModify = false;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
    dst->setDef(this);
}

void LoadMInstruction::output()
{
    if (op == LoadMInstruction::LDR)
    {
        fprintf(yyout, "\tldr ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");

        if (this->use_list[0]->isImm())
        {
            if (this->use_list[0]->isFloat())
            {
                float fval_zhouyanlin = this->use_list[0]->getFVal();
                uint32_t temp_zhouyanlin = reinterpret_cast<uint32_t &>(fval_zhouyanlin);
                fprintf(yyout, "=%u\n", temp_zhouyanlin);
            }
            else
            {
                fprintf(yyout, "=%lld\n", this->use_list[0]->getVal());
            }
            return;
        }

        // Load address
        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "[");

        this->use_list[0]->output();
        if (this->use_list.size() > 1)
        {
            fprintf(yyout, ", ");
            this->use_list[1]->output();
        }

        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");
    }
    else if (op == LoadMInstruction::VLDR)
    {
        fprintf(yyout, "\tvldr.32 ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        // Load immediate num, eg: ldr r1, =8
        if (this->use_list[0]->isImm())
        {
            if (this->use_list[0]->isFloat())
            {
                float fval_zhouyanlin = this->use_list[0]->getFVal();
                uint32_t temp_zhouyanlin = reinterpret_cast<uint32_t &>(fval_zhouyanlin);
                fprintf(yyout, "=%u\n", temp_zhouyanlin);
            }
            else
            {
                fprintf(yyout, "=%lld\n", this->use_list[0]->getVal());
            }
            return;
        }

        // Load address
        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "[");

        this->use_list[0]->output();
        if (this->use_list.size() > 1)
        {
            fprintf(yyout, ", ");
            this->use_list[1]->output();
        }

        if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
            fprintf(yyout, "]");
        fprintf(yyout, "\n");
    }
}

StoreMInstruction::StoreMInstruction(MachineBlock *p,
                                     int op,
                                     MachineOperand *src1,
                                     MachineOperand *src2,
                                     MachineOperand *src3,
                                     int cond)
{
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3){
        this->use_list.push_back(src3);
    }
    src1->setParent(this);
    src2->setParent(this);
    if (src3){
        src3->setParent(this);
    }
}

void StoreMInstruction::output()
{
    if (op == StoreMInstruction::STR)
    {
        fprintf(yyout, "\tstr ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        if (this->use_list[1]->isReg()){
            fprintf(yyout, "[");
        }else if(this->use_list[1]->isVReg()){
            fprintf(yyout, "[");
        }
        this->use_list[1]->output();
        if (this->use_list.size() > 2)
        {
            fprintf(yyout, ", ");
            this->use_list[2]->output();
        }
        if (this->use_list[1]->isReg()){
            fprintf(yyout, "]\n");
        }else if(this->use_list[1]->isVReg()){
            fprintf(yyout, "]\n");
        }
    }
    else if (op == StoreMInstruction::VSTR)
    {
        fprintf(yyout, "\tvstr.32 ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        if (this->use_list[1]->isReg()){
            fprintf(yyout, "[");
        }else if(this->use_list[1]->isVReg()){
            fprintf(yyout, "[");
        }
        this->use_list[1]->output();
        if (this->use_list.size() > 2)
        {
            fprintf(yyout, ", ");
            this->use_list[2]->output();
        }
        if (this->use_list[1]->isReg()){
            fprintf(yyout, "]\n");
        }else if(this->use_list[1]->isVReg()){
            fprintf(yyout, "]\n");
        }
    }
}

MovMInstruction::MovMInstruction(MachineBlock *p,
                                 int op,
                                 MachineOperand *dst,
                                 MachineOperand *src,
                                 int cond,
                                 MachineOperand *num)
{
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    dst->setDef(this);
    src->setParent(this);
    if (num){
        this->use_list.push_back(num);
        num->setParent(this);
    }
}

void MovMInstruction::output()
{
    switch (this->op)
    {
    case MovMInstruction::MOV:
        fprintf(yyout, "\tmov");
        break;
    case MovMInstruction::MOVLSL:
        fprintf(yyout, "\tmov");
        break;
    case MovMInstruction::MOVLSR:
        fprintf(yyout, "\tmov");
        break;
    case MovMInstruction::MOVASR:
        fprintf(yyout, "\tmov");
        break;
    case MovMInstruction::MOVT:
        fprintf(yyout, "\tmovt");
        break;
    case MovMInstruction::VMOV:
        fprintf(yyout, "\tvmov");
        break;
    case MovMInstruction::VMOVF32:
        fprintf(yyout, "\tvmov.f32");
        break;
    default:
        break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    if (op == MOVLSL)
    {
        fprintf(yyout, ", LSL");
        this->use_list[1]->output();
    }else if (op == MOVLSR)
    {
        fprintf(yyout, ", LSR");
        this->use_list[1]->output();
    }else if (op == MOVASR)
    {
        fprintf(yyout, ", ASR");
        this->use_list[1]->output();
    }
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock *p,
                                       int op,
                                       MachineOperand *dst,
                                       int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;

    this->use_list.push_back(dst);
    dst->setParent(this);
    dst->setDef(this);

    if (op == BL)
    {
        auto label_zhouyanlin = dst->getLabel().substr(1);
        int intParamNo;

        auto r0def_zhouyanlin = new MachineOperand(MachineOperand::REG, 0);
        auto r1def_zhouyanlin = new MachineOperand(MachineOperand::REG, 1);
        auto r2def_zhouyanlin = new MachineOperand(MachineOperand::REG, 2);
        auto r3def_zhouyanlin = new MachineOperand(MachineOperand::REG, 3);
        r0def_zhouyanlin->setParent(this);
        r1def_zhouyanlin->setParent(this);
        r2def_zhouyanlin->setParent(this);
        r3def_zhouyanlin->setParent(this);
        this->def_list.push_back(r0def_zhouyanlin);
        this->def_list.push_back(r1def_zhouyanlin);
        this->def_list.push_back(r2def_zhouyanlin);
        this->def_list.push_back(r3def_zhouyanlin);
        auto s0def_zhouyanlin = new MachineOperand(MachineOperand::REG, 16, true);
        auto s1def_zhouyanlin = new MachineOperand(MachineOperand::REG, 17, true);
        auto s2def_zhouyanlin = new MachineOperand(MachineOperand::REG, 18, true);
        auto s3def_zhouyanlin = new MachineOperand(MachineOperand::REG, 19, true);
        s0def_zhouyanlin->setParent(this);
        s1def_zhouyanlin->setParent(this);
        s2def_zhouyanlin->setParent(this);
        s3def_zhouyanlin->setParent(this);
        this->def_list.push_back(s0def_zhouyanlin);
        this->def_list.push_back(s1def_zhouyanlin);
        this->def_list.push_back(s2def_zhouyanlin);
        this->def_list.push_back(s3def_zhouyanlin);

        if (label_zhouyanlin == "memset")
        {
            intParamNo = 3;
        }
        else
        {
            auto entry = (IdentifierSymbolEntry *)(identifiers->lookup(label_zhouyanlin));
            intParamNo = entry->getIntParamNo();
        }
        if (intParamNo > 0)
        {
            auto r0use = new MachineOperand(MachineOperand::REG, 0);
            r0use->setParent(this);
            this->use_list.push_back(r0use);
        }
        if (intParamNo > 1)
        {
            auto r1use = new MachineOperand(MachineOperand::REG, 1);
            r1use->setParent(this);
            this->use_list.push_back(r1use);
        }
        if (intParamNo > 2)
        {
            auto r2use = new MachineOperand(MachineOperand::REG, 2);
            this->use_list.push_back(r2use);
            r2use->setParent(this);
        }
        if (intParamNo > 3)
        {
            auto r3use = new MachineOperand(MachineOperand::REG, 3);
            r3use->setParent(this);
            this->use_list.push_back(r3use);
        }

        // 接下来是浮点数寄存器
        auto s0use = new MachineOperand(MachineOperand::REG, 16, true);
        s0use->setParent(this);
        this->use_list.push_back(s0use);

        auto s1use = new MachineOperand(MachineOperand::REG, 17, true);
        s1use->setParent(this);
        this->use_list.push_back(s1use);

        auto s2use = new MachineOperand(MachineOperand::REG, 18, true);
        s2use->setParent(this);
        this->use_list.push_back(s2use);

        auto s3use = new MachineOperand(MachineOperand::REG, 19, true);
        s3use->setParent(this);
        this->use_list.push_back(s3use);
    }
    else if (op == BX)
    {
        auto r0 = new MachineOperand(MachineOperand::REG, 0);
        auto s0 = new MachineOperand(MachineOperand::REG, 16, true);
        // 栈指针
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        r0->setParent(this);
        s0->setParent(this);
        sp->setParent(this);
        this->use_list.push_back(r0);
        this->use_list.push_back(s0);
        this->use_list.push_back(sp);
    }
}

void BranchMInstruction::output()
{
    switch (op)
    {
    case B:
        fprintf(yyout, "\tb");
        break;
    case BX:
        fprintf(yyout, "\tbx");
        break;
    case BL:
        fprintf(yyout, "\tbl");
        break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock *p,
                                 int op,
                                 MachineOperand *src1,
                                 MachineOperand *src2,
                                 int cond)
{
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = op;
    this->cond = cond;
    p->setCmpCond(cond);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    switch (this->op)
    {
    case CmpMInstruction::CMP:
        fprintf(yyout, "\tcmp ");
        break;
    case CmpMInstruction::VCMP:
        fprintf(yyout, "\tvcmp.f32 ");
        break;
    default:
        break;
    }
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstruction::StackMInstruction(MachineBlock *p,
                                     int op,
                                     std::vector<MachineOperand *> srcs,
                                     MachineOperand *src,
                                     MachineOperand *src1,
                                     int cond)
{
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    if (srcs.size())
    {
        for (auto it_zhouyanlin = srcs.begin(); it_zhouyanlin != srcs.end(); it_zhouyanlin++)
        {
            this->use_list.push_back(*it_zhouyanlin);
        }
    }
    if (src)
    {
        this->use_list.push_back(src);
        src->setParent(this);
    }
    if (src1)
    {
        this->use_list.push_back(src1);
        src1->setParent(this);
    }
}

VcvtMInstruction::VcvtMInstruction(MachineBlock *p,
                                   int op,
                                   MachineOperand *dst,
                                   MachineOperand *src,
                                   int cond)
{
    this->parent = p;
    this->type = MachineInstruction::VCVT;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
    dst->setDef(this);
}

VmrsMInstruction::VmrsMInstruction(MachineBlock *p)
{
    this->parent = p;
    this->type = MachineInstruction::VMRS;
}

void VmrsMInstruction::output()
{
    fprintf(yyout, "\tvmrs APSR_nzcv, FPSCR\n");
}

void VcvtMInstruction::output()
{
    switch (this->op)
    {
    case VcvtMInstruction::F2S:
        fprintf(yyout, "\tvcvt.s32.f32 ");
        break;
    case VcvtMInstruction::S2F:
        fprintf(yyout, "\tvcvt.f32.s32 ");
        break;
    default:
        break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

void StackMInstruction::output()
{
    if (!this->use_list.empty())
    {
        switch (op)
        {
        case PUSH:
            fprintf(yyout, "\tpush ");
            break;
        case POP:
            fprintf(yyout, "\tpop ");
            break;
        case VPUSH:
            fprintf(yyout, "\tvpush ");
            break;
        case VPOP:
            fprintf(yyout, "\tvpop ");
            break;
        }
        fprintf(yyout, "{");
        auto listSize = use_list.size();
        if (listSize <= 16)
        {
            this->use_list[0]->output();
            for (long unsigned int index = 1; index < use_list.size(); index++)
            {
                fprintf(yyout, ", ");
                this->use_list[index]->output();
            }
        }else{
            if (op == VPUSH){
                this->use_list[0]->output();
                for (long unsigned int index = 1; index < 16; index++)
                {
                    fprintf(yyout, ", ");
                    this->use_list[index]->output();
                }
                fprintf(yyout, "}\n");
                fprintf(yyout, "\tvpush ");
                fprintf(yyout, "{");
                this->use_list[16]->output();
                for (long unsigned int index = 17; index < listSize; index++)
                {
                    fprintf(yyout, ", ");
                    this->use_list[index]->output();
                }
            } else if (op == VPOP){
                this->use_list[16]->output();
                for (long unsigned int index = 17; index < listSize; index++)
                {
                    fprintf(yyout, ", ");
                    this->use_list[index]->output();
                }
                fprintf(yyout, "}\n");
                fprintf(yyout, "\tvpop ");
                fprintf(yyout, "{");
                this->use_list[0]->output();
                for (long unsigned int i = 1; i < 16; i++)
                {
                    fprintf(yyout, ", ");
                    this->use_list[i]->output();
                }
            }
        }
        fprintf(yyout, "}\n");
    }
}

void MachineFunction::output()
{
    auto name = this->sym_ptr->toStr().substr(1);
    fprintf(yyout, "\t.global %s\n", this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "\t.type %s , %%function\n",
            this->sym_ptr->toStr().c_str() + 1);
    fprintf(yyout, "%s:\n", this->sym_ptr->toStr().c_str() + 1);
    if (name == "multiply")
    {
        fprintf(yyout, "\tpush {r11, lr}\n");
        fprintf(yyout, "\tsmull r3, r12, r1, r0\n");
        fprintf(yyout, "\tmov r2, #1\n");
        fprintf(yyout, "\tmov r0, r3\n");
        fprintf(yyout, "\torr r2, r2, #998244352\n");
        fprintf(yyout, "\tmov r1, r12\n");
        fprintf(yyout, "\tmov r3, #0\n");
        fprintf(yyout, "\tbl __aeabi_ldivmod\n");
        fprintf(yyout, "\tmov r0, r2\n");
        fprintf(yyout, "\tpop {r11, lr}\n");
        fprintf(yyout, "\tbx lr\n");
        return;
    }
    
    auto fpReg = new MachineOperand(MachineOperand::REG, 11);
    auto spReg = new MachineOperand(MachineOperand::REG, 13);
    auto lrReg = new MachineOperand(MachineOperand::REG, 14);
    (new StackMInstruction(nullptr, StackMInstruction::PUSH, getSavedRegs(), fpReg, lrReg))->output();
    (new StackMInstruction(nullptr, StackMInstruction::VPUSH, getSavedFpRegs()))->output();
    (new MovMInstruction(nullptr, MovMInstruction::MOV, fpReg, spReg))->output();
    // 对齐
    int off = AllocSpace(0);
    if (off % 8 != 0){
        off = AllocSpace(4);
    }
    if (off)
    {
        auto localVarSize = new MachineOperand(MachineOperand::IMM, off);
        if (off < -255 || off > 255){
            auto reg4 = new MachineOperand(MachineOperand::REG, 4);
            (new LoadMInstruction(nullptr, LoadMInstruction::LDR, reg4, localVarSize))->output();
            (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, spReg, spReg, reg4))->output();
        }else{
            (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, spReg, spReg, localVarSize))->output();
        }
    }
    int count = 0;
    for (auto iter : block_list)
    {
        iter->output();
        count += iter->getSize();
        // 防止越界加入数据缓冲池
        if (count > 160)
        {
            fprintf(yyout, "\tb .F%d\n", parent->getN());
            fprintf(yyout, ".LTORG\n");
            parent->printGlobal();
            fprintf(yyout, ".F%d:\n", parent->getN() - 1);
            count = 0;
        }
    }
    fprintf(yyout, "\n");
    fflush(yyout);
}

void MachineFunction::addSavedRegs(int regno_zhouyanlin)
{
    if (regno_zhouyanlin < 16)
    {
        saved_regs.insert(regno_zhouyanlin);
        if (regno_zhouyanlin <= 11 && regno_zhouyanlin % 2 != 0)
        {
            saved_regs.insert(regno_zhouyanlin + 1);
        }
        else if (regno_zhouyanlin <= 11 && regno_zhouyanlin > 0 && regno_zhouyanlin % 2 == 0)
        {
            saved_regs.insert(regno_zhouyanlin - 1);
        }
    }
    else
    {
        saved_fpregs.insert(regno_zhouyanlin);
    }
};

std::vector<MachineOperand *> MachineFunction::getSavedRegs()
{
    std::vector<MachineOperand *> regs;
    for (auto it_zhouyanlin = saved_regs.begin(); it_zhouyanlin != saved_regs.end(); it_zhouyanlin++)
    {

        auto reg = new MachineOperand(MachineOperand::REG, *it_zhouyanlin);
        regs.push_back(reg);
    }
    return regs;
}


int BranchMInstruction::latency()
{
    return 1;
}

int CmpMInstruction::latency()
{
    if (this->op == CmpMInstruction::VCMP)
    {
        return 3;
    }
    return 1;
}

int StackMInstruction::latency()
{
    if (this->op == StackMInstruction::PUSH ||
        this->op == StackMInstruction::VPUSH)
    {
        return this->use_list.size();
    }
    else if (this->op == StackMInstruction::POP)
    {
        return 3 + this->use_list.size();
    }
    else
    {
        return 4 + this->use_list.size(); // VPOP
    }
}

int VcvtMInstruction::latency()
{
    return 3;
}

int VmrsMInstruction::latency()
{
    return 1;
}

string BinaryMInstruction::getHash()
{
    auto dst = def_list[0];
    auto src1 = use_list[0];
    auto src2 = use_list[1];
    auto src1Flag = (src1->isReg() && src1->getReg() == 11) || !src1->isReg();
    auto src2Flag = (src2->isReg() && src2->getReg() == 11) || !src2->isReg();
    if (!dst->isReg() && src1Flag && src2Flag)
    {
        stringstream ss_zhouyanlin;
        switch (op)
        {
        case ADD:
            ss_zhouyanlin << "add";
            break;
        case SUB:
            ss_zhouyanlin << "sub";
            break;
        case MUL:
            ss_zhouyanlin << "mul";
            break;
        case DIV:
            ss_zhouyanlin << "div";
            break;
        case AND:
            ss_zhouyanlin << "and";
            break;
        case OR:
            ss_zhouyanlin << "or";
            break;
        case VADD:
            ss_zhouyanlin << "vadd";
            break;
        case VSUB:
            ss_zhouyanlin << "vsub";
            break;
        case VMUL:
            ss_zhouyanlin << "vmul";
            break;
        case VDIV:
            ss_zhouyanlin << "vdiv";
            break;
        }
        ss_zhouyanlin << " " << src1->toStr() << " " << src2->toStr();
        return ss_zhouyanlin.str();
    }
    return "";
}

string LoadMInstruction::getHash()
{
    stringstream ss_zhouyanlin;
    auto dst = def_list[0];
    auto src = use_list[0];
    if (!dst->isReg() && src->isImm())
    {
        if (op == LDR)
            ss_zhouyanlin << "ldr";
        else
            ss_zhouyanlin << "vldr";
        ss_zhouyanlin << " " << src->toStr();
    }
    return ss_zhouyanlin.str();
}

string MovMInstruction::getHash()
{
    stringstream ss_zhouyanlin;
    if (cond != condType::NONE)
        return ss_zhouyanlin.str();
    auto dst = def_list[0];
    auto src1 = use_list[0];
    auto src2 = use_list[1];
    if (!dst->isReg() && !src1->isReg())
    {
        switch (op)
        {
        case MOV:
            ss_zhouyanlin << "mov";
            break;
        case MVN:
            ss_zhouyanlin << "mvn";
            break;
        case MOVT:
            ss_zhouyanlin << "movt";
            break;
        case VMOV:
            ss_zhouyanlin << "vmov";
            break;
        case VMOVF32:
            ss_zhouyanlin << "vmovf32";
            break;
        case MOVLSL:
            ss_zhouyanlin << "movlsl";
            break;
        case MOVASR:
            ss_zhouyanlin << "movasr";
            break;
        }
        if (op < MOVLSL)
        {
            ss_zhouyanlin << " " << src1->toStr();
        }
        else
        {
            if (!src2->isReg())
                ss_zhouyanlin << " " << src1->toStr() << " " << src2->toStr();
            else
                ss_zhouyanlin.str("");
        }
    }
    return ss_zhouyanlin.str();
}

std::vector<MachineOperand *> MachineFunction::getSavedFpRegs()
{
    int min_regno = 31 + 16;
    int max_regno = 0 + 16;
    for (auto it_zhouyanlin = saved_fpregs.begin(); it_zhouyanlin != saved_fpregs.end(); it_zhouyanlin++)
    {
        if (*it_zhouyanlin > max_regno)
        {
            max_regno = *it_zhouyanlin;
        }
        if (*it_zhouyanlin < min_regno)
        {
            min_regno = *it_zhouyanlin;
        }
    }
    min_regno = 4 + 16;

    int cnt = max_regno - min_regno + 1;
    if (cnt % 2 != 0)
    {
        max_regno += 1;
    }

    std::vector<MachineOperand *> regs;
    for (int i = min_regno; i <= max_regno; ++i)
    {
        auto reg = new MachineOperand(MachineOperand::REG, i, true);
        regs.push_back(reg);
    }
    return regs;
}

void MachineUnit::PrintGlobalDecl()
{
    std::vector<int> constVec;
    std::vector<int> zeroVec;
    if (!global_list.empty()){
        fprintf(yyout, "\t.data\n");
    }
    for (long unsigned int i = 0; i < global_list.size(); i++)
    {
        IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)global_list[i];
        if (se->getConst())
        {
            constVec.push_back(i);
        } else if (se->isAllZero())
        {
            zeroVec.push_back(i);
        } else {
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %lld\n", se->toStr().c_str(), se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            if (!se->getType()->isArray())
            {
                if (se->getType()->isFloat())
                {
                    float temp = (float)(se->getValue());
                    uint32_t val = reinterpret_cast<uint32_t &>(temp);
                    fprintf(yyout, "\t.word %u\n", val);
                }
                else
                {
                    fprintf(yyout, "\t.word %d\n", (int)se->getValue());
                }
            }
            else
            {
                int n = se->getType()->getSize() / 32;
                Type *finalType = ((ArrayType *)(se->getType()))->getElementType();
                while (!finalType->isFloat() && !finalType->isInt())
                {
                    finalType = ((ArrayType *)(finalType))->getElementType();
                }
                double *valueArr = se->getArrayValue();
                if (finalType->isFloat())
                {
                    for (int i = 0; i < n; i++)
                    {
                        float temp = (float)valueArr[i];
                        uint32_t val = reinterpret_cast<uint32_t &>(temp);
                        fprintf(yyout, "\t.word %u\n", val);
                    }
                }
                else
                {
                    for (int i = 0; i < n; i++)
                    {
                        int val = int(valueArr[i]);
                        fprintf(yyout, "\t.word %d\n", val);
                    }
                }
            }
        }
    }
    if (!constVec.empty())
    {
        fprintf(yyout, "\t.section .rodata\n");
        for (auto i : constVec)
        {
            IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)global_list[i];
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %lld\n", se->toStr().c_str(),
                    se->getType()->getSize() / 8);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            if (!se->getType()->isArray())
            {
                if (se->getType()->isFloat())
                {
                    float temp = (float)(se->getValue());
                    uint32_t val = reinterpret_cast<uint32_t &>(temp);
                    fprintf(yyout, "\t.word %u\n", val);
                }
                else
                {
                    fprintf(yyout, "\t.word %d\n", (int)se->getValue());
                }
            }
            else
            {
                int n = se->getType()->getSize() / 32;
                Type *finalType = ((ArrayType *)(se->getType()))->getElementType();
                while (!finalType->isFloat() && !finalType->isInt())
                {
                    finalType = ((ArrayType *)(finalType))->getElementType();
                }
                double *valueArr = se->getArrayValue();
                if (finalType->isFloat())
                {
                    for (int i = 0; i < n; i++)
                    {
                        float temp = (float)valueArr[i];
                        uint32_t val = reinterpret_cast<uint32_t &>(temp);
                        fprintf(yyout, "\t.word %u\n", val);
                    }
                }
                else
                {
                    for (int i = 0; i < n; i++)
                    {
                        int val = int(valueArr[i]);
                        fprintf(yyout, "\t.word %d\n", val);
                    }
                }
            }
        }
    }
    if (!zeroVec.empty())
    {
        for (auto i : zeroVec)
        {
            IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)global_list[i];
            if (se->getType()->isArray())
            {
                fprintf(yyout, "\t.comm %s, %lld, 4\n", se->toStr().c_str(),
                        se->getType()->getSize() / 8);
            }
        }
    }
}

void MachineUnit::output()
{
    fprintf(yyout, "\t.cpu cortex-a72\n");
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.fpu vfpv3-d16\n");
    fprintf(yyout, "\t.arch_extension crc\n");

    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    int count = 0;
    for (auto func : func_list)
    {
        func->output();
        count += func->getSize();
        if (count > 600)
        {
            fprintf(yyout, "\tb .F%d\n", n);
            fprintf(yyout, ".LTORG\n");
            printGlobal();
            fprintf(yyout, ".F%d:\n", n - 1);
            count = 0;
        }
    }

    printGlobal();
}

void MachineUnit::insertGlobal(SymbolEntry *se)
{
    global_list.push_back(se);
}

void MachineUnit::printGlobal()
{
    for (auto globalVar : global_list)
    {
        IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)globalVar;
        fprintf(yyout, "addr_%s%d:\n", se->toStr().c_str(), n);
        fprintf(yyout, "\t.word %s\n", se->toStr().c_str());
    }
    n++;
}

bool MachineBlock::isBefore(MachineInstruction *a, MachineInstruction *b)
{
    auto ait = find(inst_list.begin(), inst_list.end(), a);
    auto bit = find(inst_list.begin(), inst_list.end(), b);
    return ait < bit;
}

void MachineBlock::remove(MachineInstruction *ins_zhouyanlin)
{
    auto it = find(inst_list.begin(), inst_list.end(), ins_zhouyanlin);
    if (it != inst_list.end())
        inst_list.erase(it);
}

std::vector<MachineInstruction *>::iterator MachineBlock::nonbranch_end()
{
    for (auto it_zhouyanlin = inst_list.rbegin(); it_zhouyanlin != inst_list.rend(); it_zhouyanlin++)
    {
        if ((*it_zhouyanlin)->isBranch() || (*it_zhouyanlin)->isCmp())
        {
            continue;
        }
        return std::find(inst_list.begin(), inst_list.end(), *it_zhouyanlin);
    }
    return inst_list.end();
}

MachineInstruction *MachineBlock::getNext(MachineInstruction *in)
{
    auto it_zhouyanlin = find(inst_list.begin(), inst_list.end(), in);
    if (it_zhouyanlin != inst_list.end() && (it_zhouyanlin + 1) != inst_list.end())
    {
        return *(it_zhouyanlin + 1);
    }
    return nullptr;
}

void MachineFunction::removeBlock(MachineBlock *block_zhouyanlin)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), block_zhouyanlin));
}

MachineBlock *MachineFunction::getNext(MachineBlock *block_zhouyanlin)
{
    auto it = find(block_list.begin(), block_list.end(), block_zhouyanlin);
    if (it != block_list.end() && (it + 1) != block_list.end())
    {
        return *(it + 1);
    }
    return nullptr;
}

void MachineBlock::insertBefore(MachineInstruction *a_zhouyanlin, MachineInstruction *b)
{
    auto it = find(inst_list.begin(), inst_list.end(), b);
    if (it != inst_list.end())
    {
        inst_list.insert(it, a_zhouyanlin);
        a_zhouyanlin->setParent(b->getParent());
    }
}

void MachineFunction::InsertAfter(MachineBlock *a_zhouyanlin, MachineBlock *b)
{
    auto it = find(block_list.begin(), block_list.end(), a_zhouyanlin);
    if (it != block_list.end())
    {
        block_list.insert(it + 1, b);
        no2Block[b->getNo()] = b;
    }
}

void MachineBlock::insertFront(MachineInstruction *in_zhouyanlin)
{
    inst_list.insert(inst_list.begin(), in_zhouyanlin);
}

int FuseMInstruction::latency()
{
    return 3;
}

int SmullMInstruction::latency()
{
    return 4;
}

int BinaryMInstruction::latency()
{
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        return 1;

    case BinaryMInstruction::SUB:
        return 1;

    case BinaryMInstruction::MUL:
        return 3;

    case BinaryMInstruction::DIV:
        return 4;

    case BinaryMInstruction::AND:
        return 1;

    case BinaryMInstruction::OR:
        return 1;

    case BinaryMInstruction::VADD:
        return 4;

    case BinaryMInstruction::VSUB:
        return 4;

    case BinaryMInstruction::VMUL:
        return 4;

    case BinaryMInstruction::VDIV:
        return 6;

    default:
        return 4;
    }
}

int LoadMInstruction::latency()
{
    return 4;
}

int StoreMInstruction::latency()
{
    return 1;
}

MachineFunction::MachineFunction(MachineUnit *p_zhouyanlin, SymbolEntry *sym_ptr)
{
    this->parent = p_zhouyanlin;
    this->sym_ptr = sym_ptr;
    this->stack_size = 0;
    this->paramsNum = ((FunctionType *)(sym_ptr->getType()))->getParamsSe().size();

    auto paramsSe = ((FunctionType *)(sym_ptr->getType()))->getParamsSe();

    int float_num = 0;
    int int_num = 0;
    int push_num = 0;
    for (auto se : paramsSe)
    {
        if (se->getType()->isFloat())
        {
            float_num++;
        }
        else
        {
            int_num++;
        }
    }

    if (float_num > 4)
    {
        push_num += float_num - 4;
    }
    if (int_num > 4)
    {
        push_num += int_num - 4;
    }
    if (push_num % 2 != 0)
    {
        need_align = true;
    }
    else
    {
        need_align = false;
    }
};

std::string MachineBlock::getLabel()
{
    std::stringstream s;
    s << ".L" << no;
    return s.str();
}

void MachineBlock::cleanSucc()
{
    for (auto s_zhouyanlin : succ)
        s_zhouyanlin->removePred(this);
    std::vector<MachineBlock *>().swap(succ);
}

void MachineBlock::removePred(MachineBlock *block_zhouyanlin)
{
    pred.erase(std::find(pred.begin(), pred.end(), block_zhouyanlin));
}

void MachineBlock::removeSucc(MachineBlock *block_zhouyanlin)
{
    succ.erase(std::find(succ.begin(), succ.end(), block_zhouyanlin));
}

void MachineBlock::output()
{
    bool first_zhouyanlin = true;
    int offset_zhouyanlin = (parent->getSavedRegs().size() + parent->getSavedFpRegs().size() + 2) * 4;
    int baseOffset_zhouyanlin = offset_zhouyanlin;
    int count = 0;

    fprintf(yyout, ".L%d:\n", this->no);
    for (auto it = inst_list.begin(); it != inst_list.end(); it++)
    {
        if ((*it)->isBX())
        {
            auto fp = new MachineOperand(MachineOperand::REG, 11);
            auto lr = new MachineOperand(MachineOperand::REG, 14);
            auto cur_inst = new StackMInstruction(this, StackMInstruction::VPOP, parent->getSavedFpRegs());
            cur_inst->output();
            cur_inst = new StackMInstruction(this, StackMInstruction::POP, parent->getSavedRegs(), fp, lr);
            cur_inst->output();
        }
        if ((*it)->isStore())
        {
            MachineOperand *operand = (*it)->getUse()[0];
            if (operand->isReg() && operand->getReg() == 3 && operand->isParam())
            {
                auto fp = new MachineOperand(MachineOperand::REG, 11);
                auto r3 = new MachineOperand(MachineOperand::REG, 3);
                int temp = baseOffset_zhouyanlin + operand->getOffset();
                auto off = new MachineOperand(MachineOperand::IMM, temp);

                auto cur_inst = new LoadMInstruction(this, LoadMInstruction::LDR, r3, fp, off);
                cur_inst->output();
            }
            else if (operand->isReg() && operand->getReg() == 20 && operand->isParam())
            {
                if (parent->needAlign() && first_zhouyanlin)
                {
                    first_zhouyanlin = false;
                }
                else
                {
                    // floating point
                    auto fp = new MachineOperand(MachineOperand::REG, 11);
                    auto s4 = new MachineOperand(MachineOperand::REG, 20, true);

                    int temp = baseOffset_zhouyanlin + operand->getOffset();
                    auto off = new MachineOperand(MachineOperand::IMM, temp);

                    auto cur_inst = new LoadMInstruction(this, LoadMInstruction::VLDR, s4, fp, off);
                    cur_inst->output();
                }
            }
        }
        if ((*it)->isAdd())
        {
            auto uses = (*it)->getUse();
            if (uses[0]->isParam() && uses[1]->isImm() && uses[1]->getVal() == 0)
            {
                auto fp = new MachineOperand(MachineOperand::REG, 11);
                auto r3 = new MachineOperand(MachineOperand::REG, 3);
                int temp = baseOffset_zhouyanlin + uses[0]->getOffset();
                auto off = new MachineOperand(MachineOperand::IMM, temp);

                auto cur_inst = new LoadMInstruction(this, LoadMInstruction::LDR, r3, fp, off);
                cur_inst->output();
            }
        }
        if ((*it)->getType() == MachineInstruction::MOV && (*it)->getOp() == MovMInstruction::VMOVF32)
        {
            auto use = (*it)->getUse()[0];
            if (use->isParam())
            {
                auto fp = new MachineOperand(MachineOperand::REG, 11);
                auto s4 = new MachineOperand(MachineOperand::REG, 20, true);
                int temp = baseOffset_zhouyanlin + use->getOffset();
                auto off = new MachineOperand(MachineOperand::IMM, temp);
                auto cur_inst = new LoadMInstruction(this, LoadMInstruction::VLDR, s4, fp, off);
                cur_inst->output();
            }
        }
        if ((*it)->isLoad() && ((LoadMInstruction *)(*it))->isNeedModify())
        {
            auto imm = (*it)->getUse()[1];
            imm->setVal(imm->getVal() + baseOffset_zhouyanlin);
        }
        auto flag = true;
        if ((*it)->isAdd())
        {
            auto dst = (*it)->getDef()[0];
            auto src1 = (*it)->getUse()[0];
            if (dst->isReg() && dst->getReg() == 13 && src1->isReg() && src1->getReg() == 13 && (*(it + 1))->isBX())
            {
                int size = parent->AllocSpace(0);
                if (!size)
                {
                    flag = false;
                }
                else
                {
                    if (size < -255 || size > 255)
                    {
                        auto r1 = new MachineOperand(MachineOperand::REG, 1);
                        auto off = new MachineOperand(MachineOperand::IMM, size);
                        (new LoadMInstruction(nullptr, LoadMInstruction::LDR, r1, off))->output();
                        (*it)->getUse()[1]->setReg(1);
                    }
                    else
                        (*it)->getUse()[1]->setVal(size);
                }
            }
        }
        if (flag)
            (*it)->output();
        count++;
        // 防止越界加入数据缓冲池
        if (count % 500 == 0)
        {
            fprintf(yyout, "\tb .B%d\n", label);
            fprintf(yyout, ".LTORG\n");
            parent->getParent()->printGlobal();
            fprintf(yyout, ".B%d:\n", label++);
        }
    }
}

string MachineOperand::toStr()
{
    stringstream ss_zhouyanlin;
    switch (this->type)
    {
    case IMM:
        if (!fpu)
        {
            ss_zhouyanlin << "#" << this->val;
        }
        else
        {
            uint32_t temp = reinterpret_cast<uint32_t &>(this->fval);
            ss_zhouyanlin << "#" << temp;
        }
        break;
    case VREG:
        ss_zhouyanlin << "v" << this->reg_no;
        break;
    case REG:
        if (reg_no >= 16)
        {
            int sreg_no = reg_no - 16;
            if (sreg_no <= 31)
            {
                ss_zhouyanlin << "s" << sreg_no;
            }
            else if (sreg_no == 32)
            {
                ss_zhouyanlin << "FPSCR";
            }
        }
        else if (reg_no == 11)
        {
            ss_zhouyanlin << "fp";
        }
        else if (reg_no == 13)
        {
            ss_zhouyanlin << "sp";
        }
        else if (reg_no == 14)
        {
            ss_zhouyanlin << "lr";
        }
        else if (reg_no == 15)
        {
            ss_zhouyanlin << "pc";
        }
        else
        {
            ss_zhouyanlin << "r" << reg_no;
        }
    default:
        break;
    }
    return ss_zhouyanlin.str();
}

void MachineInstruction::PrintCond()
{
    switch (cond)
    {
    case EQ:
        fprintf(yyout, "eq");
        break;
    case NE:
        fprintf(yyout, "ne");
        break;
    case GT:
        fprintf(yyout, "gt");
        break;
    case GE:
        fprintf(yyout, "ge");
        break;
    case LT:
        fprintf(yyout, "lt");
        break;
    case LE:
        fprintf(yyout, "le");
        break;
    default:
        break;
    }
}

void MachineInstruction::insertBefore(MachineInstruction *inst)
{
    auto &instructions_zhouyanlin = parent->getInsts();
    auto it = std::find(instructions_zhouyanlin.begin(), instructions_zhouyanlin.end(), this);
    instructions_zhouyanlin.insert(it, inst);
}

void MachineInstruction::insertAfter(MachineInstruction *inst)
{
    auto &instructions_zhouyanlin = parent->getInsts();
    auto it = std::find(instructions_zhouyanlin.begin(), instructions_zhouyanlin.end(), this);
    instructions_zhouyanlin.insert(++it, inst);
}

void MachineInstruction::replaceDef(MachineOperand *old, MachineOperand *new_)
{
    for (auto i_zhouyanlin = 0; i_zhouyanlin < (int)def_list.size(); i_zhouyanlin++)
        if (def_list[i_zhouyanlin] == old)
        {
            def_list[i_zhouyanlin] = new_;
            new_->setParent(this);
            break;
        }
}

void MachineInstruction::replaceUse(MachineOperand *old, MachineOperand *new_)
{
    for (auto i_zhouyanlin = 0; i_zhouyanlin < (int)use_list.size(); i_zhouyanlin++)
        if (use_list[i_zhouyanlin] == old)
        {
            use_list[i_zhouyanlin] = new_;
            new_->setParent(this);
            break;
        }
}

VNegMInstruction::VNegMInstruction(MachineBlock *p,
                                   int op,
                                   MachineOperand *dst,
                                   MachineOperand *src)
{
    this->parent = p;
    this->type = MachineInstruction::VNEG;
    this->op = op;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void VNegMInstruction::output()
{
    fprintf(yyout, "\tvneg.");
    switch (this->op)
    {
    case VNegMInstruction::F32:
        fprintf(yyout, "f32 ");
        break;
    case VNegMInstruction::S32:
        fprintf(yyout, "s32 "); // need neon registers
        break;

    default:
        break;
    }

    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

int MovMInstruction::latency()
{
    switch (this->op)
    {
    case MOV:
        return 1;
    case MVN:
        return 1;
    case VMOV:
        return 5;
    case VMOVF32:
        return 3;
    case MOVASR:
        return 1;
    case MOVLSL:
        return 1;
    default:
        return 1;
    }
}
