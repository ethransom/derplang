#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "../debug.h"

#define DEF_TEST(NAME, BLOCK) bool NAME() { BLOCK return true; error: return false; }

