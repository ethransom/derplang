#include <stdio.h>
#include <assert.h>

#include "../object.h"

int main(int argc, char const *argv[])
{
	// printf("Size of Cream_data_type: %d\n", sizeof(Cream_data_type));
	// printf("Size of unsigned char: %d\n", 	sizeof(unsigned char));
	// printf("Size of char: %d\n", 			sizeof(char));
	// printf("Size of void pointer: %d\n", 	sizeof(void*));
	
	// printf("Size of Cream_obj: %d\n",		sizeof(Cream_obj));

	// puts("");

	// Cream_obj stack_obj; 
	// stack_obj.type = TYPE_OBJECT;
	// stack_obj.klass = 1234;

	Cream_obj* heap_obj = cream_obj_create();

	assert(!(OBJ_IS_NATIVE(heap_obj)));

	assert(!OBJ_IS_FROZEN(heap_obj));

	cream_obj_freeze(heap_obj);
	cream_obj_freeze(heap_obj); // make sure we're setting the bit, not just flipping

	assert(OBJ_IS_FROZEN(heap_obj));

	cream_obj_destroy(heap_obj);

	return 0;
}