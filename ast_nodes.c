#include <stdlib.h>
#include <stdio.h>

#include "ast_nodes.h"

#include "debug.h"

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
		case NSTRING:
			treeprintf(indent, "string val: %s\n", expr->string);
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
		case NLOOKUP:
			treeprintf(indent, "NLookup name: %s\n", expr->lookup.name);
			break;
		case NIFSTRUCTURE:
			treeprintf(indent, "NIfStructure\n");
				treeprintf(indent + 1, "expr:\n");
					ast_exp_print(expr->if_structure.expr, indent + 1);
				treeprintf(indent + 1, "block:\n");
					ast_list_print(expr->if_structure.block, indent + 1);
			break;
		case NFUNCDEF:
			treeprintf(indent, "NFuncDef name: %s\n", expr->func_def.name);
				treeprintf(indent + 1, "args:\n");
				LIST_FOREACH(expr->func_def.arg_list, first, next, cur) {
					treeprintf(indent + 2, "%s\n", (char*) cur->data);
				}
				treeprintf(indent + 1, "block:\n");
					ast_list_print(expr->func_def.block, indent + 1);
			break;
		case NNULL:
		default:
			fprintf(stderr, "tried to print AST Node of unknown type!!!\n");
			break;
	}
}

void ast_list_free(ExprList* list) {
	LIST_FOREACH(list, first, next, cur) {
		NExpression* expr = cur->data;
		ast_expr_free(expr);
	}
	List_destroy(list);
}

// DONT FREE STRINGS
// They are owned by bytecode
void ast_expr_free(NExpression* expr) {
	// make sure node's children get freed
	switch(expr->type) {
		case NASSIGNMENT:
			// free(expr->assignment.name);
			ast_expr_free(expr->assignment.val);
			break;
		case NCALL:
			// free(expr->call.name);
			ast_list_free(expr->call.args);
			break;
		case NBINARYOP:
			ast_expr_free(expr->binary_op.left);
			ast_expr_free(expr->binary_op.right);
			break;
		case NLOOKUP:
			// free(expr->lookup.name);
			break;
		case NIFSTRUCTURE:
			ast_expr_free(expr->if_structure.expr);
			ast_list_free(expr->if_structure.block);
			break;
		case NFUNCDEF: {
			// free(expr->func_def.name);
			LIST_FOREACH(expr->func_def.arg_list, first, next, cur) {
				free(cur->data);
			}
			ast_list_free(expr->func_def.block);
    }
			break;

		case NINTEGER:
		case NDOUBLE:
		case NSTRING:
			break;

		case NNULL:
		default:
			log_err("tried to free AST node of unknown type '%d'!!!", expr->type);
			break;
	}

	free(expr);
}

bool ast_compile(ExprList* root, List* bytecodes) {
	// List* bytecodes = List_create();

	ast_list_compile(root, bytecodes);

	return true;
}

void ast_list_compile(ExprList* list, List* output) {
	LIST_FOREACH(list, first, next, cur) {
		NExpression* expr = cur->data;
		ast_expr_compile(expr, output);
	}
}

instr* _mkbytecode(Cream_code_type type, char* str, int i) {
	instr* code = malloc(sizeof(instr));
	check_mem(code);
	code->code = type;
	code->arg1 = str;
	code->arg2 = i;
	return code;
error:
	return NULL;
}

void ast_expr_compile(NExpression* expr, List* output) {
	instr* tmp; // used in the following macro
#define CODE(TYPE, STR, INT) tmp = _mkbytecode(TYPE, STR, INT); \
	check(tmp, "Unable to allocate bytecode"); \
	List_push(output, (void*) tmp);

	debug("compiling %d", expr->type);

	switch(expr->type) {
		case NASSIGNMENT:
			ast_expr_compile(expr->assignment.val, output);
			CODE(CODE_ASSIGN, expr->assignment.name, 0);
			break;
		case NINTEGER: {
			// int* ptr = malloc(sizeof(int));
			// memcpy(ptr, &expr->integer, sizeof(int));
			// CODE(CODE_PUSH_INT, (char*) ptr, expr->integer);
			CODE(CODE_PUSH_INT, NULL, expr->integer);
		}
			break;
		case NDOUBLE:
			break;
		case NSTRING:
			CODE(CODE_PUSH_STR, expr->string, 0);
			break;
		case NCALL:
			ast_list_compile(expr->call.args, output);
			CODE(CODE_CALL, expr->call.name, expr->call.args->length);
			debug("Added call...");
			break;
		case NBINARYOP:
			// ast_expr_compile(expr->binary_op.left, output);
			// ast_expr_compile(expr->binary_op.right, output);
			break;
		case NLOOKUP:
			CODE(CODE_PUSH_LOOKUP, expr->lookup.name, 0);
			break;
		case NIFSTRUCTURE:
			break;
		case NFUNCDEF:
			break;
		case NNULL:
		default:
			sentinel("Tried to compile unknown bytecode: '%d'", expr->type);
			break;
	}
	return;
error:
	return;
}
