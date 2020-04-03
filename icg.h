#pragma once

#include "header.h"
#include "ast.h"

#define TACSIZE 100

// TODO: verify if operand is same as union data
typedef union {
    Symbol *ptr;
    int num_const;
    char str_const[20];

    int tsym;
} operanddata;

typedef enum OperandType
{
    OP_PTR,
    OP_NUM_CONST,
    OP_STR_CONST,
    OP_TSYM,
    OP_NONE
} OperandType;

typedef struct operand
{
    OperandType type;
    operanddata data;
} operand;

typedef enum Operation
{
    O_ADD,
    O_SUB,
    O_MUL,
    O_DIV,

    O_ASSIGN
} Operation;

typedef struct quad
{
    char op[10];
    Operation o_op;

    operand op1;
    operand op2;

    operand result;
} quad;

extern quad tacTable[TACSIZE]; // check what to do about this 100 for larger programs

void tac_postorder(Node *n);
void tac_disptable();