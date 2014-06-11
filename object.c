#include <assert.h>
#include <stdlib.h>

#include <stdio.h>

#include "debug.h"
#include "object.h"

Cream_obj* cream_obj_create() {
	// puts("cream obj created");
	Cream_obj *obj = malloc(sizeof(Cream_obj));
	check_mem(obj);

	obj->type = TYPE_OBJECT;

	obj->flags = 0;

	return obj;

error:
	return NULL;
}

void cream_obj_freeze(Cream_obj *obj) {
	obj->flags |= FLAG_FROZEN;
}

void cream_obj_destroy(Cream_obj *obj) {
	// puts("cream obj destroyed");
	assert(obj != NULL);

	free(obj);
}
