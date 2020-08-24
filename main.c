int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Call: etapa1 fileName\n");
    exit(1);
  }
  yyin = fopen(argv[1], "r");
  if (yyin == 0) {
    fprintf(stderr, "Cannot open file %s\n", argv[1]);
    exit(2);
  }

  initMe();

  int tok;
  while (Running) {
    tok = yylex();
    switch (tok) {
      case LIT_INTEGER: printf("Literal inteiro '%s' [line:%d] \n", yytext, lineNumber);break;
      case LIT_FLOAT: printf("Literal FLOAT '%s' [line:%d] \n",yytext, lineNumber); break;
      case TK_IDENTIFIER: printf("Identificador '%s' [line:%d] \n", yytext, lineNumber);break;
      case OPERATOR_LE: printf("OPERATOR_LE %s line[%d]\n", yytext, lineNumber);break;
      case OPERATOR_GE: printf("OPERATOR_GE %s line[%d]\n", yytext, lineNumber);break;
      case LIT_TRUE: printf("True %s [line:%d]\n",yytext, getLineNumber() ); break;
      case LIT_FALSE: printf("False %s [line:%d]\n",yytext, getLineNumber() ); break;
      case LIT_CHAR: printf("Char %s [line:%d]\n",yytext, getLineNumber() ); break;
      case LIT_STRING: printf("String %s [line:%d]\n",yytext, getLineNumber() ); break;
      case OPERATOR_EQ: printf("OPERATOR_EQ %s line[%d]\n", yytext, lineNumber);break;
      case OPERATOR_DIF: printf("OPERATOR_DIF %s line[%d]\n", yytext, lineNumber);break;
      case TOKEN_ERROR: printf("ERRO [line:%d]\n",lineNumber); break;
      case KW_CHAR: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_INT: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_FLOAT: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_BOOL: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_IF: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_THEN: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_ELSE: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_WHILE: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_LOOP: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_READ: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_PRINT: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case KW_RETURN: printf("Palavra reservada %s [line:%d]\n", yytext, getLineNumber());break;
      case ',': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case ';': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case ':': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '(': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case ')': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '[': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case ']': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '{': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '}': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '+': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '-': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '*': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '/': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '<': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '>': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '=': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '|': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '^': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '~': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '&': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '$': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      case '#': printf("Caractere especial %c [line:%d]\n",tok, lineNumber );break;
      default: printf("Simbolo %c [line:%d]\n", tok, lineNumber); break;
    }
  }

  // hashPrint();
  return 0;
}
