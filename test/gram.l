%option yylineno

%{
	#include <string.h>
    #include "tokens.h"

	#include "header.h"

	Symbol table[TABLE_SIZE];
	int lastSym = -1;
%}

%%

#.*	;

[ \t]	;

\n	{
		printf("NEWLINE\n");
		// return NEWLINE;
	}

"if"	{
			printf("IF\n");
			// return IF;
		}

"else"	{
			printf("ELSE\n");
			// return ELSE;
		}

"for"	{
			printf("FOR\n");
			// return FOR;
		}

"while"	{
			printf("WHILE\n");
			// return WHILE;
		}

"in"	{
			printf("IN\n");
			// return IN;
		}

"<"		{
			printf("LT\n");
  			// return LT;
		}
        
"<="	{
			printf("LE\n");
  			// return LE;
		}
        
"=="	{
			printf("EQ\n");
  			// return EQ;
		}
        
"!="	{
			printf("IF\n");
  			// return NE;
		}

">"		{
			printf("GT\n");
  			// return GT;
		}
        
">="	{
			printf("GE\n");
  			// return GE;
		}
        
"&&"	{
			printf("AND2\n");
  			// return AND2;
		}
        
"||"	{
			printf("OR2\n");
  			// return OR2;
		}
        
"<-"	{
			printf("<-\n");
  			// return LEFT_ASSIGN;
		}
        
"="		{
			printf("=\n");
  			// return EQ_ASSIGN;
		}

"print("	{
				printf("print(\n");
				// return PRINT_;
			}

")"			{
				printf(")\n");
				// return PRINT__;
			}
        
[0-9]+(\.[0-9]+)?	{
						printf("NUM_CONST: %s, %d\n", yytext, yylineno);
						// return NUM_CONST;
					}

\".*\"	{
			printf("STR_CONST: %s\n", yytext);
  			// return STR_CONST;
		}



[a-zA-Z_][a-zA-Z0-9_.]*	{
							installID(yytext, yylineno);
							printf("SYMBOL: %s\n", yytext);
  							// return SYMBOL;
						}

.	{	
		// return yytext[0];	
	}

%%

int yywrap() { return 1; }

int exists(char *sym)
{
    int flag=0;
    for(int i=0;i<TABLE_SIZE;i++){
        if(strcmp(sym,table[i].sym) == 0){
            flag=1;
            break;
        }
    }
    return(flag);
}

void installID(char *sym_name, int lineno)
{
	if(!exists(sym_name))
	{
		lastSym++;
		strcpy(table[lastSym].sym, sym_name);
		table[lastSym].lineno = lineno;
	}
}

void display_table(Symbol *table, int n)                                            
{                                                                                   
    
    int i;                                                                          
    
    printf("%-20s %-10s %-20s %-10s\n",
           "SYMBOL", "TYPE", "ADDRESS", "LINE NO.");                                
    for (i = 0; i < n; i++) {
        char *sym = table[i].sym;                                                   
        /*
        char *type;
        switch (table[i].type) {                                                    
            case NUMBER:
                type = "NUMBER";                                                    
                break;
            case STRING:
                type = "STRING";                                                    
                break;                                                              
        }
        printf("%-20s %-10s %-20s %-10s\n",                                         
        sym, type, ad);                                                             
        */
        printf("%-20s %-10s %-20s %010d\n",                                         
        sym, "", "", table[i].lineno);                                              
    }                                                                               

}


int main()
{

	int c;

	/*
	extern FILE *yyin;

	yyin = fopen("test.r", "r"); 
	// yylex();

	
	while(!feof(yyin))
	{
		yylex();
	} */

	/*
	while(!feof(stdin))
	{
		yylex();
		printf("feof: %d\n", feof(stdin));
	}
	*/
	yylex();

	display_table(table, lastSym+1);
}
