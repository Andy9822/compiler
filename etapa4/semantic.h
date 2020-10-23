#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#endif // !1

#include "ast.h"

extern int SemanticErrors;

void check_and_set_declarations(AST* node);
void check_undeclared();
