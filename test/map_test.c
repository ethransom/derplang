#include "test.h" 

// #include "../map.h"

#include "../map.c"

// bool test_cream_vm_str2code() {
//   check(cream_vm_str2code("foobar") == CODE_NULL, "didn't NULL when given bad value");
  
//   check(cream_vm_str2code("call") == CODE_CALL, "didn't ret correct enum val");
//   check(cream_vm_str2code("jump") == CODE_JUMP, "didn't ret correct enum val");
  
//   return true;
// error:
//   return false;
// }

int map_test() {
	
	return 1;
error:
	return 0;
}

//

int main(int argc, char const *argv[]) {
	Map* map = Map_create();
	check_mem(map);

	Map_set(map, "key", (void*) 42);
	Map_set(map, "key2", (void*) 999);

	printf("Map_get: %d\n", (int) Map_get(map, "key"));
	check((int)Map_get(map, "key") == 42, "map didn't return keyed value");

	printf("Map_get: %d\n", (int) Map_get(map, "fakekey"));
	check(Map_get(map, "fakekey") == NULL, "map didn't return NULL for dummy key");

	Map_set(map, "key", (void*) 911);
	printf("Map_get: %d\n", (int) Map_get(map, "key"));
	check((int)Map_get(map, "key") == 911, "map didn't modify value");

	Map_clear(map);

	puts("TESTS PASSED");
	return 0;
error:
	puts("TESTS FAILED!!!");
	return 1;
}