#include <stdio.h>

#include "vm.h"

void file_blob_init(file_blob_t* blob, char* name) {
	blob->fn_c = 0;
	blob->literal_c = 0;
	blob->name = name;
}

fn_blob_t* file_blob_add_fn(file_blob_t* blob, char* name, fn_type_t type) {
	check(blob->fn_c < 4, "file_blob_t is full!");

	fn_blob_t* fn = malloc(sizeof(fn_blob_t));
	check_mem(fn);
	fn->name = name;
	fn->type = type;
	if (type == FN_BYTECODE) {
		fn->bytecode.blob = NULL;
		fn->bytecode.blob_len = 0;
	} else {
		log_warn("unknown fn_blob_t type");
	}

	blob->fns[blob->fn_c] = fn;
	blob->fn_c++;

	return fn;

error:
	return NULL;
}

literal_t* file_blob_add_literal(file_blob_t* blob) {
	check(blob->literal_c < 4, "file_blob_t is full of literals!");

	literal_t* lit = malloc(sizeof(literal_t));
	check_mem(lit);

	blob->literals[blob->literal_c] = lit;
	blob->literal_c++;

	return lit;

error:
	return NULL;
}

void file_blob_print(file_blob_t* blob) {
	printf("'%s' has %zd functions\n", blob->name, blob->fn_c);
	for (int i = 0; i < blob->fn_c; i++) {
		fn_blob_t* fn = blob->fns[i];

		switch (fn->type) {
			case FN_BYTECODE:
				printf("BYTECODE FUNC: %s (length: %zd)\n",
					fn->name, fn->bytecode.blob_len);
				bytecode_vec_print(fn->bytecode.blob, fn->bytecode.blob_len);
				break;
			default:
				log_warn("fn_blob_t has unknown type, skipping...");
				continue;
		}

		printf("END FUNC %s\n", blob->name);
	}
}

//============================== HELPER FUNCTIONS ==============================
// marked as static and not forward-declared in the header so as to be private

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

bool vm_stack_push(Cream_vm* vm, Cream_obj* obj) {
	debug("Pushing stack...");

	if (vm->stack_len >= VM_STACK_HEIGHT) {
		log_err("Stack overflow!");
		return false;
	}

	vm->stack[vm->stack_len] = obj;
	vm->stack_len++;

	return true;
}

Cream_obj* vm_stack_pop(Cream_vm* vm) {
	debug("Popping stack...");

	if (vm->stack_len <= 0) {
		log_err("Stack underflow!");
		return NULL;
	}

	vm->stack_len -= 1;
	Cream_obj* obj = vm->stack[vm->stack_len];

	return obj;
}

Stack_frame* vm_call_stack_pop(Cream_vm* vm) {
	Stack_frame* last_frame = List_pop(vm->call_stack);
	if (last_frame == NULL) return NULL;

	Map_destroy(last_frame->symbol_table);

	return last_frame;
}

/*
 * Perform the arithmetic operation indicated by optype
 * Pushes the result of the operation to the stack
*/
// TODO: potentially inline?
static void vm_arithmetic(Cream_vm* vm, Vm_arithmetic_optype optype) {
	Cream_obj* right = vm_stack_pop(vm);
	Cream_obj* left = vm_stack_pop(vm);
	check(right->type == TYPE_INTEGER, "Expected integer");
	check(left->type == TYPE_INTEGER, "Expected integer");

	int r = right->int_val;
	int l = left->int_val;

	Cream_obj* result = malloc(sizeof(Cream_obj));
	check_mem(result);

	if (optype == OP_ADD || optype == OP_SUB ||
		optype == OP_MUL || optype == OP_DIV) {

		debug("performing arithmetic operation on %d and %d", l, r);
		result->type = TYPE_INTEGER;
	} else {
		debug("performing comparison operation on %d and %d", l, r);
		result->type = TYPE_BOOLEAN;
	}

	switch (optype) {
		case OP_ADD:		result->int_val = l + r; break;
		case OP_SUB:		result->int_val = l - r; break;
		case OP_MUL:		result->int_val = l * r; break;
		case OP_DIV:		result->int_val = l / r; break;

		case OP_CMP_EQ:		result->bool_val = (l == r); break;
		case OP_CMP_NEQ:	result->bool_val = (l != r); break;
		case OP_CMP_LT:		result->bool_val = (l <  r); break;
		case OP_CMP_LT_EQ:	result->bool_val = (l <= r); break;
		case OP_CMP_GT:		result->bool_val = (l >  r); break;
		case OP_CMP_GT_EQ:	result->bool_val = (l >= r); break;

		default: sentinel("encountered unknown optype");
	}

	debug("result of operation: b: %d i: %d", result->bool_val, result->int_val);

	vm_stack_push(vm, result);
	// free(right);
	// free(left);

	return;

error:
	vm->err = err_create(&InternalErr, NULL);
}

