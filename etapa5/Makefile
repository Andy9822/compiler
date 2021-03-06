#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

ETAPA=etapa5

etapa3:  main.o y.tab.o lex.yy.o hash.o ast.o decompiler.o semantic.o type_inference.o tacs.o
	gcc  main.o y.tab.o lex.yy.o hash.o ast.c decompiler.c semantic.c type_inference.c tacs.c -o $(ETAPA)

main.o: main.c
	gcc -c main.c

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

test_bool_arithm:
	- ./etapa5 tests/tac_generation/arithmetic_boolean_operations/basic_input.txt out.txt > tests/tac_generation/arithmetic_boolean_operations/basic_result.txt
	diff tests/tac_generation/arithmetic_boolean_operations/basic_expected.txt tests/tac_generation/arithmetic_boolean_operations/basic_result.txt

test_loop_while:
	- ./etapa5 tests/tac_generation/loop_while/basic_input.txt out.txt > tests/tac_generation/loop_while/basic_result.txt
	diff tests/tac_generation/loop_while/basic_expected.txt tests/tac_generation/loop_while/basic_result.txt

test_if_statement:
	- ./etapa5 tests/tac_generation/if_statement/basic_input.txt out.txt > tests/tac_generation/if_statement/basic_result.txt
	diff tests/tac_generation/if_statement/basic_expected.txt tests/tac_generation/if_statement/basic_result.txt

test_begin_end_func:
	- ./etapa5 tests/tac_generation/begin_end_func/basic_input.txt out.txt > tests/tac_generation/begin_end_func/basic_result.txt
	diff tests/tac_generation/begin_end_func/basic_expected.txt tests/tac_generation/begin_end_func/basic_result.txt

test_return_print_read:
	- ./etapa5 tests/tac_generation/return_print_read/basic_input.txt out.txt > tests/tac_generation/return_print_read/basic_result.txt
	diff tests/tac_generation/return_print_read/basic_expected.txt tests/tac_generation/return_print_read/basic_result.txt

test_vector_access:
	- ./etapa5 tests/tac_generation/vector_access/basic_input.txt out.txt > tests/tac_generation/vector_access/basic_result.txt
	diff tests/tac_generation/vector_access/basic_expected.txt tests/tac_generation/vector_access/basic_result.txt

test_func_call_arg:
	- ./etapa5 tests/tac_generation/func_call_arg/basic_input.txt out.txt > tests/tac_generation/func_call_arg/basic_result.txt
	diff tests/tac_generation/func_call_arg/basic_expected.txt tests/tac_generation/func_call_arg/basic_result.txt

test: test_bool_arithm test_loop_while test_if_statement test_begin_end_func test_return_print_read test_vector_access test_func_call_arg

tar:
	- tar cvzf $(ETAPA).tgz .
	sudo chmod 744 $(ETAPA).tgz

submit: tar
	scp $(ETAPA).tgz  argarramones@html.inf.ufrgs.br:public_html/

clean:
	rm -f scanner.c lex.yy.c y.tab.c y.tab.h *.o $(ETAPA)
