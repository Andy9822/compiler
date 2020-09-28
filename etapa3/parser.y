%{
    #include "ast.h"
    int getLineNumber(void);
    int yylex();    // TODO find better workaround
%}

%union
{
    HASH_NODE *symbol;
    AST *ast;
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

%type<ast> expression
%type<ast> varliteral

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

atribuition: TK_IDENTIFIER '=' expression {astPrint($3);}
    | TK_IDENTIFIER '[' expression ']' '=' expression
    ;

expression: TK_IDENTIFIER  {$$ = AST_CREATE_SYMBOL($1);}
    | TK_IDENTIFIER  '[' expression ']' {$$ = astCreate(AST_VECTOR_ACCESS, 0, AST_CREATE_SYMBOL($1), $3, 0, 0, 0);}
    | varliteral                {$$ = $1;}
    | expression '+' expression {$$ = astCreate(AST_ADD, 0, $1, $3, 0, 0, 0);}
    | expression '-' expression {$$ = astCreate(AST_SUB, 0, $1, $3, 0, 0, 0);}
    | expression '/' expression 
    | expression '*' expression 
    | expression '>' expression 
    | expression '<' expression 
    | expression '|' expression
    | expression '^' expression
    | '~' expression            {$$ = astCreate(AST_NOT, 0, $2, 0, 0, 0, 0);}
    | '-' expression            {$$ = astCreate(AST_MINUS, 0, $2, 0, 0, 0, 0);}
    | expression OPERATOR_DIF expression
    | expression OPERATOR_EQ expression
    | expression OPERATOR_GE expression
    | expression OPERATOR_LE expression
    | '(' expression ')'            {$$ = $2;}
    | functioncall                  
    ;


varliteral: LIT_CHAR     {$$ = AST_CREATE_SYMBOL($1);}
    | LIT_FALSE          {$$ = AST_CREATE_SYMBOL($1);} 
    | LIT_TRUE           {$$ = AST_CREATE_SYMBOL($1);}
    | LIT_FLOAT          {$$ = AST_CREATE_SYMBOL($1);}
    | LIT_INTEGER        {$$ = AST_CREATE_SYMBOL($1);}
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