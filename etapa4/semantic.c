#include "semantic.h"
#include "hash.h"

int SemanticErrors = 0;

int getDatatypeFromSymbol(int type)
{
    if (type == SYMBOL_KW_INT)
    {   
        // printf("got INT\n");
        return DATATYPE_INT;
    }
    
    if (type == SYMBOL_KW_FLOAT)
    {
        // printf("got float\n");        
        return DATATYPE_FLOAT;
    }
    
    if (type == SYMBOL_KW_BOOL)
    {
        // printf("got bool\n");
        return DATATYPE_BOOL;
    }
    
    if (type == SYMBOL_KW_CHAR)
    {
        // printf("got bool\n");
        return DATATYPE_CHAR;
    }

    return 0;
}

int getDatatypeFromLiteral(int type)
{
    if (type == SYMBOL_LIT_INTEGER)
    {   
        return DATATYPE_INT;
    }
    
    if (type == SYMBOL_LIT_FLOAT)
    {        
        return DATATYPE_FLOAT;
    }
    
    if (type == SYMBOL_LIT_CHAR)
    {
        return DATATYPE_CHAR;
    }

    if (type == SYMBOL_LIT_TRUE || type == SYMBOL_LIT_FALSE)
    {
        return DATATYPE_BOOL;
    }
    

    return 0;
}

int isNumberType(int type)
{
    if (type == DATATYPE_CHAR || type == DATATYPE_INT ||type == DATATYPE_FLOAT)
    {
        return 1;
    }
    
    return 0;
}

int isBoolType(int type)
{
    if (type == DATATYPE_BOOL)
    {
        return 1;
    }
    
    return 0;
}

int compatibleTypes(int var_type, int literal_type)
{
    if (isNumberType(var_type) && isNumberType(literal_type))
    {
        return 1;
    }
    
    if (isBoolType(var_type) && isBoolType(literal_type))
    {
        return 1;
    }

    return 0;
    
}

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
        case AST_VEC_DEC:
            if (node->son[0]->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: variable %s has already been declared\n", node->son[0]->son[0]->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->son[0]->symbol->type = SYMBOL_VARIABLE;
            node->son[0]->son[0]->symbol->data_type = getDatatypeFromSymbol(node->son[0]->son[1]->symbol->type);
            break;

        case AST_VEC_INIT:
            if (node->son[0]->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: function %s has already been declared\n", node->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->son[0]->symbol->type = SYMBOL_VECTOR;
            break;

        case AST_FUNC_VOID_DEC:
        case AST_FUNC_PARAMS_DEC:
            if (node->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: function %s has already been declared\n", node->son[0]->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->symbol->type = SYMBOL_FUNCTION;
            node->son[0]->symbol->data_type = getDatatypeFromSymbol(node->son[1]->symbol->type);;
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

void validate_AST_VARDEC(AST * node)
{
    int var_type = node->son[0]->son[0]->symbol->data_type;
    int literal_type = getDatatypeFromLiteral(node->son[1]->symbol->type);
    if (!compatibleTypes(var_type, literal_type))
    {
        printf("Semantic Error: variable %s has invalid type declaration\n", node->son[0]->son[0]->symbol->text);
        // printf("vartype : %d\n", var_type);
        // printf("literal_type %s = %d\n", node->son[1]->symbol->text, literal_type);
        SemanticErrors++;
    }
}

void validate_AST_VEC_INIT(AST * node)
{
    // int var_type = node->son[0]->son[0]->symbol->data_type;
    // printf("vetor de tipo: %d \n", var_type);
    int var_type = getDatatypeFromSymbol(node->son[0]->son[1]->symbol->type);

    int len = strtol(node->son[1]->symbol->text, NULL, 16);
    int actual_len = 0;

    AST * values_subtree = node->son[2];
    while (values_subtree)
    {
        if (!compatibleTypes(var_type, getDatatypeFromLiteral(values_subtree->son[0]->symbol->type)))
        {
            SemanticErrors++;
            printf("Semantic Error: invalid values type in vector %s initialization\n", node->son[0]->son[0]->symbol->text);
            return;
            break;
        }
        actual_len++;
        values_subtree = values_subtree->son[1];
    }

    if (len != actual_len)
    {
        SemanticErrors++;
        printf("Semantic Error: number of initialization values does not match len %d of vector %s\n", len, node->son[0]->son[0]->symbol->text);
    }
}

void check_operands(AST* node)
{
    int i;
    if (node == 0) return;

    switch (node->type)
    {
        case AST_VARDEC:
            validate_AST_VARDEC(node);
            break;

        case AST_VEC_INIT:
            validate_AST_VEC_INIT(node);
            break;

    
    }
    for (i = 0; i < MAX_SONS; i++)
    {
        check_operands(node->son[i]);
    }
}
