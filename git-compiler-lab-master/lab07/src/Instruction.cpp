#include "Instruction.h"
#include <assert.h>
#include <sstream>
#include <utility>
#include "BasicBlock.h"
#include "Function.h"
#include "Type.h"

#include <iostream>
#include <map>

extern FILE *yyout;
extern bool optimize;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb_zhouyanlin)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb_zhouyanlin != nullptr)
    {
        insert_bb_zhouyanlin->insertBack(this);
        parent = insert_bb_zhouyanlin;
    }
    mark = false;
    node = nullptr;
    constVal = 0;
}

Instruction::~Instruction()
{
    parent->remove(this);
}

bool Instruction::isEssential()
{
    if (isRet())
    {
        if (getUse().empty())
            return true;
        auto preds = parent->getParent()->getPreds();
        if (preds.empty())
            return true;
        for (auto it_zhouyanlin : preds)
            for (auto in_zhouyanlin : it_zhouyanlin.second)
                if (in_zhouyanlin->getDef()->usersNum())
                    return true;

        return false;
    }
    // input/output
    if (isCall())
    {
        IdentifierSymbolEntry *funcSE_zhouyanlin = (IdentifierSymbolEntry *)(((CallInstruction *)this)->getFuncSE());
        if (funcSE_zhouyanlin->isSysy() || funcSE_zhouyanlin->getName() == "llvm.memset.p0.i32")
        {
            return true;
        }
        else
        {
            auto func_zhouyanlin = funcSE_zhouyanlin->getFunction();
            if (func_zhouyanlin->getEssential() == 1)
            {
                return true;
            }
        }
    }
    if (isStore())
    {
        return true;
    }
    return false;
}

bool Instruction::isIntMul()
{
    if (instType == BINARY && opcode == BinaryInstruction::MUL)
        if (operands[0]->getType()->isInt())
            return true;
    return false;
}

bool Instruction::isIntDiv()
{
    if (instType == BINARY && opcode == BinaryInstruction::DIV)
        if (operands[0]->getType()->isInt())
            return true;
    return false;
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb_zhouyanlin)
{
    parent = bb_zhouyanlin;
}

void Instruction::setNext(Instruction *inst_zhouyanlin)
{
    next = inst_zhouyanlin;
}

void Instruction::setPrev(Instruction *inst_zhouyanlin)
{
    prev = inst_zhouyanlin;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

void Instruction::remove()
{
    this->getNext()->setPrev(this->getPrev());
    this->getPrev()->setNext(this->getNext());
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    int opdcnt_zhouyanlin = operands.size();
    for (int i_zhouyanlin = 1; i_zhouyanlin < opdcnt_zhouyanlin; i_zhouyanlin++)
    {
        operands[i_zhouyanlin]->removeUse(this);
    }
}

bool Instruction::reGenNode()
{
    bool ret_zhouyanlin = true;
    auto &children = node->getChildren();
    for (int i_zhouyanlin = 0; i_zhouyanlin < (int)children.size(); i_zhouyanlin++)
    {
        auto &child = children[i_zhouyanlin];
        if (!child)
        {
            auto operand = operands[i_zhouyanlin + 1];
            auto se = operand->getEntry();
            assert(se->isTemporary());
            child = operand->getDef()->getNode();
            if (!child)
                ret_zhouyanlin = false;
            children[i_zhouyanlin] = child;
        }
    }
    return ret_zhouyanlin;
}

BinaryInstruction::BinaryInstruction(unsigned opcode,
                                     Operand *dst,
                                     Operand *src1,
                                     Operand *src2,
                                     BasicBlock *insert_bb)
    : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

void BinaryInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
    else if (operands[2] == old_zhouyanlin)
    {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void BinaryInstruction::replaceDef(Operand *new_)
{
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

std::pair<int, int> BinaryInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res_zhouyanlin, val1, val2;
    if (value.find(this->getUse()[0]) == value.end())
        val1 = this->getUse()[0]->getInitLatticeValue();
    else
        val1 = value[this->getUse()[0]];
    if (value.find(this->getUse()[1]) == value.end())
        val2 = this->getUse()[1]->getInitLatticeValue();
    else
        val2 = value[this->getUse()[1]];

    if (val1.first == -1 || val2.first == -1)
        res_zhouyanlin = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res_zhouyanlin.first = 0;
        switch (opcode)
        {
        case ADD:
            res_zhouyanlin.second = val1.second + val2.second;
            break;
        case SUB:
            res_zhouyanlin.second = val1.second - val2.second;
            break;
        case MUL:
            res_zhouyanlin.second = val1.second * val2.second;
            break;
        case DIV:
            res_zhouyanlin.second = val1.second / val2.second;
            break;
        case MOD:
            res_zhouyanlin.second = val1.second % val2.second;
            break;
        case AND:
            res_zhouyanlin.second = val1.second & val2.second;
            break;
        case OR:
            res_zhouyanlin.second = val1.second | val2.second;
            break;
        default:
            break;
        }
    }
    else
        res_zhouyanlin.first = 1;
    return res_zhouyanlin;
}

void BinaryInstruction::output() const
{
    std::string s1_zhouyanlin, s2_zhouyanlin, s3_zhouyanlin, op, type;
    s1_zhouyanlin = operands[0]->toStr();
    s2_zhouyanlin = operands[1]->toStr();
    s3_zhouyanlin = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        if (type == "float")
        {
            op = "fadd";
        }
        else
        {
            op = "add";
        }
        break;
    case SUB:
        if (type == "float")
        {
            op = "fsub";
        }
        else
        {
            op = "sub";
        }
        break;
    case MUL:
        if (type == "float")
        {
            op = "fmul";
        }
        else
        {
            op = "mul";
        }
        break;
    case DIV:
        if (type == "float")
        {
            op = "fdiv";
        }
        else
        {
            op = "sdiv";
        }
        break;
    case MOD:
        op = "srem";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1_zhouyanlin.c_str(), op.c_str(),
            type.c_str(), s2_zhouyanlin.c_str(), s3_zhouyanlin.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode,
                               Operand *dst,
                               Operand *src1,
                               Operand *src2,
                               BasicBlock *insert_bb)
    : Instruction(CMP, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

void CmpInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
    else if (operands[2] == old_zhouyanlin)
    {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void CmpInstruction::replaceDef(Operand *new_)
{
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op_zhouyanlin, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        op_zhouyanlin = "eq";
        break;
    case NE:
        op_zhouyanlin = "ne";
        break;
    case L:
        op_zhouyanlin = "slt";
        break;
    case LE:
        op_zhouyanlin = "sle";
        break;
    case G:
        op_zhouyanlin = "sgt";
        break;
    case GE:
        op_zhouyanlin = "sge";
        break;
    default:
        op_zhouyanlin = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op_zhouyanlin.c_str(),
            type.c_str(), s2.c_str(), s3.c_str());
}

std::pair<int, int> CmpInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res_zhouyanlin, val1, val2;
    if (value.find(this->getUse()[0]) == value.end())
        val1 = this->getUse()[0]->getInitLatticeValue();
    else
        val1 = value[this->getUse()[0]];
    if (value.find(this->getUse()[1]) == value.end())
        val2 = this->getUse()[1]->getInitLatticeValue();
    else
        val2 = value[this->getUse()[1]];
    if (val1.first == -1 || val2.first == -1)
    {
        res_zhouyanlin = {-1, 0};
    }
    else if (val1.first == 0 && val2.first == 0)
    {
        res_zhouyanlin.first = 0;
        switch (opcode)
        {
        case E:
            res_zhouyanlin.second = val1.second == val2.second;
            break;
        case NE:
            res_zhouyanlin.second = val1.second != val2.second;
            break;
        case L:
            res_zhouyanlin.second = val1.second < val2.second;
            break;
        case GE:
            res_zhouyanlin.second = val1.second >= val2.second;
            break;
        case G:
            res_zhouyanlin.second = val1.second > val2.second;
            break;
        case LE:
            res_zhouyanlin.second = val1.second <= val2.second;
            break;
        default:
            break;
        }
    }
    else
    {
        res_zhouyanlin.first = 1;
    }
    return res_zhouyanlin;
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to_zhouyanlin, BasicBlock *insert_bb)
    : Instruction(UNCOND, insert_bb)
{
    branch = to_zhouyanlin;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb_zhouyanlin)
{
    branch = bb_zhouyanlin;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock *true_branch,
                                     BasicBlock *false_branch,
                                     Operand *cond,
                                     BasicBlock *insert_bb)
    : Instruction(COND, insert_bb)
{
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
    originTrue = originFalse = nullptr;
}

void CondBrInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[0] == old_zhouyanlin)
    {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    }
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond_zhouyanlin, type;
    cond_zhouyanlin = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(),
            cond_zhouyanlin.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb_zhouyanlin)
{
    false_branch = bb_zhouyanlin;
    if (!originFalse)
        originFalse = false_branch;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb_zhouyanlin)
{
    true_branch = bb_zhouyanlin;
    if (!originTrue)
        originTrue = true_branch;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb)
    : Instruction(RET, insert_bb)
{
    if (src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst_zhouyanlin, type;
    dst_zhouyanlin = operands[0]->toStr();
    if (se->getType()->isInt() || se->getType()->isFloat())
    {
        type = se->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst_zhouyanlin.c_str(),
                type.c_str());
    }
    else if (se->getType()->isArray())
    {
        type = se->getType()->toStr();
        // type = operands[0]->getSymbolEntry()->getType()->toStr();
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst_zhouyanlin.c_str(),
                type.c_str());
    }
}

