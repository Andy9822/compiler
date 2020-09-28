// AST - Abstract Syntax Tree

#include "ast.h"

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3, AST* s4) 
{
   AST * newnode;
   newnode->type = type;
   newnode->symbol = symbol;
   newnode->son[0] = s0;
   newnode->son[1] = s1;
   newnode->son[2] = s2;
   newnode->son[3] = s3;
   newnode->son[4] = s4;
   return newnode;
}

AST *astPrint(AST *node) 
{
   if (node ==0 )
   {
      return 0;
   }
   
   fprintf(stderr, "ast (");
   switch (node->type)
   {
      case AST_SYMBOL: fprintf(stderr, "AST_SYMBOL"); break;
      
      case AST_ADD: fprintf(stderr, "AST_ADD"); break;
      
      case AST_SUB: fprintf(stderr, "AST_SUB"); break;
      
      default: fprintf(stderr, "AST_UNDEFINED"); break;
   }

   if (node->symbol!=0)
   {
      fprintf(stderr, ", %s", node->symbol->text);
   }
   else
   {
      fprintf(stderr, ", 0");
   }
   
   

   int i;
   for (i = 0; i < MAX_SONS; i++)
   {
      astPrint(node->son[i]);
   }
   
   fprintf(stderr, ")\n");
}




