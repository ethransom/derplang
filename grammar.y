%{

// this would not be possible without the help of the following article:
// http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/

#include <stdio.h>
#include <stdlib.h>

#include "ast_nodes.h"

#include "debug.h"

void yyerror(const char *s);

#define $$ = malloc(sizeof(NExpression)) $$ = $$ = malloc(sizeof(NExpression))

// #include "node.h"
// NBlock *programBlock; /* the top level root node of our final AST */
ExprList *programBlock; /* the top level root node of our final AST */

// extern int yylex();
// void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%define parse.lac full
%define parse.error verbose

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
%token <string> TIDENTIFIER TINTEGER TDOUBLE TSTRING
%token <token> TKEYWORDIF TKEYWORDFUNCDEF TKEYWORDWHILE TTRUE TFALSE
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACK TRBRACK TCOMMA TDOT
%token <token> TPLUS TMINUS TMUL TDIV

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
// %type <ident>
%type <list> program func_decl_args
// %type <expr> numeric expr
// %type <varvec> func_decl_args
// %type <exprvec> call_args
// %type <block> program stmts block
// %type <stmt> stmt var_assign func_decl
// %type <stmt> stmt
%type <expr_list> stmts call_args block
// %type <stmts> stmt
%type <expression> var_assign stmt expr literal ident if_structure
%type <expression> func_decl anon_func_decl while_structure
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

stmt : var_assign | if_structure | while_structure | func_decl | expr
     ;
// stmt : var_assign | func_decl
//      | expr { $$ = new NExpressionStatement(*$1); }
//      ;

var_assign : TIDENTIFIER TEQUAL expr { $$ = ast_expr_new(NASSIGNMENT); $$->assignment.name = $1; $$->assignment.val = $3; }
         ;

block : TLBRACE stmts TRBRACE { $$ = $2; }
      | TLBRACE TRBRACE { $$ = List_create(); }
      ;

if_structure : TKEYWORDIF expr block  { $$ = ast_expr_new(NIFSTRUCTURE); $$->if_structure.expr = $2; $$->if_structure.block = $3; }

while_structure : TKEYWORDWHILE expr block { $$ = ast_expr_new(NWHILESTRUCTURE); $$->while_structure.expr = $2; $$->if_structure.block = $3; }

func_decl :  TKEYWORDFUNCDEF TIDENTIFIER TLPAREN func_decl_args TRPAREN block
            { $$ = ast_expr_new(NFUNCDEF); $$->func_def.name = $2; $$->func_def.arg_list = $4; $$->func_def.block = $6; /*delete $4;*/ }
          ;

anon_func_decl : TKEYWORDFUNCDEF TLPAREN func_decl_args TRPAREN block { $$ = ast_expr_new(NANONFUNCDEF); $$->anon_func_def.arg_list = $3; $$->anon_func_def.block = $5;}

func_decl_args : /*blank*/  { $$ = List_create(); }
          | TIDENTIFIER { $$ = List_create(); List_push($$, $1); }
          | func_decl_args TCOMMA TIDENTIFIER { List_push($1, $3); }
          ;

ident : TIDENTIFIER { debug("casting ident into expr"); $$ = ast_expr_new(NLOOKUP); $$->lookup.name = $1; /*delete $1;*/ /*$1;*/ }
      ;

literal : TINTEGER { $$ = ast_expr_new(NINTEGER); $$->integer = atol($1); free($1); }
        | TDOUBLE  { $$ = ast_expr_new(NDOUBLE);  $$->tdouble = atof($1); free($1); }
        | TSTRING  { $$ = ast_expr_new(NSTRING);  $$->string = $1; }
        | TTRUE    { $$ = ast_expr_new(NBOOL);    $$->tbool = true; }
        | TFALSE   { $$ = ast_expr_new(NBOOL);    $$->tbool = false; }
        | TLBRACK call_args TRBRACK { $$ = ast_expr_new(NARRAY); $$->array = $2; }
        ;

expr : TIDENTIFIER TLPAREN call_args TRPAREN { $$ = ast_expr_new(NCALL); $$->call.name = $1; $$->call.args = $3; /*delete $3;*/ }
     | ident /*{ $$ = ast_expr_new(NLOOKUP); $$->lookup.name = $1; }*/
     | literal
     | expr comparison expr { $$ = ast_expr_new(NBINARYOP);
     						  $$->binary_op.left  = $1;
     						  $$->binary_op.op 	  = $2;
     						  $$->binary_op.right = $3; }
     | TLPAREN expr TRPAREN { $$ = $2; }
     | anon_func_decl
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