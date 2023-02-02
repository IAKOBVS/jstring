#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>

#define JSTR_ALLOC(JSTR_NAME, JSTR) \
	JSTR_NAME.len = strlen(JSTR); \
	JSTR_NAME.size = 2 * JSTR_NAME.len; \
	JSTR_NAME.str = malloc(2 * JSTR_NAME.len); \
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

#define jstrCat(JSTR, ...) jstr_cat(&JSTR, __VA_ARGS__, "")
#define jstrAdd(JSTR_DEST, JSTR_SRC) jstr_add(&JSTR_DEST, &JSTR_SRC)
#define jstrAddStr(JSTR_DEST, JSTR_STR) jstr_add_str(&JSTR_DEST, JSTR_SRC)

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
