#ifndef CREAM_VM_H
#define CREAM_VM_H

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "map.h"
#include "utils.h"
#include "debug.h"
#include "object.h"

#define MAX_LINE_LEN 64
#define MAX_IDENT_LEN 16

// TODO: move this to bytecodes.h
// any updates to this MUST be mirrored in the string array in bytecodes.c
typedef enum {
    CODE_NULL, // dummy code, does nothing

    CODE_PUSH,
    CODE_PUSH_INT,
    CODE_PUSH_FLOAT,
    CODE_PUSH_STR,

    // mathematical and logical operations
    CODE_ADD,
    CODE_SUB,
    CODE_MUL,
    CODE_DIV,
    CODE_CMP_EQ,
    CODE_CMP_NEQ,
    CODE_CMP_LT,
    CODE_CMP_LT_EQ,
    CODE_CMP_GT,
    CODE_CMP_GT_EQ,

    CODE_CALL,
    CODE_REGISTER,

    CODE_PUSH_LOOKUP,
    CODE_ASSIGN,

    CODE_RET,

    // control flow handlers
    CODE_JUMP_IF_FALSE,
    CODE_JUMP,
    CODE_REPEAT
} Cream_code_type;

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

// TODO: move to bytecodes.h
typedef struct {
  Cream_code_type code;

  // TODO: compress with union
  char* arg1;
  int arg2; // interestingly, the second arg is always an integer
  double float_val;
} instr;

typedef struct {
  int return_addr;
  Map* symbol_table;
} Stack_frame;

typedef struct {
  List* stack;
  List* call_stack; // list of Stack_frames
  Map* func_map;

  bool err;

  int pointer;
  instr* bytecode;
  size_t num_bytecodes;

  List* std_lib;
} Cream_vm;

typedef void (*Cream_native_fn)(Cream_vm* vm, int argc);

typedef struct {
  char* name;
  Cream_native_fn fn;
} Cream_native;

#include "stdlib.h"

Cream_vm* cream_vm_create();

void cream_vm_destroy(Cream_vm *obj);

void cream_add_native(Cream_vm* vm, char* name, Cream_native_fn fn);

bool cream_run_native(Cream_vm* vm, char* name, int argc);

void cream_vm_run(Cream_vm *vm);

#endif
