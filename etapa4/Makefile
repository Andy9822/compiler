#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

ETAPA=etapa4

etapa3:  main.o y.tab.o lex.yy.o hash.o ast.o decompiler.o
	gcc  main.o y.tab.o lex.yy.o hash.o ast.c decompiler.c -o $(ETAPA)

main.o: main.c
	gcc -c main.c

decompiler.o: decompiler.c
	gcc -c decompiler.c

hash.o: hash.c
	gcc -c hash.c
	
ast.o: ast.c
	gcc -c ast.c

y.tab.o: y.tab.c 
	gcc -c y.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: scanner.c
	lex scanner.l

build:
	make clean --no-print-directory
	make --no-print-directory

test-previous:
	./$(ETAPA) tests/ast-generation/basic_input.txt tests/ast-generation/output.txt
	./$(ETAPA) tests/ast-generation/output.txt tests/ast-generation/final.txt      
	diff tests/ast-generation/output.txt tests/ast-generation/final.txt
	rm tests/ast-generation/output.txt tests/ast-generation/final.txt  

tar:
	tar cvzf $(ETAPA).tgz .
	chmod 744 $(ETAPA).tgz

submit: tar
	scp $(ETAPA).tgz  argarramones@html.inf.ufrgs.br:public_html/

clean:
	rm -f scanner.c lex.yy.c y.tab.c y.tab.h *.o $(ETAPA)
