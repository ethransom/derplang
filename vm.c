#include <stdio.h>

#include "vm.h"

#include "bytecodes.h"

//============================== HELPER FUNCTIONS ==============================
// marked as static and not forward-declared in the header so as to be private

//
static Stack_frame* vm_add_stack_frame(Cream_vm* vm) {
	Stack_frame* frame = malloc(sizeof(Stack_frame));
	check_mem(frame);
	Map* symbol_table = Map_create();
	frame->symbol_table = symbol_table;
	List_push(vm->call_stack, frame);
	return frame;
error:
	log_err("OUT OF MEMR");
	return NULL;
}

// =========== PUSH HELPERS ========== //
// push various types to the stack
// TODO: move `Cream_obj` creation to `object.c` to allow for memory mgmt.
// returns true if successful, false if not
static void vm_push_int(Cream_vm *vm, int i) {
	Cream_obj* data = cream_obj_create();
	data->type = TYPE_INTEGER;
	data->int_val = i;
	List_push(vm->stack, data);
}
static void vm_push_float(Cream_vm *vm, float f) {
	Cream_obj* data = cream_obj_create();
	data->type = TYPE_FLOAT;
	data->float_val = f;
	List_push(vm->stack, data);
}
static void vm_push_str(Cream_vm *vm, char* s) {
	Cream_obj* data = cream_obj_create();
	data->type = TYPE_STRING;
	data->str_val = s;
	List_push(vm->stack, data);
}
// static bool vm_push_bool(Cream_vm *vm, bool b);

Cream_vm* cream_vm_create() {
	// puts("cream obj created");
	Cream_vm *vm = malloc(sizeof(Cream_vm));
	check_mem(vm);

	vm->stack = List_create();
	check_mem(vm->stack);

	vm->call_stack = List_create();
	check_mem(vm->call_stack);
	check_mem(vm_add_stack_frame(vm));

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
	check(name != NULL, "Given NULL as a name!");
	debug("Searching for '%s'...", name);
	LIST_FOREACH(vm->std_lib, first, next, cur) {
		Cream_native* native = cur->data;
		debug("comparing '%s' and '%s'...", native->name, name);
		if (strcmp(native->name, name) == 0) {
			native->fn(vm, argc);
			return true;
		}
	}

error:
	return false;
}

void cream_vm_run(Cream_vm *vm) {
	debug("beginning execution...");

	size_t num_bytecodes = vm->num_bytecodes;
	debug("%zu bytecodes", num_bytecodes);

	for (unsigned int pointer = 0; pointer < num_bytecodes; pointer++) {
		instr* bytecode = vm->bytecode + pointer;

		debug("code: %s @ %d",
			code_type_to_str(bytecode->code),
			pointer
		);

		switch (bytecode->code) {
			case CODE_PUSH:
				sentinel("CODE_PUSH is no good!");
				break;
			case CODE_PUSH_INT:
				vm_push_int(vm, bytecode->arg2);
				break;
			case CODE_PUSH_FLOAT:
				vm_push_float(vm, bytecode->float_val);
				break;
			case CODE_PUSH_STR:
				vm_push_str(vm, bytecode->arg1);
				break;

			// ================== MATHEMATICAL OPERATIONS =================== //
			case CODE_ADD:
			case CODE_SUB:
			case CODE_MUL:
			case CODE_DIV: {
				Cream_obj* right = List_pop(vm->stack);
				Cream_obj* left = List_pop(vm->stack);
				check(right->type == TYPE_INTEGER, "Expected integer");
				check(left->type == TYPE_INTEGER, "Expected integer");
				Cream_obj* result = malloc(sizeof(Cream_obj));
				check_mem(result);
				result->type = TYPE_INTEGER;

				result->int_val = left->int_val + right->int_val;
				List_push(vm->stack, result);
				free(right);
				free(left);
			}
				break;

			// ================== COMPARISON OPERATIONS =================== //
			case CODE_CMP_EQ:
			case CODE_CMP_NEQ:
			case CODE_CMP_LT:
			case CODE_CMP_LT_EQ:
			case CODE_CMP_GT:
			case CODE_CMP_GT_EQ:
				sentinel("Comparisions not implemented");
				break;

			case CODE_CALL: {
				char* identifier = bytecode->arg1;
				int argc = bytecode->arg2;

				debug("'%d' arg call to '%s'", argc, identifier);

				// check defined functions
				int addr = (int) Map_get(vm->func_map, identifier);
				if (addr != -1) {
					Stack_frame* frame = vm_add_stack_frame(vm);
					frame->return_addr = pointer;

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
				debug("Registered function '%s' @ %d", bytecode->arg1, pointer);
				Map_set(vm->func_map, bytecode->arg1, (void*) pointer);
				pointer += bytecode->arg2;
			}
				break;
			case CODE_PUSH_LOOKUP: {
				debug("pushing contents of '%s' to stack", bytecode->arg1);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Cream_obj* data = Map_get(frame->symbol_table, bytecode->arg1);
				if (data == frame->symbol_table->not_found_val) {
					log_err("undefined variable '%s'", bytecode->arg1);
					vm->err = true;
				}
				List_push(vm->stack, data);
			}
				break;
			case CODE_ASSIGN: {
				debug("assigning to %s", bytecode->arg1);
				Cream_obj* data = List_pop(vm->stack);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Map_set(frame->symbol_table, bytecode->arg1, data);
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
				int num = atoi(bytecode->arg1);
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
					code_type_to_str(bytecode->code),
					bytecode->code
				);
				break;
		}

		check(!vm->err, "error on line %d", pointer);
	}

	return;
error:
	List_clear(vm->stack);
}

