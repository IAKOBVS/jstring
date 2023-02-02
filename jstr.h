#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>
#include "/home/james/c/vargc.h"

#define JSTR_ALLOC(JSTR_NAME, JSTR) \
	JSTR_NAME.len = strlen(JSTR); \
	JSTR_NAME.size = 2 * JSTR_NAME.len; \
	JSTR_NAME.str = malloc(JSTR_NAME.size); \
	memcpy(JSTR_NAME.str, JSTR, JSTR_NAME.len)
#define jstrNew(JSTR_NAME, JSTR) \
	Jstr JSTR_NAME = { .str = JSTR}; \
	JSTR_ALLOC(JSTR_NAME, JSTR)
#define jstrFree(JSTR_NAME) \
	do { \
		if (JSTR_NAME.size) \
			free(JSTR_NAME.str); \
		JSTR_NAME.size = 0; \
	} while (0)

#define jstrPr(JSTR_NAME) printf("string: %s: \nsize is %zu\nlen is %zu", JSTR_NAME.str, JSTR_NAME.size, JSTR_NAME.len)

#define jstrCat(JSTR, ...) \
	_jstrCat(&JSTR, __VA_ARGS__, "")
#define jstrJoin(JSTR_DEST, JSTR_SRC) \
	_jstrJoin(&JSTR_DEST, &JSTR_SRC)
#define jstrAdd(JSTR_DEST, JSTR_STR) \
	_jstrAdd(&JSTR_DEST, JSTR_SRC)

typedef struct Jstr {
	char *str;
	size_t size;
	size_t len;
} Jstr;

int _jstrCat(struct Jstr *dest, int argc, ...);
int _jstrJoin(Jstr *dest, Jstr *src);
int _jstrAdd(Jstr *dest, char *src);
int _isjstr(Jstr *structPtr);
#endif
