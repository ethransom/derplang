/*
 * Contains the C implementation of the Cream standard library
*/

#pragma once

#include "vm.h"
#include "object.h"

#include "debug.h"

err_t* cream_stdlib_println(Cream_vm* vm, int argc, Cream_obj** argv);

err_t* cream_stdlib_print(Cream_vm* vm, int argc, Cream_obj** argv);

err_t* cream_stdlib_len(Cream_vm* vm, int argc, Cream_obj** argv);
