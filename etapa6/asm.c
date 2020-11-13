#include "asm.h"
#include "hash.h"
#include "tacs.h"
#include "type_inference.h"

////////////////////// ASM EXPLICIT COMMANDS /////////////////////
void intRegisterToVariable(char* varName, FILE* fout)
{
    fprintf(fout, "\tmovl	%%eax, _%s(%%rip)\n", varName);
}

void callPrintFunction(char* printFuncName, FILE* fout)
{
    fprintf(fout, "\tleaq	%s(%%rip), %%rdi\n", printFuncName);
    fprintf(fout, "\tcall	printf@PLT\n\n");
}

void intRegisterToFloatRegister(int registerNumber, FILE* fout)
{
    fprintf(fout, "\tcvtsi2ssl	%%eax, %%xmm%d\n", registerNumber);
}

void intVarToFloatRegister(char* varName, int registerNumber, FILE* fout)
{
    fprintf(fout, "\tmovl	_%s(%%rip), %%eax\n", varName);
    intRegisterToFloatRegister(registerNumber, fout);
}

void floatVarToFloatRegister(char* varName, int registerNumber, FILE* fout)
{
    fprintf(fout, "\tmovss	_%s(%%rip), %%xmm%d\n", varName, registerNumber);
}

void intNumToFloatRegister(long int value, int registerNumber, FILE* fout)
{
    fprintf(fout, "\tmovl	$%li, %%eax\n", value);
    intRegisterToFloatRegister(registerNumber, fout);
}

void floatNumToFloatRegister(long int value, int registerNumber, FILE* fout)
{
    fprintf(fout, "\tmovl	$%li, %s(%%rip)\n", value, FLOAT_TEMP_VAR);
    fprintf(fout, "\tmovss	 %s(%%rip), %%xmm%d\n", FLOAT_TEMP_VAR, registerNumber);
}

void saveFloatRegisterToFloatVar(char* resultVar, FILE* fout)
{
    fprintf(fout, "\tmovss	%%xmm0, _%s(%%rip)\n", resultVar);
}

void printWhiteLine(FILE* fout)
{
    fprintf(fout, "\n");
}
////////////////////// END ASM EXPLICIT COMMANDS /////////////////////


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
        if (string[i] == '.')
        {
            hasFoundPoint = 1;
            break;
        }
    }

    return hasFoundPoint;
}

int castFloatHexStringToInt(char* string)
{
    int tamanho = strlen(string) + 1; // add \0 position
    int vectorSize = tamanho + 2 + 2; // consider 0x and p0 
    char floatString[vectorSize]; 
    floatString[0] = '0';
    floatString[1] = 'x';

    // If it starts with 0 we have to ignore it
    int offsetFloatLiteral = (string[0] == '0') ? 1 : 0;
    strcpy(floatString + 2, (string) + offsetFloatLiteral);

    floatString[vectorSize - 3] = 'p';
    floatString[vectorSize - 2] = '0';
    floatString[vectorSize - 1] = '\0';

    // Convert float bits to integer
    float floatNumber = strtod(floatString, NULL);
    int floatInInteger;
    memcpy(&floatInInteger, &floatNumber, sizeof(float));

    return floatInInteger;
}

void copyFloatLiteralToCharIntVariable(TAC* tac, FILE* fout)
{
    int floatInInteger = castFloatHexStringToInt(tac->op1->text);

    fprintf(fout, "\tmovl	$%d, %%eax\n", floatInInteger);
    fprintf(fout, "\tmovd	%%eax, %%xmm0\n");
    fprintf(fout, "\tcvttss2sil	%%xmm0, %%eax\n");
    fprintf(fout, "\tmovl     %%eax, _%s(%%rip)\n", tac->res->text);
}

void copyToIntVar(TAC* tac, FILE* fout)
{
    if ( isIntLiteral(tac->op1->type) )
    {
        // Se valor for inteiro, faz strtol com base 16. Se for char, só pega o caractere mesmo
        int value = tac->op1->type == SYMBOL_LIT_INTEGER ? strtol(tac->op1->text, NULL, 16) : tac->op1->text[1];
        fprintf(fout, "\tmovl	$%d, _%s(%%rip) \n", value, tac->res->text);
    }
    else if (tac->op1->type == SYMBOL_LIT_FLOAT)
    {
        copyFloatLiteralToCharIntVariable(tac, fout);
    }
    else if(tac->op1->type == SYMBOL_VARIABLE)
    {
        //Se variável é int/char apenas move valores dela
        if (tac->op1->data_type == DATATYPE_CHAR || tac->op1->data_type == DATATYPE_INT)
        {
            fprintf(fout, "\tmovl	_%s(%%rip), %%eax\n", tac->op1->text);
            intRegisterToVariable(tac->op1->text, fout);
        }
        //Se variável é float casta pra int antes de mover
        else if (isFloatVariable(tac->op1->data_type))
        {
            floatVarToFloatRegister(tac->op1->text, 0, fout);
            fprintf(fout, "\tcvttss2sil	%%xmm0, %%eax\n");
            intRegisterToVariable(tac->res->text, fout);
        }
    }
}

