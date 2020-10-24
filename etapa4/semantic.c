#include "semantic.h"
#include "hash.h"

unsigned long long int SemanticErrors = 0;

void check_and_set_declarations(AST *node)
{
    int i;
    if (node == 0)
    {
        return;
    }

    switch (node->type)
    {
        case AST_VARDEC:
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
        case AST_VEC_DEC:
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

void validate_ATRIBUITION(AST * node)
{
    int var_symbol_type = node->son[0]->symbol->type;

    if (var_symbol_type == SYMBOL_VARIABLE)
    {
        int var_type = node->son[0]->symbol->data_type;
        int expression_type = infer_type(node->son[1]);

        if (!compatibleTypes(var_type, expression_type))
        {
            SemanticErrors++;
            printf("Semantic Error: invalid value type for %s atribuition \n", node->son[0]->symbol->text); 
        }
    }

    else if (var_symbol_type == SYMBOL_VECTOR)
    {
        SemanticErrors++;
        printf("Semantic Error: variable %s must be accessed as a vector \n", node->son[0]->symbol->text);
    }
    else 
    {
        SemanticErrors++;
        printf("Semantic Error: identifier %s does not belong to a variable \n", node->son[0]->symbol->text);
    }
}

void validate_AST_ADD_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    
    if ( !isNumberType(left_type) || !isNumberType(right_type) )
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operands in ADD-like operation\n");
    }
}

void validate_AST_OR_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    
    if ( !isBoolType(left_type) || !isBoolType(right_type) )
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operands in OR-like operation\n");
    }
}

void validate_AST_NOT(AST * node)
{
    int expression_type = infer_type(node->son[0]);
    
    if ( !isBoolType(expression_type))
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operand in AST_NOT \n");
    }
}

void validate_AST_MINUS(AST * node)
{
    int expression_type = infer_type(node->son[0]);
    
    if ( !isNumberType(expression_type))
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operand in AST_MINUS \n");
    }
}

void validate_AST_DIF_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
}

void validate_AST_PARENTHESIS(AST * node)
{
    int expression_type = infer_type(node);
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

        case AST_ADD:
        case AST_SUB:
        case AST_DIV:
        case AST_MULT:
        case AST_GREATER:
        case AST_LESSER:
        case AST_GE:
        case AST_LE:
            validate_AST_ADD_like(node);
            break;
        
        case AST_NOT:
            validate_AST_NOT(node);
            break;
        
        case AST_MINUS:
            validate_AST_MINUS(node);
            break;

        case AST_OR:
        case AST_AND:
            validate_AST_OR_like(node);
            break;

        case AST_DIF:
        case AST_EQ:
            validate_AST_DIF_like(node);
            break;

        case AST_PARENTHESIS:
            validate_AST_PARENTHESIS(node);
            break;

        case AST_ATRIBUITION:
            validate_ATRIBUITION(node);
            break;
    
    }
    for (i = 0; i < MAX_SONS; i++)
    {
        check_operands(node->son[i]);
    }
}
