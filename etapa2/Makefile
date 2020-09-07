#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

etapa2: y.tab.c lex.yy.c 
	gcc -o etapa2.out lex.yy.c

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.c: scanner.l
	lex scanner.l

build:
	make clean --no-print-directory
	make --no-print-directory

test-globals:
	python run_lexer.py globals
	python ./tests/compare_results.py globals

test:
	make test-globals

clean:
	rm -f lex.yy.c y.tab.c etapa2.out
