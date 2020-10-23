#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 997

typedef struct hash_node {
  int type;
  char *text;
  struct hash_node *next;
} HASH_NODE;


void hash_init();
int hashString(char string[]);
HASH_NODE * hashInsert(char string[], int type);
HASH_NODE *hashFind(char *text);
void hashPrint();
#endif