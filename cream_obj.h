#ifndef CREAM_OBJ_H
#define CREAM_OBJ_H

// #include "cream_utils.h"

typedef struct {
  int is_native;
} Cream_obj;

Cream_obj* cream_obj_create(int id);

void cream_obj_destroy(Cream_obj *obj);

#endif
