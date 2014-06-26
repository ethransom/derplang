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
				printf("%s", (char*) data->data);
				break;
			case TYPE_INTEGER:
				printf("%d", *((int*) data->data));
				break;
			case TYPE_FLOAT:
				// printf("%f", (double) data->data);
				break;
			case TYPE_BOOLEAN:
				printf("%s", (*((bool*) data->data) == 0 ? "true" : "false"));
				break;
		}

		// free(data);
	}

	return;

error:
	vm->err = true;
}
