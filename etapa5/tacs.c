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
    case TAC_SYMBOL:     fprintf(stderr, "TAC_SYMBOL");  break;
    case TAC_ADD:        fprintf(stderr, "TAC_ADD");  break;
    case TAC_SUB:        fprintf(stderr, "TAC_SUB");  break;
    case TAC_DIV:        fprintf(stderr, "TAC_DIV");  break;
    case TAC_MULT:       fprintf(stderr, "TAC_MULT");  break;
    case TAC_GREATER:    fprintf(stderr, "TAC_GREATER");  break;
    case TAC_LESSER:     fprintf(stderr, "TAC_LESSER");  break;
    case TAC_OR:         fprintf(stderr, "TAC_OR");  break;
    case TAC_AND:        fprintf(stderr, "TAC_AND");  break;
    case TAC_DIF:        fprintf(stderr, "TAC_DIF");  break;
    case TAC_EQ:         fprintf(stderr, "TAC_EQ");  break;
    case TAC_GE:         fprintf(stderr, "TAC_GE");  break;
    case TAC_LE:         fprintf(stderr, "TAC_LE");  break;
    case TAC_NOT:        fprintf(stderr, "TAC_NOT");  break;
    case TAC_MINUS:      fprintf(stderr, "TAC_MINUS");  break;
    case TAC_COPY:       fprintf(stderr, "TAC_COPY"); break;
    case TAC_COPY_VEC:   fprintf(stderr, "TAC_COPY_VEC"); break;
    case TAC_JMP:        fprintf(stderr, "TAC_JMP"); break;
    case TAC_JMP_FALSE:  fprintf(stderr, "TAC_JMP_FALSE"); break;
    case TAC_LABEL:      fprintf(stderr, "TAC_LABEL"); break;
    case TAC_PRINT:      fprintf(stderr, "TAC_PRINT"); break;
    case TAC_RETURN:     fprintf(stderr, "TAC_RETURN"); break;
    case TAC_READ:       fprintf(stderr, "TAC_READ"); break;
    case TAC_WHILE:      fprintf(stderr, "TAC_WHILE"); break;
    case TAC_BEGINFUN:   fprintf(stderr, "TAC_BEGINFUN"); break;
    case TAC_ENDFUN:     fprintf(stderr, "TAC_ENDFUN"); break;
    case TAC_FUNCALL:    fprintf(stderr, "TAC_FUNCALL"); break;
    case TAC_FUNC_ARG:   fprintf(stderr, "TAC_FUNC_ARG"); break;
    case TAC_VEC_ACCESS: fprintf(stderr, "TAC_VEC_ACCESS"); break;
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
TAC* createFunc(HASH_NODE* symbol, TAC* code_expr)
{
  return tacJoin(tacJoin(tacCreate(TAC_BEGINFUN, symbol, 0, 0), code_expr), tacCreate(TAC_ENDFUN, symbol, 0, 0)); 
}

TAC* createAttribution(HASH_NODE* symbol, TAC* code_expr)
{
  return tacJoin(code_expr, tacCreate(TAC_COPY, symbol, code_expr->res, 0)); 
}

TAC* createBooleanArithmetic2Operands(int tac_operation, TAC* code0, TAC* code1)
{
  return  tacJoin(tacJoin(code0, code1), tacCreate(tac_operation, makeTemp(), code0->res, code1->res)); 
}

TAC* createNegation(int tac_operation, TAC* code0)
{
  return  tacJoin(code0, tacCreate(tac_operation, makeTemp(), code0->res, 0)); 
}

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

