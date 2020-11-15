#include "hash.h"

void hash_init() {
  int i;
  for (i = 0; i < HASH_SIZE; i++) {
    HASH_NODE *node = HASH_TABLE[i];
    while (node) {
      HASH_NODE *nodeToRemove = node;
      node = node->next;
      free(nodeToRemove);
    }
    HASH_TABLE[i] = 0;
  }
}

int hashString(char string[]) {
  int hash = 1;
  int i;
  for (i = 0; i < strlen(string); i++) {
    hash = ((hash * string[i]) % HASH_SIZE) + 1;
  }

  return hash-1;
}

HASH_NODE *hashFind(char *text){
  HASH_NODE *node;
  int adress = hashString(text);
  for(node = HASH_TABLE[adress]; node; node = node->next) {
    if(!strcmp(node->text, text)){
      return node;
    }
  }
  return 0;
}

HASH_NODE * hashInsert(char string[], int type) {
  HASH_NODE *new_node;
  int index = hashString(string);

  new_node = hashFind(string);
  if (new_node == 0)
  {
    new_node = (HASH_NODE *) calloc(1,sizeof(HASH_NODE));
    new_node->text = (char *) calloc(strlen(string)+1, sizeof(char));

    new_node->type = type;
    new_node->text = strcpy(new_node->text, string);
    new_node->next = HASH_TABLE[index];
    new_node->data_type = DATATYPE_UNDEFINED;
    new_node->scope_variables = NULL;
    new_node->init_value = NULL;
    new_node->vec_init_value = NULL;
    new_node->idx = 0;
    HASH_TABLE[index] = new_node;
  }
  
  // printf("!!!HASHMAP!!! Inseri %s em Table[%d]\n",HASH_TABLE[index]->text, index);
  return new_node;
}

void hashPrint() {
  int i;
  HASH_NODE *node;
  for (i = 0; i < HASH_SIZE; i++) {
    node = HASH_TABLE[i];
    while (node) {
      printf("Table [%d] has %s with type %d and datatype %d\n",i, node->text, node->type, node->data_type);
      node = node->next;
    }
  }
}


int hash_check_undeclared() 
{
  int i, undeclared = 0; 
  HASH_NODE *node;
  for (i = 0; i < HASH_SIZE; i++) {
    node = HASH_TABLE[i];
    while (node) {
      if (node->type == SYMBOL_IDENTIFIER)
      {
        printf("Semantic error: identifier %s undeclared\n", node->text);
        undeclared++;
      }
      node = node->next;
    }
  }
  return undeclared;
}

void insert_local_variable(HASH_NODE* node, char* local_variable_name, int local_variable_type)
{
  // Aloca espaço e cria estrutura da nova variavel
  LOCAL_VARIABLE *new_variable;
  new_variable = (LOCAL_VARIABLE *) calloc(1,sizeof(LOCAL_VARIABLE));
  new_variable->text = (char *) calloc(strlen(local_variable_name)+1, sizeof(char));
  strcpy(new_variable->text, local_variable_name);
  new_variable->type = local_variable_type;
  new_variable->next = NULL;
  
  
  //Se ja tinha scopo, percorre todas as variaveis ate a ultima
  if (node->scope_variables)
  {
    LOCAL_VARIABLE* ptr_variable = node->scope_variables;
    
    while (ptr_variable->next)
    {
      ptr_variable = ptr_variable->next;
    }
    
    ptr_variable->next = new_variable;
  }

  // Se nao tinha nenhuma variavel no escopo, adiciona no proprio nodo
  else
  { 
    node->scope_variables = new_variable;
  }

  // Atualiza o nodo daquele identificador para ter o tipo
  HASH_NODE * local_var_node = hashFind(local_variable_name);
  local_var_node->type = local_variable_type;
}

void insert_vec_value(HASH_NODE* node, char* vec_value)
{
  VEC_VALUES* new_vec_value = (VEC_VALUES *) calloc(1,sizeof(VEC_VALUES));
  new_vec_value->value = (char *) calloc(strlen(vec_value)+1, sizeof(char));
  strcpy(new_vec_value->value, vec_value);
  new_vec_value->next = NULL;

  if (node->vec_init_value == NULL)
  {
    node->vec_init_value = new_vec_value;
    return;
  }

  VEC_VALUES* iterator = node->vec_init_value;
  while (iterator->next)
  {
    iterator = iterator->next;
  }
  
  iterator->next = new_vec_value;

  return;
}

void print_scope_variables(HASH_NODE* node)
{
  LOCAL_VARIABLE *ptr_variable = node->scope_variables;
  if (!ptr_variable)
  {
    return;
  }

  while (ptr_variable)
  {
    printf("variavel %s tipo %d \n", ptr_variable->text, ptr_variable->type);
    ptr_variable = ptr_variable->next;
  }
}

int get_scope_len(HASH_NODE* node)
{
  int i = 0;
  LOCAL_VARIABLE *ptr_variable = node->scope_variables;
  if (!ptr_variable)
  {
    return i;
  }

  while (ptr_variable)
  {
    ptr_variable = ptr_variable->next;
    i+=1;
  }

  return i;
}

char* get_vec_value_at_index(HASH_NODE* node, long int idx)
{
  VEC_VALUES* iterator;
  iterator = node->vec_init_value;

  long int i;

  for (i = 0; i < idx; i++)
  {
    iterator = iterator->next;
  }
  
  return iterator->value;
}

int get_scope_index(HASH_NODE* node, int idx)
{
  LOCAL_VARIABLE *ptr_variable = node->scope_variables;

  if (idx == 0)
  {
    return ptr_variable->type;
  }
  idx -=1;
  
  while (ptr_variable)
  {
    if (idx == 0)
    {
      return ptr_variable->type;
    }

    // printf("variavel %s tipo %d \n", ptr_variable->text, ptr_variable->type);
    ptr_variable = ptr_variable->next;
    idx-=1;
  }
}

char* get_scope_var_name_at_index(HASH_NODE* node, int idx)
{
  LOCAL_VARIABLE *ptr_variable = node->scope_variables;

  if (idx == 0)
  {
    return ptr_variable->text;
  }
  idx -=1;
  
  while (ptr_variable)
  {
    if (idx == 0)
    {
      return ptr_variable->text;
    }

    // printf("variavel %s tipo %d \n", ptr_variable->text, ptr_variable->type);
    ptr_variable = ptr_variable->next;
    idx-=1;
  }
}

HASH_NODE *makeTemp() 
{
  static int serial = 0;
  char buffer[256] = ".";

  sprintf(buffer, "myWeeirT_emp%d", serial++);
  return hashInsert(buffer, SYMBOL_VARIABLE);
}

HASH_NODE *makeLabel() 
{
  static int serial = 0;
  char buffer[256] = ".";

  sprintf(buffer, "myWeeirL_abael%d", serial++);
  return hashInsert(buffer, SYMBOL_LABEL);
}