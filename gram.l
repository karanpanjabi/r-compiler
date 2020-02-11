%{
    #include "y.tab.h"

	enum type {NUMBER, STRING};

	typedef struct Symbol
	{
		char sym[20];
		enum type type;
		void *value_ptr;
		int lineno;
	} Symbol;

	Symbol 
%}

%%

#.*	;

[ \t]	;

\n	{
		printf("NEWLINE\n");
		return NEWLINE;
	}

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
			printf("<-\n");
  			return LEFT_ASSIGN;
		}
        
"="		{
			printf("=\n");
  			return EQ_ASSIGN;
		}

"print("	{
				printf("print(\n");
				return PRINT_;
			}

")"			{
				printf(")\n");
				return PRINT__;
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
							printf("SYMBOL: %s\n", yytext);
  							return SYMBOL;
						}

.	{	return yytext[0];	}

%%

int yywrap() { return 1; }


int main()
{
	while(!feof(stdin))
	{
		yylex();
	}
}
