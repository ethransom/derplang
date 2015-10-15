#include "bytecodes.h"

#include "debug.h"

// must match definition of Cream_code_type in 'vm.h'
char Cream_code_names[][16] = {
    "NULL",
    "PUSH",
    "PUSH_INT",
    "PUSH_FLOAT",
    "PUSH_STR",
    "PUSH_BOOL",
    "PUSH_ARRAY",
    "PUSH_FN",

    // mathematical and logical operations
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "CMP_EQ",
    "CMP_NEQ",
    "CMP_LT",
    "CMP_LT_EQ",
    "CMP_GT",
    "CMP_GT_EQ",

    "CALL",
    "REGISTER",
    "PUSH_LOOKUP",
    "ASSIGN",
    "RET",
    "JUMP_IF_FALSE",
    "JUMP",
    "REPEAT"
};

const char* code_type_to_str(Cream_code_type code) {
	return Cream_code_names[(int) code];
}

void bytecode_print(instr* code) {
	printf("%s", code_type_to_str(code->code));
	if (code->arg1 != NULL)
		printf(" '%s'", code->arg1);
	printf(" %d\n", code->arg2);
}

void bytecode_vec_print(instr* instrv, int instrc) {
	for (int i = 0; i < instrc; i++) {
		bytecode_print(&instrv[i]);
	}
}

instr* bytecodes_compress(List* input) {
	instr* output = malloc(sizeof(instr) * input->length);
	check_mem(output);
	int i = 0;
	while (input->length > 0) {
		instr* inst = List_dequeue(input);
		output[i] = *inst;
		i++;
	}

	return output;
error:
	return NULL;
}
