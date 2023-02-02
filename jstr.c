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

int _jstrCat(Jstr *dest, ...)
{
	va_list ap;
	va_start(ap, dest);
	int argLen=0;
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		argLen += strlen(argvStr);
	}
	va_end(ap);
	int i = dest->len;
	dest->len += argLen;
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	/* while (dest->str[i]) */
	/* 	++i; */
	va_start(ap, dest);
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		do {
			(dest->str)[i++] = *argvStr++;
		} while (*argvStr);
	}
	va_end(ap);
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERROR:
	perror("int jstrcat(char *dest->string, ...): ");
	return 0;
}

int _jstrJoin(Jstr *dest, Jstr *src)
{
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	int i = dest->len;
	int j = 0;
	while (dest->str[i])
		++i;
	do {
		(dest->str)[i++] = (src->str)[j++];
	} while (j < src->len);
	dest->len += src->len;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;
ERROR:
	perror("int jstradd(Jstr *dest, Jstr *src): ");
	return 0;
}

int _jstrAdd(Jstr *dest, char *src)
{
	size_t srcLen;
	ERROR_IF( !(srcLen = strlen(src)));
	if (dest->size < 2 * dest->len)
		ERROR_IF(REALLOC_FAILS);
	int i = dest->len;
	int j = 0;
	/* while (dest->str[i]) */
	/* 	++i; */
	do {
		(dest->str)[i++] = (src)[j++];
	} while (j < srcLen);
	dest->len += srcLen;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERROR:
	perror("int stradd(Jstr *dest, char *src): ");
	return 0;
}

int _isJstr(Jstr *structPtr)
{
	if (!*((unsigned char *)&*structPtr))
		return 0;
	return 1;
}
