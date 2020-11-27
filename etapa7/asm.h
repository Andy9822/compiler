#ifndef ASM_HEADER
#define ASM_HEADER

#include "tacs.h"
#include "type_inference.h"

#define isFloatVariable(x) (x == DATATYPE_UNDEFINED || x == DATATYPE_FLOAT)
#define isIntVariable(x) (x == DATATYPE_INT || x == DATATYPE_CHAR)
#define isIntLiteral(x) (x == SYMBOL_LIT_INTEGER || x == SYMBOL_LIT_CHAR)
#define isBoolLiteral(x) (x == SYMBOL_LIT_TRUE || x == SYMBOL_LIT_FALSE)
#define DATATYPE_TEMP_FLOAT DATATYPE_UNDEFINED
#define FLOAT_TEMP_VAR "floatTemp"
#define DEFAULT_REGISTER 0
#define ALTERNATIVE_REGISTER 1

void generateASM(TAC* first);

int castFloatHexStringToInt(char* string);

#endif // MACRO