LoadInstruction::LoadInstruction(Operand *dst,
                                 Operand *src_addr,
                                 BasicBlock *insert_bb)
    : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

void LoadInstruction::replaceDef(Operand *new_)
{
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}

void LoadInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst_zhouyanlin = operands[0]->toStr();
    std::string src_zhouyanlin = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst_zhouyanlin.c_str(),
            dst_type.c_str(), src_type.c_str(), src_zhouyanlin.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr,
                                   Operand *src,
                                   BasicBlock *insert_bb)
    : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

void StoreInstruction::replaceUse(Operand *old, Operand *new_)
{
    if (operands[0] == old)
    {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    }
    else if (operands[1] == old)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dstReg = operands[0]->toStr();
    std::string dstType = operands[0]->getType()->toStr();
    std::string srcReg = operands[1]->toStr();
    std::string srcType = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", srcType.c_str(), srcReg.c_str(), dstType.c_str(), dstReg.c_str());
}

MachineOperand *Instruction::genMachineOperand(Operand *ope)
{
    auto se_zhouyanlin = ope->getEntry();
    MachineOperand *mope = nullptr;
    if (se_zhouyanlin->isConstant())
    {
        mope = new MachineOperand(
            MachineOperand::IMM,
            (int)dynamic_cast<ConstantSymbolEntry *>(se_zhouyanlin)->getValue());
    }
    else if (se_zhouyanlin->isTemporary())
    {
        mope = new MachineOperand(
            MachineOperand::VREG,
            dynamic_cast<TemporarySymbolEntry *>(se_zhouyanlin)->getLabel());
    }
    else if (se_zhouyanlin->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry *>(se_zhouyanlin);
        if (id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else if (id_se->isParam())
        {
            auto no = id_se->getParamNo();
            if (no < 4)
                mope = new MachineOperand(MachineOperand::REG, id_se->getParamNo());
            else
            {
                mope = new MachineOperand(MachineOperand::REG, 3);
                mope->setParam();
                mope->setParamNo(no);
            }
            mope->setAllParamNo(id_se->getStackParamNo());
        }
        else
            exit(0);
    }
    return mope;
}

MachineOperand *Instruction::genMachineFloatOperand(Operand *ope)
{
    auto se_zhouyanlin = ope->getEntry();
    if (!se_zhouyanlin->getType()->isFloat())
    {

        return genMachineOperand(ope);
    }
    MachineOperand *mope = nullptr;
    if (se_zhouyanlin->isConstant())
    {
        mope = new MachineOperand(
            MachineOperand::IMM,
            (float)dynamic_cast<ConstantSymbolEntry *>(se_zhouyanlin)->getValue());
    }
    else if (se_zhouyanlin->isTemporary())
    {
        mope = new MachineOperand(
            MachineOperand::VREG,
            dynamic_cast<TemporarySymbolEntry *>(se_zhouyanlin)->getLabel(), true);
    }
    else if (se_zhouyanlin->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry *>(se_zhouyanlin);
        if (id_se->isGlobal())
        {
            mope = new MachineOperand(id_se->toStr().c_str());
        }
        else if (id_se->isParam())
        {
            auto no = id_se->getParamNo();
            if (id_se->getParamNo() < 4)
            {
                mope = new MachineOperand(MachineOperand::REG,
                                          id_se->getParamNo() + 16, true);
            }
            else
            {
                mope = new MachineOperand(MachineOperand::REG, 20, true);
                mope->setParam();
                mope->setParamNo(no);
            }
            mope->setAllParamNo(id_se->getStackParamNo());
        }
    }
    return mope;
}

MachineOperand *Instruction::genMachineReg(int reg_zhouyanlin)
{
    return new MachineOperand(MachineOperand::REG, reg_zhouyanlin);
}

MachineOperand *Instruction::genMachineFReg(int freg_zhouyanlin)
{
    return new MachineOperand(MachineOperand::REG, freg_zhouyanlin + 16, true);
}

MachineOperand *Instruction::genMachineVReg(bool fpu_zhouyanlin)
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel(),
                              fpu_zhouyanlin);
}

MachineOperand *Instruction::genMachineImm(int val_zhouyanlin)
{
    return new MachineOperand(MachineOperand::IMM, val_zhouyanlin);
}

MachineOperand *Instruction::genMachineLabel(int block_no_zhouyanlin)
{
    std::ostringstream buf;
    buf << ".L" << block_no_zhouyanlin;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    
    auto cur_func = builder_zhouyanlin->getFunction();
    int size = se->getType()->getSize() / 8;
    if (size < 0)
        size = 4;
    int offset = cur_func->AllocSpace(size);
    dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())
        ->setOffset(-offset);
}
void BinaryInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    // complete other instructions
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();

    if (operands[0]->getType()->isFloat())
    {
        auto flag = false;
        auto dst = genMachineFloatOperand(operands[0]);
        auto src1 = genMachineFloatOperand(operands[1]);
        auto src2 = genMachineFloatOperand(operands[2]);
        MachineInstruction *cur_inst = nullptr;
        if (src1->isImm())
        {
            auto tmp_reg = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                           tmp_reg, internal_reg);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src1 = new MachineOperand(*tmp_reg);
        }
        if (src2->isImm())
        {
            if (src2->getFVal() == 0 && opcode == ADD)
                flag = true;
            else
            {
                auto tmp_reg = genMachineVReg(true);
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src2);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                internal_reg = new MachineOperand(*internal_reg);
                cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                               tmp_reg, internal_reg);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                src2 = new MachineOperand(*tmp_reg);
            }
        }

        switch (opcode)
        {
        case ADD:
            if (flag)
                cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOVF32, dst, src1);
            else
                cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::VADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::VSUB, dst, src1, src2);
            break;
        case AND:
            break;
        case OR:
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::VMUL, dst, src1, src2);
            break;
        case DIV:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::VDIV, dst, src1, src2);
            break;
        case MOD:
            // error
            break;
        default:
            break;
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    else
    {
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineOperand(operands[1]);
        auto src2 = genMachineOperand(operands[2]);
        MachineInstruction *cur_inst = nullptr;
        if (src1->isImm() && src2->isImm() && src2->getVal() == 0 &&
            opcode == ADD)
        {
            if (!(src1->getVal() < 256 && src1->getVal() > -255))
            {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src1);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                src1 = new MachineOperand(*internal_reg);
            }
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, src1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            return;
        }
        if (src1->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg, src1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        
        if (src2->isImm())
        {
            if ((opcode <= BinaryInstruction::OR &&
                 ((ConstantSymbolEntry *)(operands[2]->getEntry()))->getValue() > 255) ||
                opcode >= BinaryInstruction::MUL)
            {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(
                    cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src2);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                src2 = new MachineOperand(*internal_reg);
            }
        }
        switch (opcode)
        {
        case ADD:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::ADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::SUB, dst, src1, src2);
            break;
        case AND:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::AND, dst, src1, src2);
            break;
        case OR:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::OR, dst, src1, src2);
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::MUL, dst, src1, src2);
            break;
        case DIV:
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::DIV, dst, src1, src2);
            break;
        case MOD:
        {
            auto dst1 = genMachineVReg();
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::DIV, dst1, src1, src2);
            src1 = new MachineOperand(*src1);
            src2 = new MachineOperand(*src2);
            auto temp = new MachineOperand(*dst1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            // c2 = c1 * b
            auto dst2 = genMachineVReg();
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::MUL, dst2, temp, src2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            dst2 = new MachineOperand(*dst2);
            // c = a - c2
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::SUB, dst, src1, dst2);
            break;
        }
        default:
            break;
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
}

void CmpInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();

    if (operands[1]->getType()->isFloat())
    {
        auto src1 = genMachineFloatOperand(operands[1]);
        auto src2 = genMachineFloatOperand(operands[2]);
        MachineInstruction *cur_inst = nullptr;
        if (src1->isImm())
        {
            auto tmp_reg = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, tmp_reg, internal_reg);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src1 = new MachineOperand(*tmp_reg);
        }
        if (src2->isImm())
        {
            auto tmp_reg = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, tmp_reg, internal_reg);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src2 = new MachineOperand(*tmp_reg);
        }
        cur_inst = new CmpMInstruction(cur_block_zhouyanlin, CmpMInstruction::VCMP, src1, src2, opcode);
        cur_block_zhouyanlin->InsertInst(cur_inst);
        cur_inst = new VmrsMInstruction(cur_block_zhouyanlin);
        cur_block_zhouyanlin->InsertInst(cur_inst);

        if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, opcode);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, 7 - opcode);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else if (opcode == CmpInstruction::E)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, E);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, NE);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else if (opcode == CmpInstruction::NE)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, NE);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, E);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
    else
    {
        auto src1 = genMachineOperand(operands[1]);
        auto src2 = genMachineOperand(operands[2]);
        MachineInstruction *cur_inst = nullptr;
        if (src1->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        if (src2->isImm() && ((ConstantSymbolEntry *)(operands[2]->getEntry()))->getValue() > 255)
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
        cur_inst = new CmpMInstruction(cur_block_zhouyanlin, CmpMInstruction::CMP, src1, src2, opcode);
        cur_block_zhouyanlin->InsertInst(cur_inst);
        if (opcode >= CmpInstruction::L && opcode <= CmpInstruction::GE)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, opcode);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, 7 - opcode);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else if (opcode == CmpInstruction::E)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, E);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, NE);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else if (opcode == CmpInstruction::NE)
        {
            auto dst = genMachineOperand(operands[0]);
            auto trueOperand = genMachineImm(1);
            auto falseOperand = genMachineImm(0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, trueOperand, NE);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, falseOperand, E);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    std::stringstream s;
    s << ".L" << branch->getNo();
    MachineOperand *dst = new MachineOperand(s.str());
    auto cur_inst = new BranchMInstruction(cur_block_zhouyanlin, BranchMInstruction::B, dst);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    std::stringstream s;
    s << ".L" << true_branch->getNo();
    MachineOperand *dst = new MachineOperand(s.str());
    auto cur_inst = new BranchMInstruction(cur_block_zhouyanlin, BranchMInstruction::B, dst, cur_block_zhouyanlin->getCmpCond());
    cur_block_zhouyanlin->InsertInst(cur_inst);
    s.str("");
    s << ".L" << false_branch->getNo();
    dst = new MachineOperand(s.str());
    cur_inst = new BranchMInstruction(cur_block_zhouyanlin, BranchMInstruction::B, dst);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    if (!operands.empty())
    {
        MachineOperand *dst;
        MachineOperand *src;
        MachineInstruction *cur_inst;

        if (operands[0]->getType()->isFloat())
        {
            dst = new MachineOperand(MachineOperand::REG, 16, true);
            src = genMachineFloatOperand(operands[0]);
            if (src->isImm())
            {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, src);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                src = internal_reg;
            }
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, dst, src);
        }
        else
        {
            dst = new MachineOperand(MachineOperand::REG, 0);
            src = genMachineOperand(operands[0]);
            if (operands[0]->isConst())
            {
                auto val = operands[0]->getConstVal();
                if (val > 255 || val <= -255)
                {
                    auto r0 = new MachineOperand(MachineOperand::REG, 0);
                    cur_block_zhouyanlin->InsertInst(new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, r0, src));
                    src = r0;
                }
            }
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, src);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    auto cur_func = builder_zhouyanlin->getFunction();
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto size = new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    auto cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, sp, sp, size);
    cur_block_zhouyanlin->InsertInst(cur_inst);

    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto cur_inst2 = new BranchMInstruction(cur_block_zhouyanlin, BranchMInstruction::BX, lr);
    cur_block_zhouyanlin->InsertInst(cur_inst2);
}

CallInstruction::CallInstruction(Operand *dst, SymbolEntry *func, std::vector<Operand *> params, BasicBlock *insert_bb)
    : Instruction(CALL, insert_bb), func(func), dst(dst)
{
    operands.push_back(dst);
    if (dst)
        dst->setDef(this);
    for (auto param : params)
    {
        operands.push_back(param);
        param->addUse(this);
    }
    insert_bb->getParent()->setHasCall();
    IdentifierSymbolEntry *funcSE_zhouyanlin = (IdentifierSymbolEntry *)func;
    if (!funcSE_zhouyanlin->isSysy() && funcSE_zhouyanlin->getName() != "llvm.memset.p0.i32")
        funcSE_zhouyanlin->getFunction()->addPred(this);
}






void ShlInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto num = genMachineOperand(operands[2]);
    // 目前只是立即数
    assert(num->isImm());
    if (src->isImm())
    {
        auto temp = genMachineVReg();
        cur_block_zhouyanlin->InsertInst(
            new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, temp, src));
        src = new MachineOperand(*temp);
    }
    auto cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOVLSL, dst, src, MachineInstruction::NONE, num);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

AshrInstruction::AshrInstruction(Operand *dst,
                                 Operand *src,
                                 Operand *num,
                                 BasicBlock *insert_bb)
    : Instruction(ASHR, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    operands.push_back(num);
    dst->setDef(this);
    src->addUse(this);
    num->addUse(this);
}

void AshrInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
    else if (operands[2] == old_zhouyanlin)
    {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void AshrInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

AshrInstruction::~AshrInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

std::pair<int, int> AshrInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res_zhouyanlin, val1, val2;
    if (value.find(this->getUse()[0]) == value.end())
        val1 = this->getUse()[0]->getInitLatticeValue();
    else
        val1 = value[this->getUse()[0]];
    if (value.find(this->getUse()[1]) == value.end())
        val2 = this->getUse()[1]->getInitLatticeValue();
    else
        val2 = value[this->getUse()[1]];

    if (val1.first == -1 || val2.first == -1)
        res_zhouyanlin = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res_zhouyanlin.first = 0;
        res_zhouyanlin.second = val1.second >> val2.second;
    }
    else
        res_zhouyanlin.first = 1;
    return res_zhouyanlin;
}

void AshrInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string num = operands[2]->toStr();
    std::string src_type_zhouyanlin;
    src_type_zhouyanlin = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = ashr %s %s, %s\n", dst.c_str(), src_type_zhouyanlin.c_str(),
            src.c_str(), num.c_str());
}

bool AshrInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::ASHR);
    auto se1_zhouyanlin = operands[1]->getEntry();
    auto se2_zhouyanlin = operands[2]->getEntry();
    SSAGraphNode *node1, *node2;
    if (se1_zhouyanlin->isConstant())
    {
        int val1 = ((ConstantSymbolEntry *)se1_zhouyanlin)->getValue();
        node1 = new SSAGraphNode(val1);
    }
    else
        node1 = operands[1]->getDef()->getNode();
    if (se2_zhouyanlin->isConstant())
    {
        int val2 = ((ConstantSymbolEntry *)se2_zhouyanlin)->getValue();
        node2 = new SSAGraphNode(val2);
    }
    else
        node2 = operands[2]->getDef()->getNode();
    bool flag = true;
    if (node1 == nullptr || node2 == nullptr)
        flag = false;
    node->addChild(node1);
    node->addChild(node2);
    return flag;
}

std::string AshrInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "shl ";
    s_zhouyanlin << operands[1]->toStr() << " " << operands[2]->toStr();
    return s_zhouyanlin.str();
}





void CallInstruction::addPred()
{
    IdentifierSymbolEntry *funcSE_zhouyanlin = (IdentifierSymbolEntry *)func;
    if (!funcSE_zhouyanlin->isSysy() && funcSE_zhouyanlin->getName() != "llvm.memset.p0.i32")
        funcSE_zhouyanlin->getFunction()->addPred(this);
}

void CallInstruction::replaceDef(Operand *new__zhouyanlin)
{
    if (dst)
    {
        operands[0]->removeDef(this);
        operands[0] = new__zhouyanlin;
        new__zhouyanlin->setDef(this);
        dst = new__zhouyanlin;
    }
}

void CallInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    for (int i_zhouyanlin = 1; i_zhouyanlin < (int)operands.size(); i_zhouyanlin++)
        if (operands[i_zhouyanlin] == old_zhouyanlin)
        {
            operands[i_zhouyanlin]->removeUse(this);
            operands[i_zhouyanlin] = new_;
            new_->addUse(this);
        }
}

void CallInstruction::output() const
{
    fprintf(yyout, "  ");
    if (operands[0])
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());
    FunctionType *type = (FunctionType *)(func->getType());
    fprintf(yyout, "call %s %s(", type->getRetType()->toStr().c_str(), func->toStr().c_str());
    for (long unsigned int i_zhouyanlin = 1; i_zhouyanlin < operands.size(); i_zhouyanlin++)
    {
        if (i_zhouyanlin != 1)
            fprintf(yyout, ", ");
        fprintf(yyout, "%s %s", operands[i_zhouyanlin]->getType()->toStr().c_str(), operands[i_zhouyanlin]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

CallInstruction::~CallInstruction()
{
    if (operands[0])
    {
        operands[0]->setDef(nullptr);
        if (operands[0]->usersNum() == 0)
            delete operands[0];
    }
    for (long unsigned int i_zhouyanlin = 1; i_zhouyanlin < operands.size(); i_zhouyanlin++)
        operands[i_zhouyanlin]->removeUse(this);
}

ZextInstruction::ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(ZEXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void ZextInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

void ZextInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

void ZextInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = zext %s %s to i32\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str());
}

ZextInstruction::~ZextInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

XorInstruction::XorInstruction(Operand *dst,
                               Operand *src,
                               BasicBlock *insert_bb)
    : Instruction(XOR, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void XorInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

void XorInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

void XorInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = xor %s %s, true\n", dst->toStr().c_str(), src->getType()->toStr().c_str(), src->toStr().c_str());
}

XorInstruction::~XorInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

GepInstruction::GepInstruction(Operand *dst, Operand *arr, Operand *idx, BasicBlock *insert_bb, bool paramFirst)
    : Instruction(GEP, insert_bb), paramFirst(paramFirst)
{
    operands.push_back(dst);
    operands.push_back(arr);
    operands.push_back(idx);
    dst->setDef(this);
    arr->addUse(this);
    idx->addUse(this);
    first = false;
    init = nullptr;
    last = false;
    noAsm = false;
}

void GepInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

void GepInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
    else if (operands[2] == old_zhouyanlin)
    {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void GepInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *arr = operands[1];
    Operand *idx = operands[2];
    std::string arrType_zhouyanlin = arr->getType()->toStr();

    if (paramFirst)
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s, i32 %s\n",
                dst->toStr().c_str(), arrType_zhouyanlin.substr(0, arrType_zhouyanlin.size() - 1).c_str(), arrType_zhouyanlin.c_str(), arr->toStr().c_str(), idx->toStr().c_str());
    else
        fprintf(
            yyout, "  %s = getelementptr inbounds %s, %s %s, i32 0, i32 %s\n",
            dst->toStr().c_str(), arrType_zhouyanlin.substr(0, arrType_zhouyanlin.size() - 1).c_str(), arrType_zhouyanlin.c_str(), arr->toStr().c_str(), idx->toStr().c_str());
}

