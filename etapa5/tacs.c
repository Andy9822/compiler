#include "tacs.h"
#include "hash.h"
#include "ast.h"


TAC* tacCreate(int type, HASH_NODE* res, HASH_NODE* op1, HASH_NODE* op2)
{
  TAC* new_tac = 0;
  new_tac = (TAC*) calloc(1,sizeof(TAC));
  new_tac->type = type;
    new_tac->res = res;
    new_tac->op1 = op1;
    new_tac->op2 = op2;
    new_tac->prev = 0;
    new_tac->next = 0;
  return new_tac;
}

void tacPrint(TAC* tac)
{
  if (!tac || tac->type == TAC_SYMBOL) return;

  fprintf(stderr, "TAC(");
  switch (tac->type)
  {
    case TAC_SYMBOL:  fprintf(stderr, "TAC_SYMBOL");  break;
    case TAC_ADD:  fprintf(stderr, "TAC_ADD");  break;
    case TAC_SUB: fprintf(stderr, "TAC_SUB"); break;
    case TAC_COPY: fprintf(stderr, "TAC_COPY"); break;
    case TAC_JMP: fprintf(stderr, "TAC_JMP"); break;
    case TAC_JMP_FALSE: fprintf(stderr, "TAC_JMP_FALSE"); break;
    case TAC_LABEL: fprintf(stderr, "TAC_LABEL"); break;
    case TAC_PRINT: fprintf(stderr, "TAC_PRINT"); break;
    case TAC_READ: fprintf(stderr, "TAC_READ"); break;
    case TAC_WHILE: fprintf(stderr, "TAC_WHILE"); break;
    default: fprintf(stderr, "TAC_UNDEFINED"); break;
  }
  
  fprintf(stderr, ", %s", (tac->res) ? tac->res->text : "0");
  fprintf(stderr, ", %s", (tac->op1) ? tac->op1->text : "0");
  fprintf(stderr, ", %s", (tac->op2) ? tac->op2->text : "0");
  fprintf(stderr, ");\n");
  
}

void tacPrintBackwards(TAC* tac)
{
  if (!tac) return;

  tacPrintBackwards(tac->prev);
  tacPrint(tac);
  
}


TAC* tacJoin(TAC* l1, TAC* l2)
{
  TAC* point;

  if (!l1) return l2;
  if (!l2) return l1;
  
  for (point = l2; point->prev != 0; point = point->prev)
  {
    ;
  }

  point->prev = l1;
  return l2;
  
}


// ############################# Code generation helpers #############################
TAC* createIf(TAC* code0, TAC* code1)
{
  TAC* jmp_tac = 0;
  TAC* label_tac = 0;
  HASH_NODE* new_label = 0;
  new_label = makeLabel();

  // Cria jmp_false para caso a clausula não seja verdadeira, pular o commando 
  jmp_tac = tacCreate(TAC_JMP_FALSE, new_label, code0->res, 0);
  jmp_tac->prev = code0;

  label_tac = tacCreate(TAC_LABEL, new_label, 0, 0);
  label_tac->prev = code1;

  return tacJoin(jmp_tac, label_tac);

}

TAC* createWhile(TAC* code0, TAC* code1)
{
  TAC* jmp_begin_tac = 0;
  TAC* jmp_false_tac = 0;

  TAC* label_begin_tac = 0;
  TAC* label_end_tac = 0;

  HASH_NODE* beggining_label = 0;
  beggining_label = makeLabel();

  HASH_NODE* end_label = 0;
  end_label = makeLabel();

  // Antes do while, será criado um label para representar inicio do while
  label_begin_tac = tacCreate(TAC_LABEL, beggining_label, 0, 0);

  // Cria jmp_false para caso a clausula não seja verdadeira, pular o commando 
  jmp_false_tac = tacCreate(TAC_JMP_FALSE, end_label, code0->res, 0);
  jmp_false_tac->prev = code0;

  // No fim do comando while, será criado um JMP para o início e assim reavaliar clausula, podendo ou não iterar de novo
  jmp_begin_tac = tacCreate(TAC_JMP, beggining_label, 0, 0);
  jmp_begin_tac->prev = code1;

  // No fim do while, será criado um label para representar o fim do bloco while
  label_end_tac = tacCreate(TAC_LABEL, end_label, 0, 0);
  label_end_tac->prev = jmp_begin_tac;

  return tacJoin(label_begin_tac, tacJoin(jmp_false_tac, label_end_tac));

}

