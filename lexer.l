%{
#include <stdio.h>
#include <string.h>

#include "ast_nodes.h"
#include "lexer.h"
#include "grammar.tab.h"

#define SAVE_TOKEN yylval.string = strdup(yytext)
#define TOKEN(t) (yylval.token = t)

%}
%%

[ \t\n]                 ;
"if"					return TOKEN(TKEYWORDIF);
"fn"					return TOKEN(TKEYWORDFUNCDEF);
\"(\\.|[^"])*\" 	    SAVE_TOKEN; return TSTRING; /* " */
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return TIDENTIFIER;
[0-9]+\.[0-9]*          SAVE_TOKEN; return TDOUBLE;
[0-9]+                  SAVE_TOKEN; return TINTEGER;
"="                     return TOKEN(TEQUAL);
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
"{"                     return TOKEN(TLBRACE);
"}"                     return TOKEN(TRBRACE);
"."                     return TOKEN(TDOT);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);
.                       printf("Unknown token!\n"); yyterminate();

%%
// [ \t\r] 			;  // ignore all whitespace
// [0-9]+\.[0-9]+  	{ yylval.fval = atof(yytext); return FLOAT; printf("Flex found a floating-point number: %s\n", yytext); }
// [0-9]+          	{ yylval.ival = atoi(yytext); return INT;	printf("Flex found an integer: %s\n", yytext); }
// [a-zA-Z0-9]+    	{
	// we have to copy because we can't rely on yytext not changing underneath us:
	// yylval.sval = strdup(yytext);
	// return STRING;
	// printf("Flex found an identifier: %s\n", yytext);
// }
