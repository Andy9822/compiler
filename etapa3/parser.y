%{
    #include "hash.h"
%}

%union
{
    HASH_NODE *symbol;
}

%token KW_CHAR      
%token KW_INT       
%token KW_FLOAT     
%token KW_BOOL      

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
%token LIT_STRING   
%token TOKEN_ERROR

%{
// Declarations to get rid of warnings abount not finding function when compiling
// It will work just fine in the linking step of the compilation
int yyerror();
%}

%left '|' '^'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'
%left '~'

%%

programa: declist
    ;

declist: vardec ';' declist
    | funcdec ';' declist
    |
    ;

funcdec: TK_IDENTIFIER '(' ')' '=' vartype block
    | TK_IDENTIFIER '(' assignment assignmentlist ')' '=' vartype block
    ;

vardec: assignment ':' varliteral 
    | vector
    ;

vartype: KW_CHAR
    | KW_BOOL
    | KW_FLOAT
    | KW_INT
    ;

assignmentlist: ',' assignment assignmentlist 
    |
    ;

assignment: TK_IDENTIFIER '=' vartype
    ;

vector: assignment '[' LIT_INTEGER ']'
    | assignment '[' LIT_INTEGER ']' ':' vecvalues
    ;

vecvalues: varliteral vecvalues
    |
    ;

block: '{' lcmd '}'
    ;

lcmd: cmd lcmd
    | cmd
    ;

cmd: atribuition
    | readcommand
    | printcommand
    | returncommand
    | flowcommand
    | block
    |
    ;

flowcommand: ifcommand
    | KW_WHILE '(' expression ')'
    | KW_LOOP '(' TK_IDENTIFIER ':' expression ',' expression ',' expression ')' cmd
    ;

ifcommand: KW_IF '(' expression ')' KW_THEN cmd
    | KW_IF '(' expression ')' KW_THEN cmd KW_ELSE cmd
    ;

returncommand: KW_RETURN expression
    ;

printcommand: KW_PRINT LIT_STRING printlist
    | KW_PRINT expression printlist
    ;

printlist: ',' LIT_STRING printlist
    | ',' expression printlist
    |
    ;

readcommand: KW_READ TK_IDENTIFIER
    ;

atribuition: TK_IDENTIFIER '=' expression
    | TK_IDENTIFIER '[' expression ']' '=' expression
    ;

expression: TK_IDENTIFIER  {fprintf(stderr, "Recebi %s\n", $1->text);}
    | TK_IDENTIFIER  '[' expression ']'
    | varliteral 
    | expression '+' expression 
    | expression '-' expression 
    | expression '/' expression 
    | expression '*' expression 
    | expression '>' expression 
    | expression '<' expression 
    | expression '|' expression
    | expression '^' expression
    | '~' expression 
    | '-' expression 
    | expression OPERATOR_DIF expression
    | expression OPERATOR_EQ expression
    | expression OPERATOR_GE expression
    | expression OPERATOR_LE expression
    | '(' expression ')' 
    | functioncall 
    ;


varliteral: LIT_CHAR
    | LIT_FALSE 
    | LIT_TRUE
    | LIT_FLOAT
    | LIT_INTEGER  {fprintf(stderr, "Recebi %s\n", $1->text);}
    ;

functioncall: TK_IDENTIFIER '(' ')'
    | TK_IDENTIFIER '(' expression expressionlist ')'
    ;
    
expressionlist: ',' expression expressionlist
    |
    ;


%%

int yyerror(){
    fprintf(stderr, "Syntax error [line:%d]\n", getLineNumber());
    exit(3);
}