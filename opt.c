#include <stdio.h>

#include "icg.h"

void const_prop_aux(int i)
{
    quad *quadi = &tacTable[i];
    if (quadi->o_op == O_ASSIGN && (quadi->op1.type == OP_NUM_CONST || quadi->op1.type == OP_STR_CONST))
    {
        for (int j = i + 1; j < tacNum; j++)
        {
            quad *quadj = &tacTable[j];
            if (quadj->o_op == O_LABEL ||  (quadj->o_op == O_ASSIGN && quadj->result.data.ptr == quadi->result.data.ptr))
            {
                break;
            }

            // replace reference with value if it's present
            // this changes the quad
            if ((quadj->op1.type == OP_PTR && quadj->op1.data.ptr == quadi->result.data.ptr) ||
                (quadj->op1.type == OP_TSYM && quadj->op1.data.tsym == quadi->result.data.tsym))
                quadj->op1 = quadi->op1;

            if ((quadj->op2.type == OP_PTR && quadj->op2.data.ptr == quadi->result.data.ptr) ||
                (quadj->op1.type == OP_TSYM && quadj->op1.data.tsym == quadi->result.data.tsym))
                quadj->op2 = quadi->op1;
        }
    }
}


void const_prop()
{
    // start from assignment
    // replace reference with value
    // end before reassignment if any

    for (int i = 0; i < tacNum - 1; i++)
    {
        const_prop_aux(i);
    }
}

void const_folding()
{
    // check if both operands are constants
    // do the operation and store it as op1, op2 should be none

    for (int i = 0; i < tacNum; i++)
    {
        quad *quadi = &tacTable[i];

        if (quadi->op1.type == OP_NUM_CONST && quadi->op2.type == OP_NUM_CONST)
        {
            // assuming integer result for now, TODO: handle floats
            int res = -1;
            int flag = 0;
            int num1 = quadi->op1.data.num_const;
            int num2 = quadi->op2.data.num_const;
            switch (quadi->o_op)
            {
            case O_ADD:
                res = num1 + num2;
                break;
            case O_SUB:
                res = num1 - num2;
                break;
            case O_MUL:
                res = num1 * num2;
                break;
            case O_DIV:
                res = num1 / num2;
                break;
            
            case O_LT:
                res = num1 < num2;
                break;
            case O_LE:
                res = num1 <= num2;
                break;
            case O_EQ:
                res = num1 == num2;
                break;
            case O_NE:
                res = num1 != num2;
                break;
            case O_GE:
                res = num1 >= num2;
                break;
            case O_GT:
                res = num1 > num2;
                break;
            case O_AND2:
                res = num1 && num2;
                break;
            case O_OR2:
                res = num1 || num2;
                break;

            default:
                flag = 1;
                break;
            }

            if(flag == 1)
            { continue; }
            quadi->op1.data.num_const = res;
            quadi->op2.type = OP_NONE;
            quadi->o_op = O_ASSIGN; // can be used further to check const_prop

            const_prop_aux(i);
        }
    }
}