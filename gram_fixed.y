%define parse.error verbose

%{
    #include <stdio.h>
	#include "header.h"

	extern Symbol table[TABLE_SIZE];
	extern int lastSym;
	int valid = 1;
%}


%token NUM_CONST STR_CONST SYMBOL
%token LT LE EQ NE GE GT AND2 OR2
%token LEFT_ASSIGN EQ_ASSIGN
%token IF ELSE FOR WHILE
%token IN

%token PRINT_
%token NEWLINE

%token PLUS MINUS STAR FSLASH
%token SEMICOLON COLON
%token LEFT_PAREN RIGHT_PAREN
%token LEFT_CURLY RIGHT_CURLY


%left		WHILE FOR
%right		IF
%left		ELSE
%right		LEFT_ASSIGN
%right		EQ_ASSIGN
%left		OR2
%left		AND2
%nonassoc	GT GE LT LE EQ NE
%left		PLUS MINUS
%left		STAR FSLASH
%left		COLON
%nonassoc	LEFT_PAREN

%%

start: exprlist
    ;

exprlist:
	|	expr_or_assign			
	|	exprlist SEMICOLON expr_or_assign	
	|	exprlist SEMICOLON			
	|	exprlist NEWLINE expr_or_assign	
	|	exprlist NEWLINE
    |   exprlist NEWLINE print_statement
    |   exprlist SEMICOLON print_statement
	|	print_statement
    ;

expr_or_assign:   expr
	|   equal_assign
    |   statement
    ;

statement:
    	IF ifcond expr_or_assign
	|	IF ifcond expr_or_assign ELSE expr_or_assign
	|	FOR forcond expr_or_assign
	|	WHILE cond expr_or_assign

    ;

equal_assign:    expr EQ_ASSIGN expr_or_assign
    ;

print_statement: PRINT_ expr RIGHT_PAREN
    ;


cond:	LEFT_PAREN expr RIGHT_PAREN
	|	LEFT_PAREN expr RIGHT_PAREN NEWLINE
    ;


ifcond:	LEFT_PAREN expr RIGHT_PAREN
	|	LEFT_PAREN expr RIGHT_PAREN NEWLINE	
    ;


forcond:	LEFT_PAREN SYMBOL IN expr RIGHT_PAREN
	|	LEFT_PAREN SYMBOL IN expr RIGHT_PAREN NEWLINE
    ;



expr:   SYMBOL
    |   NUM_CONST
    |   STR_CONST

    |	LEFT_CURLY exprlist RIGHT_CURLY
	|	LEFT_PAREN expr_or_assign RIGHT_PAREN

    |	expr COLON expr
	|	expr PLUS expr
	|	expr MINUS expr
	|	expr STAR expr
	|	expr FSLASH expr

    |	expr LT expr			
	|	expr LE expr			
	|	expr EQ expr			
	|	expr NE expr			
	|	expr GE expr			
	|	expr GT expr			
	|	expr AND2 expr			
	|	expr OR2 expr	

	|	expr LEFT_ASSIGN expr
    
    ;

%%

#include <ctype.h>
int yyerror(const char *s)
{
    printf("Invalid program\n");
    valid = 0;
	extern int yylineno;
	printf("Line no: %d \n The error is: %s\n",yylineno,s);
    return 0;
}

int main()
{
    yyparse();

	if(valid)
	{
		printf("Valid program\n");
	}

	display_table(table, lastSym+1);

}