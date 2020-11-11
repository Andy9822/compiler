#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

ETAPA=etapa6

etapa3:  main.o y.tab.o lex.yy.o hash.o ast.o decompiler.o semantic.o type_inference.o tacs.o asm.o
	gcc  main.o y.tab.o lex.yy.o hash.o ast.c decompiler.c semantic.c type_inference.c tacs.c asm.c -o $(ETAPA)

main.o: main.c
	gcc -c main.c

asm.o: asm.c
	gcc -c asm.c

tacs.o: tacs.c
	gcc -c tacs.c

type_inference.o: type_inference.c
	gcc -c type_inference.c

semantic.o: semantic.c
	gcc -c semantic.c

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

tar:
	- tar cvzf $(ETAPA).tgz .
	sudo chmod 744 $(ETAPA).tgz

submit: tar
	scp $(ETAPA).tgz  argarramones@html.inf.ufrgs.br:public_html/

clean:
	rm -f scanner.c lex.yy.c y.tab.c y.tab.h *.o $(ETAPA)
