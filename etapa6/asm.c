#include "asm.h"
#include "tacs.h"
#include "type_inference.h"


int isCharInitValue(char* string)
{
    if (string[0] == '\'')
    {
        return 1;
    }
    return 0;
}

int isFloatValue(char* string)
{
    int hasFoundPoint = 0;
    int i;

    for(i = 0; string[i] != '\0'; i++)
    {
        if (string[0] == '.')
        {
            hasFoundPoint = 1;
            break;
        }
    }

    return hasFoundPoint;
}

void generateNormalVariable(HASH_NODE* node,FILE* fout)
{
    if (isNumberType(node->data_type))
    {
        if (isCharInitValue(node->init_value))
        {
            fprintf(fout, "_%s:\t.long\t%i\n", node->text, node->init_value[1]);
        }
        
        else if (!isFloatValue(node->init_value))
        {
            long int init_value = strtol(node->init_value, NULL, 16);
            fprintf(fout, "_%s:\t.long\t%li\n", node->text, strtol(node->init_value, NULL, 16));
        }

        else
        {           
            fprintf(fout, "_%s:\t.long\t%s\n", node->text, node->init_value);
        }
        
    }
    
    else if (isBoolType(node->data_type))
    {
        int bool_value = (strcmp(node->init_value, "TRUE") == 0) ? 1 : 0;
        fprintf(fout, "_%s:\t.long\t%d\n", node->text, bool_value);
    }

    else // temp variables
    {
        fprintf(fout, "_%s:\t.long\t0\n", node->text);
    }
}

void generateGlobalVariables(FILE* fout)
{
  int i;
  HASH_NODE* node;

  for (i = 0; i < HASH_SIZE; i++)
  {
    for (node=HASH_TABLE[i]; node; node = node->next)
    {
      if (node->type == SYMBOL_VARIABLE)
      {
          generateNormalVariable(node, fout);
      }
    }
  }
  
  fprintf(fout, "\n");
}

void generateASM(TAC* first)
{
  TAC* tac;

  FILE *fout;
  fout = fopen("my_assembly.s", "w");

  // Init headers
  fprintf(fout, ".section	.rodata\n");
  fprintf(fout, ".globl	main\n");
  fprintf(fout, ".data\n\n");
  fprintf(fout, ".print_int_string:\n");
  fprintf(fout, "\t.string	\"%%d\\n\"\n\n");
  fprintf(fout, ".print_float_string:\n");
  fprintf(fout, "\t.string	\"%%f\\n\"\n\n");
  fprintf(fout, ".print_string:\n");
  fprintf(fout, "\t.string	\"%%s\\n\"\n\n");

  // Hash Table
  generateGlobalVariables(fout);
  
  // Each TAC
  for (tac = first; tac; tac = tac->next)
  {
    switch (tac->type)
    {
    case TAC_BEGINFUN:
        fprintf(fout, "%s:\n", tac->res->text);
        fprintf(fout, ".LFB0:\n");
        fprintf(fout, "\tpushq	%%rbp\n");
        fprintf(fout, "\tmovq	%%rsp, %%rbp\n\n");
      /* code */
      break;
    
    case TAC_ENDFUN:
        fprintf(fout, "\tpopq	%%rbp\n");
        fprintf(fout, "\tret\n");
        break;
    
    case TAC_PRINT:
        switch (tac->res->data_type)
        {
        case DATATYPE_CHAR:
          printf("print char %s\n", tac->res->text);
          break;
        
        case DATATYPE_INT:
          fprintf(fout, "\tmovl	_%s(%%rip), %%esi\n", tac->res->text);
          fprintf(fout, "\tleaq	.print_int_string(%%rip), %%rdi\n");
          fprintf(fout, "\tcall	printf@PLT\n\n");
          // printf("print int %s\n", tac->res->text);
          
          break;
        
        case DATATYPE_FLOAT:
          printf("print float %s\n", tac->res->text);
          break;
        
        case DATATYPE_BOOL:
          printf("print bool %s\n", tac->res->text);
          break;
        }
      break;
    
    default:
      break;
    }
  }

  fclose(fout);
}