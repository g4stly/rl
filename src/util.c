#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "interface/main_ui.h"

static char *read_until(const char *string, char token)
{
	char c;
	int index = 0;
	size_t word_len = sizeof(char*); // hehehe
	char *word = malloc(word_len * sizeof(char));
	if (!word) die("malloc(): %i", errno);
	memset(word, 0, word_len * sizeof(char));
	
	while ((c = string[index]) != token && c) {
		if (index >= word_len - 2) {
			word_len = word_len * 2;
			word = realloc(word, word_len * sizeof(char));
			if (!word) die("realloc(): %i", errno);
			memset(word+index, 0, word_len - (word_len/2));
		}
		word[index++] = c;
	}
	word[index] = '\0';
	return word;
}

int randomInt(int limit)
{
	int rv;
	int divisor = RAND_MAX/(limit+1);
	do {
		rv = rand() / divisor;
	} while (rv > limit);
	return rv;
}

char **split_string(const char *string, char token)
{
	char *word;
	char *alt_string;
	int index = 0;
	int read_length = 0;
	size_t alt_len = 0;
	size_t list_len = 2;
	char **list = malloc(list_len * sizeof(char *)); // legendary
	if (!list) die("malloc(): %i", errno);
	memset(list, 0, list_len * sizeof(char *));

	// double null the string
	alt_len = strlen(string) + 2;
	alt_string = malloc(alt_len * sizeof(char));
	memcpy(alt_string, string, alt_len - 1);
	alt_string[alt_len - 1] = '\0';
	
	while ((word = read_until(alt_string + read_length, token)) && *word) {
		if (index >= list_len - 2) {
			list_len = list_len * 2;
			list = realloc(list, list_len * sizeof(char *));
			if (!list) die("realloc(): %i", errno);
		}
		list[index++] = word;
		// the next line is why we double null
		read_length = read_length + strlen(word) + 1;
	}
	list[index] = NULL;
	return list;
}

void free_ptrArray(char **array)
{
	char *ptr = *array;
	for (int i = 0; ptr; ptr = array[++i]) {
		free(ptr);
	}
	free(array);
}

unsigned int string_compare(const char *str1, const char *str2, int n)
{
	int matches = 0;
	for (int i = 0; i < n; i++) {
		if (str1[i] != str2[i]) break;
		matches++;
	}
	return matches;
}

void mega_die(struct Interface *ui, const char *fmt, ...) 
{
	interface_Shutdown(ui);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
			fputc(' ', stderr);
			perror(NULL);
	}

	exit(1);
}

void die(const char *fmt, ...) 
{
	interface_Shutdown(main_ui);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
			fputc(' ', stderr);
			perror(NULL);
	}

	exit(1);
}
