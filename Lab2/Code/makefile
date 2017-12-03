parser:syntax.y lab2.c lexical.l semantic.c
	flex -o ./lex.yy.c ./lexical.l
	bison -o ./syntax.tab.c -d -v ./syntax.y
	gcc -w -c ./syntax.tab.c -o ./syntax.tab.o
	gcc -w -std=c99   -c -o node.o node.c
	gcc -w -std=c99   -c -o lab2.o lab2.c
	gcc -w -std=c99   -c -o semantic.o semantic.c
	gcc -w -o parser ./node.o ./semantic.o ./syntax.tab.o ./lab2.o -lfl -ly
	@rm -f lex.yy.c syntax.tab.c syntax.tab.h syntax.output *.o

.PHONY: clean test
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output *.o

test: parser
	./parser ../test/17.txt