GepInstruction::~GepInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

FptosiInstruction::FptosiInstruction(Operand *dst,
                                     Operand *src,
                                     BasicBlock *insert_bb)
    : Instruction(FPTOSI, insert_bb), dst(dst), src(src)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void FptosiInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = fptosi %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

void FptosiInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

FptosiInstruction::~FptosiInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

SitofpInstruction::SitofpInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(FPTOSI, insert_bb), dst(dst), src(src)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

void SitofpInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = sitofp %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

void ZextInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, src);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

void XorInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto trueOperand = genMachineImm(1);
    auto falseOperand = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst,
                                        trueOperand, MachineInstruction::EQ);
    cur_block_zhouyanlin->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst,
                                   falseOperand, MachineInstruction::NE);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

void LoadInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    MachineInstruction *cur_inst = nullptr;
    // Load global operand
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[1]->getEntry())->isGlobal())
    {
        if (operands[0]->getType()->isFloat())
        {
            auto dst = genMachineFloatOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg1, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::VLDR,
                                            dst, internal_reg2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            auto dst = genMachineOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            // example: load r0, addr_a
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg1, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            // example: load r1, [r0]
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            dst, internal_reg2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
    // Load local operand
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [r0, #4]
        if (operands[0]->getType()->isFloat())
        {
            auto dst = genMachineFloatOperand(operands[0]);
            auto src1 = genMachineReg(11);
            int off =
                dynamic_cast<TemporarySymbolEntry *>(operands[1]->getEntry())
                    ->getOffset();
            auto src2 = genMachineImm(off);
            if (off > 255 || off < -255)
            {
                auto operand = genMachineVReg();
                cur_block_zhouyanlin->InsertInst((new LoadMInstruction(
                    cur_block_zhouyanlin, LoadMInstruction::LDR, operand, src2)));
                src2 = new MachineOperand(*operand);
                operand = genMachineVReg();
                cur_block_zhouyanlin->InsertInst((new BinaryMInstruction(
                    cur_block_zhouyanlin, BinaryMInstruction::ADD, operand, src1, src2)));
                cur_inst =
                    new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::VLDR, dst,
                                         new MachineOperand(*operand));
            }
            else
            {
                cur_inst = new LoadMInstruction(
                    cur_block_zhouyanlin, LoadMInstruction::VLDR, dst, src1, src2);
            }
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            auto dst = genMachineOperand(operands[0]);
            auto src1 = genMachineReg(11);
            int off =
                dynamic_cast<TemporarySymbolEntry *>(operands[1]->getEntry())
                    ->getOffset();
            auto src2 = genMachineImm(off);
            if (off > 255 || off < -255)
            {
                auto operand = genMachineVReg();
                cur_block_zhouyanlin->InsertInst((new LoadMInstruction(
                    cur_block_zhouyanlin, LoadMInstruction::LDR, operand, src2)));
                src2 = operand;
            }
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            dst, src1, src2);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
    else
    { // Load operand from temporary variable
        // example: load r1, [r0]
        MachineOperand *dst = nullptr;
        MachineOperand *src = nullptr;
        if (operands[0]->getType()->isFloat())
        {
            dst = genMachineFloatOperand(operands[0]);
        }
        else
        {
            dst = genMachineOperand(operands[0]);
        }

        src = genMachineOperand(operands[1]);

        if (operands[0]->getType()->isFloat() ||
            operands[1]->getType()->isFloat())
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::VLDR,
                                            dst, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            dst, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
}

void StoreInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    MachineInstruction *cur_inst = nullptr;

    bool src_float = operands[1]->getType()->isFloat();

    MachineOperand *dst = nullptr;
    MachineOperand *src = nullptr;

    dst = genMachineOperand(operands[0]);

    if (src_float)
    {
        src = genMachineFloatOperand(operands[1]);
    }
    else
    {
        src = genMachineOperand(operands[1]);
    }

    // store constant
    if (operands[1]->getEntry()->isConstant())
    {
        auto dst1 = genMachineVReg(src_float);
        if (src_float)
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, dst1, internal_reg);
        }
        else
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, dst1, src);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
        
        src = new MachineOperand(*dst1);
    }
    // store to local
    if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc())
    {
        auto src1 = genMachineReg(11);
        int off = dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())
                      ->getOffset();
        auto src2 = genMachineImm(off);
        if (off > 255 || off < -255)
        {
            auto operand = genMachineVReg();
            cur_block_zhouyanlin->InsertInst((new LoadMInstruction(
                cur_block_zhouyanlin, LoadMInstruction::LDR, operand, src2)));
            src2 = operand;
        }
        if (src_float)
        {
            if (off > 255 || off < -255)
            {
                auto reg = genMachineVReg();
                cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, reg, src1, src2);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::VSTR,
                                          src, new MachineOperand(*reg));
            }
            else
            {
                cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::VSTR, src, src1, src2);
            }
        }
        else
        {
            cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::STR, src, src1, src2);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    // store to global
    else if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[0]->getEntry())->isGlobal())
    {
        auto internal_reg1 = genMachineVReg();
        // example: load r0, addr_a
        if (src_float)
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg1, dst);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::VSTR,
                                             src, internal_reg1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg1, dst);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            // example: store r1, [r0]
            cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::STR,
                                             src, internal_reg1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
    // store to pointer
    else if (operands[0]->getType()->isPtr())
    {
        if (src_float)
        {
            cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::VSTR,
                                             src, dst);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            cur_inst = new StoreMInstruction(cur_block_zhouyanlin, StoreMInstruction::STR,
                                             src, dst);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
}


void GepInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    MachineInstruction *cur_inst;
    auto dst = genMachineOperand(operands[0]);
    auto idx = genMachineOperand(operands[2]);
    if (init)
    {
        if (last)
        {
            auto base = genMachineOperand(init);
            MachineOperand *imm = genMachineImm(off + 4);
            int off = this->off + 4;
            if (off > 255)
            {
                MachineOperand *temp = genMachineVReg();
                cur_block_zhouyanlin->InsertInst(new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, temp, imm));
                imm = temp;
            }
            cur_inst = new BinaryMInstruction(
                cur_block_zhouyanlin, BinaryMInstruction::ADD, dst, base, imm);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            noAsm = true;
        }
        return;
    }
    MachineOperand *base = nullptr;
    int size;
    auto idx1 = genMachineVReg();
    if (idx->isImm())
    {
        if (idx->getVal() < 255)
        {
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, idx1, idx);
        }
        else
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, idx1, idx);
        }
        idx = new MachineOperand(*idx1);
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    if (paramFirst)
    {
        size = ((PointerType *)(operands[1]->getType()))->getType()->getSize() / 8;
    }
    else
    {
        if (first)
        {
            base = genMachineVReg();
            if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry *)(operands[1]->getEntry()))->isGlobal())
            {
                auto src = genMachineOperand(operands[1]);
                cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, base, src);
            }
            else
            {
                int offset = ((TemporarySymbolEntry *)(operands[1]->getEntry()))->getOffset();
                if (offset > -255 && offset < 255)
                {
                    cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, base, genMachineImm(offset));
                }
                else
                {
                    cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, base, genMachineImm(offset));
                }
            }
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }

        ArrayType *type = (ArrayType *)(((PointerType *)(operands[1]->getType()))->getType());
        Type *elementType = type->getElementType();
        size = elementType->getSize() / 8;
    }
    auto size1 = genMachineVReg();
    if (size > -255 && size < 255)
    {
        cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, size1, genMachineImm(size));
    }
    else
    {
        cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, size1, genMachineImm(size));
    }
    cur_block_zhouyanlin->InsertInst(cur_inst);
    size1 = new MachineOperand(*size1);
    auto off = genMachineVReg();



    cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::MUL, off, idx, size1);
    off = new MachineOperand(*off);
    cur_block_zhouyanlin->InsertInst(cur_inst);
    if (paramFirst || !first)
    {
        auto arr = genMachineOperand(operands[1]);
        auto in = operands[1]->getDef();
        if (in && in->isGep())
        {
            auto gep = (GepInstruction *)in;
            if (gep->hasNoAsm())
            {
                gep->setInit(nullptr, 0);
                gep->genMachineCode(builder_zhouyanlin);
            }
        }
        cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, dst, arr, off);
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    else
    {
        auto addr = genMachineVReg();
        auto base1 = new MachineOperand(*base);
        if (operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry *)(operands[1]->getEntry()))->isGlobal())
        {
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, addr, base1, off);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            addr = new MachineOperand(*addr);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, dst, addr);
        }
        else
        {
            auto fp = genMachineReg(11);
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, addr, fp, base1);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            addr = new MachineOperand(*addr);
            cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, dst, addr, off);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
}

PhiInstruction::PhiInstruction(Operand *dst, BasicBlock *insert_bb)
    : Instruction(PHI, insert_bb)
{
    operands.push_back(dst);
    this->dst = dst;
    this->originDef = dst;
    dst->setDef(this);
}

PhiInstruction::~PhiInstruction()
{
    dst->setDef(nullptr);
    if (dst->usersNum() == 0)
        delete dst;
    for (auto it_zhouyanlin : srcs)
        it_zhouyanlin.second->removeUse(this);
}

void PhiInstruction::cleanUse()
{
    for (auto it_zhouyanlin : srcs)
        it_zhouyanlin.second->removeUse(this);
}

