#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stdlib.h"

err_t* cream_stdlib_println(Cream_vm* vm, int argc, Cream_obj** argv) {
	err_t* err = cream_stdlib_print(vm, argc, argv);

	if (err == NULL) {
		printf("\n");
	}

	return err;
}

err_t* cream_stdlib_print(Cream_vm* vm, int argc, Cream_obj** argv) {
	for (int i = 0; i < argc; i++) {
		Cream_obj* data = argv[i];

		debug("Printing type: %d", data->type);

		switch (data->type) {
			case TYPE_STRING:
				printf("%s", data->str_val);
				break;
			case TYPE_ARRAY:
				putchar('[');
				for (int i = 0; i < data->arr_val.len; ++i) {
					if (i != 0) putchar(',');
					err_t* err = cream_stdlib_print(vm, 1, &data->arr_val.vec[i]);
					if (err != NULL) return err;
				}
				putchar(']');
				break;
			case TYPE_INTEGER:
				printf("%d", data->int_val);
				break;
			case TYPE_FLOAT:
				printf("%f", (double) data->float_val);
				break;
			case TYPE_BOOLEAN:
				printf("%s", data->bool_val ? "true" : "false");
				break;
			default:
				sentinel("Tried to print unknown type");
				break;
		}

		if (i != (argc - 1)) // not on last loop
			putchar(' ');
	}

	return NULL;

error:
	return err_create(&InternalErr, NULL);
}

err_t* cream_stdlib_len(Cream_vm* vm, int argc, Cream_obj** argv) {
	if (argc != 1) 
		return err_create(&ArgErr, "len() takes exactly one argument");

	Cream_obj* data = argv[0];
	int len;

	switch (data->type) {
		case TYPE_STRING:
			len = strlen(data->str_val);
			break;
		case TYPE_ARRAY:
			len = data->arr_val.len;
			break;
		default:
			return err_create(&ArgErr, "couldn't find len of given object");
	}

	vm_push_int(vm, len);

	return NULL;
}