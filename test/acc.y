%{
    #include "header.h"

	extern Symbol table[TABLE_SIZE];
	extern int lastSym;
%}

%%

start : ;

%%

int yyerror()
{
    return 0;
}

int main()
{
    yyparse();

    display_table(table, lastSym+1);
}