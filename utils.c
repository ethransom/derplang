#include <stdio.h>

#include "utils.h"

// returns true if c is present in str
// str must be null-terminated
bool derp_char_in_str(char c, char* str) {
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] == c) return true;
		i++;
	}

	return false;
}

// copy from inbuf to outbuf, advancing i all the while, until a character in term is encountered
char* derp_read_until(char* inbuf, int* i, char* term) {
	char outbuf[16];
	int j = 0;
	while (!derp_char_in_str(inbuf[*i], term)) {
		outbuf[j] = inbuf[*i];
		j++;
		(*i)++;
	}

	// prepare the heaped string
	outbuf[j] = '\0';
	char *str = malloc(j);
	check_mem(str);
	strcpy(str, outbuf);

	return str;
error:
	if (str != NULL) free(str);
	return NULL;
}

// copy from inbuf to outbuf, advancing i all the while, until a character NOT in term is encountered
char* derp_read_from_whitelist(char* inbuf, int* i, char* whitelist) {
	char outbuf[16];
	int j = 0;
	while (derp_char_in_str(inbuf[*i], whitelist)) {
		outbuf[j] = inbuf[*i];
		j++;
		(*i)++;
	}

	// prepare the heaped string
	outbuf[j] = '\0';
	char *str = malloc(j);
	check_mem(str);
	strcpy(str, outbuf);

	return str;
error:
	if (str != NULL) free(str);
	return NULL;
}

// copy from inbuf to outbuf, advancing i all the while, until a character in term is encountered
char* derp_read_until_from_str(char* inbuf, int* i, char* term, char* allowed) {
	char outbuf[16];
	int j = 0;
	while (derp_char_in_str(inbuf[*i], allowed)) {
		if (derp_char_in_str(inbuf[*i], term)) break;
		outbuf[j] = inbuf[*i];
		j++;
		(*i)++;
	}

	// prepare the heaped string
	outbuf[j] = '\0';
	char *str = malloc(j);
	check_mem(str);
	strcpy(str, outbuf);

	return str;
error:
	if (str != NULL) free(str);
	return NULL;
}

// copy from inbuf to outbuf, advancing i all the while, until a character in term is encountered
// if a character not in term or allowed is encountered, abort
char* derp_read_until_from_str_panicky(char* inbuf, int* i, char* term, char* allowed) {
	char outbuf[16];
	int j = 0;
	while (derp_char_in_str(inbuf[*i], allowed)) {
		if (derp_char_in_str(inbuf[*i], term)) break;
		outbuf[j] = inbuf[*i];
		j++;
		(*i)++;
	}

	// if a character not in term or allowed is encountered, abort
	if (!derp_char_in_str(inbuf[*i], allowed) && !derp_char_in_str(inbuf[*i], term)) {
		debug("aborted on weird char: '%c'", inbuf[*i]);
		goto error;
	}

	// prepare the heaped string
	outbuf[j] = '\0';
	char *str = NULL;
	str = malloc(j);
	check_mem(str);
	strcpy(str, outbuf);

	return str;
error:
	if (str != NULL) free(str);
	return NULL;
}
