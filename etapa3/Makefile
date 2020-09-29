#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

etapa3:  main.o y.tab.o lex.yy.o hash.o ast.o
	gcc  main.o y.tab.o lex.yy.o hash.o ast.c -o etapa3

main.o: main.c
	gcc -c main.c

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

test-globals:
	python run_lexer.py globals
	python ./tests/compare_results.py globals

test-functions:
	python run_lexer.py functions
	python ./tests/compare_results.py functions

test-prof:
	python run_lexer.py prof
	python ./tests/compare_results.py prof

test-rafael:
	python run_lexer.py rafael
	python ./tests/compare_results.py rafael

test:
	make test-globals && make test-functions && make test-prof && make test-rafael

clean:
	rm -f scanner.c lex.yy.c y.tab.c y.tab.h *.o etapa3
