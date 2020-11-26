// AST - Abstract Syntax Tree

#ifndef AST_HEADR
#define AST_HEADR

#include "hash.h"

#define MAX_SONS 5

enum
{
   AST_UNDEFINED = 0,
   AST_SYMBOL = 1,
   AST_ADD,
   AST_SUB,
   AST_MULT,
   AST_DIV,
   AST_GREATER,
   AST_LESSER,
   AST_OR,
   AST_AND,
   AST_NOT,
   AST_MINUS,
   AST_DIF,
   AST_EQ,
   AST_GE,
   AST_LE,
   AST_PARENTHESIS,

   AST_BLOCK,
   AST_LCMD,
   AST_LCMDL,
   
   AST_DECLIST,
   
   AST_FUNC_VOID_DEC,   
   AST_FUNC_PARAMS_DEC,
   
   AST_VARDEC,
   AST_VEC_DEC,

   AST_VEC_INIT,
   AST_VEC_INIT_VALUES,

   AST_FUNC_PARAM_ASSIGN_LIST,
   AST_FUNC_CALL,
   AST_FUNC_PARAMS_CALL,
   AST_EXPRESSION_LIST,

   AST_ASSIGNMENT,
   
   AST_VECTOR_ACCESS,

   AST_WHILE,
   AST_LOOP,
   AST_IF,
   AST_IF_ELSE,
   AST_RETURN,

   AST_PRINTCMD,
   AST_PRINTLIST,

   AST_READ,
   AST_ATRIBUITION,
   AST_ATRIBUITION_VEC,   
   
};

typedef struct ast_node
{
   int type;
   HASH_NODE *symbol;
   struct ast_node *son[MAX_SONS];
} AST;

AST *astCreate(int type, HASH_NODE *symbol, AST* s0, AST* s1, AST* s2, AST* s3, AST* s4);
AST *astPrint(AST *node, int level);

// Alias for creating easilys and compactly an ast_symbol
#define astCreateSymbol(x) astCreate(AST_SYMBOL, x, 0, 0, 0, 0, 0)


#endif // AST_HEADR


