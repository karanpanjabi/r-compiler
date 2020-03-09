a.out : lex.yy.c y.tab.c header.h
	gcc lex.yy.c y.tab.c
	
lex.yy.c : gram.l gram_fixed.y header.h
	yacc -d -v gram_fixed.y
	lex gram.l
	
clean :
	rm -f a.out lex.yy.c y.tab.c y.tab.h y.output