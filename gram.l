%{
    #include "y.tab.h"
%}

%%


"if"	{
			return IF;
		}

"else"	{
			return ELSE;
		}

"for"	{
			return FOR;
		}

"while"	{
			return WHILE;
		}

"in"	{
			return IN;
		}

"<"		{
  			return LT;
		}
        
"<="	{
  			return LE;
		}
        
"=="	{
  			return EQ;
		}
        
"!="	{
  			return NE;
		}

">"		{
  			return GT;
		}
        
">="	{
  			return GE;
		}
        
"&&"	{
  			return AND2;
		}
        
"||"	{
  			return OR2;
		}
        
"<-"	{
  			return LEFT_ASSIGN;
		}
        
"="		{
  			return EQ_ASSIGN;
		}
        
[0-9]+	{
			printf("NUM_CONST: %s\n", yytext);
  			return NUM_CONST;
		}

\".*\"	{
			printf("STR_CONST: %s\n", yytext);
  			return STR_CONST;
		}

[a-zA-Z_][a-zA-Z0-9_]*	{
							printf("SYMBOL: %s\n", yytext)
  							return SYMBOL;
						}


%%

int main()
{
	yylex();
}