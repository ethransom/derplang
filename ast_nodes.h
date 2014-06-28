#pragma once

#include "list.h"

#define MAKE_EXPR 

typedef List ExprList;

// just a placeholder to prevent circular declaration
struct nExpression;

typedef struct {
	char* name;
	struct nExpression* val;
} NAssignment;

typedef struct {
	char* name;
	ExprList* args;
} NCall;

typedef struct {
	struct nExpression* left;
	int op;
	struct nExpression* right;
} NBinaryOp;

// expression superclass
typedef enum {
	NNULL, // if the default is NASSIGNMENT, mistakes lead to segfaults
	NASSIGNMENT,
	NINTEGER,
	NDOUBLE,
	NCALL,
	NBINARYOP
} ExprType;

typedef struct nExpression {
	ExprType type;
	union {
		NAssignment assignment;
		int integer;
		double tdouble;
		NCall call;
		NBinaryOp binary_op;
	};
} NExpression;

NExpression* ast_expr_new(ExprType type);

void ast_list_print(ExprList* list, int indent);

void ast_exp_print(NExpression* expr, int indent);
