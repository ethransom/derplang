#include <stdio.h>

#include "vm.h"

// must match definition of Cream_code_type in 'vm.h'
char Cream_code_names[10][16] = {
    "NULL",
    "PUSH",
    "CALL",
    "REGISTER",
    "PUSH_LOOKUP",
    "ASSIGN",
    "RET",
    "JUMP_IF_FALSE",
    "JUMP",
    "REPEAT"
};



Cream_vm* cream_vm_create() {
	// puts("cream obj created");
	Cream_vm *vm = malloc(sizeof(Cream_vm));
	check_mem(vm);

	vm->stack = List_create();
	check_mem(vm->stack);

	vm->call_stack = List_create();
	check_mem(vm->call_stack);

	vm->func_map = Map_create();
	check_mem(vm->func_map);
	vm->func_map->not_found_val = (void*) -1;

	vm->bytecode = NULL;
	vm->err = false;

	vm->std_lib = List_create();
	check_mem(vm->stack);

	cream_add_native(vm, "println", cream_stdlib_println);
	cream_add_native(vm, "print", cream_stdlib_print);

	return vm;
error:
	return NULL;
}

void cream_vm_destroy(Cream_vm *vm) {
	// puts("cream vm destroyed");
	check(vm != NULL, "VM is already destroyed!");

	{
		LIST_FOREACH(vm->stack, first, next, cur) free(cur->data);
	}
	List_destroy(vm->stack);

	List_destroy(vm->call_stack);

	// because the func_map only contains pointers to unowned memory, it's okay to just free it
	Map_destroy(vm->func_map);

	if (vm->bytecode != NULL) {
		for (unsigned int pointer = 0; pointer < vm->num_bytecodes; pointer++) {
			if ((vm->bytecode)[pointer].arg1 != NULL) {
				free((vm->bytecode)[pointer].arg1);
			}
		}
		free(vm->bytecode);
	}

	LIST_FOREACH(vm->std_lib, first, next, cur) {
		free(cur->data);
	}
	List_destroy(vm->std_lib);

	free(vm);
error:
	return;
}

void cream_add_native(Cream_vm* vm, char* name, Cream_native_fn fn) {
	Cream_native* native = malloc(sizeof(Cream_native));
	check_mem(native);

	native->name = name;
	native->fn = fn;

	List_push(vm->std_lib, (void*) native);

error:
	return;
}

bool cream_run_native(Cream_vm* vm, char* name, int argc) {
	debug("Searching for '%s'...", name);
	LIST_FOREACH(vm->std_lib, first, next, cur) {
		Cream_native* native = cur->data;
		debug("comparing '%s' and '%s'...", native->name, name);
		if (strcmp(native->name, name) == 0) {
			native->fn(vm, argc);
			return true;
		}
	}

	return false;
}

const char* cream_code_to_str(Cream_code_type* code) {
	return Cream_code_names[(int) *code];
}

// void cream_vm_load_file(char* filename) {
// 
// }

void cream_vm_run(Cream_vm *vm) {
	debug("beginning execution...");
	size_t num_bytecodes = vm->num_bytecodes;
	debug("%d bytecodes", num_bytecodes);
	for (unsigned int pointer = 0; pointer < num_bytecodes; pointer++) {
		// instr* in = (vm->block)[pointer]
		char* arg1 = (vm->bytecode)[pointer].arg1;
		int arg2 = (vm->bytecode)[pointer].arg2;

		debug("code: %s @ %d", 
			cream_code_to_str(&((vm->bytecode)[pointer]).code),
			pointer
		);

		switch ((vm->bytecode)[pointer].code) {
			case CODE_PUSH: {
				debug("Evaluating push");
				Cream_obj* data = malloc(sizeof(Cream_obj));
				check_mem(data);

				data->type = (int) arg2;
				switch (data->type) {
					case TYPE_STRING:
						data->data = arg1;
						break;
					case TYPE_INTEGER: {
						int* ptr = malloc(sizeof(int));
						check_mem(ptr);
						*ptr = atoi(arg1);
						data->data = (void*) ptr;
					}
						break;
					case TYPE_FLOAT:
						// ...
						break;
					case TYPE_BOOLEAN:
						// ...
						break;
					default:
						sentinel("Unknown 'Cream_data' type!");
						break;
				}

				List_push(vm->stack, data);
			}
				break;
			case CODE_CALL: {
				char* identifier = arg1;
				int argc = arg2;

				debug("'%d' arg call to '%s'", argc, identifier);

				// check defined functions
				int addr = (int) Map_get(vm->func_map, identifier);
				if (addr != -1) {
					Stack_frame* frame = malloc(sizeof(Stack_frame));
					check_mem(frame);
					Map* symbol_table = Map_create(); 
					frame->symbol_table = symbol_table;
					frame->return_addr = pointer;
					List_push(vm->call_stack, frame);

					debug("jumping to '%s' @ %d", identifier, addr);
					pointer = addr;
					break;
				}

				// check stdlib functions
				bool success = cream_run_native(vm, identifier, argc);
				
				check(success != false, "unknown function: '%s'", identifier);
			}
				break;
			case CODE_REGISTER: {
				debug("Registered function '%s' @ %d", arg1, pointer);
				Map_set(vm->func_map, arg1, (void*) pointer);
				pointer += arg2;
			}
				break;
			case CODE_PUSH_LOOKUP: {
				debug("pushing contents of '%s' to stack", arg1);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Cream_obj* data = Map_get(frame->symbol_table, arg1);
				List_push(vm->stack, data);
			}
				break;
			case CODE_ASSIGN: {
				debug("assigning to %s", arg1);
				Cream_obj* data = List_pop(vm->stack);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Map_set(frame->symbol_table, arg1, data);
			}
				break;
			case CODE_RET: {
				Stack_frame* last_frame = List_pop(vm->call_stack);
				int addr = (int) last_frame->return_addr;
				debug("returning to %d from %d", addr, pointer);
				Map_destroy(last_frame->symbol_table);
				pointer = addr;
				free(last_frame);
			}
				break;
			// case CODE_JUMP_IF_FALSE:
				break;
			case CODE_JUMP: {
				int num = atoi(arg1);
				debug("jumping %d", num);
				pointer += num;
			}
				break;
			// case CODE_REPEAT:
				break;
			case CODE_NULL:
				break;
			default:
				log_warn("Unknown code: %s (%d)", 
					cream_code_to_str(&(vm->bytecode)[pointer].code), 
					(vm->bytecode)[pointer].code
				);
				break;
		}

		check(!vm->err, "error on line %d", pointer);
	}

	return;
error:
	List_clear(vm->stack);
}

