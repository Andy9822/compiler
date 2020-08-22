%{
  #include "tokens.h"
  int Running = 1;
  int lineNumber = 1;

  int getLineNumber(void) {
    return lineNumber;
  }

  int isRunning(void) {
    return Running;
  }
%}

HEX [A-F0-9]
LETTER [A-Za-z]
CHAR '(\\.|[^'\n\t])'
STRING \"(\\.|[^"\n])*\"
SPECIAL_CHARS [,;:()*{}+-/<>=|^~&$\[#\]$]
OPERATOR_LE  "<="
OPERATOR_GE ">="
OPERATOR_EQ "=="
OPERATOR_DIFF "!="

%x COMMENT

%%
  /* Palavras reservada*/
"char" return KW_CHAR;
"int" return KW_INT;
"float" return KW_FLOAT;
"bool" return KW_BOOL;
"if" return KW_IF;
"then" return KW_THEN;
"else" return KW_ELSE;
"while" return KW_WHILE;
"loop" return KW_LOOP;
"read" return KW_READ;
"print" return KW_PRINT;
"return" return KW_RETURN;
"TRUE" return LIT_TRUE;
"FALSE" return LIT_FALSE;

  /* Chars e Strings */
{CHAR} return LIT_CHAR;
{STRING} return LIT_STRING;

  /* Caracteres especiais */
{SPECIAL_CHARS} return yytext[0];

  /* Operadores compostos*/
{OPERATOR_LE} return OPERATOR_LE;
{OPERATOR_GE} return OPERATOR_GE;
{OPERATOR_EQ} return OPERATOR_EQ;
{OPERATOR_DIFF} return OPERATOR_DIF;

  /* Identificadores */
({LETTER}|[_@])+({LETTER}|[0-9_@])* return TK_IDENTIFIER;

  /*Literais Inteiro e Float*/
{HEX}+ return LIT_INTEGER;
{HEX}*"."{HEX}+ return LIT_FLOAT;

  /* Ignorar tabs */
[ \t]

  /* contabilizar linha*/
"\n"   ++lineNumber;

  /* Comentarios*/
"//".*
"/*"   BEGIN(COMMENT);

.      return TOKEN_ERROR;

 /* Gramatica dos comentarios*/
<COMMENT>.
<COMMENT>"\n"   ++lineNumber;
<COMMENT>"*/"   BEGIN(INITIAL);


%%

#include "main.c"

int yywrap()
{
  Running = 0;
  return 1;
}
