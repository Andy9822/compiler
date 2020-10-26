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
#define SYMBOL_LOCAL_VARIABLE 15
#define SYMBOL_USED_LOCAL_VARIABLE 16

enum
{
  DATATYPE_ERROR = -1,
  DATATYPE_UNDEFINED = 0,
  DATATYPE_CHAR = 1,
  DATATYPE_INT,
  DATATYPE_FLOAT,
  DATATYPE_BOOL,
};

typedef struct local_variable {
  char* text;
  int type;
  struct local_variable *next;
} LOCAL_VARIABLE;

typedef struct hash_node {
  int type;
  int data_type;
  char *text;
  struct hash_node *next;
  struct local_variable * scope_variables;
} HASH_NODE;


void hash_init();
int hashString(char string[]);
HASH_NODE * hashInsert(char string[], int type);
HASH_NODE *hashFind(char *text);
void hashPrint();
int hash_check_undeclared();
void insert_local_variable(HASH_NODE* node, char* local_variable_name, int local_variable_type);
void print_scope_variables(HASH_NODE* node);
int get_scope_len(HASH_NODE* node);
int get_scope_index(HASH_NODE* node, int idx);
char* get_scope_var_name_at_index(HASH_NODE* node, int idx);
#endif