void PhiInstruction::output() const
{
    fprintf(yyout, "  %s = phi %s", dst->toStr().c_str(), dst->getType()->toStr().c_str());
    bool first_zhouyanlin = true;
    for (auto it_zhouyanlin = srcs.begin(); it_zhouyanlin != srcs.end(); it_zhouyanlin++)
    {
        if (!first_zhouyanlin)
            fprintf(yyout, ", ");
        else
            first_zhouyanlin = false;
        fprintf(yyout, "[ %s , %%B%d ]", it_zhouyanlin->second->toStr().c_str(),
                it_zhouyanlin->first->getNo());
    }
    fprintf(yyout, "\n");
}

std::pair<int, int> PhiInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res_zhouyanlin, tmp;
    res_zhouyanlin = {1, 0};
    
    return res_zhouyanlin;
}

void PhiInstruction::addSrc(BasicBlock *block_zhouyanlin, Operand *src)
{
    operands.push_back(src);
    srcs.insert(std::make_pair(block_zhouyanlin, src));
    src->addUse(this);
}

void PhiInstruction::removeSrc(BasicBlock *block_zhouyanlin)
{
    for (auto it_zhouyanlin = srcs.begin(); it_zhouyanlin != srcs.end(); it_zhouyanlin++)
    {
        if (it_zhouyanlin->first == block_zhouyanlin)
        {
            srcs.erase(block_zhouyanlin);
            removeUse(it_zhouyanlin->second);
            it_zhouyanlin->second->removeUse(this);
            return;
        }
    }
    return;
}

bool PhiInstruction::findSrc(BasicBlock *block_zhouyanlin)
{
    for (auto it_zhouyanlin = srcs.begin(); it_zhouyanlin != srcs.end(); it_zhouyanlin++)
    {
        if (it_zhouyanlin->first == block_zhouyanlin)
        {
            return true;
        }
    }
    return false;
}

void PhiInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    for (auto &it : srcs)
    {
        if (it.second == old_zhouyanlin)
        {
            it.second->removeUse(this);
            it.second = new_;
            new_->addUse(this);
        }
    }
    for (auto it = operands.begin() + 1; it != operands.end(); it++)
        if (*it == old_zhouyanlin)
            *it = new_;
}

void PhiInstruction::removeUse(Operand *use_zhouyanlin)
{
    auto it = find(operands.begin() + 1, operands.end(), use_zhouyanlin);
    if (it != operands.end())
        operands.erase(it);
}

void PhiInstruction::replaceDef(Operand *new__zhouyanlin)
{
    dst->removeDef(this);
    dst = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

void PhiInstruction::replaceOriginDef(Operand *new__zhouyanlin)
{
    this->originDef = new__zhouyanlin;
}

void PhiInstruction::changeSrcBlock(
    std::map<BasicBlock *, std::vector<BasicBlock *>> changes,
    bool autoInline)
{
    std::map<Operand *, BasicBlock *> originBlocks;
    if (autoInline)
    {
        for (auto it_zhouyanlin : srcs)
            originBlocks[it_zhouyanlin.second] = it_zhouyanlin.first;
    }
    bool flag_zhouyanlin;
    while (true)
    {
        flag_zhouyanlin = false;
        for (auto &it : srcs)
        {
            if (changes.find(it.first) != changes.end())
            {
                auto vec = changes[it.first];
                auto src = srcs[it.first];
                for (auto b : vec)
                {
                    if (srcs.find(b) != srcs.end() && srcs[b] != src)
                    {
                        auto &phiBlocks = parent->getPhiBlocks();
                        auto iter = phiBlocks.find(b);
                        BasicBlock *b1;
                        if (iter != phiBlocks.end())
                        {
                            b1 = iter->second;
                        }
                        else
                        {
                            // 需要添加一个block
                            b1 = new BasicBlock(b->getParent());
                            phiBlocks.insert(std::make_pair(b, b1));
                            b->addSucc(b1);
                            b->removeSuccFromEnd(this->getParent());
                            auto i = (CondBrInstruction *)(b->rbegin());
                            if (autoInline)
                            {
                                auto nowSrc = srcs[b];
                                auto originBlock = originBlocks[nowSrc];
                                while (true)
                                {
                                    if (i->getOriginFalse() == originBlock)
                                    {
                                        i->setTrueBranch(b1);
                                        break;
                                    }
                                    else if (i->getOriginTrue() ==
                                             originBlock)
                                    {
                                        i->setFalseBranch(b1);
                                        break;
                                    }
                                    if (changes.count(originBlock))
                                        originBlock = changes[originBlock][0];
                                }
                            }
                            else
                            {
                                auto temp = it.first;
                                int ii = 0;
                                while (true)
                                {
                                    if (i->getOriginTrue() == temp)
                                    {
                                        i->setTrueBranch(b1);
                                        break;
                                    }
                                    else if (i->getOriginFalse() == temp)
                                    {
                                        i->setFalseBranch(b1);
                                        break;
                                    }
                                    if (changes.count(temp))
                                        temp = changes[temp][0];
                                    ii++;
                                    if (ii == 10)
                                    {
                                        ii = -1;
                                        break;
                                    }
                                }
                                if (ii == -1)
                                {
                                    if (i->getOriginTrue() == this->parent)
                                    {
                                        i->setTrueBranch(b1);
                                    }
                                    else if (i->getOriginFalse() ==
                                             this->parent)
                                    {
                                        i->setFalseBranch(b1);
                                    }
                                }
                            }
                            b1->addPred(b);
                            auto unCond =
                                new UncondBrInstruction(this->getParent(), b1);
                            unCond->setNoStraighten();
                            this->getParent()->removePredFromEnd(b);
                            this->getParent()->addPred(b1);
                            b1->addSucc(this->getParent());
                        }
                        addSrc(b1, src);
                    }
                    else
                        addSrc(b, src);
                }
                auto operand = it.second;
                operand->removeUse(this);
                operands.erase(
                    find(operands.begin() + 1, operands.end(), operand));
                srcs.erase(it.first);
                flag_zhouyanlin = true;
                break;
            }
        }
        if (!flag_zhouyanlin)
            break;
    }
}

Operand *PhiInstruction::getSrc(BasicBlock *block_zhouyanlin)
{
    if (srcs.find(block_zhouyanlin) != srcs.end())
        return srcs[block_zhouyanlin];
    return nullptr;
}
void FptosiInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    MachineInstruction *cur_inst;
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();

    auto src_operand = genMachineFloatOperand(src);
    auto dst_operand = genMachineOperand(dst);

    if (src_operand->isImm())
    {
        auto tmp = genMachineVReg(true);
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                        internal_reg, src_operand);
        cur_block_zhouyanlin->InsertInst(cur_inst);
        internal_reg = new MachineOperand(*internal_reg);
        cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, tmp, internal_reg);
        cur_block_zhouyanlin->InsertInst(cur_inst);
        src_operand = tmp;
    }
    auto vcvtDst = genMachineVReg(true);
    cur_inst = new VcvtMInstruction(cur_block_zhouyanlin, VcvtMInstruction::F2S, vcvtDst,
                                    src_operand);
    cur_block_zhouyanlin->InsertInst(cur_inst);
    auto movUse = new MachineOperand(*vcvtDst);
    cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, dst_operand, movUse);

    cur_block_zhouyanlin->InsertInst(cur_inst);
}

Instruction *SitofpInstruction::copy()
{
    return new SitofpInstruction(*this);
}

Instruction *BitcastInstruction::copy()
{
    return new BitcastInstruction(*this);
}

Instruction *ShlInstruction::copy()
{
    return new ShlInstruction(*this);
}

Instruction *AshrInstruction::copy()
{
    return new AshrInstruction(*this);
}

void BitcastInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
}

void PhiInstruction::cleanUseInOperands()
{
    std::vector<Operand *> v({dst});
    v.swap(operands);
}

void SitofpInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
        src = new_;
    }
}

void FptosiInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
        src = new_;
    }
}

bool Instruction::isAddZero()
{
    if (isAdd())
    {
        auto src2 = operands[2];
        if (src2->isConst() && src2->getConstVal() == 0)
            return true;
    }
    return false;
}

std::string CallInstruction::getHash()
{
    IdentifierSymbolEntry *funcSE_zhouyanlin = (IdentifierSymbolEntry *)func;
    if (funcSE_zhouyanlin->isSysy() || funcSE_zhouyanlin->getName() == "llvm.memset.p0.i32")
    {
        return "";
    }
    else
    {
        auto func = funcSE_zhouyanlin->getFunction();
        if (func->getEssential() == 1)
        {
            return "";
        }
        else
        {
            std::stringstream s_zhouyanlin;
            s_zhouyanlin << "call ";
            s_zhouyanlin << funcSE_zhouyanlin->toStr();
            for (auto it = operands.begin() + 1; it != operands.end(); it++)
                s_zhouyanlin << " " << *it;
            return s_zhouyanlin.str();
        }
    }
}

void SitofpInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    MachineInstruction *cur_inst;
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();

    auto src_operand = genMachineOperand(src);

    if (src_operand->isImm())
    {
        auto tmp = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, tmp,
                                        src_operand);
        cur_block_zhouyanlin->InsertInst(cur_inst);
        src_operand = new MachineOperand(*tmp);
    }
    auto movDst = genMachineVReg(true);
    cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV, movDst,
                                   src_operand);
    cur_block_zhouyanlin->InsertInst(cur_inst);
    auto vcvtUse = new MachineOperand(*movDst);
    auto dst_operand = genMachineFloatOperand(dst);
    cur_inst = new VcvtMInstruction(cur_block_zhouyanlin, VcvtMInstruction::S2F, dst_operand, vcvtUse);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

