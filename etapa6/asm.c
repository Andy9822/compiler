#include "asm.h"
#include "hash.h"
#include "tacs.h"
#include "type_inference.h"


int isCharValue(char* string)
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
        if (isCharValue(node->init_value))
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

void copyFloatLiteralToCharIntVariable(TAC* tac, FILE* fout)
{
    int tamanho = strlen(tac->op1->text) + 1; // add \0 position
    int vectorSize = tamanho + 2 + 2; // consider 0x and p0 
    char floatString[vectorSize]; 
    floatString[0] = '0';
    floatString[1] = 'x';

    // If it starts with 0 we have to ignore it
    int offsetFloatLiteral = (tac->op1->text[0] == '0') ? 1 : 0;
    strcpy(floatString + 2, (tac->op1->text) + offsetFloatLiteral);

    floatString[vectorSize - 3] = 'p';
    floatString[vectorSize - 2] = '0';
    floatString[vectorSize - 1] = '\0';

    // Convert float bits to integer
    float floatNumber = strtod(floatString, NULL);
    int floatInInteger;
    memcpy(&floatInInteger, &floatNumber, sizeof(float));

    // Passa o valor hexa float constante para o eax para poder depois mover para  xmm0  
    // faz  tcvttss2sil sobre xmm0 para eax | salva valor eax na variavel
    // TODO remover esses 2 prints
    // printf("%s recebe %f\n", tac->res->text, floatNumber);
    // printf("%s recebe %d\n", tac->res->text, floatInInteger);
    fprintf(fout, "\tmovl	$%d, %%eax\n", floatInInteger);
    fprintf(fout, "\tmovd	%%eax, %%xmm0\n");
    fprintf(fout, "\tcvttss2sil	%%xmm0, %%eax\n");
    fprintf(fout, "\tmovl     %%eax, _%s(%%rip)\n", tac->res->text);
}

void processCopy(TAC* tac, FILE* fout) 
{
    switch (tac->res->data_type)
    {
    case DATATYPE_CHAR:
    case DATATYPE_INT:
        if (tac->op1->type == SYMBOL_LIT_INTEGER || tac->op1->type == SYMBOL_LIT_CHAR)
        {
            // Se valor for inteiro, faz strtol com base 16. Se for char, só pega o caractere mesmo
            int value = tac->op1->type == SYMBOL_LIT_INTEGER ? strtol(tac->op1->text, NULL, 16) : tac->op1->text[1];
            fprintf(fout, "\tmovl\t$%d, %s(%%rip) \n", value, tac->res->text);
        }
        else if (tac->op1->type == SYMBOL_LIT_FLOAT)
        {
            copyFloatLiteralToCharIntVariable(tac, fout);
        }
        else if(tac->op1->type == SYMBOL_VARIABLE)
        {
            if (tac->op1->data_type == DATATYPE_CHAR || tac->op1->data_type == DATATYPE_INT)
            {
                printf("char/int recebendo char int sussa \n");
                fprintf(fout, "\tmovl	_%s(%%rip), %%eax\n", tac->op1->text);
                fprintf(fout, "\tmovl	%%eax, _%s(%%rip)\n", tac->op1->text);
               
            }
        }
        
        
        // SYMBOL_VARIABLE
        break;
    
    case DATATYPE_FLOAT:
        /* code */
        break;
    
    case DATATYPE_BOOL:
        /* code */
        break;
    
    default:
        break;
    }
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
    
    case TAC_COPY:
        processCopy(tac, fout);
        break;
    
    default:
      break;
    }
  }

  fclose(fout);
}