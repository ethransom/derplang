%{
#include <stdio.h>
#include <string.h>

// #define _POSIX_C_SOURCE 1

#include "lexer.h"
#include "grammar.tab.h"

%}
%%
[ \t\r] 			;  // ignore all whitespace
[0-9]+\.[0-9]+  	{ yylval.fval = atof(yytext); return FLOAT; printf("Flex found a floating-point number: %s\n", yytext); }
[0-9]+          	{ yylval.ival = atoi(yytext); return INT;	printf("Flex found an integer: %s\n", yytext); }
[a-zA-Z0-9]+    	{ 
	// we have to copy because we can't rely on yytext not changing underneath us:
	yylval.sval = strdup(yytext);
	return STRING;
	printf("Flex found an identifier: %s\n", yytext);
}
%%
// # \"(\\.|[^"])*\" 	{ printf("Flex found a string: %s\n", yytext); } // foobar "
