#include "semantic.h"
#include "hash.h"
#include "ast.h"

unsigned long long int SemanticErrors = 0;

void process_local_variable(AST * param_node, AST * func_node)
{
    //Set parameter identifier as local_variable
    if (param_node->son[0]->symbol->type == SYMBOL_LOCAL_VARIABLE)
    {
        printf("Semantic Error: local variable %s has already been declared in other function\n", param_node->son[0]->symbol->text);
        ++SemanticErrors;
    }

    if (param_node->son[0]->symbol->type == SYMBOL_VARIABLE)
    {
        printf("Semantic Error: parameter %s has already been declared as variable\n", param_node->son[0]->symbol->text);
        ++SemanticErrors;
    }
    
    param_node->son[0]->symbol->type = SYMBOL_LOCAL_VARIABLE;
    // Save identifier in function's scope variables list
    char* var_name = param_node->son[0]->symbol->text;
    int var_type = param_node->son[1]->symbol->type;
    // printf("vou inserir local var %s de tipo %d \n",var_name, var_type);
    insert_local_variable(func_node->symbol, var_name, var_type);
}

void init_zero_vec_dec(HASH_NODE* hash_node, char* size)
{
    int i;
    long int vec_size = strtol(size, NULL, 16);

    for (i = 0; i < vec_size; i++)
    {
        insert_vec_value(hash_node, "0");
    }
    
}

void fill_vec_dec_values(HASH_NODE* hash_node, char* size, AST* value_node)
{
    long int vec_size = strtol(size, NULL, 16);
    hash_node->idx = vec_size;
    while (value_node)
    {
        insert_vec_value(hash_node, value_node->son[0]->symbol->text);
        value_node = value_node->son[1];
    }
}