BitcastInstruction::BitcastInstruction(Operand *dst,
                                       Operand *src,
                                       BasicBlock *insert_bb)
    : Instruction(BITCAST, insert_bb), dst(dst), src(src)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
    flag = false;
}

void BitcastInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *src = operands[1];
    fprintf(yyout, "  %s = bitcast %s %s to %s\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str(),
            dst->getType()->toStr().c_str());
}

BitcastInstruction::~BitcastInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void BitcastInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto ptr = (PointerType *)(dst->getType());
    auto type = ptr->getType();
    if (!(type->isInt() && type->getSize() == 8))
    {
        auto block_zhouyanlin = builder_zhouyanlin->getBlock();
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        auto zero = genMachineImm(0);
        auto in_zhouyanlin = new BinaryMInstruction(block_zhouyanlin, BinaryMInstruction::ADD, dst, src, zero);
        block_zhouyanlin->InsertInst(in_zhouyanlin);
    }
}

bool AllocaInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::ALLOCA);
    return true;
};

bool LoadInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::LOAD);
    if (operands[1]->getDef() == nullptr)
    {
        SSAGraphNode *node1;
        if (operands[1]->getEntry()->isVariable())
        {
            IdentifierSymbolEntry *idSe_zhouyanlin = (IdentifierSymbolEntry *)operands[1]->getEntry();
            if (idSe_zhouyanlin->isGlobal())
            {
                node1 = new SSAGraphNode(operands[1], SSAGraphNode::GLOBAL);
            }
            else if (idSe_zhouyanlin->isParam())
            {
                node1 = new SSAGraphNode(operands[1], SSAGraphNode::FUNCPARA);
            }
        }
        node->addChild(node1);
        return true;
    }
    auto node1 = operands[1]->getDef()->getNode();
    node->addChild(node1);
    return true;
}

std::pair<int, int> LoadInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> l_zhouyanlin;
    if (value.find(this->getUse()[0]) == value.end())
        l_zhouyanlin = this->getUse()[0]->getInitLatticeValue();
    else
        l_zhouyanlin = value[this->getUse()[0]];
    return l_zhouyanlin;
}

bool BinaryInstruction::genNode()
{
    // add 0
    if (operands[2]->isZero())
    {
        auto se = operands[1]->getEntry();
        if (se->isConstant())
        {
            int val_zhouyanlin = ((ConstantSymbolEntry *)se)->getValue();
            if (opcode == AND || opcode == MUL)
                val_zhouyanlin = 0;
            node = new SSAGraphNode(val_zhouyanlin);
            return true;
        }
        else
        {
            if (operands[1]->getDef() == nullptr)
            {
                if (operands[1]->getEntry()->isVariable())
                {
                    IdentifierSymbolEntry *idSe_zhouyanlin = (IdentifierSymbolEntry *)operands[1]->getEntry();
                    if (idSe_zhouyanlin->isGlobal())
                    {
                        node = new SSAGraphNode(operands[1], SSAGraphNode::GLOBAL);
                    }
                    else if (idSe_zhouyanlin->isParam())
                    {
                        node = new SSAGraphNode(operands[1], SSAGraphNode::FUNCPARA);
                    }
                }
            }
            else
            {
                auto def_zhouyanlin = operands[1]->getDef();
                node = def_zhouyanlin->getNode();
            }
            if (node != nullptr)
                return true;
            else
            {
                return false;
            }
        }
    }
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    // const +/-/... const
    if (se1->isConstant() && se2->isConstant())
    {
        int val1_zhouyanlin = ((ConstantSymbolEntry *)se1)->getValue();
        int val2_zhouyanlin = ((ConstantSymbolEntry *)se2)->getValue();
        int val_zhouyanlin;
        switch (opcode)
        {
        case SUB:
            val_zhouyanlin = val1_zhouyanlin - val2_zhouyanlin;
            break;
        case ADD:
            val_zhouyanlin = val1_zhouyanlin + val2_zhouyanlin;
            break;
        case AND:
            val_zhouyanlin = val1_zhouyanlin && val2_zhouyanlin;
            break;
        case OR:
            val_zhouyanlin = val1_zhouyanlin || val2_zhouyanlin;
            break;
        case MUL:
            val_zhouyanlin = val1_zhouyanlin * val2_zhouyanlin;
            break;
        case DIV:
            val_zhouyanlin = val1_zhouyanlin / val2_zhouyanlin;
            break;
        case MOD:
            val_zhouyanlin = val1_zhouyanlin % val2_zhouyanlin;
            break;
        }
        node = new SSAGraphNode(val_zhouyanlin);
        return true;
    }
    // 1 temp 1 const
    node = new SSAGraphNode(this, opcode + SSAGraphNode::SUB);
    SSAGraphNode *node1, *node2;
    if (se1->isConstant())
    {
        int val1_zhouyanlin = ((ConstantSymbolEntry *)se1)->getValue();
        node1 = new SSAGraphNode(val1_zhouyanlin);
    }
    else
        node1 = operands[1]->getDef()->getNode();
    if (se2->isConstant())
    {
        int val2_zhouyanlin = ((ConstantSymbolEntry *)se2)->getValue();
        node2 = new SSAGraphNode(val2_zhouyanlin);
    }
    else
        node2 = operands[2]->getDef()->getNode();
    bool flag = true;
    if (node1 == nullptr || node2 == nullptr)
        flag = false;
    node->addChild(node1);
    node->addChild(node2);
    return flag;
}

bool CmpInstruction::genNode()
{
    node = new SSAGraphNode(this, opcode + SSAGraphNode::E);
    auto se1 = operands[1]->getEntry();
    auto se2 = operands[2]->getEntry();
    SSAGraphNode *node1_zhouyanlin, *node2_zhouyanlin;
    if (se1->isConstant())
    {
        int val1 = ((ConstantSymbolEntry *)se1)->getValue();
        node1_zhouyanlin = new SSAGraphNode(val1);
    }
    else
        node1_zhouyanlin = operands[1]->getDef()->getNode();
    if (se2->isConstant())
    {
        int val2 = ((ConstantSymbolEntry *)se2)->getValue();
        node2_zhouyanlin = new SSAGraphNode(val2);
    }
    else
        node2_zhouyanlin = operands[2]->getDef()->getNode();
    bool flag = true;
    if (node1_zhouyanlin == nullptr || node2_zhouyanlin == nullptr)
        flag = false;
    node->addChild(node1_zhouyanlin);
    node->addChild(node2_zhouyanlin);
    return flag;
}

bool CallInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::CALL);
    return true;
}

bool ZextInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::ZEXT);
    auto node1_zhouyanlin = operands[1]->getDef()->getNode();
    bool flag = true;
    if (node1_zhouyanlin == nullptr)
        flag = false;
    node->addChild(node1_zhouyanlin);
    return flag;
}

bool XorInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::XOR);
    auto se1_zhouyanlin = operands[1]->getEntry();
    SSAGraphNode *node1;
    if (se1_zhouyanlin->isConstant())
    {
        int val1 = ((ConstantSymbolEntry *)se1_zhouyanlin)->getValue();
        node1 = new SSAGraphNode(val1);
    }
    else
        node1 = operands[1]->getDef()->getNode();
    bool flag = true;
    if (node1 == nullptr)
        flag = false;
    node->addChild(node1);
    return flag;
}

bool BitcastInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::BITCAST);
    auto node1 = operands[1]->getDef()->getNode();
    bool flag = true;
    if (node1 == nullptr)
        flag = false;
    node->addChild(node1);
    return flag;
}

bool GepInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::GEP);
    SSAGraphNode *node1_zhouyanlin;
    if (operands[1]->getDef() == nullptr)
    {
        if (operands[1]->getEntry()->isVariable())
        {
            IdentifierSymbolEntry *idSe = (IdentifierSymbolEntry *)operands[1]->getEntry();
            if (idSe->isGlobal())
            {
                node1_zhouyanlin = new SSAGraphNode(operands[1], SSAGraphNode::GLOBAL);
            }
            else if (idSe->isParam())
            {
                node1_zhouyanlin = new SSAGraphNode(operands[1], SSAGraphNode::FUNCPARA);
            }
        }
        node->addChild(node1_zhouyanlin);
    }
    else
    {
        node1_zhouyanlin = operands[1]->getDef()->getNode();
        node->addChild(node1_zhouyanlin);
    }

    SSAGraphNode *node2;
    auto se2 = operands[2]->getEntry();
    if (se2->isConstant())
    {
        int val2 = ((ConstantSymbolEntry *)se2)->getValue();
        node2 = new SSAGraphNode(val2);
    }
    else
        node2 = operands[2]->getDef()->getNode();
    node->addChild(node2);
    bool flag = true;
    if (node1_zhouyanlin == nullptr || node2 == nullptr)
        flag = false;
    return flag;
}

bool PhiInstruction::genNode()
{
    bool ret_zhouyanlin = true;
    node = new SSAGraphNode(this, SSAGraphNode::PHI);
    for (int i = 1; i < (int)operands.size(); i++)
    {
        auto operand = operands[i];
        auto se = operand->getEntry();
        SSAGraphNode *node1;
        if (se->isConstant())
        {
            int val = ((ConstantSymbolEntry *)se)->getValue();
            node1 = new SSAGraphNode(val);
        }
        else
        {
            node1 = operand->getDef()->getNode();
            if (!node1)
                ret_zhouyanlin = false;
        }
        node->addChild(node1);
    }
    return ret_zhouyanlin;
}

