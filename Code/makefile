parser:syntax.y lab1.c lexical.l
	flex -o ./lex.yy.c ./lexical.l
	bison -o ./syntax.tab.c -d -v ./syntax.y
	gcc -c ./syntax.tab.c -o ./syntax.tab.o
	gcc -std=c99   -c -o node.o node.c
	gcc -std=c99   -c -o lab1.o lab1.c
	gcc -o parser ./node.o ./lab1.o ./syntax.tab.o -lfl -ly
	rm -f lex.yy.c syntax.tab.c syntax.tab.h syntax.output *.o

clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output *.o

test:
	./parser ../Test/10.txt