TAC* createLoop(HASH_NODE* symbol, TAC* code_expr_start, TAC* code_expr_end, TAC* code_expr_step, TAC* code_cmd)
{
  //Attribution for start expression
  TAC* tac_expr_start = createAttribution(symbol, code_expr_start); 

  // We need to jump to the end of the loop block in case clausule is false
  TAC* jmp_false_tac = 0;
  TAC* label_end_tac = 0;
  HASH_NODE* end_label = 0;
  end_label = makeLabel();

  // We need a label at the beggining of the loop block to be able to jump back and test the clausule, therefore being able to iterate or leave the loop
  TAC* jmp_begin_tac = 0;
  TAC* label_begin_tac = 0;
  HASH_NODE* beggining_label = 0;
  beggining_label = makeLabel();

  // We create the beggining label right after the starting attribution 
  label_begin_tac = tacCreate(TAC_LABEL, beggining_label, 0, 0);
  label_begin_tac->prev = tac_expr_start;

  // We create a jump to the end in case clausule is false
  jmp_false_tac = tacCreate(TAC_JMP_FALSE, end_label, code_expr_end->res, 0);
  jmp_false_tac->prev = code_expr_end;

  // Right after the command block we manually update our identifier with our "step expression"
  HASH_NODE* temp_add = makeTemp();
  TAC* tac_add_expr = tacCreate(TAC_ADD, temp_add, symbol, code_expr_step->res);
  tac_add_expr->prev = code_expr_step;
  TAC* tac_update_expr = tacCreate(TAC_COPY, symbol, temp_add, 0);
  tac_update_expr->prev = tac_add_expr;

  // We create a jump to the beggining right after the command block
  jmp_begin_tac = tacCreate(TAC_JMP, beggining_label, 0, 0);
  jmp_begin_tac->prev = tac_update_expr;

  // We create the end label at the end of all commands 
  label_end_tac = tacCreate(TAC_LABEL, end_label, 0, 0);
  label_end_tac->prev = jmp_begin_tac;



  return tacJoin(label_begin_tac, tacJoin(jmp_false_tac, tacJoin(code_cmd, label_end_tac)));
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
    
    case AST_ADD: result = createBooleanArithmetic2Operands(TAC_ADD, code[0], code[1]); break;
    case AST_SUB: result = createBooleanArithmetic2Operands(TAC_SUB, code[0], code[1]); break;
    case AST_DIV: result = createBooleanArithmetic2Operands(TAC_DIV, code[0], code[1]); break;
    case AST_MULT: result = createBooleanArithmetic2Operands(TAC_MULT, code[0], code[1]); break;
    case AST_GREATER: result = createBooleanArithmetic2Operands(TAC_GREATER, code[0], code[1]); break;
    case AST_LESSER: result = createBooleanArithmetic2Operands(TAC_LESSER, code[0], code[1]); break;
    case AST_OR: result = createBooleanArithmetic2Operands(TAC_OR, code[0], code[1]); break;
    case AST_AND: result = createBooleanArithmetic2Operands(TAC_AND, code[0], code[1]); break;
    case AST_DIF: result = createBooleanArithmetic2Operands(TAC_DIF, code[0], code[1]); break;
    case AST_EQ: result = createBooleanArithmetic2Operands(TAC_EQ, code[0], code[1]); break;
    case AST_GE: result = createBooleanArithmetic2Operands(TAC_GE, code[0], code[1]); break;
    case AST_LE: result = createBooleanArithmetic2Operands(TAC_LE, code[0], code[1]); break;

    case AST_NOT: result = createNegation(TAC_NOT, code[0]); break;
    case AST_MINUS: result = createNegation(TAC_MINUS, code[0]); break;
    
    case AST_ATRIBUITION:  
      result = createAttribution(node->son[0]->symbol, code[1]);
      break;

    // O ->res é o vetor, o ->op1 é o índice, e o ->op2 é o valor a ser salvo nessa posição do vetor
    case AST_ATRIBUITION_VEC:  
      result = tacJoin(code[1], tacJoin(code[2], tacCreate(TAC_COPY_VEC, node->son[0]->symbol, code[1]->res, code[2]->res)));       
      break;

    case AST_FUNC_VOID_DEC:  
      result = createFunc(node->son[0]->symbol, code[2]);
      break;
    
    case AST_FUNC_PARAMS_DEC:  
      result = createFunc(node->son[0]->symbol, code[4]);
      break;
    
    case AST_PRINTCMD:
    case AST_PRINTLIST:
      result = tacJoin(tacJoin(code[0], tacCreate(TAC_PRINT, code[0]?code[0]->res:0, 0, 0)), code[1]);
      break;

    case AST_RETURN:
      result = tacJoin(code[0], tacCreate(TAC_RETURN, code[0]?code[0]->res:0, 0, 0));
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

    // O loop espera que a expr2 seja um booleana. Após cada iteração e somar um step (expr3) se testa se expr2 é verdeira ou jmp_false
    case AST_LOOP:  
      result = createLoop(node->son[0]->symbol, code[1], code[2], code[3], code[4]);
      break;

    // O ->res é a variável temporária onde a execução da função, o ->op1, vai ser depositado
    case AST_FUNC_CALL: ;
      HASH_NODE* func_return_temp = makeTemp();
      result = tacCreate(TAC_FUNCALL, func_return_temp, code[0]->res, 0);
      break;

    case AST_VECTOR_ACCESS: ;
      HASH_NODE* access_vector_temp = makeTemp();
      result = tacJoin(code[1], tacCreate(TAC_VEC_ACCESS, access_vector_temp, code[0]->res, code[1]->res));
      break;

    case AST_FUNC_PARAMS_CALL: ;
      HASH_NODE* func_return_params_temp = makeTemp();
      TAC* tac_func_call = tacCreate(TAC_FUNCALL, func_return_params_temp, code[0]->res, 0);
      result = tacJoin(tacJoin(tacJoin(code[1], tacCreate(TAC_FUNC_ARG, code[1]?code[1]->res:0, 0, 0)), code[2]), tac_func_call);
      break;

    case AST_EXPRESSION_LIST: ;
      result = tacJoin(tacJoin(code[0], tacCreate(TAC_FUNC_ARG, code[0]?code[0]->res:0, 0, 0)), code[1]);
      break;

    default:           
      result = tacJoin(code[0], tacJoin(code[1], tacJoin(code[2], tacJoin(code[3], code[4])))); 
      break;
  }

  return result;
}

