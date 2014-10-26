/*
 * Contains the C implementation of the Cream standard library
*/

#pragma once

#include "vm.h"
#include "object.h"

#include "debug.h"

bool cream_stdlib_println(int argc, Cream_obj** argv);

bool cream_stdlib_print(int argc, Cream_obj** argv);
