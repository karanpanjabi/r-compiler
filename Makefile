a.out : lex.yy.c y.tab.c
	gcc lex.yy.c y.tab.c
	
lex.yy.c : gram.l gram_fixed.y
	yacc -d -v gram_fixed.y
	lex gram.l
	
