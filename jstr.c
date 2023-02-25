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

int private_jstrCat(Jstr *RESTRICT dest, ...)
{
	size_t totalLen = dest->len;
	va_list ap;
	char *RESTRICT argv;
	va_start(ap, dest);
	while ((totalLen += (argv = va_arg(ap, char *)) ? strlen(argv) : 0), argv);
	va_end(ap);
	if (dest->size < (totalLen += dest->len)) {
		size_t tmpSize = dest->size;
		do {
			tmpSize *= 2;
		} while (tmpSize < totalLen);
		if (likely((dest->data = realloc(dest->data, tmpSize))));
		else goto ERROR;
		dest->size = tmpSize;
	}
	va_start(ap, dest);
	char *RESTRICT tmpDest = dest->data + dest->len;
	for (argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		do {
			*tmpDest++ = *argv++;
		} while (*argv);
	va_end(ap);
	*tmpDest = '\0';
	dest->len = totalLen;
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

int jstrAppend(Jstr *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	const size_t newLen = dest->len + srcLen;
	if (dest->size < newLen) {
		size_t tmpSize = dest->size;
		do {
			tmpSize *= 2;
		} while (tmpSize < newLen);
		if (likely((dest->data = realloc(dest->data, tmpSize))));
		else goto ERROR;
		dest->size = tmpSize;
	}
	memcpy(dest->data + dest->len, src, srcLen);
	dest->len = newLen;
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

int jstrPush(Jstr *RESTRICT dest, const char c)
{
	if (likely(dest->size >= dest->len + 1));
	else {
		const size_t tmpSize = dest->size * 2;
		if (likely((dest->data = realloc(dest->data, tmpSize))));
		else goto ERROR;
		dest->size = tmpSize;
	}
	*(dest->data + dest->len) = c;
	*(dest->data + ++dest->len) = '\0';
	return 1;

ERROR:
	perror(CURR_FUNC);
	return 0;
}

ALWAYS_INLINE void jstrPop(Jstr *RESTRICT dest)
{
	dest->data[--dest->len] = '\0';
}

ALWAYS_INLINE void jstrSwap(Jstr *RESTRICT dest, Jstr *RESTRICT src)
{
	char *RESTRICT tmpSrc = src->data;
	const size_t srcSize = src->size;
	const size_t srcLen = src->len;
	src->data = dest->data;
	src->size = dest->size;
	src->len = dest->len;
	dest->data = tmpSrc;
	dest->size = srcSize;
	dest->len = srcLen;
}

ALWAYS_INLINE void jstrSwapStr(Jstr *RESTRICT dest, char **RESTRICT src, size_t *srcLen, size_t *srcSize)
{
	char *tmpSrc = *src;
	const size_t tmpSrcSize = *srcSize;
	const size_t tmpSrcLen = *srcLen;
	*src = dest->data;
	*srcSize = dest->size;
	*srcLen = dest->len;
	dest->data = tmpSrc;
	dest->size = tmpSrcSize;
	dest->len = tmpSrcLen;
}

ALWAYS_INLINE int jstrCmp(Jstr *RESTRICT dest, Jstr *RESTRICT src)
{
	return (dest->len != src->len) ? 1 : memcmp(dest->data, src->data, dest->len);
}

ALWAYS_INLINE int jstrReplace(Jstr *RESTRICT dest, char *RESTRICT src, const size_t srcLen)
{
	if (dest->size > srcLen + 1);
	else
		if (likely((dest->data = realloc(dest->data, srcLen + 1))));
		else goto ERROR_FREE;
	memcpy(dest->data, src, srcLen + 1);
	return 1;

ERROR_FREE:
	free(dest->data);
	perror("jstrReplace");
	return 0;
}
