#include "err.h"

#include <stdio.h>
#include <stdlib.h>

err_t InternalErr = {"RuntimeErr",  NULL, NULL, 0};
err_t LookupErr   = {"LookupErr",   NULL, NULL, 0};
err_t TypeErr     = {"TypeErr",     NULL, NULL, 0};

void err_print(err_t* err) {
	printf("%s: %s (%s:%d)\n", err->name, err->msg, err->file, err->line);
}

// TODO: variadic macro or variable arg function?
err_t* err_create(err_t* templ, char* msg) {
	err_t* err = malloc(sizeof(err_t));
	err->name = templ->name;
	err->msg = msg;
	return err;
}
