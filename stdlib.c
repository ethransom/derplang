#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stdlib.h"

bool cream_stdlib_println(int argc, Cream_obj** argv) {
	if (!cream_stdlib_print(argc, argv)) {
		return false;
	}

	printf("\n");

	return true;
}

bool cream_stdlib_print(int argc, Cream_obj** argv) {
	for (int i = 0; i < argc; i++) {
		Cream_obj* data = argv[i];

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

	return true;

error:
	return false;
}
