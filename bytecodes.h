#pragma once

#include "list.h"
#include "vm.h"

typedef List BCList;

typedef instr* BCVec;

const char* code_type_to_str(Cream_code_type* code);

void bytecode_print(instr* code);

void bytecode_vec_print(instr* instrv, int instrc);

// compress the bytecodes in `input` into a contiguous block
// of memory. Return pointer to that block if successful. NULL otherwise.
instr* bytecodes_compress(List* input);
