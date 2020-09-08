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

funcdec: TK_IDENTIFIER '(' ')' '=' vartype body
    | TK_IDENTIFIER '(' assignment assignmentlist ')' '=' vartype body
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

body: '{' lcmd '}'
    ;

lcmd: cmd lcmd
    |
    ;

cmd: atribuition
    | readcommand
    | printcommand
    | returncommand
    | flowcommand
    | body
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

expression: TK_IDENTIFIER 
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
    | LIT_INTEGER
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