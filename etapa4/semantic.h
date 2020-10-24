#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "ast.h"

#endif // !1

#include "type_inference.h"

unsigned long long int SemanticErrors;

void check_and_set_declarations(AST* node);
void check_undeclared();
void check_operands(AST* node);
