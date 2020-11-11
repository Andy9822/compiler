#ifndef TYPEINFERENCE_HEADER
#define TYPEINFERENCE_HEADER

#include "ast.h"
#endif // !1

int getDatatypeFromSymbol(int type);
int getDatatypeFromLiteral(int type);
int isNumberType(int type);
int isBoolType(int type);
int compatibleTypes(int var_type, int literal_type);

int infer_type(AST * node);