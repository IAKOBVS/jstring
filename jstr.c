#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

int private_jstrCat(Jstr *dest, ...)
{
	va_list ap;
	va_start(ap, dest);
	size_t argLen = 0;
	for (char *argv = va_arg(ap, char *); argv; argv = va_arg(ap, char*))
		argLen += strlen(argv);
	va_end(ap);
	if (dest->size < 2 * (dest->len + argLen)) {
		size_t tmpSize = MAX(2 * dest->size, 2 * (dest->len + argLen));
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	va_start(ap, dest);
	for (size_t i = dest->len;; ) {
		char *argv = va_arg(ap, char *);
		if (argv);
		else break;
		do {
			dest->data[i] = *argv;
			++i, ++argv;
		} while (*argv);
	}
	va_end(ap);
	dest->data[(dest->len += argLen)] = '\0';
	return 1;

ERROR:
	perror("");
	return 0;
}

int private_jstrPushStr(Jstr *dest, char *src, size_t srcLen)
{
	if (dest->size < 2 * (dest->len + srcLen)) {
		size_t tmpSize = MAX(2 * dest->size, 2 * (dest->len + srcLen));
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	for (size_t i = dest->len; *src; ++i, ++src)
		dest->data[i] = *src;
	dest->data[(dest->len += srcLen)] = '\0';
	return 1;

ERROR:
	perror("");
	return 0;
}

int private_jstrPush(Jstr *dest, char c)
{
	size_t tmpLen = dest->len + 1;
	if (dest->size < 2 * tmpLen) {
		size_t tmpSize = MAX(2 * dest->size, 2 * tmpLen);
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	dest->data[dest->len] = c;
	dest->data[(dest->len += 1)] = '\0';
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
