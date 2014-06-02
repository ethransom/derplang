#include "test.h"

#include "parser_test.c"
#include "map_test.c"

int main () {
  puts("Beginning tests...");

  int failed_tests = 0;

  failed_tests += !(parser_test());
  failed_tests += !(map_test());

  if (failed_tests != 0) {
    printf("Failed %d test(s)\n", failed_tests);
  } else {
  	puts("All tests pass");
  }

  return failed_tests;
}

