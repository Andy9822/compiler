// AST - Abstract Syntax Tree

#ifndef AST_HEADR
#define AST_HEADR

#include "hash.h"

#define MAX_SONS 5

enum
{
   AST_SYMBOL = 1,
   AST_ADD,
   AST_SUB,

   AST_VECTOR_ACCESS,
   AST_NOT,
   AST_MINUS,
   
};

typedef struct ast_node
{
   int type;
   HASH_NODE *symbol;
   struct ast_node *son[MAX_SONS];
} AST;

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3, AST* s4);
AST *astPrint(AST *node);

// Alias for creating easilys and compactly an ast_symbol
#define AST_CREATE_SYMBOL(x) astCreate(AST_SYMBOL, x, 0, 0, 0, 0, 0)


#endif // AST_HEADR


