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

#define jstrInit(JSTR) \
	Jstr JSTR = { \
		.size = 0, \
		.len = 0 \
	}

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

#define jstrMinimize(JARR) \
	JARR.str = realloc(JARR.str, JARR.len)

#define jstrReserve(JARR, ALLOC_SIZE) \
	do { \
		JARR.str = malloc(ALLOC_SIZE); \
		JARR.size = ALLOC_SIZE; \
	} while (0)

#define jstrResize(ALLOC_SIZE) \
	do { \
		JARR.str = realloc(ALLOC_SIZE) \
		JARR.size = ALLOC_SIZE; \
		JARR.len = ALLOC_SIZE; \
	} while (0)

/* #define jstrMatch(JARR, DELIM) \ */

typedef struct Jstr {
	char *str;
	size_t size;
	size_t len;
} Jstr;

int _jstrCat(struct Jstr *dest, ...);
#define jstrCat(JSTR, ...) \
	_jstrCat(&JSTR, __VA_ARGS__, NULL)

int _jstrAdd(Jstr *dest, char *src, size_t srcLen);
#define jstrAdd(JSTR_DEST, STR_SRC) \
	_jstrAdd(&JSTR_DEST, STR_SRC)
#define jstrAddJstr(JSTR_DEST, JSTR_SRC) \
	_jstrAdd(&JSTR_DEST, &JSTR_SRC, JSTR->len)

int isJstr(Jstr *structPtr);
#endif
