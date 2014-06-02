#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#include "cream_obj.h"

Cream_obj* cream_obj_create(int id) {
	// puts("cream obj created");
	Cream_obj *obj = malloc(sizeof(Cream_obj));
	assert(obj != NULL);

	return obj;
}

void cream_obj_destroy(Cream_obj *obj) {
	// puts("cream obj destroyed");
	assert(obj != NULL);

	free(obj);
}
