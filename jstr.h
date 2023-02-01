#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>

#define freejstr(JSTR_NAME) \
	do { \
		if (JSTR_NAME.size) \
			free(JSTR_NAME.str); \
		JSTR_NAME.size = 0; \
	} while (0)
#define allocjstr(JSTR_NAME, JSTR) \
	JSTR_NAME.len = strlen(JSTR); \
	JSTR_NAME.size = 2 * JSTR_NAME.len; \
	JSTR_NAME.str = malloc(2 * JSTR_NAME.len); \
	memcpy(JSTR_NAME.str, JSTR, JSTR_NAME.len)
#define newjstr(JSTR_NAME, JSTR) \
	Jstr JSTR_NAME = { \
	.str = JSTR}; \
	allocjstr(JSTR_NAME, JSTR)
#define jstrcat(STR, ...) jstr_cat(STR, __VA_ARGS__, "")

typedef struct Jstr {
	char *str;
	size_t size;
	size_t len;
} Jstr;

int jstr_cat(struct Jstr *dest, ...);
int jstradd(Jstr *dest, Jstr *src);
int stradd(Jstr *dest, char *src);
int aredigits(char* src);
int isjstr(Jstr *structPtr);
#endif
