#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#include <stdlib.h>
#include "/home/james/c/vargc.h"

#define MIN_SIZE 8

#define MAX(NUM1, NUM2) \
	((NUM1 > NUM2) ? (NUM1) : (NUM2))

#define jstrNew(JSTR, CONST_STRING) \
	do { \
		JSTR.len = strlen(CONST_STRING); \
		JSTR.size = MAX(JSTR.len, MIN_SIZE); \
		if (!(JSTR.str = malloc(JSTR.size))) { \
			perror(""); exit(EXIT_FAILURE); } \
		JSTR.str = malloc(JSTR.size); \
		memcpy(JSTR.str, CONST_STRING, JSTR.len); \
	} while (0)

#define jstrDelete(JSTR) \
	do { \
		if (JSTR.size) \
			free(JSTR.str); \
		JSTR.size = 0; \
	} while (0)
#define jstrDeletePtr(JSTR) \
	do { \
		if (JSTR->size) \
			free(JSTR->str); \
		JSTR->size = 0; \
	} while (0)

#define jstrPr(JSTR) printf("string: %s: \nsize is %zu\nlen is %zu", JSTR.str, JSTR.size, JSTR.len)

typedef struct Jstr {
	char *str;
	size_t size;
	size_t len;
} Jstr;

int _jstrCat(struct Jstr *dest, int argc, ...);
#define jstrCat(JSTR, ...) \
	_jstrCat(&JSTR, PP_NARG(__VA_ARGS__), __VA_ARGS__, "")

int _jstrJoin(Jstr *dest, Jstr *src);
#define jstrJoin(JSTR_DEST, JSTR_SRC) \
	_jstrJoin(&JSTR_DEST, &JSTR_SRC)

int _jstrAdd(Jstr *dest, char *src);
#define jstrAdd(JSTR_DEST, JSTR_STR) \
	_jstrAdd(&JSTR_DEST, JSTR_SRC)

int isjstr(Jstr *structPtr);
#endif
