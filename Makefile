#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

etapa1: lex.yy.c
	gcc -o etapa1 lex.yy.c
lex.yy.c: scanner.l
	lex scanner.l

test-operators:
	python run_lexer.py operators
	python ./tests/compare_results.py operators

test-reserved:
	python run_lexer.py reserved
	python ./tests/compare_results.py reserved

test-identifiers:
	python run_lexer.py identifiers
	python ./tests/compare_results.py identifiers

test-symbols:
	python run_lexer.py symbols
	python ./tests/compare_results.py symbols

test:
	make test-operators && make test-reserved && make test-identifiers && make test-symbols

clean:
	rm lex.yy.c etapa1
