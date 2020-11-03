#ifndef TACS_HEADER
#define TACS_HEADER

#include "hash.h"
#include "ast.h"

enum
{
  TAC_SYMBOL = 1,
  TAC_ADD,
  TAC_SUB,
  TAC_DIV,
  TAC_MULT,
  TAC_GREATER,
  TAC_LESSER,
  TAC_OR,
  TAC_AND,
  TAC_DIF,
  TAC_EQ,
  TAC_GE,
  TAC_LE,
  TAC_NOT,
  TAC_MINUS,
  TAC_COPY,
  TAC_IF,
  TAC_IF_ELSE,
  TAC_JMP,
  TAC_JMP_FALSE,
  TAC_LABEL,
  TAC_RETURN,
  TAC_PRINT,
  TAC_READ,
  TAC_WHILE,
  TAC_BEGINFUN,
  TAC_ENDFUN,
  TAC_FUNCALL,
  TAC_FUNC_ARG,
};

typedef struct tac_node 
{
    int type;
    HASH_NODE* res; 
    HASH_NODE* op1; 
    HASH_NODE* op2; 
    struct tac_node* prev;
    struct tac_node* next;
} TAC;


TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2);
TAC* tacJoin(TAC* l1, TAC* l2);
void tacPrint(TAC* tac);
void tacPrintBackwards(TAC* tac);

// Code generation

TAC* generateCode(AST* node);

#endif