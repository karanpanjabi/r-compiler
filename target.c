#include <stdio.h>
#include "target.h"

/*
for every quad in tactable

*/

target_instr instrs[100];
int instr_idx = 0;
Register reg_arr[REGSIZE];

static int robinidx = 0;

TargetOpcode get_target_opcode(Operation i_op)
{
    switch (i_op)
    {
    case O_ADD:
        return TO_ADD;
    case O_SUB:
        return TO_SUB;
    case O_MUL:
        return TO_MUL;
    case O_DIV:
        return TO_DIV;
    default:
        break;
    }
}

Register *is_var_in_reg(operand *i_op)
{
    for (int i = 0; i < REGSIZE; i++)
    {
        operand *ptr = &reg_arr[i].points_to;
        if (ptr->type == i_op->type)
        {
            if ((ptr->type == OP_TSYM && ptr->data.tsym == i_op->data.tsym) || (ptr->type == OP_PTR && ptr->data.ptr == i_op->data.ptr))
                return &reg_arr[i];
        }
    }
    return NULL;
}

void addTargetInstr(target_instr *inst)
{
    instrs[instr_idx++] = *inst;
}

void addLDInst(Register *dst, operand *src)
{
    // add LD instr
    // LD Dst(Reg), Src(memloc)
    target_instr instr;
    instr.opcode = TO_LD;
    instr.result.type = OP_REGISTER;
    instr.result.data.reg = dst;
    instr.result.data.reg->points_to = *src;

    instr.op1 = *src;
    instr.op2.type = OP_NONE;

    addTargetInstr(&instr);
}

void addSTInst(operand *dst, Register *src)
{
    // add ST instr
    // ST Dst, Src
    target_instr instr;
    instr.opcode = TO_ST;
    instr.result = *dst;

    instr.op1.type = OP_REGISTER;
    instr.op1.data.reg = src;
    instr.op2.type = OP_NONE;

    addTargetInstr(&instr);
}

Register *add_var_to_reg(operand *i_op)
{
    // try to get a free reg
    for (int i = 0; i < REGSIZE; i++)
    {
        if (reg_arr[i].points_to.type == OP_NONE)
        {
            reg_arr[i].points_to = *i_op;
            addLDInst(&reg_arr[i], i_op);
            return &reg_arr[i];
        }
    }

    // if none go in round robin order

    // TODO: check whether to store back old value using ST

    reg_arr[robinidx].points_to = *i_op;
    Register *ptr = &reg_arr[robinidx];
    addLDInst(ptr, i_op);
    robinidx = (robinidx + 1) % REGSIZE;

    return ptr;
}

Register *get_free_reg()
{
    for (int i = 0; i < REGSIZE; i++)
    {
        if (reg_arr[i].points_to.type == OP_NONE)
        {
            return &reg_arr[i];
        }
    }

    Register *ptr = &reg_arr[robinidx];
    robinidx = (robinidx + 1) % REGSIZE;

    ptr->points_to.type = OP_NONE;

    return ptr;
}

Register *get_reg_ptr(operand *i_op)
{
    // this function should add LD instr if it's not there
    Register *ptr = is_var_in_reg(i_op);
    if (ptr != NULL)
        return ptr;

    add_var_to_reg(i_op);
}

void set_src_operand(operand *i_op, operand *t_op)
{
    switch (i_op->type)
    {
    case OP_NUM_CONST:
        *t_op = *i_op;
        break;

    case OP_TSYM:
    case OP_PTR:
        t_op->type = OP_REGISTER;
        t_op->data.reg = get_reg_ptr(i_op);
        break;
    }
}

// bad design
Register *set_dst_operand(operand *i_op, operand *t_op)
{
    // get the register corresponding to target if any
    // if none get a free register, point it to this operand
    t_op->type = OP_REGISTER;

    Register *ptr = is_var_in_reg(i_op);

    if (ptr == NULL)
    {
        ptr = get_free_reg();
        ptr->points_to = *i_op;
    }

    t_op->data.reg = ptr;

    return ptr;
}

