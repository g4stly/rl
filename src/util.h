#ifndef _util_h
#define _util_h

#include "interface/interface.h"

char **split_string(const char *string, char token);
void die(const char *errstr, ...);
void mega_die(struct Interface *ui, const char *errstr, ...);

#endif 
