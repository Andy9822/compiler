#include "asm.h"
#include "hash.h"
#include "tacs.h"
#include "type_inference.h"

long int actualFunctionLabel = 0;
long int actualLabel = 0;
long int actualLiteralLabel = 0;
int isMainFunction = 0;
VALUES_LIST* literals_to_print = NULL;

HASH_NODE* actual_func_node = NULL;
int actual_func_index = 0;
int actual_func_params_count = 0;
VALUES_LIST* parameters_names = NULL;

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

void vectorIndexAdressToRegisters(char* varName, FILE* fout)
{
    fprintf(fout, "\tcltq\n");
    fprintf(fout, "\tleaq	0(,%%rax,4), %%rdx\n"); 
    fprintf(fout, "\tleaq	_%s(%%rip), %%rax\n", varName); 
}

void floatVecToFloatRegister(char* varName, int registerNumber, FILE* fout)
{
    vectorIndexAdressToRegisters(varName, fout);
    fprintf(fout, "\tmovss	(%%rdx,%%rax), %%xmm%d\n", registerNumber); 
}

void floatRegisterToIntRegister(int registerNumber, FILE* fout)
{
    fprintf(fout, "\tcvttss2sil	%%xmm%d, %%eax\n", registerNumber);
}

void floatVarToIntVar(char* src, char* dst, int registerNumber, FILE* fout)
{
    floatVarToFloatRegister(src, registerNumber, fout);
    floatRegisterToIntRegister(DEFAULT_REGISTER, fout);
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
    floatRegisterToIntRegister(DEFAULT_REGISTER, fout);
    saveIntRegisterToVariable(varName, fout);
}