TAC* createIfElse(TAC* code0, TAC* code1, TAC* code2)
{
  TAC* jmp_false_tac = 0;
  TAC* jmp_end_tac = 0;
  TAC* label_false_tac = 0;
  TAC* label_end_else_tac = 0;
  
  HASH_NODE* else_label = 0;
  else_label = makeLabel();

  HASH_NODE* end_else_label = 0;
  end_else_label = makeLabel();

  // Cria jmp_false para caso a clausula seja não seja verdadeira, pular o then
  jmp_false_tac = tacCreate(TAC_JMP_FALSE, else_label, code0->res, 0);
  jmp_false_tac->prev = code0;

  // Cria jmp (incondicional) após o bloco do if-then para ele não entrar no bloco do else e ir para fim de if-then-else
  jmp_end_tac = tacCreate(TAC_JMP, end_else_label, 0, 0);
  jmp_end_tac->prev = code1;

  // Se a expressao era falsa, o jmp_false saltará ate o label do else
  label_false_tac = tacCreate(TAC_LABEL, else_label, 0, 0);
  label_false_tac->prev = jmp_end_tac;

  // No fim do else, será criado um label para representar o fim do bloco if-else-then
  label_end_else_tac = tacCreate(TAC_LABEL, end_else_label, 0, 0);
  label_end_else_tac->prev = code2;

  return tacJoin(jmp_false_tac, tacJoin(label_false_tac, label_end_else_tac));

}

// ############################# Code generation #############################
TAC* generateCode(AST* node)
{
  if (!node) return 0;
  
  int i;
  TAC* code[MAX_SONS];
  // Process Children
  for (i = 0; i < MAX_SONS; i++)
  {
    code[i] = generateCode(node->son[i]);
  }
  

  TAC* result;
  //Process node itself
  switch (node->type)
  {
    case AST_SYMBOL:   
      result = tacCreate(TAC_SYMBOL, node->symbol, 0, 0); 
      break;

    // TODO: call binary_operation() function passing "TAC_ADD" option to reuse code
    case AST_ADD:      
      result = tacJoin(tacJoin(code[0], code[1]), tacCreate(TAC_ADD, makeTemp(), code[0]->res, code[1]->res)); 
      break; //Use optional chaining if segfault appears
    
    case AST_ATRIBUITION:  
      result = tacJoin(code[1], tacCreate(TAC_COPY, node->son[0]->symbol, code[1]->res, 0)); 
      break;
    
    case AST_PRINTCMD:
      //TODO isso só printa 1 elemento, se for uma lista o resto (??????)
      result = tacJoin(tacJoin(code[0], code[1]), tacCreate(TAC_PRINT, code[0]?code[0]->res:0, 0, 0));
      break;
    
    case AST_READ:
      result = tacCreate(TAC_READ, code[0]?code[0]->res:0, 0, 0);
      break;
    
    case AST_IF:  
      result = createIf(code[0], code[1]); 
      break;


    case AST_IF_ELSE:  
      result = createIfElse(code[0], code[1], code[2]); 
      break;
    
    case AST_WHILE:  
      result = createWhile(code[0], code[1]); 
      break;

    default:           
      result = tacJoin(code[0], tacJoin(code[1], tacJoin(code[2], tacJoin(code[3], code[4])))); 
      break;
  }

  return result;
}

