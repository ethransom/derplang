#pragma once

typedef struct {
  char* name;
  char* msg;
  char* file;
  int line;
} err_t;

err_t InternalErr;
err_t LookupErr;
err_t TypeErr;

void err_print(err_t* err);

err_t* err_create(err_t* templ, char* msg);