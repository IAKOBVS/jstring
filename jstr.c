#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
/* #include <assert.h> */

#include "jstr.h"

#define ERROR_IF(STATE) \
	if (STATE) \
		goto ERROR

#define GET_SIZE(VAR1) \
	VAR1 = MAX(2 * dest->size, 2 * dest->len)

#define REALLOC_FAILS \
	(!(dest->str = realloc(dest->str, (GET_SIZE(dest->size)))))

#define NEED_MEM \
	(dest->size < 2 * dest->len)

int _jstrCat(Jstr *dest, int argc, ...)
{
	va_list ap;
	va_start(ap, argc);
	size_t argLen=0;
	for (int i=0; i<argc; ++i) {
		char *argv = va_arg(ap, char*);
		argLen += strlen(argv);
	}
	va_end(ap);
	if (dest->size < 2 * argLen && (!(dest->str = realloc(dest->str, (dest->size = (MAX(2 * dest->size, 2 * dest->len)))))))
		goto ERROR;
	va_start(ap, argc);
	for (size_t j = dest->len, i=0; i<argc; ++i) {
		char *argv = va_arg(ap, char *);
		do {
			dest->str[j++] = *argv++;
		} while (*argv);
	}
	va_end(ap);
	dest->str[(dest->len += argLen) + 1] = '\0';
	return dest->size;

ERROR:
	perror("");
	return 0;
}

int _jstrJoin(Jstr *dest, Jstr *src)
{
	if (dest->size < 2 * src->len && (!(dest->str = realloc(dest->str, (dest->size = (MAX(2 * dest->size, 2 * dest->len)))))))
		goto ERROR;
	size_t i = dest->len;
	size_t j = 0;
	do {
		dest->str[i++] = src->str[j++];
	} while (j < src->len);
	dest->str[(dest->len += src->len) + 1] = '\0';
	return dest->size;

ERROR:
	perror("");
	return 0;
}

int _jstrAdd(Jstr *dest, char *src)
{
	size_t srcLen;
	if (!(srcLen = strlen(src))
	|| (dest->size < 2 * dest->len && (!(dest->str = realloc(dest->str, (dest->size = (MAX(2 * dest->size, 2 * dest->len))))))))
		goto ERROR;
	size_t i = dest->len;
	size_t j = 0;
	do {
		dest->str[i++] = src[j++];
	} while (j < srcLen);
	dest->str[(dest->len += srcLen) + 1] = '\0';
	return dest->size;

ERROR:
	perror("");
	return 0;
}

int isJstr(Jstr *structPtr)
{
	if (!*((unsigned char *)&*structPtr))
		return 0;
	return 1;
}
