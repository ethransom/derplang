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

typedef enum {
    CODE_NULL, // dummy code, does nothing
    CODE_PUSH,
    CODE_CALL, 
    CODE_REGISTER,
    CODE_PUSH_LOOKUP,
    CODE_ASSIGN,
    CODE_RET,
    CODE_JUMP_IF_FALSE,
    CODE_JUMP,
    CODE_REPEAT
} Cream_code_type;

typedef struct {
  Cream_code_type code;
  char* arg1;
  int arg2; // interestingly, the second arg is always an integer
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

const char* cream_code_to_str(Cream_code_type* code);

// void cream_vm_load_file(char* filename);

void cream_vm_run(Cream_vm *vm);

#endif
