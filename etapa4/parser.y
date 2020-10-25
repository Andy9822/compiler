%{
    #include "semantic.h"
    int getLineNumber(void);
    int yylex();    // TODO find better workaround
    AST* Root = NULL;
%}

%union
{
    HASH_NODE *symbol;
    AST *ast;
}

%token<symbol> KW_CHAR      
%token<symbol> KW_INT       
%token<symbol> KW_FLOAT     
%token<symbol> KW_BOOL      

%token KW_IF        
%token KW_THEN      
%token KW_ELSE      
%token KW_WHILE     
%token KW_LOOP      
%token KW_READ      
%token KW_PRINT     
%token KW_RETURN    

%token OPERATOR_LE  
%token OPERATOR_GE  
%token OPERATOR_EQ  
%token OPERATOR_DIF 
%token<symbol> TK_IDENTIFIER

%token<symbol> LIT_INTEGER  
%token<symbol> LIT_FLOAT    
%token<symbol> LIT_TRUE     
%token<symbol> LIT_FALSE    
%token<symbol> LIT_CHAR     
%token<symbol> LIT_STRING   
%token TOKEN_ERROR

%type<ast> programa
%type<ast> declist

%type<ast> funcdec
%type<ast> vardec

%type<ast> vartype

%type<ast> assignmentlist
%type<ast> assignment

%type<ast> vector
%type<ast> vecvalues

%type<ast> block
%type<ast> lcmd
%type<ast> cmd

%type<ast> flowcommand
%type<ast> ifcommand
%type<ast> returncommand
%type<ast> printcommand
%type<ast> printlist
%type<ast> readcommand
%type<ast> atribuition
%type<ast> expression

%type<ast> varliteral

%type<ast> functioncall
%type<ast> expressionlist

%{
// Declarations to get rid of warnings about not finding function when compiling
// It will work just fine in the linking step of the compilation
int yyerror();
%}

%left '|' '^'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'
%left '~'

%%

programa: declist {$$ = $1; Root = $$; check_and_set_declarations(Root); check_undeclared(); check_operands(Root, 0); /*astPrint($1, 0);*/}
    ;

declist: vardec ';' declist {$$ = astCreate( AST_DECLIST, NULL, $1, $3, NULL, NULL, NULL); /*astPrint($1, 0);*/}
    | funcdec ';' declist   {$$ = astCreate( AST_DECLIST, NULL, $1, $3, NULL, NULL, NULL); /*astPrint($1, 0);*/}
    |                       {$$ = 0;}
    ;

funcdec: TK_IDENTIFIER '(' ')' '=' vartype block                        {$$ = astCreate(AST_FUNC_VOID_DEC, 0, astCreateSymbol($1), $5, $6, 0, 0);}
    | TK_IDENTIFIER '(' assignment assignmentlist ')' '=' vartype block {$$ = astCreate(AST_FUNC_PARAMS_DEC, 0, astCreateSymbol($1), $3, $4, $7, $8);}
    ;

vardec: assignment ':' varliteral {$$ = astCreate(AST_VARDEC, 0, $1, $3, 0, 0, 0); /*fprintf(stderr, "Recebi varliteral %s\n", $3->symbol->text);*/}
    | vector                      {$$ = $1;}
    ;

vartype: KW_CHAR    {$$ = astCreateSymbol($1);}
    | KW_BOOL       {$$ = astCreateSymbol($1);}
    | KW_FLOAT      {$$ = astCreateSymbol($1);}
    | KW_INT        {$$ = astCreateSymbol($1);}
    ;

assignmentlist: ',' assignment assignmentlist   {$$ = astCreate(AST_FUNC_PARAM_ASSIGN_LIST, 0, $2, $3, 0, 0, 0);}
    |                                           {$$ = 0;}
    ;

assignment: TK_IDENTIFIER '=' vartype           {$$ = astCreate(AST_ASSIGNMENT, 0,  astCreateSymbol($1), $3, 0, 0, 0);}
    ;

vector: assignment '[' LIT_INTEGER ']'              {$$ = astCreate(AST_VEC_DEC, 0,  $1,  astCreateSymbol($3), 0, 0, 0);}
    | assignment '[' LIT_INTEGER ']' ':' vecvalues  {$$ = astCreate(AST_VEC_INIT, 0,  $1,  astCreateSymbol($3), $6, 0, 0);}
    ;

vecvalues: varliteral vecvalues {$$ = astCreate(AST_VEC_INIT_VALUES, 0,  $1, $2, 0, 0, 0);}
    |                           {$$ = 0;}
    ;

block: '{' lcmd '}' {$$ = astCreate(AST_BLOCK, 0,  $2, 0, 0, 0, 0);}
    ;

lcmd: cmd lcmd      {$$ = astCreate(AST_LCMDL, 0,  $1, $2, 0, 0, 0);}
    | cmd           {$$ = astCreate(AST_LCMD, 0,  $1, 0, 0, 0, 0);}
    ;

cmd: atribuition    {$$ = $1;}
    | readcommand   {$$ = $1;}
    | printcommand  {$$ = $1;}
    | returncommand {$$ = $1;}
    | flowcommand   {$$ = $1;}
    | block         {$$ = $1;}
    |               {$$ = 0;}
    ;