void convert_to_machine(quad *i_instr)
{
    target_instr t_instr;
    switch (i_instr->o_op)
    {
    case O_ADD:
    case O_SUB:
    case O_MUL:
    case O_DIV:
    {

        t_instr.opcode = get_target_opcode(i_instr->o_op);
        set_src_operand(&i_instr->op1, &t_instr.op1);
        set_src_operand(&i_instr->op2, &t_instr.op2);

        Register *dstreg = set_dst_operand(&i_instr->result, &t_instr.result);

        addTargetInstr(&t_instr);

        // add ST instr for target
        addSTInst(&i_instr->result, dstreg);
    }
    break;

    // case O_LT:
    case O_LE:
    {

        t_instr.opcode = TO_LE;
        set_src_operand(&i_instr->op1, &t_instr.op1);
        set_src_operand(&i_instr->op2, &t_instr.op2);

        Register *dstreg = set_dst_operand(&i_instr->result, &t_instr.result);

        addTargetInstr(&t_instr);

        addSTInst(&i_instr->result, dstreg);
    }
    break;

        // case O_EQ:
        // case O_NE:
        // case O_GE:
        // case O_GT:
        // case O_AND2:
        // case O_OR2:

    case O_ASSIGN:
        // a = b
        // LD reg, b # may not be required if b in reg
        // ST a, reg
        {

            if (i_instr->op1.type == OP_NUM_CONST)
            {
                t_instr.opcode = TO_ST;
                t_instr.result = i_instr->result;
                t_instr.op1 = i_instr->op1;
                t_instr.op2 = i_instr->op2;

                addTargetInstr(&t_instr);
            }

            else
            {
                Register *reg = is_var_in_reg(&i_instr->op1);

                if (reg == NULL)
                {
                    get_free_reg();
                    addLDInst(reg, &i_instr->op1);
                }

                addSTInst(&i_instr->result, reg);
            }
        }
        break;

        // case O_PRINT:

    case O_LABEL:
    {

        t_instr.opcode = TO_LABEL_;
        t_instr.result = i_instr->result;

        t_instr.op1.type = OP_NONE;
        t_instr.op2.type = OP_NONE;

        addTargetInstr(&t_instr);
    }
    break;

    case O_IFFALSEGOTO:
    {

        t_instr.opcode = TO_BEZ;
        set_src_operand(&i_instr->op1, &t_instr.op1);
        t_instr.op2.type = OP_NONE;

        t_instr.result = i_instr->result;

        addTargetInstr(&t_instr);
    }
    break;

    case O_GOTO:
    {

        t_instr.opcode = TO_BR;
        t_instr.result = i_instr->result;

        t_instr.op1.type = OP_NONE;
        t_instr.op2.type = OP_NONE;

        addTargetInstr(&t_instr);
    }
    break;
    }
}

void init_registers()
{
    for (int i = 0; i < REGSIZE; i++)
    {
        reg_arr[i].num = i;
        reg_arr[i].points_to.type = OP_NONE;
    }
}

void generate_machine()
{
    init_registers();
    for (int i = 0; i < tacNum; i++)
    {
        convert_to_machine(&tacTable[i]);
    }
}

void display_target_opcode(TargetOpcode opcode)
{
    switch (opcode)
    {
    case TO_ADD:
        printf("ADD");
        break;
    case TO_SUB:
        printf("SUB");
        break;
    case TO_MUL:
        printf("MUL");
        break;
    case TO_DIV:
        printf("DIV");
        break;
    case TO_LD:
        printf("LD");
        break;
    case TO_ST:
        printf("ST");
        break;

    case TO_BR:
        printf("BR");
        break;
    case TO_BEZ:
        printf("BEZ");
        break;

    case TO_LT:
        printf("LT");
        break;
    case TO_LE:
        printf("LE");
        break;
        // case O_EQ:
        //     printf("EQ");
        //     break;
        // case O_NE:
        //     printf("NE");
        //     break;
        // case O_GE:
        //     printf("GE");
        //     break;
        // case O_GT:
        //     printf("GT");
        //     break;
        // case O_AND2:
        //     printf("AND2");
        //     break;
        // case O_OR2:
        //     printf("OR2");
        //     break;

        // case O_LABEL:
        //     printf("LABEL");
        //     break;
        // case O_IFFALSEGOTO:
        //     printf("IFFALSEGOTO");
        //     break;
        // case O_GOTO:
        //     printf("GOTO");
        //     break;

        // case O_PRINT:
        //     printf("PRINT");
        //     break;
        // default:
        //     break;
    }
}

void display_target_operand(operand *op)
{
    switch (op->type)
    {
    case OP_TSYM:
        printf("t%d", op->data.tsym);
        break;
    case OP_PTR:
        printf("%s", op->data.ptr->sym);
        break;
    case OP_REGISTER:
        printf("R%d", op->data.reg->num);
        break;
    case OP_NUM_CONST:
        printf("%d", op->data.num_const);
        break;
    case OP_STR_CONST:
        printf("%s", op->data.str_const);
        break;
    case OP_LABEL:
        printf("L%d: ", op->data.label);
        break;
    default:
        break;
    }
}

void display_instr(target_instr *ins)
{
    display_target_opcode(ins->opcode);
    printf(" ");
    display_target_operand(&ins->result);

    if (ins->op1.type != OP_NONE)
    {
        printf(", ");
        display_target_operand(&ins->op1);
    }

    if (ins->op2.type != OP_NONE)
    {
        printf(", ");
        display_target_operand(&ins->op2);
    }

    printf("\n");
}

void print_machine()
{
    for (int i = 0; i < instr_idx; i++)
    {
        display_instr(&instrs[i]);
    }
}