%define parse.error verbose

%{
    #include <stdio.h>
%}


%token NUM_CONST STR_CONST SYMBOL
%token LT LE EQ NE GE GT AND2 OR2
%token LEFT_ASSIGN EQ_ASSIGN
%token IF ELSE FOR WHILE
%token IN

%token PRINT_ PRINT__
%token NEWLINE


%left		WHILE FOR
%right		IF
%left		ELSE
%right		LEFT_ASSIGN
%right		EQ_ASSIGN
%left		OR2
%left		AND2
%nonassoc   	GT GE LT LE EQ NE
%left		'+' '-'
%left		'*' '/'
%left		':'
%nonassoc	'('

%%

start: exprlist
    ;

exprlist:
	|	expr_or_assign			
	|	exprlist ';' expr_or_assign	
	|	exprlist ';'			
	|	exprlist NEWLINE expr_or_assign	
	|	exprlist NEWLINE
    |   exprlist NEWLINE print_statement
    |   exprlist ';' print_statement
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

print_statement: PRINT_ expr PRINT__
    ;


cond:	'(' expr ')'
    ;


ifcond:	'(' expr ')'
    ;


forcond:	'(' SYMBOL IN expr ')'
    ;



expr:   SYMBOL
    |   NUM_CONST
    |   STR_CONST

    |	'{' exprlist '}'
	|	'(' expr_or_assign ')'

    |	expr ':' expr
	|	expr '+' expr
	|	expr '-' expr
	|	expr '*' expr
	|	expr '/' expr

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

int main()
{
    yyparse();
}