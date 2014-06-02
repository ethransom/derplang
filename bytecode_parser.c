#define _GNU_SOURCE
// ^ so we can use fmemopen

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "bytecode_parser.h"

#define ALPHA "ABCDEFGHIJKLMNOPQRSTUVQXYZabcdefghijklmnopqrstuvqxyz"
#define NUM "0123456789"

// converts string to corresponding Cream_code_type value
// TODO: use perfect hash function instead of linear search?
Cream_code_type cream_str2_bytecode(char* str) {
	debug("str2code: '%s'", str);
	Cream_code_type type;
	if (!strcmp(str, "push")) {
		type = CODE_PUSH;
	} else if (!strcmp(str, "call")) {
		type = CODE_CALL;
	} else if (!strcmp(str, "register")) {
		type = CODE_REGISTER;
	} else if (!strcmp(str, "push_lookup")) {
		type = CODE_PUSH_LOOKUP;
	} else if (!strcmp(str, "assign")) {
		type = CODE_ASSIGN;
	} else if (!strcmp(str, "ret")) {
		type = CODE_RET;
	} else if (!strcmp(str, "jump_if_False")) {
		type = CODE_JUMP_IF_FALSE;
	} else if (!strcmp(str, "jump")) {
		type = CODE_JUMP;
	} else if (!strcmp(str, "repeat")) {
		type = CODE_REPEAT;
	} else {
		type = CODE_NULL;
	}


	return type;
}

// attempts the read of an identifier
char* cream_try_read_identifier(char* line, int* i) {
	int bkup = *i;
	check(line[*i] == ':', "Identifier must start with ':'");
	(*i)++;
	char *str = cream_read_until_from_str_panicky(line, i, " ,\n", ALPHA);
	check_debug(str != NULL, "Bad identifier (must be alphabetic)");
	cream_read_from_whitelist(line, i, " ,\n\0");

	return str;
error:
	debug("aborting, i is %d", *i);
	*i = bkup;
	debug("restored i to %d", *i);
	return NULL;
}

// attempts the read of an integer
char* cream_try_read_int(char* line, int* i) {
	int bkup = *i;
	char *str = cream_read_until_from_str_panicky(line, i, " ,\n", NUM);
	check_debug(str != NULL, "Bad integer");
	cream_read_from_whitelist(line, i, " ,\n\0");

	return str;
error:
	debug("aborting, i is %d", *i);
	*i = bkup;
	debug("restored i to %d", *i);
	return NULL;
}

// attempts the read of a boolean (true, false)
char* cream_try_read_bool(char* line, int* i) {
	int bkup = *i;
	char *str = cream_read_until_from_str_panicky(line, i, "\n ,", ALPHA);
	check_debug(str != NULL, "Bad boolean");
	check_debug(strcmp(str, "true") == 0 || strcmp(str, "false") == 0, "Bad str val: '%s'", str);
	cream_read_from_whitelist(line, i, " ,\n\0");

	return str;
error:
	debug("aborting, i is %d", *i);
	*i = bkup;
	debug("restored i to %d", *i);
	return NULL;
}

