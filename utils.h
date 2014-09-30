/*
 * Some basic utility functions
 * Only used by the bytecode parser
 *
 * If the bytecode parsing function is removed, remove this as well
*/

#ifndef CREAM_UTILS_H
#define CREAM_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

bool cream_char_in_str(char c, char* str);

char* cream_read_until(char* inbuf, int* i, char* term);

char* cream_read_from_whitelist(char* inbuf, int* i, char* whitelist);

char* cream_read_until_from_str(char* inbuf, int* i, char* term, char* allowed);

char* cream_read_until_from_str_panicky(char* inbuf, int* i, char* term, char* allowed);

#endif
