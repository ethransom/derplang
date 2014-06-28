#include <stdlib.h>
#include <stdio.h>

#include "ast_nodes.h"

#define treeprintf(NUM, STR, ...) printf("%*s" STR, (NUM) * 4, " ", ##__VA_ARGS__)

NExpression* ast_expr_new(ExprType type) {
	NExpression* expr = malloc(sizeof(NExpression));
	expr->type = type;
	return expr;
}

void ast_list_print(List* list, int indent) {
	LIST_FOREACH(list, first, next, cur) {
		NExpression* expr = cur->data;
		ast_exp_print(expr, indent);
	}
}

void ast_exp_print(NExpression* expr, int indent) {
	indent += 1;
	switch(expr->type) {
		case NNULL:
			fprintf(stderr, "tried to print AST Node of unknown type!!!\n");
			break;
		case NASSIGNMENT:
			treeprintf(indent, "NAssignment name: %s, val:\n", expr->assignment.name);
			ast_exp_print(expr->assignment.val, indent);
			break;
		case NINTEGER:
			treeprintf(indent, "integer val: %d\n", expr->integer);
			break;
		case NDOUBLE:
			treeprintf(indent, "double val: %f\n", expr->tdouble);
			break;
		case NCALL:
			treeprintf(indent, "NCall name: %s, args:\n", expr->call.name);
			ast_list_print(expr->call.args, indent);
			break;
		case NBINARYOP:
			treeprintf(indent, "NBinaryOp op: %d\n", expr->binary_op.op);
				treeprintf(indent + 1, "left:\n");
					ast_exp_print(expr->binary_op.left, indent + 1);
				treeprintf(indent + 1, "right:\n");
					ast_exp_print(expr->binary_op.right, indent + 1);
			break;
	}
}