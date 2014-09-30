#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stdlib.h"

void cream_stdlib_println(Cream_vm* vm, int argc) {
	cream_stdlib_print(vm, argc);

	printf("\n");
}

void cream_stdlib_print(Cream_vm* vm, int argc) {
	for (int i = 0; i < argc; i++) {
		check(vm->stack->length > 0, "'print': ran out of stack! :(");
		Cream_obj* data = List_pop(vm->stack);

		debug("Printing type: %d", data->type);

		switch (data->type) {
			case TYPE_STRING:
				printf("%s", data->str_val);
				break;
			case TYPE_INTEGER:
				printf("%d", data->int_val);
				break;
			case TYPE_FLOAT:
				printf("%f", (double) data->float_val);
				break;
			// case TYPE_BOOLEAN:
				// printf("%s", (*((bool*) data->data) == 0 ? "true" : "false"));
				// break;
			// case TYPE_BOOLEAN
			default:
				sentinel("Tried to print unknown type");
				break;
		}

		// free(data);
	}

	return;

error:
	vm->err = true;
}

void cream_run_gc(Cream_vm* vm, int argc) {
	vm_gc_mark(vm);
	object_sweep();
}