void copyToFloatVar(TAC* tac, FILE* fout)
{
    if ( isIntLiteral(tac->op1->type) )
    {
        // Se valor for inteiro, faz strtol com base 16. Se for char, só pega o caractere mesmo
        long int value = tac->op1->type == SYMBOL_LIT_INTEGER ? strtol(tac->op1->text, NULL, 16) : tac->op1->text[1];
        intNumToFloatRegister(value, 0, fout);
        saveFloatRegisterToFloatVar(tac->res->text, fout);
        
    }
    else if (tac->op1->type == SYMBOL_LIT_FLOAT)
    {
        long int op_value = castFloatHexStringToInt(tac->op1->text);
        floatNumToFloatRegister(op_value, 0, fout);
        saveFloatRegisterToFloatVar(tac->res->text, fout); // TODO passar pras switch essa porra de if
    }
    else if(tac->op1->type == SYMBOL_VARIABLE)
    {
        //Se variável é int/char apenas move valores dela
        if (tac->op1->data_type == DATATYPE_CHAR || tac->op1->data_type == DATATYPE_INT)
        {
            intVarToFloatRegister(tac->op1->text, 0, fout);
            saveFloatRegisterToFloatVar(tac->res->text, fout);
        }
        //Se variável é float casta pra int antes de mover
        else if (isFloatVariable(tac->op1->data_type))
        {
            floatVarToFloatRegister(tac->op1->text, 0, fout);
            saveFloatRegisterToFloatVar(tac->res->text, fout);
        }
    }
}

void processCopy(TAC* tac, FILE* fout) 
{
    switch (tac->res->data_type)
    {
        case DATATYPE_CHAR:
        case DATATYPE_INT:
            copyToIntVar(tac, fout);
            // SYMBOL_VARIABLE
            break;
        
        case DATATYPE_FLOAT:
        case DATATYPE_TEMP_FLOAT:
            copyToFloatVar(tac, fout);
            break;
        
        case DATATYPE_BOOL:
            /* code */
            break;
        
        default:
            break;
    }
}

void processArithmeticOperand(char* opText, int opType, int data_type, int registerNumber, FILE* fout)
{
    if (opType == SYMBOL_LIT_CHAR)
    {
        int op_value = opText[1];
        intNumToFloatRegister(op_value, registerNumber, fout);
    }
    else if (opType == SYMBOL_LIT_INTEGER )
    {
        long int op_value = strtol(opText, NULL, 16);
        intNumToFloatRegister(op_value, registerNumber, fout);
    }
    else if (opType == SYMBOL_LIT_FLOAT)
    {
        long int floatInInteger = castFloatHexStringToInt(opText);
        floatNumToFloatRegister(floatInInteger, registerNumber, fout);
    }
    else if (opType == SYMBOL_VARIABLE)
    {
        if (isFloatVariable(data_type))
        {
            floatVarToFloatRegister(opText, registerNumber, fout);
        }
        else if (isIntVariable(data_type))
        {
            intVarToFloatRegister(opText, registerNumber, fout);
        }
    }
}

void processArithmeticResult(char* resultVar, char* opName, FILE* fout) 
{
    fprintf(fout, "\t%sss	%%xmm1, %%xmm0\n", opName);
    saveFloatRegisterToFloatVar(resultVar, fout);
}

void processArithmeticOperation(TAC* tac, char* opName, FILE* fout) 
{
    processArithmeticOperand(tac->op1->text, tac->op1->type, tac->op1->data_type, 0, fout);  // 0 due to op1 -> register 0
    processArithmeticOperand(tac->op2->text, tac->op2->type, tac->op2->data_type, 1, fout);  // 1 due to op2 -> register 1
    processArithmeticResult(tac->res->text, opName, fout);
}

void printVariable(TAC* tac, FILE* fout)
{
    switch (tac->res->data_type)
    {
        case DATATYPE_CHAR:
            fprintf(fout, "\tmovl	_%s(%%rip), %%esi\n", tac->res->text);
            callPrintFunction(".print_char_string", fout);
        break;
        
        case DATATYPE_INT:
            fprintf(fout, "\tmovl	_%s(%%rip), %%esi\n", tac->res->text);
            callPrintFunction(".print_int_string", fout);
            
        break;
        
        case DATATYPE_FLOAT:
            floatVarToFloatRegister(tac->res->text, 0, fout);
            fprintf(fout, "\tcvtss2sd	%%xmm0, %%xmm0\n");
            callPrintFunction(".print_float_string", fout);
        break;

        case DATATYPE_BOOL:
        printf("print bool %s\n", tac->res->text);
        break;
    }
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
            int floatHexinInt = castFloatHexStringToInt(node->init_value);
            fprintf(fout, "_%s:\t.long\t%d\n", node->text, floatHexinInt);
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
    // Print Hash Table Variables
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

    // Print own variables for easier manipulation
    fprintf(fout, "floatTemp:\t.long\t0\n");

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
  fprintf(fout, ".print_char_string:\n");
  fprintf(fout, "\t.string	\"%%c\\n\"\n\n");
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
      break;
    
    case TAC_ENDFUN:
        // movl	$0, %eax pra retornar algo
        fprintf(fout, "\tpopq	%%rbp\n");
        fprintf(fout, "\tret\n");
        break;
    
    case TAC_PRINT:
        if (tac->res->type == SYMBOL_VARIABLE)
        {
            printVariable(tac, fout);
        }
            
      break;
    
    case TAC_COPY:
        processCopy(tac, fout);
        printWhiteLine(fout);
        break;
    
    case TAC_ADD:
        processArithmeticOperation(tac, "add", fout);
        printWhiteLine(fout);
        break;
    case TAC_SUB:
        processArithmeticOperation(tac, "sub", fout);
        printWhiteLine(fout);
        break;
    
    case TAC_MULT:
        processArithmeticOperation(tac, "mul", fout);
        printWhiteLine(fout);
        break;
    
    case TAC_DIV:
        processArithmeticOperation(tac, "div", fout);
        printWhiteLine(fout);
        break;
    
    default:
      break;
    }
  }

  fclose(fout);
}