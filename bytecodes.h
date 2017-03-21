/*
 * Code and types for handling bytecodes, the stuff from which all Derp programs are made
*/

#pragma once

#include "list.h"
#include <stdlib.h>

// any updates to this MUST be mirrored in the string array in bytecodes.c
typedef enum {
    CODE_NULL, // dummy code, does nothing

    CODE_PUSH,
    CODE_PUSH_INT,
    CODE_PUSH_FLOAT,
    CODE_PUSH_STR,
    CODE_PUSH_BOOL,
    CODE_PUSH_ARRAY,
    CODE_PUSH_FN,

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
} Derp_code_type;

typedef struct {
  Derp_code_type code;

  // TODO: compress with union
  char* arg1;
  int arg2; // interestingly, the second arg is always an integer
  double float_val;
} instr;

typedef List BCList;

typedef instr* BCVec;

/*
 * returns a pointer to a string representation of a bytecode's type
 * example: CODE_ADD -> "add"
*/
const char* code_type_to_str(Derp_code_type code);

// Prints an individual bytecode
void bytecode_print(instr* code);

// Prints a bytecode vector
void bytecode_vec_print(instr* instrv, int instrc);

// compress the bytecodes in `input` into a contiguous block
// of memory. Return pointer to that block if successful. NULL otherwise.
instr* bytecodes_compress(List* input);
