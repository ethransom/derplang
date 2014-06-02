#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <string.h>

#include "debug.h"
#include "vm.h"
#include "stdlib.h"

Cream_code_type cream_str2_bytecode(char* str); 

bool cream_instr_from_line(char* line, instr** instruction);

bool cream_bytecode_parse_stream(FILE* input, Cream_vm *vm);

bool cream_bytecode_parse_string(Cream_vm* vm, char* buffer);

#endif
