#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>

#define newJstr(_NAME_OF_STRING, _STRING) jstr _NAME_OF_STRING = {.str = _STRING, .size = 0}

typedef struct Jstr {
	char *str;
	size_t size;
	size_t len;
} Jstr;

/* end with "\0" or "" since the \0 is implicit */
int jstrCat(struct Jstr *dest, ...);
int jstrAdd(Jstr *dest, Jstr *src);
int strAdd(Jstr *dest, char *src);
int areDigits(char* src);
int isJstr(Jstr *structPtr);
#endif
