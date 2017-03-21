#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <string.h>

#include "debug.h"
#include "vm.h"
#include "stdlib.h"
#include "bytecodes.h"

Derp_code_type derp_str2_bytecode(char* str);

bool derp_instr_from_line(char* line, instr** instruction);

bool derp_bytecode_parse_stream(FILE* input, Derp_vm *vm);

bool derp_bytecode_parse_string(Derp_vm* vm, char* buffer);

#endif
