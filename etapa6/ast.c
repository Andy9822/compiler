// AST - Abstract Syntax Tree

#include "ast.h"

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3, AST* s4) 
{
   AST * newnode = (AST*) calloc(1, sizeof(AST));
   newnode->type = type;
   newnode->symbol = symbol;
   newnode->son[0] = s0;
   newnode->son[1] = s1;
   newnode->son[2] = s2;
   newnode->son[3] = s3;
   newnode->son[4] = s4;
   return newnode;
}

AST *astPrint(AST *node, int level) 
{
   int i;

   if (node ==0 )
   {
      return 0;
   }
   for ( i = 0; i < level; i++)
   {
      fprintf(stderr, "  |");
   }
   {
      /* code */
   }
   
   fprintf(stderr, "ast (");
   switch (node->type)
   {
      case AST_SYMBOL: fprintf(stderr, "AST_SYMBOL"); break;
      case AST_ADD: fprintf(stderr, "AST_ADD"); break;
      case AST_SUB: fprintf(stderr, "AST_SUB"); break;
      case AST_MULT: fprintf(stderr, "AST_MULT"); break;
      case AST_DIV: fprintf(stderr, "AST_DIV"); break;
      case AST_GREATER: fprintf(stderr, "AST_GREATER"); break;
      case AST_LESSER: fprintf(stderr, "AST_LESSER"); break;
      case AST_OR: fprintf(stderr, "AST_OR"); break;
      case AST_AND: fprintf(stderr, "AST_AND"); break;
      case AST_NOT: fprintf(stderr, "AST_NOT"); break;
      case AST_MINUS: fprintf(stderr, "AST_MINUS"); break;
      case AST_DIF: fprintf(stderr, "AST_DIF"); break;
      case AST_EQ: fprintf(stderr, "AST_EQ"); break;
      case AST_GE: fprintf(stderr, "AST_GE"); break;
      case AST_LE: fprintf(stderr, "AST_LE"); break;
      case AST_BLOCK: fprintf(stderr, "AST_BLOCK"); break;
      case AST_LCMD: fprintf(stderr, "AST_LCMD"); break;
      case AST_LCMDL: fprintf(stderr, "AST_LCMDL"); break;
      case AST_DECLIST: fprintf(stderr, "AST_DECLIST"); break;
      case AST_FUNC_VOID_DEC: fprintf(stderr, "AST_FUNC_VOID_DEC"); break;
      case AST_FUNC_PARAMS_DEC: fprintf(stderr, "AST_FUNC_PARAMS_DEC"); break;
      case AST_VARDEC: fprintf(stderr, "AST_VARDEC"); break;
      case AST_VEC_DEC: fprintf(stderr, "AST_VEC_DEC"); break;
      case AST_VEC_INIT: fprintf(stderr, "AST_VEC_INIT"); break;
      case AST_VEC_INIT_VALUES: fprintf(stderr, "AST_VEC_INIT_VALUES"); break;
      case AST_FUNC_PARAM_ASSIGN_LIST: fprintf(stderr, "AST_FUNC_PARAM_ASSIGN_LIST"); break;
      case AST_FUNC_CALL: fprintf(stderr, "AST_FUNC_CALL"); break;
      case AST_FUNC_PARAMS_CALL: fprintf(stderr, "AST_FUNC_PARAMS_CALL"); break;
      case AST_EXPRESSION_LIST: fprintf(stderr, "AST_EXPRESSION_LIST"); break;
      case AST_ASSIGNMENT: fprintf(stderr, "AST_ASSIGNMENT"); break;
      case AST_VECTOR_ACCESS: fprintf(stderr, "AST_VECTOR_ACCESS"); break;
      case AST_WHILE: fprintf(stderr, "AST_WHILE"); break;
      case AST_LOOP: fprintf(stderr, "AST_LOOP"); break;
      case AST_IF: fprintf(stderr, "AST_IF"); break;
      case AST_IF_ELSE: fprintf(stderr, "AST_IF_ELSE"); break;
      case AST_RETURN: fprintf(stderr, "AST_RETURN"); break;
      case AST_PRINTCMD: fprintf(stderr, "AST_PRINTCMD"); break;
      case AST_PRINTLIST: fprintf(stderr, "AST_PRINTLIST"); break;
      case AST_READ: fprintf(stderr, "AST_READ"); break;
      case AST_ATRIBUITION: fprintf(stderr, "AST_ATRIBUITION"); break;
      case AST_ATRIBUITION_VEC: fprintf(stderr, "AST_ATRIBUITION_VEC"); break;
      case AST_PARENTHESIS: fprintf(stderr, "AST_PARENTHESIS"); break;
      
      default: fprintf(stderr, "AST_UNDEFINED"); break;
   }

   if (node->symbol!=0)
   {
      fprintf(stderr, ", %s\n", node->symbol->text);
   }
   else
   {
      fprintf(stderr, ", 0\n");
   }
   
   

   for (i = 0; i < MAX_SONS; i++)
   {
      astPrint(node->son[i], level+1);
   }
 
}




