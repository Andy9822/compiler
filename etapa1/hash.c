#include "stdio.h"

#define HASH_SIZE 997

typedef struct hash_node {
  int type;
  char *text;
  struct hash_node *next;
} HASH_NODE;

HASH_NODE *HASH_TABLE[HASH_SIZE];

void hash_init() {
  int i;
  for (i = 0; i < HASH_SIZE; i++) {
    HASH_TABLE[i] = 0;
  }
}

int hashString(char string[]) {
  int hash = 1;
  int i;
  for (i = 0; i < strlen(string); i++) {
    hash = ((hash * string[i]) % HASH_SIZE) + 1;
  }
}

HASH_NODE * hashInsert(char string[]) {
  HASH_NODE *new_node;
  int index = hashString(string);

  new_node = (HASH_NODE *) calloc(1,sizeof(HASH_NODE));
  new_node->text = (char *) calloc(strlen(string)+1, sizeof(char));

  new_node->type = 1;
  new_node->text = strcpy(new_node->text, string);
  new_node->next = HASH_TABLE[index];
  HASH_TABLE[index] = new_node;
  // printf("!!!HASHMAP!!! Inseri %s em Table[%d]\n",HASH_TABLE[index]->text, index);
  return new_node;
}

void hashPrint() {
  int i;
  HASH_NODE *node;
  for (i = 0; i < HASH_SIZE; i++) {
    node = HASH_TABLE[i];
    while (node) {
      printf("Table [%d] = %s\n",i, HASH_TABLE[i]->text);
      node = node->next;
    }
  }
}
