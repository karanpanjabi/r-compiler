#include <stdio.h>
#include <string.h>
#include "icg.h"
#include "ast.h"

quad tacTable[TACSIZE];
int tacNum = 0;
int tempCount = 0;

int is_binop(Node *n)
{
    switch (n->n_type)
    {
    case N_BADD:
    case N_BSUB:
    case N_BMUL:
    case N_BDIV:
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

void tac_main(Node *n)
{

    // if n is SEQ node

    // if n is a binary op
    if (is_binop(n))
    {
        addQuadBinOp(n);
    }

    else if (n->n_type == N_ASSIGN || n->n_type == N_LEFT_ASSIGN)
    {
        addQuadBinOp(n);
    }
    
    else if(n->n_type == N_IF)
    {
        Node *cond = n->ptrlist[0];
        Node *ifbody = n->ptrlist[1];
        Node *elsebody = n->ptrlist[2];
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