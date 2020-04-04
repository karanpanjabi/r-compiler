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
    int label;
} operanddata;

typedef enum OperandType
{
    OP_NONE,
    OP_PTR,
    OP_NUM_CONST,
    OP_STR_CONST,
    OP_TSYM,
    OP_LABEL
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

    O_LT,
    O_LE,
    O_EQ,
    O_NE,
    O_GE,
    O_GT,
    O_AND2,
    O_OR2,

    O_ASSIGN,

    O_LABEL,
    O_IFFALSEGOTO,
    O_GOTO
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