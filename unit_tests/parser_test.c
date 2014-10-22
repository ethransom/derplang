#include "test.h" 

#include "../parser.h"

bool test_cream_vm_str2code() {
  check(cream_vm_str2code("foobar") == CODE_NULL, "didn't NULL when given bad value");
  
  check(cream_vm_str2code("call") == CODE_CALL, "didn't ret correct enum val");
  check(cream_vm_str2code("jump") == CODE_JUMP, "didn't ret correct enum val");
  
  return true;
error:
  return false;
}

int parser_test() {
  return test_cream_vm_str2code();
}

