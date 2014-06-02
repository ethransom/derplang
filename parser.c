#include "parser.h"

#include "debug.h"

bool cream_parse(char* input) {
	/* Create Some Parsers */
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr     = mpc_new("expr");
	mpc_parser_t* Lispy    = mpc_new("lispy");

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT,
		"                                                     \
		number   : /-?[0-9]+/ ;                             \
		operator : '+' | '-' | '*' | '/' ;                  \
		expr     : <number> | '(' <operator> <expr>+ ')' ;  \
		lispy    : /^/ <operator> <expr>+ /$/ ;             \
		",
	Number, Operator, Expr, Lispy);

	bool status;

	/* Attempt to Parse the user Input */
	mpc_result_t r;
	if (mpc_parse("<stdin>", input, Lispy, &r)) {
		/* On Success Print the AST */
		mpc_ast_print(r.output);
		mpc_ast_delete(r.output);
		status = true;
	} else {
		/* Otherwise Print the Error */
		mpc_err_print(r.error);
		mpc_err_delete(r.error);
		status = false;
	}

	/* Undefine and Delete our Parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return status;
}
