#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

static char *read_until(const char *string, char token)
{
	char c;
	int index = 0;
	size_t word_len = sizeof(char*); // hehehe
	char *word = malloc(word_len * sizeof(char));
	if (!word) die("malloc(): %i", errno);
	memset(word, 0, word_len * sizeof(char));
	fprintf(stderr, "stage A\n");
	
	while ((c = string[index]) != token && c) {
		fprintf(stderr, "stage B: %c\n", c);
		if (index >= word_len - 2) {
			word_len = word_len * 2;
			word = realloc(word, word_len * sizeof(char));
			if (!word) die("realloc(): %i", errno);
			memset(word+index, 0, word_len - (word_len/2));
			fprintf(stderr, "critical stage X\n");
		}
		word[index++] = c;
		fprintf(stderr, "stage C\n");
	}
	word[index] = '\0';
	fprintf(stderr, "critical stage Y\n");
	return word;
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
	fprintf(stderr,"stage 1\n");

	// double null the string
	alt_len = strlen(string) + 2;
	alt_string = malloc(alt_len * sizeof(char));
	memcpy(alt_string, string, alt_len - 1);
	alt_string[alt_len - 1] = '\0';
	
	while ((word = read_until(alt_string + read_length, token)) && *word) {
		fprintf(stderr, "stage 2: %s\n", word);
		if (index >= list_len - 2) {
			list_len = list_len * 2;
			list = realloc(list, list_len * sizeof(char *));
			if (!list) die("realloc(): %i", errno);
			fprintf(stderr, "critical stage 1\n");
		}
		list[index++] = word;
		// the next line is why we double null
		read_length = read_length + strlen(word) + 1;
		fprintf(stderr, "stage 3\n");
	}
	list[index] = NULL;
	fprintf(stderr, "critical stge 2\n");
	return list;
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
