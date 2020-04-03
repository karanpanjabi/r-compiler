
#pragma once

#include "header.h"

typedef union data
{
    Symbol *ptr;
    int num_const;
    char str_const[20];
} data;

typedef enum NodeType
{
    N_SEQ,
    N_IF,
    N_FOR,
    N_WHILE,
    N_SYMBOL,
    N_ASSIGN,
    N_PRINT,
    N_FORCOND,
    N_NUM_CONST,
    N_STR_CONST,
    N_RANGE,
    N_BADD,
    N_BSUB,
    N_BMUL,
    N_BDIV,
    N_LT,
    N_LE,
    N_EQ,
    N_NE,
    N_GE,
    N_GT,
    N_AND2,
    N_OR2,
    N_LEFT_ASSIGN
} NodeType;

typedef struct Node
{
    char type[20];
    NodeType n_type;
    union data value;

    int _numnodes;
    struct Node* ptrlist[10];

    int _tempNum;
} Node;

#define NodePtrList Node * []

Node* make_node(char *type, NodeType n_type, data value, Node* *list, int len);
void display_subtree(Node *n);


    