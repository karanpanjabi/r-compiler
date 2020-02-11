a.out : lex.yy.c y.tab.c
	gcc lex.yy.c y.tab.c
	
lex.yy.c : gram.l gram.y
	yacc -d -v gram.y
	lex gram.l
	
