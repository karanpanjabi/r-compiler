#pragma once

typedef struct Symbol Symbol;
typedef struct Register Register;

// TODO: verify if operand is same as union data
typedef union {
    Symbol *ptr;
    int num_const;
    char str_const[20];

    int tsym;
    int label;
    Register *reg;
} operanddata;

typedef enum OperandType
{
    OP_NONE,
    OP_PTR,
    OP_NUM_CONST,
    OP_STR_CONST,
    OP_TSYM,
    OP_LABEL,
    OP_REGISTER
} OperandType;

typedef struct operand
{
    OperandType type;
    operanddata data;
} operand;