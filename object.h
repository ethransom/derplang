/*
 * Contains code for creating and managing objects
 * Heap management and the sweep portion of the garbage collecter are here
*/

#pragma once

#include <stdbool.h>

// GENERIC Derplang value
typedef enum {
	TYPE_OBJECT,
	TYPE_ARRAY,
	TYPE_FN_REF,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_BOOLEAN
} Derp_data_type;

// forward declaration placeholders
struct derp_obj;

typedef struct derp_obj {
	Derp_data_type type;
	bool marked; // good ol' mark and sweep
	unsigned char flags; // first slot: frozen
	union {
		// unimplemented, will be used as pointer to class object
		void* klass;

		struct {
			struct derp_obj** vec;
			int len;
		} arr_val;

		struct FnBytecode* fn_ref_val;

		// 'native' or 'primitive' types
		int int_val;
		double float_val;
		char* str_val;
		bool bool_val;
	};
} Derp_obj;


#include "vm.h"

#define OBJ_IS_NATIVE(obj) (obj->type != TYPE_OBJECT)

#define FLAG_FROZEN (1 << 0)
#define OBJ_IS_FROZEN(obj) (obj->flags & FLAG_FROZEN)

void object_init(Derp_obj* object);

Derp_obj* object_create();

void object_sweep();

void derp_obj_freeze(Derp_obj *obj);

void object_destroy(Derp_obj *obj);