/*
 * populates `instr*` with the instruction created from 
 * parsing `line`. 
 * returns false if error, true if no error.
 * `instr*` will be NULL if the line contained no instruction
*/
bool cream_instr_from_line(char* line, instr** instr_ref) {
	instr* instruction = *instr_ref;

	int i = 0;

	// check for line-comments:
	if (line[i] == '#') {
		debug("Line started with a comment, skipping...");
		return true;
	}

	// check for weirdness
	// this also takes care of blank lines, I think
	if (!cream_char_in_str(line[i], ALPHA)) {
		log_warn("Line started with unknown character, skipping...");
		return true;
	}

	instruction = malloc(sizeof(instr));
	check_mem(instruction);
	instruction->code = CODE_NULL;
	instruction->arg1 = NULL;

	char* code = cream_read_until(line, &i, " \n\0"); 

	instruction->code = cream_str2_bytecode(code);

	check(instruction->code != CODE_NULL, "There is no instruction named '%s'", code);

	// advance past the space after the instruction name
	while(isspace(line[i])) {
		i++;
	}

	debug("First char after instr read: '%c'", line[i]);

	switch(instruction->code) {
		case CODE_PUSH: { // ex: push "Hello, World!" or push 2
			if (line[i] == '"') {
				i++;
				char* str = cream_read_until(line, &i, "\"");
				instruction->arg1 = str;
				instruction->arg2 = TYPE_STRING;
				debug("push found str: '%s'", str);
				i++;
			} else if (cream_char_in_str(line[i], NUM)) {
				char* str = cream_try_read_int(line, &i);
				instruction->arg1 = str;
				instruction->arg2 = TYPE_INTEGER;
				debug("push found num: '%s'", str);
				i++;
			}
		}
			break;
		case CODE_CALL: { // ex: call :println, 1
			char* str = cream_try_read_identifier(line, &i);
			check(str != NULL, "First arg of 'call' must be an identifier");
			instruction->arg1 = str;
			debug("First char after identifier read: '%c'", line[i]);
			
			char* num = cream_try_read_int(line, &i);
			check(num != NULL, "`call` arg 2 must be int");
			// should be safe, assuming `cream_try_read_int` has done its job
			instruction->arg2 = atoi(num); 
		}
			break;
		case CODE_REGISTER: { // ex: register :foobar, 5
			char* str = cream_try_read_identifier(line, &i);
			check(str != NULL, "First arg of 'register' must be an identifier");
			instruction->arg1 = str;

			char* num = cream_try_read_int(line, &i);
			check(num != NULL, "`register` arg 2 must be int");
			instruction->arg2 = atoi(num);
		}
			break;
		case CODE_PUSH_LOOKUP: { // ex: push_lookup :variable
			char* str = cream_try_read_identifier(line, &i);
			check(str != NULL, "'push_lookup' requires an identifier");
			instruction->arg1 = str;
		}
			break;
		case CODE_ASSIGN: { // ex: assign :variable
			char* str = cream_try_read_identifier(line, &i);
			check(str != NULL, "'assign' requires an identifier");
			instruction->arg1 = str;
		}
			break;
		case CODE_RET: { // ex: ret true
			char* str = cream_try_read_bool(line, &i);
			check(str != NULL, "'return' requires a boolean");
			instruction->arg1 = str;
		}
			break;
		case CODE_JUMP_IF_FALSE: { // ex: jump_if_false 4
			char* num = cream_try_read_int(line, &i);
			check(num != NULL, "`jump_if_false` requires an int");
			instruction->arg2 = atoi(num);
		}
			break;
		case CODE_JUMP: { // ex: jump 7
			char* num = cream_try_read_int(line, &i);
			check(num != NULL, "`jump` requires an int");
			instruction->arg2 = atoi(num);
		}
			break;
		case CODE_REPEAT:
			// ...
			break;
		case CODE_NULL:
		default:
			log_warn("Don't know how to handle args for unknown code: '%s'", code);
			break;
	}

	debug("Line parse finished");

	*instr_ref = instruction;
	return true;

error:
	free(instruction);
	*instr_ref = NULL;
	return false;
}

// parses the bytecode contained in the file descriptor
// populates the given vm with array of instructions
bool cream_bytecode_parse_stream(FILE* input, Cream_vm *vm) {
	// reading line-by-line snitched from:
	// http://www.daniweb.com/software-development/c/code/216411/reading-a-file-line-by-line

	List* instructions = List_create();

	int line_num = 0;

	char line[MAX_LINE_LEN];
	while (fgets(line, sizeof line, input) != NULL)	{
		// debug("line: '%s'", line);

		instr* instruction = NULL;
		bool result = cream_instr_from_line(line, &instruction);
		if (result) {
			debug("result: %d, instr: %p", result, instruction);
			if (instruction == NULL) {
				debug("instruction failed");
				continue; // true and NULL means no instruction or error
			} else {
				debug("made instruction");
				List_push(instructions, instruction);
			}
		} else {
			sentinel("error parsing bytecode line %d", line_num);
		}

		line_num++;
	}

	// compact the list
	vm->num_bytecodes = instructions->length;
	instr* block = malloc(sizeof(instr) * instructions->length);
	check_mem(block);
	int i = 0;
	while (instructions->length > 0) {
		instr* inst = List_dequeue(instructions);
		block[i] = *inst;
		i++;
	}

	vm->bytecode = block; 

	List_destroy(instructions);

	return true;

error:
	List_destroy(instructions);
	return false;
}

// wraps cream_bytecode_parse_stream to handle strings
bool cream_bytecode_parse_string(Cream_vm* vm, char* buffer) {
	FILE* file = fmemopen(buffer, strlen(buffer), "r");
	return cream_bytecode_parse_stream(file, vm);
}