flowcommand: ifcommand              {$$ = $1;}
    | KW_WHILE '(' expression ')'   {$$ = astCreate(AST_WHILE, 0,  $3, 0, 0, 0, 0);}
    | KW_LOOP '(' TK_IDENTIFIER ':' expression ',' expression ',' expression ')' cmd {$$ = astCreate(AST_LOOP, 0,  astCreateSymbol($3), $5, $7, $9, $11);}
    ;

ifcommand: KW_IF '(' expression ')' KW_THEN cmd {$$ = astCreate(AST_IF, 0,  $3, $6, 0, 0, 0);}
    | KW_IF '(' expression ')' KW_THEN cmd KW_ELSE cmd  {$$ = astCreate(AST_IF_ELSE, 0,  $3, $6, $8, 0, 0);}
    ;

returncommand: KW_RETURN expression         {$$ = astCreate(AST_RETURN, 0,  $2, 0, 0, 0, 0);}
    ;

printcommand: KW_PRINT LIT_STRING printlist {$$ = astCreate(AST_PRINTCMD, 0,  astCreateSymbol($2), $3, 0, 0, 0);}
    | KW_PRINT expression printlist         {$$ = astCreate(AST_PRINTCMD, 0,  $2, $3, 0, 0, 0);}
    ;

printlist: ',' LIT_STRING printlist         {$$ = astCreate(AST_PRINTLIST, 0,  astCreateSymbol($2), $3, 0, 0, 0);}
    | ',' expression printlist              {$$ = astCreate(AST_PRINTLIST, 0,  $2, $3, 0, 0, 0);}
    |                                       {$$ = 0;}
    ;

readcommand: KW_READ TK_IDENTIFIER          {$$ = astCreate(AST_READ, 0,  astCreateSymbol($2), 0, 0, 0, 0);}
    ;

atribuition: TK_IDENTIFIER '=' expression { $$ = astCreate(AST_ATRIBUITION, 0,  astCreateSymbol($1), $3, 0, 0, 0);}
    | TK_IDENTIFIER '[' expression ']' '=' expression {$$ = astCreate(AST_ATRIBUITION_VEC, 0, astCreateSymbol($1), $3, $6, 0, 0);}
    ;

expression: TK_IDENTIFIER               {$$ = astCreateSymbol($1);}
    | TK_IDENTIFIER  '[' expression ']' {$$ = astCreate(AST_VECTOR_ACCESS, 0, astCreateSymbol($1), $3, 0, 0, 0);}
    | varliteral                        {$$ = $1;}
    | expression '+' expression         {$$ = astCreate(AST_ADD, 0, $1, $3, 0, 0, 0);}
    | expression '-' expression         {$$ = astCreate(AST_SUB, 0, $1, $3, 0, 0, 0);}
    | expression '/' expression         {$$ = astCreate(AST_DIV, 0, $1, $3, 0, 0, 0);}
    | expression '*' expression         {$$ = astCreate(AST_MULT, 0, $1, $3, 0, 0, 0);}
    | expression '>' expression         {$$ = astCreate(AST_GREATER, 0, $1, $3, 0, 0, 0);}
    | expression '<' expression         {$$ = astCreate(AST_LESSER, 0, $1, $3, 0, 0, 0);}
    | expression '|' expression         {$$ = astCreate(AST_OR, 0, $1, $3, 0, 0, 0);}
    | expression '^' expression         {$$ = astCreate(AST_AND, 0, $1, $3, 0, 0, 0);}
    | '~' expression                    {$$ = astCreate(AST_NOT, 0, $2, 0, 0, 0, 0);}
    | '-' expression                    {$$ = astCreate(AST_MINUS, 0, $2, 0, 0, 0, 0);}
    | expression OPERATOR_DIF expression{$$ = astCreate(AST_DIF, 0, $1, $3, 0, 0, 0);}
    | expression OPERATOR_EQ expression {$$ = astCreate(AST_EQ, 0, $1, $3, 0, 0, 0);}
    | expression OPERATOR_GE expression {$$ = astCreate(AST_GE, 0, $1, $3, 0, 0, 0);}
    | expression OPERATOR_LE expression {$$ = astCreate(AST_LE, 0, $1, $3, 0, 0, 0);}
    | '(' expression ')'                {$$ = astCreate(AST_PARENTHESIS, 0, $2, 0, 0, 0, 0);}
    | functioncall                      {$$ = $1;}
    ;


varliteral: LIT_CHAR     {$$ = astCreateSymbol($1);}
    | LIT_FALSE          {$$ = astCreateSymbol($1);} 
    | LIT_TRUE           {$$ = astCreateSymbol($1);}
    | LIT_FLOAT          {$$ = astCreateSymbol($1);}
    | LIT_INTEGER        {$$ = astCreateSymbol($1);}
    ;

functioncall: TK_IDENTIFIER '(' ')'                     {$$ = astCreate(AST_FUNC_CALL, 0, astCreateSymbol($1), 0, 0, 0, 0);}
    | TK_IDENTIFIER '(' expression expressionlist ')'   {$$ = astCreate(AST_FUNC_PARAMS_CALL, 0, astCreateSymbol($1), $3, $4, 0, 0);}
    ;
    
expressionlist: ',' expression expressionlist           {$$ = astCreate(AST_EXPRESSION_LIST, 0, $2, $3, 0, 0, 0);}
    |                                                   {$$ = 0;}
    ;


%%

int yyerror(){
    fprintf(stderr, "Syntax error [line:%d]\n", getLineNumber());
    exit(3);
}