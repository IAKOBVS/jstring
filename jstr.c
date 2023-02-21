#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

#if defined(__PRETTY_FUNCTION__)
	#define CURR_FUNC __PRETTY_FUNCTION__
#elif defined(__FUNCTION__)
	#define CURR_FUNC __FUNCTION__
#elif defined(__func__)
	#define CURR_FUNC __func__
#else
	#define CURR_FUNC ""
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || (defined(__clang__) && __has_builtin(__builtin_expect))
  #define likely(x) __builtin_expect(!!(x), 1)
  #define unlikely(x) __builtin_expect(!!(x), 0)
#else
  #define likely(x) (x)
  #define unlikely(x) (x)
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))

int private_jstrCat(Jstr *restrict dest, ...)
{
	va_list ap;
	va_start(ap, dest);
	size_t newLen = dest->len;
	char *restrict argv;
	for (argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		newLen += strlen(argv);
	va_end(ap);
	if (dest->size < newLen) {
		size_t tmpSize = dest->size;
		do {
			tmpSize *= 2;
		} while (tmpSize < newLen);
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	va_start(ap, dest);
	char *restrict tmpStr = dest->data + dest->len;
	for (argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		do {
			*tmpStr++ = *argv++;
		} while (*argv);
	va_end(ap);
	*tmpStr = '\0';
	dest->len = newLen;
	return 1;

ERROR:
	perror("");
	return 0;
}

int jstrPushStr(Jstr *restrict dest, const char *restrict src, const size_t srcLen)
{
	const size_t newLen = dest->len + srcLen;
	if (dest->size < newLen) {
		size_t tmpSize = dest->size;
		do {
			tmpSize *= 2;
		} while (tmpSize < newLen);
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	char *tmpStr;
	for (tmpStr = dest->data + dest->len; *src; ++tmpStr, ++src)
		*tmpStr = *src;
	*tmpStr = '\0';
	dest->len = newLen;
	return 1;

ERROR:
	perror("");
	return 0;
}

int jstrPush(Jstr *restrict dest, const char c)
{
	size_t newLen = dest->len + 1;
	if (unlikely(dest->size < newLen)) {
		size_t tmpSize = dest->size * 2;
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	*(dest->data + dest->len - 1) = c;
	*(dest->data + dest->len) = '\0';
	dest->len = newLen;
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
