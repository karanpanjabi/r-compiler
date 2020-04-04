#include <stdio.h>
#include <string.h>
#include "icg.h"
#include "ast.h"

quad tacTable[TACSIZE];
int tacNum = 0;
int tempCount = 0;
int labelCount = 0;

int is_binop(Node *n)
{
    switch (n->n_type)
    {
    case N_BADD:
    case N_BSUB:
    case N_BMUL:
    case N_BDIV:
    case N_LT:
    case N_LE:
    case N_EQ:
    case N_NE:
    case N_GE:
    case N_GT:
    case N_AND2:
    case N_OR2:
        return 1;
    }

    return 0;
}

int is_op(Node *n)
{
    return is_binop(n);
}

void addQuad(quad q)
{
    if (tacNum == TACSIZE)
    {
        printf("Reached tac limit\n");
        return;
    }

    tacTable[tacNum++] = q;
}

Operation getoperation(Node *n)
{
    switch (n->n_type)
    {
    case N_BADD:
        return O_ADD;
    case N_BSUB:
        return O_SUB;
    case N_BMUL:
        return O_MUL;
    case N_BDIV:
        return O_DIV;

    case N_LT:
        return O_LT;
    case N_LE:
        return O_LE;
    case N_EQ:
        return O_EQ;
    case N_NE:
        return O_NE;
    case N_GE:
        return O_GE;
    case N_GT:
        return O_GT;
    case N_AND2:
        return O_AND2;
    case N_OR2:
        return O_OR2;
    }
}

void addQuadBinOp(Node *n)
{
    quad iquad;
    // operation
    sprintf(iquad.op, "%s", n->type); // Replace op symbol with name
    iquad.o_op = getoperation(n);

    // result operand
    operand result;
    n->_tempNum = tempCount++; // assign temp var for this, TODO: add temp var to symbol table
    result.type = OP_TSYM;
    result.data.tsym = n->_tempNum;

    // operand 1 and 2
    operand op1, op2;
    Node *op1node = n->ptrlist[0];
    Node *op2node = n->ptrlist[1];

    if (op1node->n_type == N_NUM_CONST)
    {
        op1.type = OP_NUM_CONST;
        op1.data.num_const = op1node->value.num_const;
    }
    else if (op1node->n_type == N_SYMBOL)
    {
        op1.type = OP_PTR;
        op1.data.ptr = op1node->value.ptr;
    }
    else if (is_op(op1node))
    {
        op1.type = OP_TSYM;
        op1.data.tsym = op1node->_tempNum;
    }
    // TODO: check if can be something else

    if (op2node->n_type == N_NUM_CONST)
    {
        op2.type = OP_NUM_CONST;
        op2.data.num_const = op2node->value.num_const;
    }
    else if (op2node->n_type == N_SYMBOL)
    {
        op2.type = OP_PTR;
        op2.data.ptr = op2node->value.ptr;
    }
    else if (is_op(op2node))
    {
        op2.type = OP_TSYM;
        op2.data.tsym = op2node->_tempNum;
    }

    iquad.result = result;
    iquad.op1 = op1;
    iquad.op2 = op2;
    addQuad(iquad);
}

void addQuadAssign(Node *n)
{
    quad iquad;

    iquad.o_op = O_ASSIGN;

    Node *left = n->ptrlist[0];
    Node *right = n->ptrlist[1];

    operand result;
    result.type = OP_PTR;
    result.data.ptr = left->value.ptr;

    operand op1;
    operand op2;

    op1.type = OP_NONE;

    if (right->n_type == N_SYMBOL)
    {
        op2.type = OP_PTR;
        op2.data.ptr = right->value.ptr;
    }
    else if (right->n_type == N_NUM_CONST)
    {
        op2.type = OP_NUM_CONST;
        op2.data.num_const = right->value.num_const;
    }
    else if (right->n_type == N_STR_CONST)
    {
        op2.type = OP_STR_CONST;
        strcpy(op2.data.str_const, right->value.str_const);
    }
    else
    {
        op2.type = OP_TSYM;
        op2.data.tsym = right->_tempNum;
    }

    iquad.result = result;
    iquad.op1 = op1;
    iquad.op2 = op2;
    addQuad(iquad);
}

void addQuadLabel(Node *n)
{
    quad iquad;
    iquad.o_op = O_LABEL;

    operand result;
    result.type = OP_LABEL;
    result.data.label = labelCount++;
    n->_labelNum = result.data.label;

    iquad.result = result;
    iquad.op1.type = OP_NONE;
    iquad.op2.type = OP_NONE;

    addQuad(iquad);
}

void addQuadIfFalseGoto(Node *n)
{
    quad iquad;
    iquad.o_op = O_IFFALSEGOTO;

    operand result;
    result.type = OP_LABEL;
    result.data.label = -1; // to be filled
    n->_gotoquadidx = tacNum;

    iquad.result = result;
    iquad.op1.type = OP_NONE; // to be filled
    iquad.op2.type = OP_NONE;
    addQuad(iquad);
}