void saveFloatRegisterToVec(char* dest, int isFloatVec, FILE* fout)
{
    if (!isFloatVec)
    {
        floatRegisterToIntRegister(DEFAULT_REGISTER, fout);
        intRegisterToFloatRegister(DEFAULT_REGISTER, fout);
        // TODO se quiser imprimir no tipo certo, aqui adicionar resto de lógica própria
    }

    floatRegisterToIntRegister(ALTERNATIVE_REGISTER, fout);
    vectorIndexAdressToRegisters(dest, fout);
    fprintf(fout, "\tmovss	%%xmm%d, (%%rdx,%%rax)\n", DEFAULT_REGISTER);
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

void callFunction(char* funcName, FILE* fout)
{
    fprintf(fout, "\tcall   %s\n", funcName);
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

void readToVariable(char* varName, FILE* fout)
{
    fprintf(fout, "\tleaq   _%s(%%rip), %%rsi\n", varName);
    fprintf(fout, "\tleaq	.scanf_string(%%rip), %%rdi\n");
    fprintf(fout, "\tmovl	$0, %%eax\n");
    fprintf(fout, "\tcall	__isoc99_scanf@PLT\n");
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

void stackPositionToFloatRegister(int position, FILE* fout)
{
    fprintf(fout, "\tmovss 	%d(%%rbp), %%xmm0\n", -4 * position);
}
void floatRegisterToStack(int registerNumber, FILE* fout)
{
    fprintf(fout, "\tmovss	%%xmm%d, %d(%%rbp)\n", registerNumber, -4 * actual_func_index);
}

void allocateStackSpace(FILE* fout)
{
    fprintf(fout, "\tsubq	$16, %%rsp\n");
}
void callPrintLiteral(long int label, FILE* fout)
{
    fprintf(fout, "\tmovq	literal%li(%%rip), %%rax\n", label);
    fprintf(fout, "\tmovq	%%rax, %%rdi\n");
	fprintf(fout, "\tcall	puts@PLT\n");
}
        
void readCharToVariable(char* varName, FILE* fout)
{
	 fprintf(fout, "\tleaq	myCharTemp(%%rip), %%rsi\n");
	 fprintf(fout, "\tleaq	.scanf_char(%%rip), %%rdi\n");
	 fprintf(fout, "\tmovl	$0, %%eax\n");
	 fprintf(fout, "\tcall	__isoc99_scanf@PLT\n");
	 fprintf(fout, "\tmovzbl	myCharTemp(%%rip), %%eax\n");
	 fprintf(fout, "\tmovsbl	%%al, %%eax\n");
	 fprintf(fout, "\tmovl	%%eax, _%s(%%rip)\n", varName);
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
    else if (op->type == SYMBOL_VARIABLE || op->type == SYMBOL_USED_LOCAL_VARIABLE)
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
void processCopyVec(TAC* tac, FILE* fout) 
{
    processOperandToFloatRegister(tac->op1, ALTERNATIVE_REGISTER, fout);
    processOperandToFloatRegister(tac->op2, DEFAULT_REGISTER, fout);
    saveFloatRegisterToVec(tac->res->text, isFloatVariable(tac->res->data_type), fout);
}


void testAndZeroOperator(HASH_NODE* op, FILE* fout)
{
    if (op->type == SYMBOL_VARIABLE || op->type == SYMBOL_USED_LOCAL_VARIABLE)
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

void processReturn(TAC* tac, FILE* fout)
{
    processOperandToFloatRegister(tac->res, DEFAULT_REGISTER, fout);
    jump(actualFunctionLabel, fout);
}

void processFuncArg(TAC* tac, FILE* fout)
{
    fprintf(fout, "\t# processFuncArg\n");
    processOperandToFloatRegister(tac->res, DEFAULT_REGISTER, fout);
    floatRegisterToStack(DEFAULT_REGISTER, fout);

    if (parameters_names == NULL)
    {
        parameters_names = createValuesListNode(tac->res->text);
    }
    else
    {
        insertValueinList(parameters_names, tac->res->text);
    }

    
    
    HASH_NODE* srcNode = hashFind(tac->res->text);
    HASH_NODE* dstNode = hashFind(get_scope_var_name_at_index(actual_func_node, actual_func_index+1));

    processOperandToFloatRegister(srcNode, DEFAULT_REGISTER, fout);
    if (isFloatVariable(dstNode->data_type))
    {
        saveFloatRegisterToFloatVar(dstNode->text, fout);
    }
    else
    {
        saveFloatRegisterToIntVar(dstNode->text, fout);
    }
    
    fprintf(fout, "\t# End processFuncArg\n");
    actual_func_index++;
}

void processBeginArg(TAC* tac, FILE* fout)
{
    actual_func_node = hashFind(tac->res->text);
    actual_func_params_count = get_scope_len(actual_func_node);
    actual_func_index = 0;
    allocateStackSpace(fout);
}

char* getActualParamName(TAC* tac, int i, FILE* fout)
{
    if (i == 0)
    {
        return parameters_names->value;
    }
    
    else
    {
        int idx = 1;
        VALUES_LIST* iterator = parameters_names->next;
        while (idx < i)
        {
            iterator = iterator->next;
            idx++;
        }
        return iterator->value;
    }
}

void restoreStackVariable(HASH_NODE* node, FILE* fout)
{
    stackPositionToFloatRegister(actual_func_index, fout);

    if (isFloatVariable(node->data_type))
    {
        fprintf(fout, "# entrei float\n");
        saveFloatRegisterToFloatVar(node->text, fout);
    }
    else
    {
        saveFloatRegisterToIntVar(node->text, fout);
    }
}
void restoreArgvVariables(TAC* tac, FILE* fout)
{
    int i;
    HASH_NODE* node;
    actual_func_index = 0;
    for (i = 0; i < actual_func_params_count; i++)
    {
        node = hashFind(getActualParamName(tac, i, fout));
        if ( (node->type == SYMBOL_VARIABLE || node->type == SYMBOL_USED_LOCAL_VARIABLE) && node->data_type != DATATYPE_UNDEFINED)
        {
            restoreStackVariable(node, fout); 
        }
        actual_func_index++;
        
    }
}

void processFuncCall(TAC* tac, FILE* fout)
{
    callFunction(tac->op1->text, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);

    // This is not necessary and even impossible due to semantic verification does not allow variables and/or parameters with same name 
    //restoreArgvVariables(tac, fout);
    // It should be used in case we accept global variables with same name as parameters or parameters of differente funcs with same name
}

void processRead(TAC* tac, FILE* fout)
{
    if (tac->res->data_type == DATATYPE_CHAR)
    {
        readCharToVariable(tac->res->text, fout);
    }

    else
    {
        readToVariable(tac->res->text, fout);
        if (isIntVariable(tac->res->data_type) || tac->res->data_type == DATATYPE_BOOL)
        {
            floatVarToIntVar(tac->res->text, tac->res->text, DEFAULT_REGISTER, fout);
        }
    }
    
    
}

void processVectorAccess(TAC* tac, FILE* fout)
{
    processOperandToFloatRegister(tac->op2, DEFAULT_REGISTER, fout);
    floatRegisterToIntRegister(DEFAULT_REGISTER, fout);
    floatVecToFloatRegister(tac->op1->text, DEFAULT_REGISTER, fout);
    saveFloatRegisterToFloatVar(tac->res->text, fout);
    // TODO in case print chars
    // if tac->res->data_type == DATATYPE_CHAR
    // floatRegisterToCharVar()
    // tac->res->data_type = tac->op1->data_type;
    // Ajustar print DATATYPE_CHAR
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
        
        case DATATYPE_UNDEFINED: // Variáveis temporárias
        case DATATYPE_FLOAT:
            floatVarToFloatRegister(tac->res->text, 0, fout);
            fprintf(fout, "\tcvtss2sd	%%xmm0, %%xmm0\n");
            callPrintFunction(".print_float_string", fout);
        break;
    }
}

void printLiteral(TAC* tac, FILE* fout)
{
    HASH_NODE* node = hashFind(tac->res->text);
    long int actualID = actualLiteralLabel;
    setId(node, &actualLiteralLabel);
    if (actualID == actualLiteralLabel)
    {
        callPrintLiteral(node->id, fout);
        return;
    }
    
    if (literals_to_print == NULL)
    {
        literals_to_print = createValuesListNode(tac->res->text);
    }
    else
    {
        insertValueinList(literals_to_print, tac->res->text);
    }

    callPrintLiteral(actualLiteralLabel-1, fout);
}

void generateNormalVariable(HASH_NODE* node,FILE* fout)
{
    if (isNumberType(node->data_type))
    {
        if (isCharValue(node->init_value))
        {
            if (isIntVariable(node->data_type))
            {
                fprintf(fout, "_%s:\t.long\t%i\n", node->text, node->init_value[1]);
            }
            else
            {
                float floatValue = node->init_value[1];
                long int value = *(int*) &floatValue;
                fprintf(fout, "_%s:\t.long\t%li\n", node->text, value);
            }
        }
        
        else if (!isFloatValue(node->init_value))
        {
            if (isIntVariable(node->data_type))
            {
                long int init_value = strtol(node->init_value, NULL, 16);
                fprintf(fout, "_%s:\t.long\t%li\n", node->text, strtol(node->init_value, NULL, 16));
            }
            else
            {
                float floatValue = strtol(node->init_value, NULL, 16);
                long int value = *(int*) &floatValue;
                fprintf(fout, "_%s:\t.long\t%li\n", node->text, value);
            }
        }

        else
        {  
            if (isIntVariable(node->data_type))
            {
                int floatHexinInt = castFloatHexStringToInt(node->init_value);
                float floatValue = *(float*) &floatHexinInt;
                int value = floatValue;
                fprintf(fout, "_%s:\t.long\t%d\n", node->text, value);
            }
            else
            {
                int floatHexinInt = castFloatHexStringToInt(node->init_value);
                fprintf(fout, "_%s:\t.long\t%d\n", node->text, floatHexinInt);
            }
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

void generateGlobalStrings(FILE* fout)
{
    fprintf(fout, "\n# String literals to be printed\n");
    VALUES_LIST* iterator;
    for (iterator = literals_to_print; iterator; iterator = iterator->next)
    {
        long int id = hashFind(iterator->value)->id;

        if (iterator->value[0] != '\"')
        {
            fprintf(fout, ".literal_value%li:\t.string	\"%s\"\n", id, iterator->value);
        }
        else
        {
            fprintf(fout, ".literal_value%li:\t.string	%s\n", id, iterator->value);
        }
        fprintf(fout, "literal%li:\t.quad\t.literal_value%li\n", id, id);
    }
}

void generateVectorVariable(HASH_NODE* node, FILE* fout)
{
    if(node->idx == 0) return;
    int i;
    long int size = node->idx;
    
    fprintf(fout, "_%s:\n", node->text);
    for (i = 0; i < size; i++)
    {
        float vec_value_float = strtol(get_vec_value_at_index(node, i), NULL, 16);
        long int vec_value_int = *(int*) &vec_value_float;
        fprintf(fout, "\t.long	%li\n", vec_value_int);
    }
}

void generateGlobalVariables(FILE* fout)
{
    fprintf(fout, "\n# Global variables\n");
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
            else if (node->type == SYMBOL_VECTOR)
            {
                generateVectorVariable(node, fout);                                
            }
            else if (node->type == SYMBOL_USED_LOCAL_VARIABLE)
            {
                fprintf(fout, "_%s:\t.long\t%i\n", node->text, 0);
            }
        }
    }

    // Print own variables for easier manipulation
    fprintf(fout, "floatTemp:\t.long\t0\n");
    fprintf(fout, "_0myIntTemp:\t.long\t0\n");
    fprintf(fout, "myCharTemp:\t.byte\t0\n");
    fprintf(fout, ".true_string:	.string	\"TRUE\"\n");
    fprintf(fout, ".false_string:	.string	\"FALSE\"\n");
    fprintf(fout, ".true:	.quad	.true_string\n");
    fprintf(fout, ".false:	.quad	.false_string\n");
    fprintf(fout, ".scanf_string:   .string	\"%%f\"\n");
    fprintf(fout, ".scanf_char:    .string  \"%%c\"\n");


    fprintf(fout, "\n");
}

void generateASM(TAC* first)
{
    TAC* tac;

    FILE *fout;
    fout = fopen("my_assembly.s", "w");

    // Init headers
    fprintf(fout, "# Data section\n");
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
                actualFunctionLabel = actualLabel++;
                fprintf(fout, "%s:\n", tac->res->text);
                if (strcmp(tac->res->text, "main") == 0)
                {
                    isMainFunction = 1;
                }
                
                fprintf(fout, "\tpushq	%%rbp\n");
                fprintf(fout, "\tmovq	%%rsp, %%rbp\n\n");
                break;

            case TAC_ENDFUN:
                label(actualFunctionLabel, fout);
                if (strcmp(tac->res->text, "main") == 0)
                {
                    isMainFunction = 0;
                    floatRegisterToIntRegister(DEFAULT_REGISTER, fout);
                    fprintf(fout, "\tleave\n");
                }
                else
                {
                    fprintf(fout, "\tpopq	%%rbp\n");
                }
                
                fprintf(fout, "\tret\n");
                break;

            case TAC_PRINT:
                if (tac->res->type == SYMBOL_VARIABLE || tac->res->type == SYMBOL_USED_LOCAL_VARIABLE)
                {
                    printVariable(tac, fout);
                }
                else
                {
                    printLiteral(tac, fout);
                }
                
                    
                break;

            case TAC_COPY:
                processCopy(tac, fout);
                printWhiteLine(fout);
                break;

            case TAC_COPY_VEC:
                processCopyVec(tac, fout);
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

            case TAC_READ:
                processRead(tac, fout);
                break;

            case TAC_RETURN:
                processReturn(tac, fout);
                break;

            case TAC_FUNCALL:
                processFuncCall(tac, fout);
                break;

            case TAC_VEC_ACCESS:
                processVectorAccess(tac, fout);
                break;

            case TAC_BEGIN_ARG:
                processBeginArg(tac, fout);
                break;

            case TAC_FUNC_ARG:
                processFuncArg(tac, fout);
                break;

            default:
                break;
        }
    }

    // String literal to be print
    generateGlobalStrings(fout);

    fclose(fout);
    }