#ifndef CREAM_OBJ_H
#define CREAM_OBJ_H

#include <stdbool.h>

// GENERIC Cream value
typedef enum {
	TYPE_OBJECT,
	TYPE_STRING,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_BOOLEAN
} Cream_data_type;

typedef struct {
	Cream_data_type type;
	unsigned char flags; // first slot: frozen
	union {
		void* klass; // unused for now
		void* data; // used if native
	};
} Cream_obj;

#define OBJ_IS_NATIVE(obj) (obj->type != TYPE_OBJECT)

#define FLAG_FROZEN (1 << 0)
#define OBJ_IS_FROZEN(obj) (obj->flags & FLAG_FROZEN)

Cream_obj* cream_obj_create();

void cream_obj_freeze(Cream_obj *obj);

void cream_obj_destroy(Cream_obj *obj);

#endif