void check_and_set_declarations(AST *node)
{
    int i;
    if (node == 0)
    {
        return;
    }


    // printf("entrei check and set declarations \n");
    // printf("type %d \n", node->type);
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
            node->son[0]->son[0]->symbol->init_value = node->son[1]->symbol->text;
            
            break;

        case AST_VEC_INIT:
            fill_vec_dec_values(node->son[0]->son[0]->symbol, node->son[1]->symbol->text, node->son[2]);
            // TODO percorrer AST_VEC_INIT_VALUES e colocar somehow no HASH_NODE
        case AST_VEC_DEC:
            if (node->son[0]->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: variable %s has already been declared\n", node->son[0]->son[0]->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->son[0]->symbol->type = SYMBOL_VECTOR;
            node->son[0]->son[0]->symbol->data_type = getDatatypeFromSymbol(node->son[0]->son[1]->symbol->type);
            if (node->type == AST_VEC_DEC)
            {
                init_zero_vec_dec(node->son[0]->son[0]->symbol, node->son[1]->symbol->text);
            }
            
            break;


        case AST_FUNC_VOID_DEC:
            if (node->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: function %s has already been declared\n", node->son[0]->symbol->text);
                ++SemanticErrors;
            }
            node->son[0]->symbol->type = SYMBOL_FUNCTION;
            node->son[0]->symbol->data_type = getDatatypeFromSymbol(node->son[1]->symbol->type);
            break;

        case AST_FUNC_PARAMS_DEC:;
            AST * func_node = node->son[0];
            char* func_name = func_node->symbol->text;

            if (node->son[0]->symbol->type != SYMBOL_IDENTIFIER)
            {
                printf("Semantic Error: function %s has already been declared\n", func_name);
                ++SemanticErrors;
            }

            AST * parameter_node = node->son[1];
            AST * parameter_list_node = node->son[2];
            process_local_variable(parameter_node, func_node);
            while (parameter_list_node)
            {
                parameter_node = parameter_list_node->son[0];
                process_local_variable(parameter_node, func_node);
                parameter_list_node = parameter_list_node->son[1];

            }
            // printf("Função %s tem: \n", func_name);
            // print_scope_variables(func_node->symbol);
            // printf("Função %s eh do tipo: \n", getDatatypeFromSymbol(node->son[3]->symbol->type));
            node->son[0]->symbol->type = SYMBOL_FUNCTION;
            node->son[0]->symbol->data_type = getDatatypeFromSymbol(node->son[3]->symbol->type);
            break;

        default:
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

    int len = atoi(node->son[1]->symbol->text);
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
        // Check expression
        check_operands(node->son[1], 1);
        int var_type = node->son[0]->symbol->data_type;
        int expression_type = infer_type(node->son[1]);
        // printf("expression ast type : %d  \n", node->son[1]->type);
        // printf("expression : %s  \n", node->son[1]->symbol->text);
        // printf("expression symbol type : %d  \n", node->son[1]->symbol->type);

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

void validate_ATRIBUITION_VEC(AST * node)
{
    int var_symbol_type = node->son[0]->symbol->type;

    if (var_symbol_type == SYMBOL_VECTOR)
    {
        check_operands(node->son[1], 1);
        check_operands(node->son[2], 1);

        int var_type = node->son[0]->symbol->data_type;
        int index_type = infer_type(node->son[1]);
        int expression_type = infer_type(node->son[2]);

        if (index_type != DATATYPE_CHAR && index_type != DATATYPE_INT)
        {
            SemanticErrors++;
            printf("Semantic Error: invalid index type for vector %s atribuition \n", node->son[0]->symbol->text);
        }
        
        else if (!compatibleTypes(var_type, expression_type))
        {
            SemanticErrors++;
            printf("Semantic Error: invalid value type for %s atribuition \n", node->son[0]->symbol->text); 
        }
    }

    else if (var_symbol_type == SYMBOL_VARIABLE)
    {
        SemanticErrors++;
        printf("Semantic Error: variable %s can't be accessed as a vector \n", node->son[0]->symbol->text);
    }

    else 
    {
        SemanticErrors++;
        printf("Semantic Error: identifier %s does not belong to a vector \n", node->son[0]->symbol->text);
    }
}

void validate_AST_ADD_like(AST * node)
{
    check_operands(node->son[0], 1);
    check_operands(node->son[1], 1);

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
    check_operands(node->son[0], 1);
    check_operands(node->son[1], 1);

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
    check_operands(node->son[0], 1);
    int expression_type = infer_type(node->son[0]);
    
    if ( !isBoolType(expression_type))
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operand in AST_NOT \n");
    }
}

void validate_AST_MINUS(AST * node)
{
    check_operands(node->son[0], 1);
    int expression_type = infer_type(node->son[0]);
    
    if ( !isNumberType(expression_type))
    {
        SemanticErrors++;
        printf("Semantic Error: invalid type operand in AST_MINUS \n");
    }
}

void validate_AST_DIF_like(AST * node)
{
    check_operands(node->son[0], 1);
    check_operands(node->son[1], 1);
    int left_type = infer_type(node->son[0]);
    int right_type = infer_type(node->son[1]);    
}

void validate_VECTOR_ACCESS(AST * node)
{
    int identifier_type = node->son[0]->symbol->type;
    if (identifier_type != SYMBOL_VECTOR)
    {
        printf("Semantic Error: forbidden access to %s with index, only allowed for vectors \n", node->son[0]->symbol->text);
        SemanticErrors++;
        return;
    }
    
    int vector_type = infer_type(node->son[0]);
    int index_type = infer_type(node->son[1]);

    
    if (index_type == DATATYPE_CHAR || index_type == DATATYPE_INT)
    {
        return;
    }
    
    printf("Semantic Error: using invalid type as index for acccessing %s \n", node->son[0]->symbol->text);
    SemanticErrors++;
}

void validate_AST_PARENTHESIS(AST * node)
{
    check_operands(node->son[0], 1);
    int expression_type = infer_type(node);
}

void validate_IF_like(AST * node)
{
    check_operands(node->son[0], 1);
    int expression_type = infer_type(node->son[0]);

    if (expression_type != DATATYPE_BOOL)
    {
        SemanticErrors++;
        printf("Semantic Error: if clausule needs to be a boolean expression\n");
    }
    
}

void validate_FUNC_CALL(AST * node)
{
    int identifier_type = node->son[0]->symbol->type;
    if (identifier_type != SYMBOL_FUNCTION)
    {
        SemanticErrors++;
        printf("Semantic Error: forbid to call identifier %s(), only allowed for functions\n", node->son[0]->symbol->text);
    }
    
    else if (node->son[0]->symbol->scope_variables != NULL)
    {
        SemanticErrors++;
        printf("Semantic Error: function %s expects arguments\n", node->son[0]->symbol->text);
    }
    
}
void validate_FUNC_PARAMETERS_CALL(AST * node)
{
    int identifier_type = node->son[0]->symbol->type;
    if (identifier_type != SYMBOL_FUNCTION)
    {
        SemanticErrors++;
        printf("Semantic Error: forbid to call identifier %s(...), only allowed for functions\n", node->son[0]->symbol->text);
        return;
    }
    AST * func_node = node->son[0];

    if (func_node->symbol->scope_variables == 0)
    {
        SemanticErrors++;
        printf("Semantic Error: function %s() doesn't expect any parameter\n", node->son[0]->symbol->text);
        return;
    }

    char* func_name = node->son[0]->symbol->text;
    // printf("validando %s \n", func_name);
    
    // printf("func name %s \n", func_node->symbol->text);
    
    int expected_len = get_scope_len(func_node->symbol); 
    // printf("expected_len : %d\n", expected_len);

    int avaliated_params = 0;
    check_operands(node->son[1], 1);
    int param_type = infer_type(node->son[1]);
    int expected_param_type = getDatatypeFromSymbol(get_scope_index(func_node->symbol, 0));
    // printf("expected first param type %d \n", expected_param_type);
    // printf("first param type %d \n", param_type);

    if (!compatibleTypes(param_type, expected_param_type))
    {
        SemanticErrors++;
        printf("Semantic Error: function %s receiving invalid parameter type\n", func_name);
        return;
    }
    avaliated_params+=1;

    AST * params_list_node = node->son[2];
    if (avaliated_params == expected_len)
    {
        if (params_list_node == NULL)
        {
            return;
        }

        else
        {
            SemanticErrors++;
            printf("Semantic Error: function %s receiving more parameters than expected\n", func_name);
            return;
        }
    }

    if (params_list_node == NULL)
    {
        SemanticErrors++;
        printf("Semantic Error: function %s receiving less parameters than expected\n", func_name);
        return;
    }
    
    while (1)
    {
        // printf("ast type %d \n", params_list_node->type);
        expected_param_type = getDatatypeFromSymbol(get_scope_index(func_node->symbol, avaliated_params+1));
        param_type = infer_type(params_list_node->son[0]);
        check_operands(params_list_node->son[0], 1);
        // printf("expected param %d has type %d \n", avaliated_params+1, expected_param_type);
        // printf("%d parameter %s has type %d \n", avaliated_params+1, params_list_node->son[0]->symbol->text, param_type);
        
        if (!compatibleTypes(param_type, expected_param_type))
        {
            SemanticErrors++;
            printf("Semantic Error: function %s receiving invalid parameter type\n", func_name);
            return;
        }

        avaliated_params+=1;
        if (avaliated_params == expected_len)
        {
            break;
        }
        
        params_list_node = params_list_node->son[1];
        
        if (params_list_node == NULL)
        {
            SemanticErrors++;
            printf("Semantic Error: function %s receiving less parameters than expected\n", func_name);
            return;
        }
        
    }
    
    if (params_list_node->son[1] != NULL)
    {
        SemanticErrors++;
        printf("Semantic Error: function %s receiving more parameters than expected\n", func_name);
        return;
    }

}

void validate_FUNC_PARAMETERS_DEC(AST * node)
{
    int num_variables = get_scope_len(node->son[0]->symbol);
    int variables_counter = 1;

    //Set all scope identifiers as valid variables
    // hashPrint();
    // printf(" \n");
    while ( variables_counter <= num_variables)
    {
        char* param_name = get_scope_var_name_at_index(node->son[0]->symbol, variables_counter++); // TODO fix that shit
        HASH_NODE* param_node = hashFind(param_name);
        // printf("parameter %s type %d \n", param_name, param_node->type);
        if (param_node->type != SYMBOL_VARIABLE && param_node->type != SYMBOL_USED_LOCAL_VARIABLE) //TODO this is a workaround for the TODO above
        {
            param_node->data_type = getDatatypeFromSymbol(param_node->type);
            param_node->type = SYMBOL_VARIABLE;
        }
        else if (param_node->type == SYMBOL_USED_LOCAL_VARIABLE)
        {
            SemanticErrors++;
            printf("Semantic Error: local variable %s has already been declared in other function\n", param_name);
        }
        else
        {
            SemanticErrors++;
            printf("Semantic Error: parameter %s already declared as a variable\n", param_name);
        }
        
    }
    // hashPrint();
    // printf(" \n");
    int i;
    for (i = 0; i < MAX_SONS; i++)
    {
        check_operands(node->son[i], 0);
    }

    //Set back all scope identifiers to local_variables, meaning they're not valid variables to be used anymore
    variables_counter = 1;
    while ( variables_counter <= num_variables)
    {
        char* param_name = get_scope_var_name_at_index(node->son[0]->symbol, variables_counter++); // TODO fix that shit
        HASH_NODE* param_node = hashFind(param_name);
        // param_node->data_type = DATATYPE_ERROR;
        param_node->type = SYMBOL_USED_LOCAL_VARIABLE;
        
    }
    // hashPrint();
    // printf(" \n");
    
}
void validate_AST_SYMBOL(AST * node)
{
    if (node->symbol->type != SYMBOL_VARIABLE
     && !isNumberType(getDatatypeFromLiteral(node->symbol->type)) 
     && !isBoolType(getDatatypeFromLiteral(node->symbol->type))
     )
    {
        SemanticErrors++;
        printf("Semantic Error: using identifier %s that's not variable neither literal\n", node->symbol->text);
    }
}

void check_operands(AST* node, int flag)
{
    int i;
    if (node == 0) return;

    switch (node->type)
    {
        case AST_SYMBOL:
            if (flag)
            {
                validate_AST_SYMBOL(node);
            }
            break;
            
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

        case AST_ATRIBUITION_VEC:
            validate_ATRIBUITION_VEC(node);
            break;

        case AST_VECTOR_ACCESS:
            if (flag)
            {
                validate_VECTOR_ACCESS(node);
            }
            
            break;
        case AST_IF:
        case AST_IF_ELSE:
        case AST_WHILE:
            validate_IF_like(node);
            break;

        case AST_FUNC_CALL:
            if (flag)
            {
                validate_FUNC_CALL(node);
            }
            
            break;
        
        case AST_FUNC_PARAMS_CALL:
            if (flag)
            {
                validate_FUNC_PARAMETERS_CALL(node);
            }
            break;
    
        case AST_FUNC_PARAMS_DEC:
            validate_FUNC_PARAMETERS_DEC(node);
            break;
    
    }
    for (i = 0; i < MAX_SONS; i++)
    {
        if (node->type == AST_FUNC_PARAMS_DEC)
        {
            return;
        }
        
        check_operands(node->son[i], 0);
    }
}
