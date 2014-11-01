%{
#include <stdio.h>
#include <string.h>

#include "ast_nodes.h"
#include "lexer.h"
#include "grammar.tab.h"

#define SAVE_TOKEN yylval.string = strdup(yytext)
#define TOKEN(t) (yylval.token = t)

#define SAVE_TOKEN_NO_QUOTES \
	yylval.string = malloc(strlen(yytext) - 1); \
	memcpy(yylval.string, (yytext + 1), strlen(yytext) - 1); \
	yylval.string[strlen(yytext) - 2] = '\0';

%}

%%
[ \t\n]                 ;
"if"					return TOKEN(TKEYWORDIF);
"while"					return TOKEN(TKEYWORDWHILE);
"fn"					return TOKEN(TKEYWORDFUNCDEF);
"true"					return TOKEN(TTRUE);
"false"					return TOKEN(TFALSE);
\"(\\.|[^"])*\" 	    SAVE_TOKEN_NO_QUOTES; return TSTRING; /* " */
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
