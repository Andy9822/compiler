#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "decompiler.h"

extern FILE *yyin;
extern AST *Root;
unsigned long long int SemanticErrors;

int yyparse();

void check_argv(int argc, char **argv)
{
  if (argc == 4)
  {
    if (strcmp (argv[3], "-ast") == 0 ||  strcmp (argv[3], "-a") == 0)
    {
      astPrint(Root, 1);
    }
    if (strcmp (argv[3], "-hash") == 0 || strcmp (argv[3], "-h") == 0 )
    {
      hashPrint();
    }
  }

  if (argc == 5)
  {
    if (strcmp (argv[3], "-ast") == 0 ||  strcmp (argv[3], "-a") == 0
        || strcmp (argv[4], "-ast") == 0 ||  strcmp (argv[4], "-a") == 0)
    {
      astPrint(Root, 1);
    }
    if (strcmp (argv[3], "-hash") == 0 || strcmp (argv[3], "-h") == 0
        || strcmp (argv[4], "-hash") == 0 || strcmp (argv[4], "-h") == 0)
    {
      hashPrint();
    }
  }
}

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

  check_argv(argc, argv);

  if (SemanticErrors)
  {
    printf("Compilation ERROR! \n");
    // return 4;
  }

  else 
  {
    printf("Compilation successfull! \n");
  }
  // printf("SemanticErrors: %d! \n", SemanticErrors);
  return 0;
}
