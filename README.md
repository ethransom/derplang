# Cream

## Conventions

 * Same-file helpers should always be marked as `static`.

## Files

### `main.c`

Main entry point. Argument parsing and file opening.

### `debug.h`

Super useful debug macros.

### `list.c` & `list.h`

Custom doubly-linked list data structure.

### `map.c` & `map.h`

Custom hashmap data structure.

### `bytecode_parser.c` & `bytecode_parser.h`

Parse bytecode stored in text files. TODO: binary format?

### `stdlib.c` & `stdlib.h`

Contains standard library functions--written in C.

### `utils.c` & `utils.h`

Various utils.

### `vm.c` & `vm.h`

Virtual machine that executes bytecodes.

### `mpc.c` & `mpc.h`

Parsing/Lexing library. Used in `parser.h` and `parser.c`

### Unused
`cream_fn.c`
`cream_fn.h`
`cream_obj.c`
`cream_obj.h`
