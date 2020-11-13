#ifndef ASM_HEADER
#define ASM_HEADER

#include "tacs.h"
#include "type_inference.h"

#define isFloatVariable(x) (x == DATATYPE_UNDEFINED || x == DATATYPE_FLOAT)
#define isIntVariable(x) (x == DATATYPE_INT || x == DATATYPE_CHAR)
#define isIntLiteral(x) (x == SYMBOL_LIT_INTEGER || x == SYMBOL_LIT_CHAR)
#define DATATYPE_TEMP_FLOAT DATATYPE_UNDEFINED
#define FLOAT_TEMP_VAR "floatTemp"

void generateASM(TAC* first);

#endif // MACRO
