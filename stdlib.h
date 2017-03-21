/*
 * Contains the C implementation of the Derplang standard library
*/

#pragma once

#include "vm.h"
#include "object.h"

#include "debug.h"

err_t* derp_stdlib_println(Derp_vm* vm, int argc, Derp_obj** argv);

err_t* derp_stdlib_print(Derp_vm* vm, int argc, Derp_obj** argv);

err_t* derp_stdlib_len(Derp_vm* vm, int argc, Derp_obj** argv);

err_t* derp_stdlib_range(Derp_vm* vm, int argc, Derp_obj** argv);