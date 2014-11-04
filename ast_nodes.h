/*
 * An Abstract Syntax Tree is the ideal in-memory representation of a program
 * Contains types and methods for printing, deallocating, and converting to bytecodes
*/

#pragma once

#include <stdbool.h>

#include "list.h"
#include "vm.h"
#include "bytecodes.h"

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

// TODO: There's some duplication here. Remove?
typedef struct {
	struct nExpression* expr;
	ExprList* block;
} NIfStructure;

typedef struct {
	struct nExpression* expr;
	ExprList* block;
} NWhileStructure;

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
	NBOOL,
	NARRAY,
	NCALL,
	NBINARYOP,
	NLOOKUP,
	NIFSTRUCTURE,
	NWHILESTRUCTURE,
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
		bool tbool;
		ExprList* array;
		NCall call;
		NBinaryOp binary_op;
		NLookup lookup;
		NIfStructure if_structure;
		NWhileStructure while_structure;
		// NForStructure for_structure;
		NFuncDef func_def;
	};
} NExpression;

NExpression* ast_expr_new(ExprType type);


void ast_list_print(ExprList* list, int indent);

void ast_exp_print(NExpression* expr, int indent);


void ast_list_free(ExprList* list);

void ast_expr_free(NExpression* expr);


bool ast_compile(ExprList* root, file_blob_t* output);

bool ast_list_compile(ExprList* list, List* output, file_blob_t* blob);

bool ast_expr_compile(NExpression* expr, List* output, file_blob_t* blob);
