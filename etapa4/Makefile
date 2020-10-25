#
# UFRGS - Compiladores B - Marcelo Johann - 2009/2 - Etapa 1
#
# Makefile for single compiler call
# All source files must be included from code embedded in scanner.l
# In our case, you probably need #include "hash.c" at the beginning
# and #include "main.c" in the last part of the scanner.l
#

ETAPA=etapa4

etapa3:  main.o y.tab.o lex.yy.o hash.o ast.o decompiler.o semantic.o type_inference.o
	gcc  main.o y.tab.o lex.yy.o hash.o ast.c decompiler.c semantic.c type_inference.c -o $(ETAPA)

main.o: main.c
	gcc -c main.c

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

test-previous:
	./$(ETAPA) tests/ast-generation/basic_input.txt tests/ast-generation/output.txt
	./$(ETAPA) tests/ast-generation/output.txt tests/ast-generation/final.txt      
	diff tests/ast-generation/output.txt tests/ast-generation/final.txt
	rm tests/ast-generation/output.txt tests/ast-generation/final.txt  

test-variables:
	./etapa4 tests/semantic_verification/variables/basic_input.txt out.txt > tests/semantic_verification/variables/basic_result.txt    
	diff tests/semantic_verification/variables/basic_expected.txt tests/semantic_verification/variables/basic_result.txt  

test-vector_dec:
	./etapa4 tests/semantic_verification/vector_dec/basic_input.txt out.txt > tests/semantic_verification/vector_dec/basic_result.txt    
	diff tests/semantic_verification/vector_dec/basic_expected.txt tests/semantic_verification/vector_dec/basic_result.txt  

test-arithm_bool_ops:
	./etapa4 tests/semantic_verification/arithmetic_boolean_operations/basic_input.txt out.txt > tests/semantic_verification/arithmetic_boolean_operations/basic_result.txt    
	diff tests/semantic_verification/arithmetic_boolean_operations/basic_expected.txt tests/semantic_verification/arithmetic_boolean_operations/basic_result.txt  

test-identifier_access:
	./etapa4 tests/semantic_verification/identifier_access/basic_input.txt out.txt > tests/semantic_verification/identifier_access/basic_result.txt    
	diff tests/semantic_verification/identifier_access/basic_expected.txt tests/semantic_verification/identifier_access/basic_result.txt  

test-atribuition_identifier_access:
	./etapa4 tests/semantic_verification/atribuition_identifier_access/basic_input.txt out.txt > tests/semantic_verification/atribuition_identifier_access/basic_result.txt    
	diff tests/semantic_verification/atribuition_identifier_access/basic_expected.txt tests/semantic_verification/atribuition_identifier_access/basic_result.txt  

test-if_clausule:
	./etapa4 tests/semantic_verification/if_clausule/basic_input.txt out.txt > tests/semantic_verification/if_clausule/basic_result.txt    
	diff tests/semantic_verification/if_clausule/basic_expected.txt tests/semantic_verification/if_clausule/basic_result.txt  

test: test-variables test-vector_dec test-arithm_bool_ops test-identifier_access test-atribuition_identifier_access test-if_clausule

tar:
	tar cvzf $(ETAPA).tgz .
	chmod 744 $(ETAPA).tgz

submit: tar
	scp $(ETAPA).tgz  argarramones@html.inf.ufrgs.br:public_html/

clean:
	rm -f scanner.c lex.yy.c y.tab.c y.tab.h *.o $(ETAPA)