void addQuadGoto(Node *n)
{
    quad iquad;

    iquad.o_op = O_GOTO;

    operand result;
    result.type = OP_LABEL;
    result.data.label = -1; // to be filled
    n->_gotoquadidx = tacNum;

    iquad.result = result;
    iquad.op1.type = OP_NONE;
    iquad.op2.type = OP_NONE;
    addQuad(iquad);
}

void addQuadIfElse(Node *n)
{
    Node *cond, *iffalsegoto, *ifbody, *ifbodygoto, *preelselabel, *elsebody, *postelselabel;

    cond = n->ptrlist[0];
    iffalsegoto = n->ptrlist[1]; // modifying op1, result label
    ifbody = n->ptrlist[2];
    ifbodygoto = n->ptrlist[3]; // modifying result label
    preelselabel = n->ptrlist[4];
    elsebody = n->ptrlist[5];
    postelselabel = n->ptrlist[6];

    operand temp;

    // if false goto quad
    quad *quad_iffalsegoto = &tacTable[iffalsegoto->_gotoquadidx];
    if (cond->n_type == N_SYMBOL)
    {
        temp.type = OP_PTR;
        temp.data.ptr = cond->value.ptr;
    }
    else if (cond->n_type == N_NUM_CONST)
    {
        temp.type = OP_NUM_CONST;
        temp.data.num_const = cond->value.num_const;
    }
    else if (cond->n_type == N_STR_CONST)
    {
        temp.type = OP_STR_CONST;
        strcpy(temp.data.str_const, cond->value.str_const);
    }
    else
    {
        temp.type = OP_TSYM;
        temp.data.tsym = cond->_tempNum;
    }

    quad_iffalsegoto->op1 = temp;
    quad_iffalsegoto->result.data.label = preelselabel->_labelNum;

    // if body end quad
    quad *quad_ifbodygoto = &tacTable[ifbodygoto->_gotoquadidx];
    quad_ifbodygoto->result.data.label = postelselabel->_labelNum;
}

void tac_main(Node *n)
{
    if (is_binop(n))
    {
        addQuadBinOp(n);
    }

    else if (n->n_type == N_ASSIGN || n->n_type == N_LEFT_ASSIGN)
    {
        addQuadAssign(n);
    }

    else if (n->n_type == N_LABEL)
    {
        addQuadLabel(n);
    }

    else if (n->n_type == N_IFFALSEGOTO)
    {
        addQuadIfFalseGoto(n);
    }

    else if (n->n_type == N_GOTO)
    {
        addQuadGoto(n);
    }

    else if (n->n_type == N_IF)
    {
        addQuadIfElse(n);
    }
}

void tac_postorder(Node *n)
{
    if (n != NULL)
    {
        for (int i = 0; i < n->_numnodes; i++)
        {
            tac_postorder(n->ptrlist[i]);
        }
        tac_main(n);
    }
}

void display_operand(operand op)
{
    switch (op.type)
    {
    case OP_TSYM:
        printf("t%d", op.data.tsym);
        break;
    case OP_PTR:
        printf("%s", op.data.ptr->sym);
        break;
    case OP_NUM_CONST:
        printf("%d", op.data.num_const);
        break;
    case OP_STR_CONST:
        printf("%s", op.data.str_const);
        break;
    case OP_LABEL:
        printf("L%d", op.data.label);
        break;
    default:
        break;
    }
}

void display_operation(Operation operation)
{
    switch (operation)
    {
    case O_ADD:
        printf("ADD");
        break;
    case O_SUB:
        printf("SUB");
        break;
    case O_MUL:
        printf("MUL");
        break;
    case O_DIV:
        printf("DIV");
        break;

    case O_LT:
        printf("LT");
        break;
    case O_LE:
        printf("LE");
        break;
    case O_EQ:
        printf("EQ");
        break;
    case O_NE:
        printf("NE");
        break;
    case O_GE:
        printf("GE");
        break;
    case O_GT:
        printf("GT");
        break;
    case O_AND2:
        printf("AND2");
        break;
    case O_OR2:
        printf("OR2");
        break;

    case O_LABEL:
        printf("LABEL");
        break;
    case O_IFFALSEGOTO:
        printf("IFFALSEGOTO");
        break;
    case O_GOTO:
        printf("GOTO");
        break;
    default:
        break;
    }
}

void tac_disptable()
{
    for (int i = 0; i < tacNum; i++)
    {
        display_operand(tacTable[i].result);
        printf(" = ");
        display_operand(tacTable[i].op1);
        printf(" ");
        display_operation(tacTable[i].o_op);
        printf(" ");
        display_operand(tacTable[i].op2);
        printf("\n");
    }
}