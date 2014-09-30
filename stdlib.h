/*
 * Contains the C implementation of the Cream standard library
*/

#ifndef STDLIB_H
#define STDLIB_H

#include "vm.h"
#include "object.h"

#include "debug.h"

void cream_stdlib_println(Cream_vm* vm, int argc);

void cream_stdlib_print(Cream_vm* vm, int argc);

void cream_run_gc(Cream_vm* vm, int argc);

#endif