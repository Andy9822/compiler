#include "asm.h"
#include "hash.h"
#include "tacs.h"
#include "type_inference.h"

long int labelCount = 0;
long int actualLabel = 0;

////////////////////// ASM EXPLICIT COMMANDS /////////////////////
void saveIntRegisterToVariable(char* varName, FILE* fout)
{
    fprintf(fout, "\tmovl	%%eax, _%s(%%rip)\n", varName);
}

void intNumToRegister(long int value, FILE* fout)
{
    fprintf(fout, "\tmovl	$%li, %%eax\n", value);
}

void intNumToVar(char* varName, long int value, FILE* fout)
{
    intNumToRegister(value, fout);
    saveIntRegisterToVariable(varName, fout);
}

void intVarToRegister(char* varName, FILE* fout)
{
    fprintf(fout, "\tmovl	_%s(%%rip), %%eax\n", varName);
}

void callPrintFunction(char* printFuncName, FILE* fout)
{
    fprintf(fout, "\tmovl	$1, %%eax\n");
    fprintf(fout, "\tleaq	%s(%%rip), %%rdi\n", printFuncName);
    fprintf(fout, "\tcall	printf@PLT\n");
    fprintf(fout, "\tmovl	$0, %%eax\n\n");
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

void floatRegisterToIntRegister(FILE* fout)
{
    fprintf(fout, "\tcvttss2sil	%%xmm0, %%eax\n");
}

void floatVarToIntVar(char* src, char* dst, int registerNumber, FILE* fout)
{
    floatVarToFloatRegister(src, registerNumber, fout);
    floatRegisterToIntRegister(fout);
    saveIntRegisterToVariable(dst, fout);
}

void intNumToFloatRegister(long int value, int registerNumber, FILE* fout)
{
    intNumToRegister(value, fout);
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

void saveFloatRegisterToIntVar(char* varName, FILE* fout)
{
    floatRegisterToIntRegister(fout);
    saveIntRegisterToVariable(varName, fout);
}

void intNumToFloatVar(char* varName, long int value, FILE* fout) 
{
    intNumToFloatRegister(value, DEFAULT_REGISTER, fout);
    saveFloatRegisterToFloatVar(varName, fout);
}

void printWhiteLine(FILE* fout)
{
    fprintf(fout, "\n");
}

void compareFloatRegister(FILE* fout)
{
    fprintf(fout, "\tucomiss	%%xmm0, %%xmm1\n");
}

void compareFloatRegisterOrdered(FILE* fout)
{
    fprintf(fout, "\tcomiss	%%xmm0, %%xmm1\n");
}

void zeroRegister(char* registerName, FILE* fout)
{
    fprintf(fout, "\tpxor	%%%s, %%%s\n", registerName, registerName);
}

void testZeroFloatRegister(FILE* fout)
{
    fprintf(fout, "\tpxor	%%xmm1, %%xmm1\n");
    compareFloatRegister(fout);
}

void testZeroFloatVar(char* varName, FILE* fout)
{
    floatVarToFloatRegister(varName, DEFAULT_REGISTER, fout);
    testZeroFloatRegister(fout);
}

void testZeroIntNum(long int value, FILE* fout)
{
    intNumToFloatRegister(value, DEFAULT_REGISTER, fout);
    testZeroFloatRegister(fout);
}

void jumpZero(long int label, FILE* fout)
{
	fprintf(fout, "\tjz	.L%li\n", label);
}

void jumpZeroStr(char* labelName, FILE* fout)
{
	fprintf(fout, "\tjz	%s\n", labelName);
}

void jumpStr(char* labelName, FILE* fout)
{
	fprintf(fout, "\tjmp	%s\n", labelName);
}

void jumpNotZero(long int label, FILE* fout)
{
	fprintf(fout, "\tjnz    .L%li\n", label);
}

void jump(long int label, FILE* fout)
{
    fprintf(fout, "\tjmp  .L%li\n", label);
}

void jumpB(long int label, FILE* fout)
{
    fprintf(fout, "\tjb  .L%li\n", label);
}

void label(long int label, FILE* fout)
{
    fprintf(fout, ".L%li:\n", label);
}

void generateLabel(char* labelName, FILE* fout)
{
    fprintf(fout, "%s:\n", labelName);
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

void processOperandToFloatRegister(HASH_NODE* op, int registerNumber, FILE* fout)
{
    if (op->type == SYMBOL_LIT_CHAR)
    {
        int op_value = op->text[1];
        intNumToFloatRegister(op_value, registerNumber, fout);
    }
    else if (op->type == SYMBOL_LIT_INTEGER )
    {
        long int op_value = strtol(op->text, NULL, 16);
        intNumToFloatRegister(op_value, registerNumber, fout);
    }
    else if (isBoolLiteral(op->type))
    {
        int bool_value = (strcmp(op->text, "TRUE") == 0) ? 1 : 0;
        intNumToFloatRegister(bool_value, registerNumber, fout);
    }
    else if (op->type == SYMBOL_LIT_FLOAT)
    {
        long int floatInInteger = castFloatHexStringToInt(op->text);
        floatNumToFloatRegister(floatInInteger, registerNumber, fout);
    }
    else if (op->type == SYMBOL_VARIABLE)
    {
        if (isFloatVariable(op->data_type))
        {
            floatVarToFloatRegister(op->text, registerNumber, fout);
        }
        else if (isIntVariable(op->data_type) || op->data_type == DATATYPE_BOOL)
        {
            intVarToFloatRegister(op->text, registerNumber, fout);
        }
    }
}

void copyToIntVar(TAC* tac, FILE* fout)
{
    processOperandToFloatRegister(tac->op1, DEFAULT_REGISTER, fout);
    saveFloatRegisterToIntVar(tac->res->text, fout);
}

void copyToFloatVar(TAC* tac, FILE* fout)
{
    processOperandToFloatRegister(tac->op1, DEFAULT_REGISTER, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);
}

void processCopy(TAC* tac, FILE* fout) 
{
    switch (tac->res->data_type)
    {
        case DATATYPE_CHAR:
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            copyToIntVar(tac, fout);
            break;
        
        case DATATYPE_FLOAT:
        case DATATYPE_TEMP_FLOAT:
            copyToFloatVar(tac, fout);
            break;
        
        default:
            break;
    }
}

void testAndZeroOperator(HASH_NODE* op, FILE* fout)
{
    if (op->type == SYMBOL_VARIABLE)
    {
        testZeroFloatVar(op->text, fout);
    }
    else if (isBoolLiteral(op->type))
    {
        int bool_value = (strcmp(op->text, "TRUE") == 0) ? 1 : 0;
        testZeroIntNum(bool_value, fout);
    }
}

void processOr(TAC* tac, FILE* fout)
{
    int labelNotZero = actualLabel++;
    int labelEnd = actualLabel++;

    //Coloca op1 em xmm0 e compara com 0
    testAndZeroOperator(tac->op1, fout);
    jumpNotZero(labelNotZero, fout);

    // # Coloca op2 em xmm0 e compara com 0;
    testAndZeroOperator(tac->op2, fout);
    jumpNotZero(labelNotZero, fout);

    intNumToFloatVar(tac->res->text, 0, fout);
	jump(labelEnd, fout);

    label(labelNotZero, fout);
    intNumToFloatVar(tac->res->text, 1, fout);

    label(labelEnd, fout);
}

void processAnd(TAC* tac, FILE* fout)
{
    int labelZero = actualLabel++;
    int labelEnd = actualLabel++;

    //Coloca op1 em xmm0 e compara com 0
    testAndZeroOperator(tac->op1, fout);
    jumpZero(labelZero, fout);

    // # Coloca op2 em xmm0 e compara com 0;
    testAndZeroOperator(tac->op2, fout);
    jumpZero(labelZero, fout);

    intNumToFloatVar(tac->res->text, 1, fout);
	jump(labelEnd, fout);

    label(labelZero, fout);
    intNumToFloatVar(tac->res->text, 0, fout);

    label(labelEnd, fout);
}

void compareOperators(HASH_NODE* op1, HASH_NODE* op2,FILE* fout)
{
    processOperandToFloatRegister(op1, 0, fout);
    processOperandToFloatRegister(op2, 1, fout);
    compareFloatRegister(fout);
}

void processEqDiff(TAC* tac, int zeroValue, int notZeroValue, FILE* fout)
{
    int labelZero = actualLabel++;
    int labelEnd = actualLabel++;

    compareOperators(tac->op1, tac->op2, fout);
    jumpZero(labelZero, fout);

    intNumToFloatVar(tac->res->text, notZeroValue, fout);
	jump(labelEnd, fout);

    label(labelZero, fout);

    intNumToFloatVar(tac->res->text, zeroValue, fout);

    label(labelEnd, fout);
}

void processEq(TAC* tac, FILE* fout)
{
    processEqDiff(tac, 1, 0, fout);
}

void processDiff(TAC* tac, FILE* fout)
{
    processEqDiff(tac, 0, 1, fout);
}

void processGELE(TAC* tac, int op1Register, int op2Register, int isGEorLE,FILE* fout)
{
    int labelBranching = actualLabel++;
    int labelEnd = actualLabel++;

    processOperandToFloatRegister(tac->op1, op1Register, fout);
    processOperandToFloatRegister(tac->op2, op2Register, fout);

    compareFloatRegisterOrdered(fout);
    jumpB(labelBranching, fout);

    intNumToFloatRegister(isGEorLE, DEFAULT_REGISTER, fout);
    jump(labelEnd, fout);

    label(labelBranching, fout);
    intNumToFloatRegister(!isGEorLE, DEFAULT_REGISTER, fout);

    label(labelEnd, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);
}

void processGE(TAC* tac, FILE* fout)
{
    processGELE(tac, 1, 0, 1, fout);
}

void processLE(TAC* tac, FILE* fout)
{
    processGELE(tac, 0, 1, 1, fout);
}

void calculateGreatThan(TAC* tac, FILE* fout)
{
     processGELE(tac, 0, 1, 0, fout);
}

void calculateLessThan(TAC* tac, FILE* fout)
{
    processGELE(tac, 1, 0, 0, fout);
}

void processGT(TAC* tac, FILE* fout)
{
    calculateGreatThan(tac, fout);
}

void processLT(TAC* tac, FILE* fout)
{
    calculateLessThan(tac, fout);
}

void processJMPF(TAC* tac, FILE* fout)
{
    processOperandToFloatRegister(tac->op1, 0, fout);
    testZeroFloatRegister(fout);
    jumpZeroStr(tac->res->text, fout);
}

void processJMP(TAC* tac, FILE* fout)
{
    jumpStr(tac->res->text, fout);
}

void processNot(TAC* tac, FILE* fout)
{
    int labelNotZero = actualLabel++;
    int labelEnd = actualLabel++;

    processOperandToFloatRegister(tac->op1, 0, fout);
    intNumToFloatRegister(0, 1, fout);
    compareFloatRegister(fout);

    jumpNotZero(labelNotZero, fout);

    intNumToFloatRegister(1, 0, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);
    jump(labelEnd, fout);

    label(labelNotZero, fout);
    intNumToFloatRegister(0, 0, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);

    label(labelEnd, fout);
}

void processArithmeticResult(char* resultVar, char* opName, FILE* fout) 
{
    fprintf(fout, "\t%sss	%%xmm1, %%xmm0\n", opName);
    saveFloatRegisterToFloatVar(resultVar, fout);
}

void processArithmeticOperation(TAC* tac, char* opName, FILE* fout) 
{
    processOperandToFloatRegister(tac->op1, 0, fout);  // 0 due to op1 -> register 0
    processOperandToFloatRegister(tac->op2, 1, fout);  // 1 due to op2 -> register 1
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
        case DATATYPE_BOOL:
            fprintf(fout, "\tmovl	_%s(%%rip), %%esi\n", tac->res->text);
            callPrintFunction(".print_int_string", fout);
            
        break;
        
        case DATATYPE_FLOAT:
            floatVarToFloatRegister(tac->res->text, 0, fout);
            fprintf(fout, "\tcvtss2sd	%%xmm0, %%xmm0\n");
            callPrintFunction(".print_float_string", fout);
        break;

        // case DATATYPE_BOOL: ;    TODO se der tempo, printar em bool mesmo: TRUE ou FALSE
        //     int boolValue = atoi(tac->res->text);
        //     printf("print bool %s with value %d\n", tac->res->text, boolValue);
        // break;
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
    fprintf(fout, ".true_string:	.string	\"TRUE\"\n");
    fprintf(fout, ".false_string:	.string	\"FALSE\"\n");
    fprintf(fout, ".true:	.quad	.true_string\n");
    fprintf(fout, ".false:	.quad	.false_string\n");

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
    
    case TAC_AND:
        processAnd(tac, fout);
        break;
    
    case TAC_OR:
        processOr(tac, fout);
        break;
    
    case TAC_EQ:
        processEq(tac, fout);
        break;
    
    case TAC_DIF:
        processDiff(tac, fout);
        break;
    
    case TAC_GE:
        processGE(tac, fout);
        break;
    
    case TAC_LE:
        processLE(tac, fout);
        break;
    
    case TAC_NOT:
        processNot(tac, fout);
        break;
    
    case TAC_GREATER:
        processGT(tac, fout);
        break;
    
    case TAC_LESSER:
        processLT(tac, fout);
        break;
    
    case TAC_JMP_FALSE:
        processJMPF(tac, fout);
        break;
    
    case TAC_LABEL:
        generateLabel(tac->res->text, fout);
        break;
    
    case TAC_JMP:
        processJMP(tac, fout);
        break;
    
    default:
      break;
    }
  }

  fclose(fout);
}