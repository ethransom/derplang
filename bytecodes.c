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

fn_blob_t* file_blob_add_fn(file_blob_t* blob, char* name) {
	check(blob->fn_c < 4, "file_blob_t is full!");

	fn_blob_t* fn = malloc(sizeof(fn_blob_t));
	check_mem(fn);
	fn->name = name;
	fn->blob = NULL;
	fn->blob_len = 0xfeedbeef;

	blob->fns[blob->fn_c] = fn;
	blob->fn_c++;

	return fn;

error:
	return NULL;
}

void file_blob_print(file_blob_t* blob) {
	printf("'%s' has %zd functions\n", blob->name, blob->fn_c);
	for (int i = 0; i < blob->fn_c; i++) {
		fn_blob_t* fn = blob->fns[i];
		printf("FUNC: %s (length: %zd)\n", fn->name, fn->blob_len);
		bytecode_vec_print(fn->blob, fn->blob_len);
		printf("END FUNC %s\n", blob->name);
	}
}
