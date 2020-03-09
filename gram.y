%define parse.error verbose

%{
    #include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "header.h"
	#include "ast.h"

	int valid = 1;

	extern char * yytext;
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

start: exprlist		{
						$$ = $1;
						printf("%p\n", $1.nodeptr);
						display_subtree($$.nodeptr);
						printf("here\n");
					}
    ;

exprlist:
	|	expr_or_assign							{	$$ = $1;	}
	|	exprlist SEMICOLON expr_or_assign	
	|	exprlist SEMICOLON						{	$$ = $1;	}
	|	exprlist NEWLINE expr_or_assign	
	|	exprlist NEWLINE						{	$$ = $1;	}
    |   exprlist NEWLINE print_statement
    |   exprlist SEMICOLON print_statement
	|	print_statement
    ;

expr_or_assign:   expr	{ $$ = $1; printf("%p\n", $$.nodeptr); display_subtree($$.nodeptr); }
	|   equal_assign
    |   statement
    ;

statement:
    	IF ifcond expr_or_assign
	|	IF ifcond expr_or_assign ELSE expr_or_assign
	|	FOR forcond expr_or_assign
	|	WHILE cond expr_or_assign

    ;

equal_assign:
	SYMBOL EQ_ASSIGN expr_or_assign	{ 
										printf("eq_assign: %s %s, SYMBOL: %s\n", $3.type, $3.value, $1.value); 
										modifyID($1.value, $3.type, $3.value); 
									}
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



expr:   SYMBOL		{
						$1.nodeptr = make_node("SYMBOL", (data) getSymbol(yytext), (NodePtrList) {NULL});
						$$ = $1;
					}
    |   NUM_CONST	{ 
						printf("num_cost: %s %s\n", $1.type, $1.value);
						$1.nodeptr = make_node("NUM_CONST", (data) atoi($1.value), (NodePtrList) {NULL}); // check what to do for double type
						$$ = $1;
						display_subtree($$.nodeptr);
					}
    |   STR_CONST	{ 
						printf("str_const: %s %s\n", $1.type, $1.value);
						data temp_;
    					strcpy(temp_.str_const, $1.value);
						$1.nodeptr = make_node("STR_CONST", temp_, (NodePtrList) {NULL});
						$$ = $1; 
					}

    |	LEFT_CURLY exprlist RIGHT_CURLY
	|	LEFT_PAREN expr_or_assign RIGHT_PAREN

    |	expr COLON expr		{

							}
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

	|	SYMBOL LEFT_ASSIGN expr		{ 
										printf("left_assign: %s %s, SYMBOL: %s\n", $3.type, $3.value, $1.value); 
										modifyID($1.value, $3.type, $3.value); 
									}
    
    ;

%%

#include <ctype.h>
int yyerror(const char *s)
{
    printf("Invalid program\n");
    valid = 0;
	extern int yylineno;
	printf("Line no: %d \n The error is: %s\n",yylineno,s);

	while(1)
	{
		int tok = yylex();
		// printf("Err: %d \n", tok);
		extern char * yytext;
		printf("Err: %s \n", yytext);
		if(tok == NEWLINE || tok == SEMICOLON)
			break;
	}
	yyparse();
    return 1;
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