#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

// #include "cream_fn.h"
#include "debug.h"
#include "vm.h"
#include "object.h"
#include "list.h"
#include "bytecode_parser.h"
// #include "grammar.tab.h"
#include "lexer.h"
#include "ast_nodes.h"

extern List* programBlock;

#define EXIT_SUCCESS   0
#define EXIT_PROG_ERR  1
#define EXIT_BAD_ARGS  2
#define EXIT_EXCEPTION 3

// from 'debug.h'
extern int debug_flag;

void print_usage (FILE* stream, char* program_name) {
	fprintf (stream, "Usage:  %s INPUTFILES [options]\n", program_name);
	fprintf (stream,
		"  -h  --help             Display this usage information.\n"
		"  -s  --stdin            Use stdin as the input stream.\n"
    "  -p  --parse            Use the experimental native parser.\n"
#ifndef NDEBUG
		"  -v  --verbose          Print verbose messages.\n"
#endif
	);
#ifndef NDEBUG
	fprintf(stream, "This is a debug version of the program\n");
#endif
}

const struct option long_options[] = {
	{"help",    0, NULL, 'h'},
	{"stdin",   0, NULL, 's'},
	{"parse",   0, NULL, 'p'},
#ifndef NDEBUG
	{"verbose", 0, NULL, 'v'},
#endif
	{NULL,      0, NULL, 0  }
};

int main(int argc, char *argv[]) {
	bool parse_flag = false;
	bool use_stdin = false;
	debug_flag = 0;

	char ch;
	while ((ch = getopt_long(argc, argv, "hsvp", long_options, NULL)) != -1) {
		switch (ch) {
			case 'h':
				print_usage(stderr, argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				print_usage(stderr, argv[0]);
				exit(EXIT_BAD_ARGS);
			case 's':
				use_stdin = true;
				break;
			case 'p':
				parse_flag = true;
				break;
#ifndef NDEBUG
			case 'v':
				debug_flag = 1;
				debug("DEBUG is on");
				break;
#endif
			default:
				if (ch == 'v') {
					fprintf(stderr, "This executable doesn't include debugging utilties\n");
					exit(EXIT_BAD_ARGS);
				} else {
					fprintf(stderr, "'getopt_long' returned unknown '%c'\n", ch);
					exit(EXIT_PROG_ERR);
				}
				break;
		}
	}

	Cream_vm* vm = cream_vm_create();

	FILE* input;

	if (use_stdin) {
		debug("using 'stdin' as input");
		input = stdin;
	} else {
		int filec = argc - optind;

		if (filec == 0) {
			fprintf(stderr, "No input file given\n");
			print_usage(stderr, argv[0]);
			exit(EXIT_BAD_ARGS);
		}

		if (filec > 1) {
			log_warn("more than one input file given--ignoring all but first");
		}

		debug("reading file '%s'", argv[optind]);
		input = fopen(argv[optind], "r");
	}

	if (parse_flag) {
		yyin = input;
		do {
			yyparse();
		} while (!feof(yyin));

		ast_list_print(programBlock, 0);

		return EXIT_SUCCESS;
	}

	check(cream_bytecode_parse_stream(input, vm) == 1, "parse failed");

	cream_vm_run(vm);

	// puts("main is dumping codes: ");
	// for (int j = 0; j < vm->num_bytecodes; j++)
		// printf("code: %d\n", ((vm->bytecode)[j]).code);

	cream_vm_destroy(vm);

	return EXIT_SUCCESS;
error:
	return EXIT_PROG_ERR;
}
