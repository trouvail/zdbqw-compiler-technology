#include "SSAGraph.h"

BasicBlock *SSAGraphNode::getDefBB()
{
    if (this->type == CONST){
        return nullptr;
    }
    else{
        return this->ins->getParent();
    }
}

Operand *SSAGraphNode::getOperator()
{
    if (this->type == CONST){
        return new Operand(new ConstantSymbolEntry(TypeSystem::intType, val));
    }
    else{
        return this->ins->getDef();
    }
}

SSAGraphNode *SSAGraphNode::IV_clone(Operand *result)
{

    if (this->ins == nullptr){
        assert("instruction not exist");
    }

    Instruction *copyInst = this->ins->copy();
    BasicBlock *fartherBB = this->ins->getParent();
    SSAGraphNode *node = new SSAGraphNode(copyInst, this->type);

    copyInst->replaceDef(result);
    fartherBB->insertBefore(copyInst, this->ins->getNext());
    node->setHeader(this->header);
    node->setlow(this->low);
    node->setnum(this->num);
    node->setvist();

    for (auto child : this->children){
        node->addChild(child);
    }

    return node;
}

void SSAGraphNode::removeChild(SSAGraphNode *node)
{
    for (std::vector<SSAGraphNode *>::iterator iter = children.begin(); iter != children.end(); iter++)
    {
        if (*iter == node){
            children.erase(iter);
            iter--;
        }
    }
}