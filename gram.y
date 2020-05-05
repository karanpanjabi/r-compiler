%define parse.error verbose

%{
    #include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include "header.h"
	#include "ast.h"
	#include "icg.h"
	#include "opt.h"
	#include "target.h"

	int valid = 1;

	extern char * yytext;
	Node *root = NULL;
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
						root = $$.nodeptr;
					}
    ;

exprlist:
	|	expr_or_assign							{	$$ = $1;	}
	|	exprlist SEMICOLON expr_or_assign		{
													$$.nodeptr = make_node("SEQ", N_SEQ, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
												}
	|	exprlist SEMICOLON						{	$$ = $1;	}
	|	exprlist NEWLINE expr_or_assign			{	
													$$.nodeptr = make_node("SEQ", N_SEQ, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
												}
	|	exprlist NEWLINE						{	$$ = $1;	}
    |   exprlist NEWLINE print_statement		{
													$$.nodeptr = make_node("SEQ", N_SEQ, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
												}
    |   exprlist SEMICOLON print_statement		{
													$$.nodeptr = make_node("SEQ", N_SEQ, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
												}
	|	print_statement							{	$$ = $1;	}
    ;

expr_or_assign:   expr	{	$$ = $1;	}
	|   equal_assign	{	$$ = $1;	}
    |   statement		{	$$ = $1;	}
    ;

statement:
    	IF ifcond expr_or_assign						{
															Node *elselabel = make_node("ELSELABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															Node *postelselabel = make_node("POSTELSELABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															Node *iffalsegoto = make_node("IFFALSEGOTO", N_IFFALSEGOTO, (data) 0, (NodePtrList) {NULL}, 0);
															Node *ifbodygoto = make_node("IFBODYGOTO", N_GOTO, (data) 0, (NodePtrList) {NULL}, 0);
															$$.nodeptr = make_node("IF", N_IF, (data) 0, (NodePtrList) {$2.nodeptr, iffalsegoto, $3.nodeptr, ifbodygoto, elselabel, NULL, postelselabel}, 7);
														}
	|	IF ifcond expr_or_assign ELSE expr_or_assign	{	
															Node *elselabel = make_node("ELSELABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															Node *postelselabel = make_node("POSTELSELABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															Node *iffalsegoto = make_node("IFFALSEGOTO", N_IFFALSEGOTO, (data) 0, (NodePtrList) {NULL}, 0);
															Node *ifbodygoto = make_node("IFBODYGOTO", N_GOTO, (data) 0, (NodePtrList) {NULL}, 0);
															$$.nodeptr = make_node("IF", N_IF, (data) 0, (NodePtrList) {$2.nodeptr, iffalsegoto, $3.nodeptr, ifbodygoto, elselabel, $5.nodeptr, postelselabel}, 7);
														}
	|	FOR forcond expr_or_assign						{
															Node *iffalsegoto = make_node("IFFALSEGOTO", N_IFFALSEGOTO, (data) 0, (NodePtrList) {NULL}, 0);
															$$.nodeptr = make_node("FOR", N_FOR, (data) 0, (NodePtrList) {$2.nodeptr, iffalsegoto, $3.nodeptr}, 3);
														}
	|	WHILE cond expr_or_assign						{
															Node *iffalsegoto = make_node("IFFALSEGOTO", N_IFFALSEGOTO, (data) 0, (NodePtrList) {NULL}, 0);
															Node *bodylabel = make_node("WHILEBODYLABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															Node *bodyendgoto = make_node("WHILEBODYGOTO", N_GOTO, (data) 0, (NodePtrList) {NULL}, 0);
															Node *exitlabel = make_node("WHILEEXITLABEL", N_LABEL, (data) 0, (NodePtrList) {NULL}, 0);
															$$.nodeptr = make_node("WHILE", N_WHILE, (data) 0, (NodePtrList) {bodylabel, $2.nodeptr, iffalsegoto, $3.nodeptr, bodyendgoto, exitlabel}, 6);
														}

    ;

equal_assign:
	SYMBOL EQ_ASSIGN expr_or_assign	{ 
										// modify only if $3.nodeptr->n_type is N_NUM_CONST or N_STR_CONST
										
										if($3.nodeptr->n_type == N_NUM_CONST || $3.nodeptr->n_type == N_STR_CONST)
										{
											modifyID($1.value, $3.type, $3.value);
										}	

										$1.nodeptr = make_node("SYMBOL", N_SYMBOL, (data) getSymbol($1.value), (NodePtrList) {NULL}, 0);
										$$.nodeptr = make_node("=", N_ASSIGN, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
									}
    ;

print_statement: 
	PRINT_ expr RIGHT_PAREN			{
										$$.nodeptr = make_node("PRINT", N_PRINT, (data) 0, (NodePtrList) {$2.nodeptr}, 1);
									}
    ;


cond:	LEFT_PAREN expr RIGHT_PAREN				{	$$ = $2;	}
	|	LEFT_PAREN expr RIGHT_PAREN NEWLINE		{	$$ = $2;	}
    ;


ifcond:	LEFT_PAREN expr RIGHT_PAREN				{	$$ = $2;	}
	|	LEFT_PAREN expr RIGHT_PAREN NEWLINE		{	$$ = $2;	}
    ;


forcond:	LEFT_PAREN SYMBOL IN range RIGHT_PAREN		{	
															$2.nodeptr = make_node("SYMBOL", N_SYMBOL, (data) getSymbol($2.value), (NodePtrList) {NULL}, 0);	
															$$.nodeptr = make_node("FORCOND", N_FORCOND, (data) 0, (NodePtrList) {$2.nodeptr, $4.nodeptr}, 2);
														}
	|	LEFT_PAREN SYMBOL IN range RIGHT_PAREN NEWLINE	{
															$2.nodeptr = make_node("SYMBOL", N_SYMBOL, (data) getSymbol($2.value), (NodePtrList) {NULL}, 0);	
															$$.nodeptr = make_node("FORCOND", N_FORCOND, (data) 0, (NodePtrList) {$2.nodeptr, $4.nodeptr}, 2);
														}
    ;

range: expr COLON expr		{
								$$.nodeptr = make_node(":", N_RANGE, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}

expr:   SYMBOL		{
						$1.nodeptr = make_node("SYMBOL", N_SYMBOL, (data) getSymbol($1.value), (NodePtrList) {NULL}, 0);
						$$ = $1;
					}
    |   NUM_CONST	{ 
						
						$1.nodeptr = make_node("NUM_CONST", N_NUM_CONST, (data) atoi($1.value), (NodePtrList) {NULL}, 0); // check what to do for double type
						$$ = $1;
					}
    |   STR_CONST	{ 
						
						data temp_;
    					strcpy(temp_.str_const, $1.value);
						$1.nodeptr = make_node("STR_CONST", N_STR_CONST, temp_, (NodePtrList) {NULL}, 0);
						$$ = $1; 
					}

    |	LEFT_CURLY exprlist RIGHT_CURLY				{	$$ = $2;	}
	|	LEFT_PAREN expr_or_assign RIGHT_PAREN		{	$$ = $2;	}

    
	|	expr PLUS expr		{
								$$.nodeptr = make_node("+", N_BADD, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr MINUS expr		{
								$$.nodeptr = make_node("-", N_BSUB, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr STAR expr		{
								$$.nodeptr = make_node("*", N_BMUL, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr FSLASH expr	{
								$$.nodeptr = make_node("/", N_BDIV, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}

    |	expr LT expr		{
								$$.nodeptr = make_node("<", N_LT, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr LE expr		{
								$$.nodeptr = make_node("<=", N_LE, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr EQ expr		{
								$$.nodeptr = make_node("==", N_EQ, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr NE expr		{
								$$.nodeptr = make_node("!=", N_NE, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr GE expr		{
								$$.nodeptr = make_node(">=", N_GE, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr GT expr		{
								$$.nodeptr = make_node(">", N_GT, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr AND2 expr		{
								$$.nodeptr = make_node("&&", N_AND2, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}
	|	expr OR2 expr		{
								$$.nodeptr = make_node("||", N_OR2, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
							}

	|	SYMBOL LEFT_ASSIGN expr		{ 
										if($3.nodeptr->n_type == N_NUM_CONST || $3.nodeptr->n_type == N_STR_CONST)
										{
											modifyID($1.value, $3.type, $3.value);
										}	
									
										$1.nodeptr = make_node("SYMBOL", N_SYMBOL, (data) getSymbol($1.value), (NodePtrList) {NULL}, 0);
										$$.nodeptr = make_node("<-", N_LEFT_ASSIGN, (data) 0, (NodePtrList) {$1.nodeptr, $3.nodeptr}, 2);
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
		// extern char * yytext;
		// printf("Err: %s \n", yytext);
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

	display_subtree(root);

	display_table(table, lastSym+1);

	tac_postorder(root);

	tac_disptable();

	// display_table(table, lastSym+1);

	const_prop();

	// printf("\n\nAfter const_prop\n");

	// tac_disptable();

	const_folding();

	printf("\n\nAfter const prop and folding\n");

	for(int i=0; i<10; i++)
	{
		const_prop();
		const_folding();
	}

	tac_disptable();

	printf("\n\n");

	generate_machine();
	print_machine();
}