// =========== PUSH HELPERS ========== //
// push various types to the stack
// TODO: move `Cream_obj` creation to `object.c` to allow for memory mgmt.
// returns true if successful, false if not
void vm_push_int(Cream_vm *vm, int i) {
	Cream_obj* data = object_create(vm);
	data->type = TYPE_INTEGER;
	data->int_val = i;
	vm_stack_push(vm, data);
}
static void vm_push_float(Cream_vm *vm, float f) {
	Cream_obj* data = object_create(vm);
	data->type = TYPE_FLOAT;
	data->float_val = f;
	vm_stack_push(vm, data);
}
static void vm_push_str(Cream_vm *vm, char* s) {
	Cream_obj* data = object_create(vm);
	data->type = TYPE_STRING;
	data->str_val = strdup(s);
	vm_stack_push(vm, data);
}
static void vm_push_bool(Cream_vm *vm, bool b) {
	Cream_obj* data = object_create(vm);
	data->type = TYPE_BOOLEAN;
	data->bool_val = b;
	vm_stack_push(vm, data);
}
static void vm_push_fn_ref(Cream_vm *vm, FnBytecode* ref) {
	Cream_obj* data = object_create(vm);
	data->type = TYPE_FN_REF;
	data->fn_ref_val = (struct FnBytecode*) ref;
	vm_stack_push(vm, data);
}

Cream_vm* cream_vm_create() {
	// puts("cream obj created");
	Cream_vm *vm = malloc(sizeof(Cream_vm));
	check_mem(vm);

	vm->stack = malloc(VM_STACK_HEIGHT * sizeof(Cream_obj*));
	check_mem(vm->stack);

	vm->call_stack = List_create();
	check_mem(vm->call_stack);
	check_mem(vm_add_stack_frame(vm));

	vm->func_map = Map_create();
	check_mem(vm->func_map);
	vm->func_map->not_found_val = (void*) -1;

	vm->blob = NULL;
	vm->err = false;

	vm->std_lib = List_create();
	check_mem(vm->std_lib);

	cream_add_native(vm, "println", cream_stdlib_println);
	cream_add_native(vm, "print", cream_stdlib_print);
	cream_add_native(vm, "len", cream_stdlib_len);
	cream_add_native(vm, "range", cream_stdlib_range);

	return vm;
error:
	return NULL;
}

void cream_vm_destroy(Cream_vm *vm) {
	// puts("cream vm destroyed");
	check(vm != NULL, "VM is already destroyed!");

	{
		// LIST_FOREACH(vm->stack, first, next, cur) free(cur->data);
	}
	free(vm->stack);

	List_destroy(vm->call_stack);

	// because the func_map only contains pointers to unowned memory, it's okay to just free it
	Map_destroy(vm->func_map);

	// TODO: deallocate thing?
	// if (vm->bytecode != NULL) {
	// 	for (unsigned int pointer = 0; pointer < vm->num_bytecodes; pointer++) {
	// 		if ((vm->bytecode)[pointer].arg1 != NULL) {
	// 			free((vm->bytecode)[pointer].arg1);
	// 		}
	// 	}
	// 	free(vm->bytecode);
	// }

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
			// we've found our function, call it
			vm->stack_len -= argc; // destructive, because fn call pops the stack
			Cream_obj** stack_slice = vm->stack + vm->stack_len;

			vm->err = native->fn(vm, argc, stack_slice);
			return true;
		}
	}

error:
	return false;
}

// recurse over stack frames, marking all the objects in the symbol table
void vm_gc_mark(Cream_vm* vm) {
	debug("beginning GC mark");
	LIST_FOREACH(vm->call_stack, first, next, cur) {
		Stack_frame* frame = cur->data;
		Map* table = frame->symbol_table;
		for (struct MapNode* node = table->first; node != NULL; node = node->next) {
			debug("marking %s", node->key);
			Cream_obj* obj = node->data;
			obj->marked = true;
		}
	}
	for (int i = 0; i < vm->stack_len; ++i) {
		debug("marking stack[%d]", i);
		vm->stack[i]->marked = true;
	}
}


