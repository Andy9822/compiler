#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "decompiler.h"

extern FILE *yyin;
extern AST *Root;
unsigned long long int SemanticErrors;

int yyparse();

int main(int argc, char **argv) {

  if (argc < 3) {
    fprintf(stderr, "Call: etapa1 inputFilename outputFilename\n");
    exit(1);
  }
  yyin = fopen(argv[1], "r");
  if (yyin == 0) {
    fprintf(stderr, "Cannot open file %s\n", argv[1]);
    exit(2);
  }

  FILE *fp =fopen(argv[2], "w");;
  if (fp == 0) {
    fprintf(stderr, "Cannot open file %s\n", argv[2]);
    exit(2);
  }

  hash_init();
  yyparse();
  // decompile(Root, fp);
  // astPrint(Root, 1);
  // hashPrint();
  if (SemanticErrors)
  {
    printf("Compilation ERROR! \n");
  }
  else 
  {
    printf("Compilation successfull! \n");
  }
  // printf("SemanticErrors: %d! \n", SemanticErrors);
  return 0;
}