std::string LoadInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "load ";
    s_zhouyanlin << operands[1]->toStr();
    return s_zhouyanlin.str();
}

std::string BinaryInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    switch (opcode)
    {
    case SUB:
        s_zhouyanlin << "sub ";
        break;
    case ADD:
        s_zhouyanlin << "add ";
        break;
    case AND:
        s_zhouyanlin << "and ";
        break;
    case OR:
        s_zhouyanlin << "or ";
        break;
    case MUL:
        s_zhouyanlin << "mul ";
        break;
    case DIV:
        s_zhouyanlin << "div ";
        break;
    case MOD:
        s_zhouyanlin << "mod ";
        break;
    }
    auto ope1 = operands[1];
    auto ope2 = operands[2];

    if (opcode >= ADD && opcode <= MUL)
    {
        auto se1 = ope1->getEntry();
        auto se2 = ope2->getEntry();
        if (se2->isConstant())
        {
            if (se1->isConstant())
            {
                auto str1 = se1->toStr();
                auto str2 = se2->toStr();
                if (str1 < str2)
                    s_zhouyanlin << str1 << " " << str2;
                else
                    s_zhouyanlin << str2 << " " << str1;
            }
            else
            {
                s_zhouyanlin << ope2->toStr() << " " << ope1->toStr();
            }
        }
        else
        {
            if (se1->isConstant())
            {
                s_zhouyanlin << ope1->toStr() << " " << ope2->toStr();
            }
            else
            {
                auto str1 = se1->toStr();
                auto str2 = se2->toStr();
                if (str1 < str2)
                    s_zhouyanlin << str1 << " " << str2;
                else
                    s_zhouyanlin << str2 << " " << str1;
            }
        }
    }
    else
    {
        s_zhouyanlin << ope1->toStr() << " " << ope2->toStr();
    }
    return s_zhouyanlin.str();
}


void SitofpInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

SitofpInstruction::~SitofpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void CallInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    MachineOperand *operand;
    MachineInstruction *cur_inst;

    size_t idx;
    auto funcSE_zhouyanlin = (IdentifierSymbolEntry *)func;
    if (funcSE_zhouyanlin->getName() == "llvm.memset.p0.i32")
    {
        auto r0 = genMachineReg(0);
        auto r1 = genMachineReg(1);
        auto r2 = genMachineReg(2);
        auto int8Ptr = operands[1];
        auto bitcast = (BitcastInstruction *)(int8Ptr->getDef());
        if (!bitcast->getFlag())
        {
            auto arraySE =
                (TemporarySymbolEntry *)(bitcast->getUse()[0]->getEntry());
            int offset = arraySE->getOffset();
            operand = genMachineVReg();
            auto fp = genMachineReg(11);
            if (offset > -255 && offset < 255)
            {
                cur_block_zhouyanlin->InsertInst(
                    new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, r0, fp, genMachineImm(offset)));
            }
            else
            {
                cur_inst =
                    new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                         operand, genMachineImm(offset));
                operand = new MachineOperand(*operand);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                cur_block_zhouyanlin->InsertInst(new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD, r0, fp, operand));
            }
        }
        else
        {
            cur_block_zhouyanlin->InsertInst(new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, r0,genMachineOperand(bitcast->getUse()[0])));
        }
        cur_block_zhouyanlin->InsertInst(new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, r1, genMachineImm(0)));
        auto len = genMachineOperand(operands[3]);
        if (len->isImm() && len->getVal() > 255)
        {
            operand = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, operand, len);
            operand = new MachineOperand(*operand);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
            operand = len;
        cur_block_zhouyanlin->InsertInst(
            new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV, r2, operand));
        cur_block_zhouyanlin->InsertInst(new BranchMInstruction(
            cur_block_zhouyanlin, BranchMInstruction::BL, new MachineOperand("@memset")));
        return;
    }

    int stk_cnt = 0;
    std::vector<MachineOperand *> vec;

    bool need_align = false; 
    int float_num = 0;
    int int_num = 0;
    for (size_t i = 1; i < operands.size(); i++)
    {
        if (operands[i]->getType()->isFloat())
        {
            float_num++;
        }
        else
        {
            int_num++;
        }
    }

    int push_num = 0;
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

    int gpreg_cnt = 1;
    for (idx = 1; idx < operands.size(); idx++)
    {
        if (gpreg_cnt == 5)
            break;
        if (operands[idx]->getType()->isFloat())
        {
            continue;
        }
        operand = genMachineReg(gpreg_cnt - 1);
        auto src = genMachineOperand(operands[idx]);
        if (src->isImm() && src->getVal() > 255)
        {
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            operand, src);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV,
                                           operand, src);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
        gpreg_cnt++;
    }

    size_t int_idx = idx;

    int fpreg_cnt = 1;
    for (idx = 1; idx < operands.size(); idx++)
    {
        if (fpreg_cnt == 5 && !need_align)
            break;
        if (fpreg_cnt == 6 && need_align)
        {
            break;
        }
        if (!operands[idx]->getType()->isFloat())
        {
            continue;
        }
        operand = genMachineFReg(fpreg_cnt - 1);
        auto src = genMachineFloatOperand(operands[idx]);
        if (src->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR,
                                            internal_reg, src);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                           operand, internal_reg);
        }
        else
        {
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                           operand, src);
        }
        cur_block_zhouyanlin->InsertInst(cur_inst);
        fpreg_cnt++;
    }

    size_t float_idx = idx;

    idx = std::min(float_idx, int_idx);

    for (size_t i = operands.size() - 1; i >= idx; i--)
    {
        if (operands[i]->getType()->isFloat() && i >= float_idx)
        {
            operand = genMachineFloatOperand(operands[i]);
            if (operand->isImm())
            {
                auto dst = genMachineVReg(true);
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(
                    cur_block_zhouyanlin, LoadMInstruction::LDR, internal_reg, operand);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                internal_reg = new MachineOperand(*internal_reg);
                cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                               dst, internal_reg);
                cur_block_zhouyanlin->InsertInst(cur_inst);
                operand = new MachineOperand(*dst);
            }
            cur_inst = new StackMInstruction(
                cur_block_zhouyanlin, StackMInstruction::VPUSH, vec, operand);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            stk_cnt++;
        }
        else if (!operands[i]->getType()->isFloat() && i >= int_idx)
        {
            operand = genMachineOperand(operands[i]);
            if (operand->isImm())
            {
                auto dst = genMachineVReg();
                if (operand->getVal() < 256)
                {
                    cur_inst = new MovMInstruction(
                        cur_block_zhouyanlin, MovMInstruction::MOV, dst, operand);
                }
                else
                {
                    cur_inst = new LoadMInstruction(
                        cur_block_zhouyanlin, LoadMInstruction::LDR, dst, operand);
                }
                cur_block_zhouyanlin->InsertInst(cur_inst);
                operand = new MachineOperand(*dst);
            }
            cur_inst = new StackMInstruction(cur_block_zhouyanlin, StackMInstruction::PUSH,
                                             vec, operand);
            cur_block_zhouyanlin->InsertInst(cur_inst);
            stk_cnt++;
        }
    }

    auto label = new MachineOperand(func->toStr().c_str());
    cur_inst = new BranchMInstruction(cur_block_zhouyanlin, BranchMInstruction::BL, label);
    cur_block_zhouyanlin->InsertInst(cur_inst);
    if ((gpreg_cnt >= 5 || fpreg_cnt >= 5) && stk_cnt != 0)
    {
        auto off = genMachineImm(stk_cnt * 4);
        auto sp = new MachineOperand(MachineOperand::REG, 13);
        cur_inst = new BinaryMInstruction(cur_block_zhouyanlin, BinaryMInstruction::ADD,
                                          sp, sp, off);
        cur_block_zhouyanlin->InsertInst(cur_inst);
    }
    if (dst)
    {
        if (dst->getType()->isFloat())
        {
            operand = genMachineFloatOperand(dst);
            auto s0 = new MachineOperand(MachineOperand::REG, 16, true);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::VMOV,
                                           operand, s0);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
        else
        {
            operand = genMachineOperand(dst);
            auto r0 = new MachineOperand(MachineOperand::REG, 0);
            cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOV,
                                           operand, r0);
            cur_block_zhouyanlin->InsertInst(cur_inst);
        }
    }
}


std::string CmpInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    switch (opcode)
    {
    case E:
        s_zhouyanlin << "e ";
        break;
    case NE:
        s_zhouyanlin << "ne ";
        break;
    case L:
        s_zhouyanlin << "l ";
        break;
    case LE:
        s_zhouyanlin << "le ";
        break;
    case G:
        s_zhouyanlin << "g ";
        break;
    case GE:
        s_zhouyanlin << "ge ";
        break;
    }
    s_zhouyanlin << operands[1]->toStr() << " " << operands[2]->toStr();
    return s_zhouyanlin.str();
}

std::string XorInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "xor ";
    s_zhouyanlin << operands[1]->toStr();
    return s_zhouyanlin.str();
}

std::string GepInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "gep ";
    s_zhouyanlin << operands[1]->toStr() << " " << operands[2]->toStr();
    return s_zhouyanlin.str();
}

