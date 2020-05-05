a.out : lex.yy.c y.tab.c header.h ast.h ast.c icg.c icg.h opt.c opt.h target.c target.h gen_util.h
	gcc -g lex.yy.c y.tab.c ast.c icg.c opt.c target.c
	
lex.yy.c : gram.l gram.y header.h
	yacc -d -v gram.y
	lex gram.l
	
clean :
	rm -f a.out lex.yy.c y.tab.c y.tab.h y.output *.o