#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "jstr.h"

#define ERROR_IF(STATE) \
	if (STATE) { \
		goto ERROR; \
	}
#define MAX(NUM1, NUM2) \
	(NUM1 > NUM2) ? NUM1 : NUM2
#define GET_SIZE(VAR1) \
	VAR1 = MAX(2 * dest->size, 2 * dest->len)
#define REALLOC_FAILS \
	!(dest->str = realloc(dest->str, (GET_SIZE(dest->size))))

int _jstrCat(Jstr *dest, int argc, ...)
{
	va_list ap;
	va_start(ap, argc);
	int argLen=0;
	for (int i=0; i<argc; ++i) {
		char *argvStr = va_arg(ap, char*);
		argLen += strlen(argvStr);
	}
	va_end(ap);
	size_t j = dest->len;
	dest->len += argLen;
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	/* while (dest->str[i]) */
	/* 	++i; */
	va_start(ap, argc);
	for (int i=0; i<argc; ++i) {
		char *argvStr = va_arg(ap, char*);
		do {
			(dest->str)[j++] = *argvStr++;
		} while (*argvStr);
	}
	va_end(ap);
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;
ERROR:
	perror("int jstrCat(char *dest->string, int argc, ...): ");
	return 0;
}

int _jstrJoin(Jstr *dest, Jstr *src)
{
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	size_t i = dest->len;
	size_t j = 0;
	/* while (dest->str[i]) */
	/* 	++i; */
	do {
		(dest->str)[i++] = (src->str)[j++];
	} while (j < src->len);
	dest->len += src->len;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;
ERROR:
	perror("int jstJoin(Jstr *dest, Jstr *src): ");
	return 0;
}

int _jstrAdd(Jstr *dest, char *src)
{
	size_t srcLen;
	ERROR_IF( !(srcLen = strlen(src)));
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	size_t i = dest->len;
	size_t j = 0;
	/* while (dest->str[i]) */
	/* 	++i; */
	do {
		(dest->str)[i++] = (src)[j++];
	} while (j < srcLen);
	dest->len += srcLen;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;
ERROR:
	perror("int strAdd(Jstr *dest, char *src): ");
	return 0;
}

int _isJstr(Jstr *structPtr)
{
	if (!*((unsigned char *)&*structPtr))
		return 0;
	return 1;
}
