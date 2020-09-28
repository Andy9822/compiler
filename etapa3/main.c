#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

extern FILE *yyin;
int yyparse();

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Call: etapa1 fileName\n");
    exit(1);
  }
  yyin = fopen(argv[1], "r");
  if (yyin == 0) {
    fprintf(stderr, "Cannot open file %s\n", argv[1]);
    exit(2);
  }

  hash_init();
  yyparse();
  hashPrint();
  printf("Compilation successfull! \n");
  return 0;
}
