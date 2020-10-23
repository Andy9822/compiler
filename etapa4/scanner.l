%{
  #include <stdlib.h>
  #include <string.h>
  #include "ast.h"
  #include "hash.h"
  #include "y.tab.h"
  

  int Running = 1;
  int lineNumber = 1;

  void initMe(void) {
    hash_init();
    Running = 1;
    lineNumber = 1;
  }

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
"char" { yylval.symbol = hashInsert(yytext, SYMBOL_KW_CHAR); return KW_CHAR;}
"int" { yylval.symbol = hashInsert(yytext, SYMBOL_KW_INT); return KW_INT;}
"float" { yylval.symbol = hashInsert(yytext, SYMBOL_KW_FLOAT); return KW_FLOAT;}
"bool" { yylval.symbol = hashInsert(yytext, SYMBOL_KW_BOOL); return KW_BOOL;}

"if" return KW_IF;
"then" return KW_THEN;
"else" return KW_ELSE;
"while" return KW_WHILE;
"loop" return KW_LOOP;
"read" return KW_READ;
"print" return KW_PRINT;
"return" return KW_RETURN;

"TRUE"  { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_TRUE); return LIT_TRUE;}
"FALSE"  { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_FALSE); return LIT_FALSE;}

  /* Chars e Strings */
{CHAR} { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_CHAR); return LIT_CHAR;}
{STRING} { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_STRING); return LIT_STRING;}

  /* Caracteres especiais */
{SPECIAL_CHARS} return yytext[0];

  /* Operadores compostos*/
{OPERATOR_LE} return OPERATOR_LE;
{OPERATOR_GE} return OPERATOR_GE;
{OPERATOR_EQ} return OPERATOR_EQ;
{OPERATOR_DIFF} return OPERATOR_DIF;

  /* Identificadores */
({LETTER}|[_@])+({LETTER}|[0-9_@])* { yylval.symbol = hashInsert(yytext,SYMBOL_IDENTIFIER); return TK_IDENTIFIER;}

  /*Literais Inteiro e Float*/
{HEX}+ { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_INTEGER); return LIT_INTEGER;}
{HEX}*"."{HEX}+ { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_FLOAT); return LIT_FLOAT;}

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

int yywrap()
{
  Running = 0;
  return 1;
}
