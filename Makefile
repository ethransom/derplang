FLAGS=-Wall -std=c99

# PRODUCTION_FLAGS=
CFLAGS=-g $(FLAGS)

Modules = list.o cream_obj.o vm.o utils.o stdlib.o parser.o bytecode_parser.o map.o mpc.o

Tests = test/parser_test.o

all: main 

main: $(Modules) main.c
	gcc $(CFLAGS) main.c $(Modules) -lm -o main

test/test: $(Modules)

clean:
	rm -f cream main $(Modules)

dist: $(Modules) main.c
	gcc -o cream $(FLAGS) -lm -Ofast -DNDEBUG $(Modules) main.c