std::string PhiInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "phi";
    std::vector<std::string> strs;

    for (auto it : srcs)
    {
        std::stringstream ss;
        auto block = it.first;
        ss << "B" << block->getNo() << " " << it.second->toStr();
        strs.push_back(ss.str());
    }
    std::sort(strs.begin(), strs.end());
    for (auto str : strs)
        s_zhouyanlin << " " << str;
    return s_zhouyanlin.str();
}

ShlInstruction::ShlInstruction(Operand *dst,
                               Operand *src,
                               Operand *num,
                               BasicBlock *insert_bb)
    : Instruction(SHL, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    operands.push_back(num);
    dst->setDef(this);
    src->addUse(this);
    num->addUse(this);
}

void ShlInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands[1] == old_zhouyanlin)
    {
        operands[1]->removeUse(this);
        operands[1] = new_;
        new_->addUse(this);
    }
    else if (operands[2] == old_zhouyanlin)
    {
        operands[2]->removeUse(this);
        operands[2] = new_;
        new_->addUse(this);
    }
}

void ShlInstruction::replaceDef(Operand *new__zhouyanlin)
{
    operands[0]->removeDef(this);
    operands[0] = new__zhouyanlin;
    new__zhouyanlin->setDef(this);
}

ShlInstruction::~ShlInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void ShlInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string num = operands[2]->toStr();
    std::string src_type_zhouyanlin;
    src_type_zhouyanlin = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = shl %s %s, %s\n", dst.c_str(), src_type_zhouyanlin.c_str(), src.c_str(), num.c_str());
}

bool ShlInstruction::genNode()
{
    node = new SSAGraphNode(this, SSAGraphNode::SHL);
    auto se1_zhouyanlin = operands[1]->getEntry();
    auto se2_zhouyanlin = operands[2]->getEntry();
    SSAGraphNode *node1, *node2;
    if (se1_zhouyanlin->isConstant())
    {
        int val1 = ((ConstantSymbolEntry *)se1_zhouyanlin)->getValue();
        node1 = new SSAGraphNode(val1);
    }
    else
        node1 = operands[1]->getDef()->getNode();
    if (se2_zhouyanlin->isConstant())
    {
        int val2 = ((ConstantSymbolEntry *)se2_zhouyanlin)->getValue();
        node2 = new SSAGraphNode(val2);
    }
    else
        node2 = operands[2]->getDef()->getNode();
    bool flag = true;
    if (node1 == nullptr || node2 == nullptr)
        flag = false;
    node->addChild(node1);
    node->addChild(node2);
    return flag;
}

std::string ShlInstruction::getHash()
{
    std::stringstream s_zhouyanlin;
    s_zhouyanlin << "shl ";
    s_zhouyanlin << operands[1]->toStr() << " " << operands[2]->toStr();
    return s_zhouyanlin.str();
}

std::pair<int, int> ShlInstruction::getLatticeValue(
    std::map<Operand *, std::pair<int, int>> &value)
{
    std::pair<int, int> res_zhouyanlin, val1, val2;
    if (value.find(this->getUse()[0]) == value.end())
        val1 = this->getUse()[0]->getInitLatticeValue();
    else
        val1 = value[this->getUse()[0]];
    if (value.find(this->getUse()[1]) == value.end())
        val2 = this->getUse()[1]->getInitLatticeValue();
    else
        val2 = value[this->getUse()[1]];

    if (val1.first == -1 || val2.first == -1)
        res_zhouyanlin = {-1, 0};
    else if (val1.first == 0 && val2.first == 0)
    {
        res_zhouyanlin.first = 0;
        res_zhouyanlin.second = val1.second << val2.second;
    }
    else
        res_zhouyanlin.first = 1;
    return res_zhouyanlin;
}

void AshrInstruction::genMachineCode(AsmBuilder *builder_zhouyanlin)
{
    auto cur_block_zhouyanlin = builder_zhouyanlin->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    auto num = genMachineOperand(operands[2]);
    // 目前只是立即数
    assert(num->isImm());
    if (src->isImm())
    {
        auto temp = genMachineVReg();
        cur_block_zhouyanlin->InsertInst(new LoadMInstruction(cur_block_zhouyanlin, LoadMInstruction::LDR, temp, src));
        src = new MachineOperand(*temp);
    }
    auto cur_inst = new MovMInstruction(cur_block_zhouyanlin, MovMInstruction::MOVASR, dst,
                                        src, MachineInstruction::NONE, num);
    cur_block_zhouyanlin->InsertInst(cur_inst);
}

bool BinaryInstruction::isConstExp()
{
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst())
    {
        flag = true;
        double val1_zhouyanlin = src1->getConstVal();
        double val2_zhouyanlin = src2->getConstVal();
        switch (opcode)
        {
        case SUB:
            constVal = val1_zhouyanlin - val2_zhouyanlin;
            break;
        case ADD:
            constVal = val1_zhouyanlin + val2_zhouyanlin;
            break;
        case AND:
            constVal = val1_zhouyanlin && val2_zhouyanlin;
            break;
        case OR:
            constVal = val1_zhouyanlin || val2_zhouyanlin;
            break;
        case MUL:
            constVal = val1_zhouyanlin * val2_zhouyanlin;
            break;
        case DIV:
            constVal = val1_zhouyanlin / val2_zhouyanlin;
            break;
        case MOD:
            constVal = (int)val1_zhouyanlin % (int)val2_zhouyanlin;
            break;
        }
    }
    return flag;
}



void RetInstruction::replaceDef(Operand *new_)
{
    if (operands.size())
    {
        operands[0]->removeDef(this);
        operands[0] = new_;
        new_->setDef(this);
    }
}

void RetInstruction::replaceUse(Operand *old_zhouyanlin, Operand *new_)
{
    if (operands.size() && operands[0] == old_zhouyanlin)
    {
        operands[0]->removeUse(this);
        operands[0] = new_;
        new_->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if (!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if (operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret_zhouyanlin, type;
        ret_zhouyanlin = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret_zhouyanlin.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst,
                                     SymbolEntry *se,
                                     BasicBlock *insert_bb)
    : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

void AllocaInstruction::replaceDef(Operand *new_)
{
    operands[0]->removeDef(this);
    operands[0] = new_;
    new_->setDef(this);
}




bool CmpInstruction::isConstExp()
{
    bool flag = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst())
    {
        flag = true;
        double val1_zhouyanlin = src1->getConstVal();
        double val2_zhouyanlin = src2->getConstVal();
        switch (opcode)
        {
        case E:
            constVal = val1_zhouyanlin == val2_zhouyanlin;
            break;
        case NE:
            constVal = val1_zhouyanlin != val2_zhouyanlin;
            break;
        case L:
            constVal = val1_zhouyanlin < val2_zhouyanlin;
            break;
        case LE:
            constVal = val1_zhouyanlin <= val2_zhouyanlin;
            break;
        case G:
            constVal = val1_zhouyanlin > val2_zhouyanlin;
            break;
        case GE:
            constVal = val1_zhouyanlin >= val2_zhouyanlin;
            break;
        }
    }
    return flag;
}

bool XorInstruction::isConstExp()
{
    bool flag_zhouyanlin = false;
    auto src = operands[1];
    if (src->isConst())
    {
        flag_zhouyanlin = true;
        double val = src->getConstVal();
        assert(val == 0 || val == 1);
        if (val)
            constVal = 0;
        else
            constVal = 1;
    }
    return flag_zhouyanlin;
}

bool ShlInstruction::isConstExp()
{
    bool flag_zhouyanlin = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst())
    {
        flag_zhouyanlin = true;
        int val1_zhouyanlin = src1->getConstVal();
        int val2_zhouyanlin = src2->getConstVal();
        constVal = val1_zhouyanlin << val2_zhouyanlin;
    }
    return flag_zhouyanlin;
}

bool AshrInstruction::isConstExp()
{
    bool flag_zhouyanlin = false;
    auto src1 = operands[1];
    auto src2 = operands[2];
    if (src1->isConst() && src2->isConst())
    {
        flag_zhouyanlin = true;
        int val1 = src1->getConstVal();
        int val2 = src2->getConstVal();
        constVal = val1 >> val2;
    }
    return flag_zhouyanlin;
}

Instruction *AllocaInstruction::copy()
{
    return new AllocaInstruction(*this);
}

Instruction *LoadInstruction::copy()
{
    return new LoadInstruction(*this);
}

Instruction *StoreInstruction::copy()
{
    return new StoreInstruction(*this);
}

Instruction *BinaryInstruction::copy()
{
    return new BinaryInstruction(*this);
}

Instruction *CmpInstruction::copy()
{
    return new CmpInstruction(*this);
}

Instruction *UncondBrInstruction::copy()
{
    return new UncondBrInstruction(*this);
}

Instruction *CondBrInstruction::copy()
{
    auto ret_zhouyanlin = new CondBrInstruction(*this);
    ret_zhouyanlin->cleanOriginFalse();
    ret_zhouyanlin->cleanOriginTrue();
    return ret_zhouyanlin;
}

Instruction *RetInstruction::copy()
{
    return new RetInstruction(*this);
}

Instruction *CallInstruction::copy()
{
    return new CallInstruction(*this);
}

Instruction *ZextInstruction::copy()
{
    return new ZextInstruction(*this);
}

Instruction *XorInstruction::copy()
{
    return new XorInstruction(*this);
}

Instruction *GepInstruction::copy()
{
    return new GepInstruction(*this);
}

Instruction *PhiInstruction::copy()
{
    return new PhiInstruction(*this);
}

Instruction *FptosiInstruction::copy()
{
    return new FptosiInstruction(*this);
}

