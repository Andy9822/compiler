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
%token TK_IDENTIFIER

%token LIT_INTEGER  
%token LIT_FLOAT    
%token LIT_TRUE     
%token LIT_FALSE    
%token LIT_CHAR     
%token LIT_STRING   
%token TOKEN_ERROR

%left '|' '&'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

%%

programa: decl
    ;

decl: dec resto
    |
    ;

resto: ';' decl
    |
    ;

dec: vardec
    | KW_INT TK_IDENTIFIER '(' ')' body
    ;

vardec: TK_IDENTIFIER '=' vartype ':' varliteral 
    | vector
    ;

vartype: KW_CHAR
    | KW_BOOL
    | KW_FLOAT
    | KW_INT
    ;

varliteral: LIT_CHAR
    | LIT_FALSE
    | LIT_TRUE
    | LIT_FLOAT
    | LIT_INTEGER
    ;

vector: TK_IDENTIFIER '=' vartype '[' LIT_INTEGER ']'
    | TK_IDENTIFIER '=' vartype '[' LIT_INTEGER ']' ':' vecvalues
    ;

vecvalues: varliteral vecvalues
    |
    ;

body: '{' lcmd '}'
    |
    ;

lcmd: cmd lcmd
    |
    ;

cmd: TK_IDENTIFIER '=' expression
    | KW_IF expression cmd
    | KW_IF expression cmd KW_ELSE cmd
    ;

expression: LIT_INTEGER
    | TK_IDENTIFIER
    | expression '+' expression
    | expression '-' expression
    | expression '/' expression
    | expression '*' expression
    | expression OPERATOR_DIF expression
    | expression OPERATOR_EQ expression
    | expression OPERATOR_GE expression
    | expression OPERATOR_LE expression
    | '(' expression ')'
    ;

%%

int yyerror(){
    fprintf(stderr, "Syntax error [line:%d]\n", getLineNumber());
    exit(3);
}