#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>

#define freeJstr(JSTR_NAME) \
	do { \
		if (JSTR_NAME.size) \
			free(JSTR_NAME.str); \
		JSTR_NAME.size = 0; \
	} while (0)
#define allocJstr(JSTR_NAME, JSTR) \
	JSTR_NAME.len = strlen(JSTR); \
	JSTR_NAME.size = 2 * JSTR_NAME.len; \
	JSTR_NAME.str = malloc(2 * JSTR_NAME.len); \
	memcpy(JSTR_NAME.str, JSTR, JSTR_NAME.len)
#define initJstr(JSTR_NAME, JSTR) \
	Jstr JSTR_NAME = { \
	.str = JSTR}; \
	allocJstr(JSTR_NAME, JSTR)
#define newJstr(A, B) \
	do { \
	initJstr(A, B); \
	} while (0)

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