void cream_vm_run(Cream_vm *vm) {
	debug("beginning execution...");

	check(vm->blob->fn_c > 0, "file was empty?");

	// start at the first instruction of the first function
	fn_blob_t* cur_fn = vm->blob->fns[0];
	size_t pointer = 0;
	while (pointer < cur_fn->bytecode.blob_len) {
		instr* bytecode = &cur_fn->bytecode.blob[pointer];

		debug("code: %s @ %zd",
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
			case CODE_PUSH_BOOL:
				vm_push_bool(vm, bytecode->arg2);
				break;
			case CODE_PUSH_ARRAY: {
				Cream_obj* obj = object_create(vm);
				obj->type = TYPE_ARRAY;

				vm->stack_len -= bytecode->arg2;
				Cream_obj** stack_slice = vm->stack + vm->stack_len;

				size_t size = sizeof(Cream_obj*) * bytecode->arg2;
				Cream_obj** vec = malloc(size);
				memcpy(vec, stack_slice, size);

				obj->arr_val.vec = vec;
				obj->arr_val.len = bytecode->arg2;

				vm_stack_push(vm, obj); // push before we mess with the stack!
			}
				break;
			case CODE_PUSH_FN:
				vm_push_fn_ref(vm, (FnBytecode*) bytecode->arg1);
				break;

			// ================== MATHEMATICAL OPERATIONS =================== //
			case CODE_ADD:	vm_arithmetic(vm, OP_ADD); break;
			case CODE_SUB:	vm_arithmetic(vm, OP_SUB); break;
			case CODE_MUL:	vm_arithmetic(vm, OP_MUL); break;
			case CODE_DIV:	vm_arithmetic(vm, OP_DIV); break;

			// ================== COMPARISON OPERATIONS =================== //
			case CODE_CMP_EQ:		vm_arithmetic(vm, OP_CMP_EQ); break;
			case CODE_CMP_NEQ:		vm_arithmetic(vm, OP_CMP_NEQ); break;
			case CODE_CMP_LT:		vm_arithmetic(vm, OP_CMP_LT); break;
			case CODE_CMP_LT_EQ:	vm_arithmetic(vm, OP_CMP_LT_EQ); break;
			case CODE_CMP_GT:		vm_arithmetic(vm, OP_CMP_GT); break;
			case CODE_CMP_GT_EQ:	vm_arithmetic(vm, OP_CMP_GT_EQ); break;

			case CODE_CALL: {
				char* identifier = bytecode->arg1;
				int argc = bytecode->arg2;

				debug("'%d' arg call to '%s'", argc, identifier);

				// check defined functions
				fn_blob_t* fn = NULL;
				for (int i = 0; i < vm->blob->fn_c; i++) {
					if (strcmp(vm->blob->fns[i]->name, identifier) == 0) {
						fn = vm->blob->fns[i];
						break;
					}
				}
				if (fn != NULL) {
					Stack_frame* frame = vm_add_stack_frame(vm);
					frame->return_addr = pointer;
					frame->return_fn = cur_fn;

					debug("jumping to func: '%s'", identifier);
					// when the loop restarts, pointer gets incremented to 1
					pointer = 0;
					cur_fn = fn;
					continue;
				}

				// check stdlib functions
				bool success = cream_run_native(vm, identifier, argc);

				if (success == false) {
					vm->err = err_create(&LookupErr, "foobar"); // "unknown function: '%s'", identifier);
				}
			}
				break;
			case CODE_PUSH_LOOKUP: {
				debug("pushing contents of '%s' to stack", bytecode->arg1);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Cream_obj* data = Map_get(frame->symbol_table, bytecode->arg1);
				if (data == frame->symbol_table->not_found_val) {
					log_err("undefined variable '%s'", bytecode->arg1);
					vm->err = err_create(&LookupErr, "undefined variable");
				}
				vm_stack_push(vm, data);
			}
				break;
			case CODE_ASSIGN: {
				debug("assigning to %s", bytecode->arg1);
				Cream_obj* data = vm_stack_pop(vm);
				Stack_frame* frame = (Stack_frame*) vm->call_stack->last->data;
				Map_set(frame->symbol_table, bytecode->arg1, data);
			}
				break;
			case CODE_RET: {
				Stack_frame* last_frame = List_pop(vm->call_stack);
				debug("returning to %zd from %zd", last_frame->return_addr, pointer);
				pointer = last_frame->return_addr;
				cur_fn = last_frame->return_fn;

				Map_destroy(last_frame->symbol_table);
				free(last_frame);
			}
				break;
			case CODE_JUMP_IF_FALSE: {
				Cream_obj* top = vm_stack_pop(vm);
				if (top->type != TYPE_BOOLEAN) {
					log_err("don't know how to evaluate truthyness of non-boolean");
					vm->err = err_create(&TypeErr, "expected boolean");
					break;
				}

				if (top->bool_val) {
					debug("found cond to be truthy, continuing");
				} else {
					int num = bytecode->arg2;
					debug("found cond to be falsy, jumping %d", num);
					pointer += num;
				}
			}
				break;
			case CODE_JUMP: {
				debug("jumping to %d", bytecode->arg2);
				pointer = bytecode->arg2;
				continue;
			}
				break;
			case CODE_NULL:
				break;
			default:
				sentinel("Unknown code: %s (%d)",
					code_type_to_str(bytecode->code),
					bytecode->code
				);
				break;
		}

		check(!vm->err, "error on line %zd", pointer);

		pointer++;
	}

	return;
error:
	if (!vm->err) {
		// we crashed internally
		err_t* err = err_create(&InternalErr, "Something went wrong internally");
		err->line = pointer;
		err->file = cur_fn->name;
		vm->err = err;
	}

	err_print(vm->err);

	// print backtrace
	Stack_frame* frame;
	while ((frame = vm_call_stack_pop(vm)) != NULL) {
		char* name;
		if (frame->return_fn == NULL)
			name = "ROOT";
		else
			name = frame->return_fn->name;

		printf("\t%s:%zd\n", name, frame->return_addr);
		free(frame);
	}

	return;
}

