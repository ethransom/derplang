FLAGS=-Wall -std=c99

# PRODUCTION_FLAGS=
CFLAGS=-g $(FLAGS)

Modules = list.o object.o vm.o utils.o stdlib.o bytecode_parser.o map.o ast_nodes.o bytecodes.o grammar.tab.o lex.yy.o

Tests = test/parser_test.o

all: main 

# parser files -> .c files
grammar.tab.h grammar.tab.c: grammar.y
	bison -d grammar.y

lex.yy.c: lexer.l grammar.tab.h
	flex lexer.l


# parser.c files -> .o files
grammar.tab.o: grammar.tab.c
	gcc grammar.tab.c -lfl -lm -c

lex.yy.o: lex.yy.c
	gcc lex.yy.c -lfl -lm -c 


main: $(Modules) main.c grammar.tab.c lex.yy.c
	gcc $(CFLAGS) main.c $(Modules) -lm -o main -lfl -lm

test/test: $(Modules)

clean:
	rm -f cream main $(Modules)

dist: $(Modules) main.c
	gcc -o cream $(FLAGS) -lm -Ofast -DNDEBUG $(Modules) main.c

# DERPS are meant for testing
derp/object_derp: object.o
