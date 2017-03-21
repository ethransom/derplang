/*
 * Some basic utility functions
 * Only used by the bytecode parser
 *
 * If the bytecode parsing function is removed, remove this as well
*/

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

bool derp_char_in_str(char c, char* str);

char* derp_read_until(char* inbuf, int* i, char* term);

char* derp_read_from_whitelist(char* inbuf, int* i, char* whitelist);

char* derp_read_until_from_str(char* inbuf, int* i, char* term, char* allowed);

char* derp_read_until_from_str_panicky(char* inbuf, int* i, char* term, char* allowed);
