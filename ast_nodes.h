#pragma once

#include <stdbool.h>

#include "list.h"
#include "vm.h"

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

typedef struct {
	char* name;
} NLookup;

typedef struct {
	struct nExpression* expr;
	ExprList* block;
} NIfStructure;

// typedef struct {
// 	char* name;
// } NForStructure;

typedef struct {
	char* name;
	List* arg_list;
	ExprList* block;
} NFuncDef;

// expression superclass
typedef enum {
	NNULL, // if the default is NASSIGNMENT, mistakes lead to segfaults
	NASSIGNMENT,
	NINTEGER,
	NDOUBLE,
	NSTRING,
	NCALL,
	NBINARYOP,
	NLOOKUP,
	NIFSTRUCTURE,
	// NFORSTRUCTURE
	NFUNCDEF
} ExprType;

typedef struct nExpression {
	ExprType type;
	union {
		NAssignment assignment;
		int integer;
		double tdouble;
		char* string;
		NCall call;
		NBinaryOp binary_op;
		NLookup lookup;
		NIfStructure if_structure;
		// NForStructure for_structure;
		NFuncDef func_def;
	};
} NExpression;

NExpression* ast_expr_new(ExprType type);


void ast_list_print(ExprList* list, int indent);

void ast_exp_print(NExpression* expr, int indent);


void ast_list_free(ExprList* list);

void ast_expr_free(NExpression* expr);


bool ast_compile(ExprList* root, List* bytecodes);

bool ast_list_compile(ExprList* list, List* output);

bool ast_expr_compile(NExpression* expr, List* output);
