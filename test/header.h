
#pragma once
#define TABLE_SIZE 100

enum type {NUMBER, STRING};

typedef struct Symbol
{
    char sym[20];
    enum type type;
    void *value_ptr;
    int lineno;
} Symbol;

extern Symbol table[TABLE_SIZE];
extern int lastSym;

int exists(char *sym);
void display_table(Symbol *_table, int n);
void installID(char *sym_name, int lineno);