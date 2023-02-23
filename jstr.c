#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
# define RESTRICT __restrict__
#elif defined(_MSC_VER)
# define RESTRICT __restrict
#else
# define RESTRICT
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ALWAYS_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define ALWAYS_INLINE __forceinline inline
#else
    #define ALWAYS_INLINE inline
#endif

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
#define MIN(a,b) ((a)<(b)?(a):(b))

int private_jstrCat(Jstr *RESTRICT dest, size_t totalLen, ...)
{
	if (dest->size < (totalLen += dest->len)) {
		size_t tmpSize = dest->size;
		do {
			tmpSize *= 2;
		} while (tmpSize < totalLen);
		if (unlikely(!(dest->data = realloc(dest->data, tmpSize))))
			goto ERROR;
		dest->size = tmpSize;
	}
	va_list ap;
	va_start(ap, totalLen);
	char *RESTRICT tmpDest = dest->data + dest->len;
	for (char *RESTRICT argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		do {
			*tmpDest++ = *argv++;
		} while (*argv);
	va_end(ap);
	*tmpDest = '\0';
	dest->len = totalLen;
	return 1;

ERROR:
	perror("");
	return 0;
}

int jstrPushStr(Jstr *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
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
	char *RESTRICT tmpDest = dest->data + dest->len;
	while ((*tmpDest++ = *src++));
	dest->len = newLen;
	return 1;

ERROR:
	perror("");
	return 0;
}

int jstrPush(Jstr *RESTRICT dest, const char c)
{
	const size_t newLen = dest->len + 1;
	if (unlikely(dest->size < newLen)) {
		const size_t tmpSize = dest->size * 2;
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
