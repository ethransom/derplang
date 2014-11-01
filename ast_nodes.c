#include <stdlib.h>
#include <stdio.h>

#include "ast_nodes.h"

#include "grammar.tab.h"

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
		case NBOOL:
			treeprintf(indent, "bool val: %s\n", expr->tbool ? "true" : "false");
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
		case NWHILESTRUCTURE:
			treeprintf(indent, "NWhileStructure\n");
				treeprintf(indent + 1, "expr:\n");
					ast_exp_print(expr->while_structure.expr, indent + 1);
				treeprintf(indent + 1, "block:\n");
					ast_list_print(expr->while_structure.block, indent + 1);
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
		case NWHILESTRUCTURE:
			ast_expr_free(expr->while_structure.expr);
			ast_list_free(expr->while_structure.block);
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
		case NBOOL:
			break;

		case NNULL:
		default:
			log_err("tried to free AST node of unknown type '%d'!!!", expr->type);
			break;
	}

	free(expr);
}

bool ast_compile(ExprList* root, file_blob_t* blob) {
	// initialize the file blob
	blob->fn_c = 0;

	// first function always for global expressions
	fn_blob_t* fn = file_blob_add_fn(blob, "");
	check_mem(fn);

	List list; // used to store bytecodes for expressions in global scope
	List_init(&list);

	if (!ast_list_compile(root, &list, blob))
		return false;

	fn->blob_len = list.length;
	fn->blob = bytecodes_compress(&list);
	List_clear(&list);

	return true;

error:
	return false;
}

bool ast_list_compile(ExprList* list, List* output, file_blob_t* blob) {
	LIST_FOREACH(list, first, next, cur) {
		NExpression* expr = cur->data;
		check(ast_expr_compile(expr, output, blob), "List element couldn't compile");
	}
	return true;

error:
	return false;
}

instr* _mkbytecode(Cream_code_type type, char* str, int i, double f) {
	instr* code = malloc(sizeof(instr));
	check_mem(code);
	code->code = type;
	code->arg1 = str;
	code->arg2 = i;
	code->float_val = f;
	return code;
error:
	return NULL;
}

bool ast_expr_compile(NExpression* expr, List* output, file_blob_t* blob) {
	instr* tmp; // used in the following macro
#define CODE(TYPE, STR, INT, FLOAT) tmp = _mkbytecode(TYPE, STR, INT, FLOAT); \
	check(tmp, "Unable to allocate bytecode"); \
	List_push(output, (void*) tmp);

	debug("compiling %d", expr->type);

	switch(expr->type) {
		case NASSIGNMENT:
			check(ast_expr_compile(expr->assignment.val, output, blob), "NASSIGNMENT couldn't compile!");
			CODE(CODE_ASSIGN, expr->assignment.name, 0, 0.0);
			break;
		case NINTEGER: {
			CODE(CODE_PUSH_INT, NULL, expr->integer, 0.0);
		}
			break;
		case NDOUBLE: {
			CODE(CODE_PUSH_FLOAT, NULL, 0, expr->tdouble);
		}
			break;
		case NSTRING:
			CODE(CODE_PUSH_STR, expr->string, 0, 0.0);
			break;
		case NBOOL:
			CODE(CODE_PUSH_BOOL, NULL, expr->tbool, 0.0);
			break;
		case NCALL:
			check(ast_list_compile(expr->call.args, output, blob), "NCALL couldn't compile!");
			CODE(CODE_CALL, expr->call.name, expr->call.args->length, 0.0);
			debug("Added call...");
			break;
		case NBINARYOP:
			// postfix, so make sure the operands get pushed first
			ast_expr_compile(expr->binary_op.left, output, blob);
			ast_expr_compile(expr->binary_op.right, output, blob);

			// the type of token denotes what type of operation we're performing
			// we need to generate a bytecode based on this
			Cream_code_type type = 0;
			switch (expr->binary_op.op) {
				case TPLUS:		type = CODE_ADD; break;
				case TMINUS:	type = CODE_SUB; break;
				case TMUL:		type = CODE_MUL; break;
				case TDIV:		type = CODE_DIV; break;
				case TCEQ:		type = CODE_CMP_EQ; break;
				case TCNE:		type = CODE_CMP_NEQ; break;
				case TCLT:		type = CODE_CMP_LT; break;
				case TCLE:		type = CODE_CMP_LT_EQ; break;
				case TCGT:		type = CODE_CMP_GT; break;
				case TCGE:		type = CODE_CMP_GT_EQ; break;
				default:
					sentinel("Unknown binary_op type: %d\n", expr->binary_op.op);
			}
			CODE(type, NULL, 0, 0.0);

			break;
		case NLOOKUP:
			CODE(CODE_PUSH_LOOKUP, expr->lookup.name, 0, 0.0);
			break;
		case NIFSTRUCTURE:
			ast_expr_compile(expr->if_structure.expr, output, blob);

			CODE(CODE_JUMP_IF_FALSE, NULL, 0, 0.0);
			// keep a pointer to update once we know the body length
			instr* jump = output->last->data;

			int len_before_if = output->length;

			ast_list_compile(expr->if_structure.block, output, blob);

			jump->arg2 = output->length - len_before_if;
			break;
		case NWHILESTRUCTURE: {
			int len_before_while = output->length;

			ast_expr_compile(expr->while_structure.expr, output, blob);

			CODE(CODE_JUMP_IF_FALSE, NULL, 0, 0.0);
			// keep a pointer to update once we know the body length
			instr* jump = output->last->data;

			int len_before_body = output->length;

			ast_list_compile(expr->while_structure.block, output, blob);

			CODE(CODE_JUMP, NULL, len_before_while, 0.0);

			jump->arg2 = output->length - len_before_body;
		}
			break;
		case NFUNCDEF: {
			List list;
			List_init(&list);
			// SUPER HACK!!!!!!!!
			// replace output with our new list
			output = &list;

			debug("encountered function %s\n", expr->func_def.name);

			fn_blob_t* fn = file_blob_add_fn(blob, expr->func_def.name);
			fn->argc = expr->func_def.arg_list->length;

			LIST_FOREACH(expr->func_def.arg_list, first, next, cur) {
				CODE(CODE_ASSIGN, cur->data, 0, 0.0);
			}

			ast_list_compile(expr->func_def.block, &list, blob);

			CODE(CODE_RET, NULL, 0, 0.0);

			fn->blob_len = list.length;
			fn->blob = bytecodes_compress(&list);
			check_mem(fn->blob);
			debug("Function %s contains %zd bytecodes", fn->name, fn->blob_len);
		}
			break;
		case NNULL:
		default:
			sentinel("Tried to compile unknown bytecode: '%d'", expr->type);
			break;
	}
	return true;
error:
	return false;
}
