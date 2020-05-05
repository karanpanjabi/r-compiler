#pragma once
#include "gen_util.h"
#include "icg.h"

typedef enum TargetOpcode
{
    // TargetOpcode_...
    TO_LD,
    TO_ST,
    TO_MOV,

    TO_ADD,
    TO_SUB,
    TO_MUL,
    TO_DIV,

    TO_BR,

    TO_BLZ,
    TO_BGZ,
    TO_BEZ,
    TO_BLEZ,
    TO_BGEZ,
    TO_BNE,

    TO_GOTO,

    TO_LT,
    TO_LE,
    TO_GT,
    TO_GE,
    TO_EQ,
    TO_NE,
    TO_AND,
    TO_OR,

    TO_LABEL_
} TargetOpcode;


typedef struct Register
{
    int num;    // regnum

    // some fixed width value?

    operand points_to; // this can be used to check if this register points to a specific variable
} Register;

#define REGSIZE 8

extern Register reg_arr[REGSIZE];

// get a free register, if none use existing one
// get_free_register();

// checks if var in register already
// is_var_in_register();

// put_var_in_register(which var) -> register

// convert_to_machine(icg)

typedef struct target_instr
{
    TargetOpcode opcode;
    operand op1;
    operand op2;

    operand result;
} target_instr;

void generate_machine();
void print_machine();