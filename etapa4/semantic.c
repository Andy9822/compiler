#include "semantic.h"

int SemanticErrors = 0;

void check_and_set_declarations(AST *node)
{
    int i;
    if (node == 0)
    {
        return;
    }

    
    // printf("vou switchar node->type: %d\n", node->type);
    // printf("node->type = %d\n", node->type);
    // printf("AST_VARDEC = %d\n", AST_VARDEC);
    // fflush(stdout);
    switch (node->type)
    {
        case AST_VARDEC:
            if(node->son[0]->son[0]->symbol) 
            {
                if (node->son[0]->son[0]->symbol->type != SYMBOL_IDENTIFIER)
                {
                    fprintf(stderr, "Semantic Error: variable %s already declared\n", node->son[0]->son[0]->symbol->text);
                    ++SemanticErrors;
                }
            }
            node->son[0]->son[0]->symbol->type = SYMBOL_VARIABLE;
            break;

        case AST_VEC_DEC:
        case AST_VEC_INIT:
            if (node->symbol->type != SYMBOL_IDENTIFIER)
            {
                fprintf(stderr, "Semantic Error: function %s already declared\n", node->symbol->text);
                ++SemanticErrors;
            }
            node->symbol->type = SYMBOL_VECTOR;
            break;

        case AST_FUNC_VOID_DEC:
        case AST_FUNC_PARAMS_DEC:
            if (node->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                fprintf(stderr, "Semantic Error: function %s already declared\n", node->son[0]->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->symbol->type = SYMBOL_FUNCTION;
            break;

        default:
            // printf("default aqui");
            break;
    }

    for (i = 0; i < MAX_SONS; i++)
    {
        check_and_set_declarations(node->son[i]);
    }
}

void check_undeclared()
{
    SemanticErrors += hash_check_undeclared();
}
