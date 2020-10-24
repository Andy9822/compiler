#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

#define SYMBOL_LIT_INTEGER 1
#define SYMBOL_LIT_FLOAT 2
#define SYMBOL_LIT_CHAR 3
#define SYMBOL_LIT_TRUE 4
#define SYMBOL_LIT_FALSE 5
#define SYMBOL_LIT_STRING 6
#define SYMBOL_IDENTIFIER 7
#define SYMBOL_KW_CHAR 8
#define SYMBOL_KW_INT 9
#define SYMBOL_KW_FLOAT 10
#define SYMBOL_KW_BOOL 11
#define SYMBOL_VARIABLE 12
#define SYMBOL_VECTOR 13
#define SYMBOL_FUNCTION 14

enum
{
  DATATYPE_ERROR = -1,
  DATATYPE_CHAR = 1,
  DATATYPE_INT,
  DATATYPE_FLOAT,
  DATATYPE_BOOL,
};

typedef struct hash_node {
  int type;
  int data_type;
  char *text;
  struct hash_node *next;
} HASH_NODE;


void hash_init();
int hashString(char string[]);
HASH_NODE * hashInsert(char string[], int type);
HASH_NODE *hashFind(char *text);
void hashPrint();
int hash_check_undeclared();
#endif