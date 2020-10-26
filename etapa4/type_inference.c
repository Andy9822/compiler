#include "type_inference.h"
#include "ast.h"
#include "hash.h"

unsigned long long int SemanticErrors;

int getDatatypeFromSymbol(int type)
{
    if (type == SYMBOL_KW_CHAR)
    {
        // printf("got bool\n");
        return DATATYPE_CHAR;
    }

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

int infer_type_AST_SYMBOL(AST * node)
{
    if (node->symbol->type == SYMBOL_VARIABLE || node->symbol->type == SYMBOL_VECTOR || node->symbol->type == SYMBOL_FUNCTION)
    {
        return node->symbol->data_type;
    }

    if (SYMBOL_LIT_INTEGER <= node->symbol->type && node->symbol->type <= SYMBOL_LIT_FALSE)
    {
        return getDatatypeFromLiteral(node->symbol->type);
    }

    printf("Semantic Error: symbol %s with invalid type \n", node->symbol->text);
    SemanticErrors++;
    return DATATYPE_ERROR;
}

int infer_type_AST_AST_PARENTHESIS(AST * node)
{
    return infer_type(node->son[0]);
}

int infer_vector_access(AST * node)
{
    int vector_type = infer_type(node->son[0]);
    return vector_type;
}

int infer_AST_GREATER_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    
    if (isNumberType(left_type) && isNumberType(right_type))
    {
        return DATATYPE_BOOL;
    }
     
    SemanticErrors++;
    return DATATYPE_ERROR;
}

int infer_type_ADD_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    
    if (isNumberType(left_type) && isNumberType(right_type))
    {
        int greaterType = (left_type > right_type) ? left_type : right_type;
        return greaterType;
    }
     
    SemanticErrors++;
    return DATATYPE_ERROR;
}

int infer_AST_OR_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    
    if (isBoolType(left_type) && isBoolType(right_type))
    {
        return DATATYPE_BOOL;
    }

    SemanticErrors++;
    return DATATYPE_ERROR;
}

int infer_AST_NOT(AST * node)
{
    int expression_type = infer_type(node->son[0]);
    if (isBoolType(expression_type))
    {
        return DATATYPE_BOOL;
    }

    SemanticErrors++;
    return DATATYPE_ERROR;
}

int infer_EQ_like(AST * node)
{
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);
    return DATATYPE_BOOL;
}


int infer_FUNC_CALL(AST * node)
{
    int identifier_datatype = infer_type(node->son[0]);
    return identifier_datatype;
}

int infer_type(AST * node)
{
    // printf("entrei infer_type \n");
    // printf("to aqui guirzada com type = %d \n", node->type);
    switch (node->type)
    {
        case AST_SYMBOL:
            return infer_type_AST_SYMBOL(node);
            break;

        case AST_PARENTHESIS:
            return infer_type_AST_AST_PARENTHESIS(node);
            break;
        
        case AST_ADD:
        case AST_SUB:
        case AST_DIV:
        case AST_MULT:
            return infer_type_ADD_like(node);
            break;

        case AST_OR:
        case AST_AND:
            infer_AST_OR_like(node);
            break;

        
        case AST_GREATER:
        case AST_LESSER:
        case AST_GE:
        case AST_LE:
            infer_AST_GREATER_like(node);
            break;

        case AST_NOT:
            infer_AST_NOT(node);
            break;

        case AST_VECTOR_ACCESS:
            infer_vector_access(node);
            break;
            
        case AST_DIF:
        case AST_EQ:
            return infer_EQ_like(node);
            break;

        case AST_FUNC_CALL:
        case AST_FUNC_PARAMS_CALL:
            return infer_FUNC_CALL(node);
            break;

        default:
            break;
    }
}