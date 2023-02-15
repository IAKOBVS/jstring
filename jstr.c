#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "jstr.h"

int private_jstrCat(Jstr *dest, ...)
{
	va_list ap;
	va_start(ap, dest);
	size_t argLen = 0;
	for (char *argv = va_arg(ap, char*); argv != NULL; argv = va_arg(ap, char*))
		argLen += strlen(argv);
	va_end(ap);
	if (dest->size < 2 * (dest->len + argLen)) {
		size_t tmpSize = MAX(2 * dest->size, 2 * (dest->len + argLen));
		if (!(dest->str = realloc(dest->str, tmpSize))) goto ERROR;
		dest->size = tmpSize;
	}
	va_start(ap, dest);
	size_t i = dest->len;
	for (char *argv = va_arg(ap, char*); argv != NULL; argv = va_arg(ap, char*))
		do {
			dest->str[i] = *argv;
			++i, ++argv;
		} while (*argv);
	va_end(ap);
	dest->str[(dest->len += argLen)] = '\0';
	return 1;

ERROR:
	perror("");
	return 0;
}

int private_jstrAdd(Jstr *dest, char *src, size_t srcLen)
{
	if (dest->size < 2 * (dest->len + srcLen)) {
		size_t tmpSize = MAX(2 * dest->size, 2 * (dest->len + srcLen));
		if (!(dest->str = realloc(dest->str, tmpSize))) goto ERROR;
		dest->size = tmpSize;
	}
	size_t i = dest->len;
	while (*src) {
		dest->str[i] = *src;
		++i, ++src;
	}
	dest->str[(dest->len += srcLen)] = '\0';
	return 1;

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
