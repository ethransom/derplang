/*
 * Implements the Derplang virtual machine
 * This includes the mark phase of garbage collection
*/

#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "map.h"
#include "debug.h"
#include "object.h"
#include "bytecodes.h"
#include "err.h"

#define VM_STACK_HEIGHT 64

typedef enum {
  FN_BYTECODE,
  FN_ANON,
  FN_NATIVE,

  // FN_AST?
} fn_type_t;

typedef struct {
  instr* blob;
  size_t blob_len;
  // is only updated for anonymous fns
  size_t argc;
  // size_t varc; // number of local variables
} FnBytecode;

typedef struct {
  fn_type_t type;
  char* name;
  size_t argc; // number of parameters

  union {
    FnBytecode bytecode;
    // struct native {

    // };
  };
} fn_blob_t;

typedef struct {
  // NOTE: We might want to be storing some sort of type here in the future
  union {
    FnBytecode anon_fn;
    // TODO:
    char* string;
  };
} literal_t;

typedef struct {
  char* name;
  fn_blob_t* fns[4];
  size_t fn_c;
  literal_t* literals[4];
  size_t literal_c;
} file_blob_t;

void file_blob_init(file_blob_t* blob, char* name);

fn_blob_t* file_blob_add_fn(file_blob_t* blob, char* name, fn_type_t type);

literal_t* file_blob_add_literal(file_blob_t* blob);

void file_blob_print(file_blob_t* blob);

typedef enum {
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_CMP_EQ,
  OP_CMP_NEQ,
  OP_CMP_LT,
  OP_CMP_LT_EQ,
  OP_CMP_GT,
  OP_CMP_GT_EQ,
} Vm_arithmetic_optype;

typedef struct {
  size_t return_addr;
  fn_blob_t* return_fn;
  Map* symbol_table;
} Stack_frame;

typedef struct {
  Derp_obj** stack;
  int stack_len;

  List* call_stack; // list of Stack_frames
  Map* func_map;

  err_t* err;

  int pointer;
  // instr* bytecode;
  file_blob_t* blob;
  // size_t num_bytecodes;

  List* std_lib;
} Derp_vm;

typedef err_t* (*Derp_native_fn)(Derp_vm* vm, int argc, Derp_obj** argv);

typedef struct {
  char* name;
  Derp_native_fn fn;
} Derp_native;

#include "stdlib.h"

void vm_push_int(Derp_vm *vm, int i);

/* allocates and initializes a vm, then returns a pointer to it */
Derp_vm* derp_vm_create();

/* deallocates a vm and all its associated data */
void derp_vm_destroy(Derp_vm *obj);

/* add a function to the Derplang standard library */
void derp_add_native(Derp_vm* vm, char* name, Derp_native_fn fn);

/* call the standard libary function with the given name, with arguments
 * coming from the vm's stack */
bool derp_run_native(Derp_vm* vm, char* name, int argc);

/* stop the world, then mark all objects reachable from the current scope */
void vm_gc_mark(Derp_vm *vm);

/* run the vm */
void derp_vm_run(Derp_vm *vm);
