#ifndef _util_h
#define _util_h

#include "interface/interface.h"

char **split_string(const char *string, char token);
void die(const char *errstr, ...);
void mega_die(struct Interface *ui, const char *errstr, ...);
void free_ptrArray(char **array);
unsigned int string_compare(const char *str1, const char *str2, int n);

#endif 
