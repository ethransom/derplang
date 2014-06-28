%{

// this would not be possible without the help of the following article:
// http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/

#include <stdio.h>
#include <stdlib.h>

#include "ast_nodes.h"

void yyerror(const char *s);

#define $$ = malloc(sizeof(NExpression)) $$ = $$ = malloc(sizeof(NExpression))

// #include "node.h"
// NBlock *programBlock; /* the top level root node of our final AST */
ExprList *programBlock; /* the top level root node of our final AST */

// extern int yylex();
// void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    // Node *node;
    List *list;
    // NBlock *block;
    // NExpression *expr;
    ExprList* expr_list;
    // NIdentifier *ident;
    char *ident;
    NAssignment *assignment;
    NExpression *expression;
    // std::vector<NVariableDeclaration*> *varvec;
    // std::vector<NExpression*> *exprvec;
    char *string;
    int token;
}

/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <string> TIDENTIFIER TINTEGER TDOUBLE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <ident> ident
%type <list> program
// %type <expr> numeric expr 
// %type <varvec> func_decl_args
// %type <exprvec> call_args
// %type <block> program stmts block
// %type <stmt> stmt var_assign func_decl
// %type <stmt> stmt
%type <expr_list> stmts call_args
// %type <stmts> stmt
%type <expression> var_assign stmt expr numeric
%type <token> comparison

/* Operator precedence for mathematical operators */
%left TPLUS TMINUS
%left TMUL TDIV

%start program

%%

program : stmts { programBlock = $1; }
        ;

// program : var_assign { programBlock = $1; }
        // ;
        
stmts : stmt { $$ = List_create(); List_push($$, $1); }
      | stmts stmt { List_push($1, $2); }
      ;

stmt : var_assign | expr
     ;
// stmt : var_assign | func_decl
//      | expr { $$ = new NExpressionStatement(*$1); }
//      ;

// block : TLBRACE stmts TRBRACE { $$ = $2; }
//       | TLBRACE TRBRACE { $$ = new NBlock(); }
//       ;

// var_assign : ident ident { $$ = new NVariableDeclaration(*$1, *$2); }
         // | ident ident TEQUAL expr { $$ = new NVariableDeclaration(*$1, *$2, $4); }
         // ;
var_assign : ident TEQUAL expr { $$ = ast_expr_new(NASSIGNMENT); $$->assignment.name = $1; $$->assignment.val = $3; }
         ;
        
// func_decl : ident ident TLPAREN func_decl_args TRPAREN block 
//             { $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
//           ;
    
// func_decl_args : /*blank*/  { $$ = new VariableList(); }
//           | var_assign { $$ = new VariableList(); $$->push_back($<var_assign>1); }
//           | func_decl_args TCOMMA var_assign { $1->push_back($<var_assign>3); }
//           ;

ident : TIDENTIFIER { $$ = /*new NIdentifier(*$1); delete $1;*/ $1; }
      ;

numeric : TINTEGER { $$ = ast_expr_new(NINTEGER); $$->integer = atol($1); /*delete $1;*/ }
        | TDOUBLE { $$ = ast_expr_new(NDOUBLE); $$->tdouble = atof($1); /*delete $1;*/ }
        ;
    
expr : ident TLPAREN call_args TRPAREN { $$ = ast_expr_new(NCALL); $$->call.name = $1; $$->call.args = $3; /*delete $3;*/ }
     | ident { $<ident>$ = $1; }
     | numeric
     | expr comparison expr { $$ = ast_expr_new(NBINARYOP);
     						  $$->binary_op.left  = $1;
     						  $$->binary_op.op 	  = $2;
     						  $$->binary_op.right = $3; }
     | TLPAREN expr TRPAREN { $$ = $2; }
     ;
    
call_args : /*blank*/  { $$ = List_create(); }
          | expr { $$ = List_create(); List_push($$, $1); }
          | call_args TCOMMA expr  { List_push($1, $3); }
          ;

comparison : TCEQ | TCNE | TCLT | TCLE | TCGT | TCGE 
           | TPLUS | TMINUS | TMUL | TDIV
           ;

%%

void yyerror(const char *s) {
	fprintf(stderr, "EEK, parse error!  Message: %s\n", s);
	// might as well halt now:
	exit(-1);
}