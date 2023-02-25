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

ALWAYS_INLINE void jstrInit(Jstr *RESTRICT dest)
{
	dest->data = NULL;
	dest->len = 0;
	dest->size = 0;
}

ALWAYS_INLINE void jstrDeleteFast(Jstr *RESTRICT dest)
{
	free(dest->data);
	jstrInit(dest);
}

ALWAYS_INLINE void jstrDelete(Jstr *RESTRICT dest)
{
	if (dest->data) {
		free(dest->data);
		jstrInit(dest);
	}
}

ALWAYS_INLINE int jstrNew(Jstr *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	if (likely(dest->data = malloc((dest->size = MAX(JSTR_MIN_SIZE, 2 * srcLen)))));
	else goto ERROR;
	dest->len = srcLen;
	memcpy(dest->data, src, srcLen + 1);
	return 1;

ERROR:
	dest->size = 0;
	return 0;
}

int private_jstrCat(Jstr *RESTRICT dest, ...)
{
	size_t totalLen = dest->len;
	va_list ap;
	va_start(ap, dest);
	char *RESTRICT argv;
	while ((totalLen += (argv = va_arg(ap, char *)) ? strlen(argv) : 0), argv);
	va_end(ap);
	if (dest->size < (totalLen += dest->len)) {
		do {
			dest->size *= 2;
		} while (dest->size < totalLen);
		if (likely((dest->data = realloc(dest->data, dest->size))));
		else goto ERROR_FREE;
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

ERROR_FREE:
	jstrDeleteFast(dest);
	return 0;
}

inline int jstrAppend(Jstr *RESTRICT dest, const char *RESTRICT src, const size_t srcLen)
{
	const size_t newLen = dest->len + srcLen;
	if (dest->size < newLen) {
		do {
			dest->size *= 2;
		} while (dest->size < newLen);
		if (likely((dest->data = realloc(dest->data, dest->size))));
		else goto ERROR_FREE;
	}
	memcpy(dest->data + dest->len, src, srcLen + 1);
	dest->len = newLen;
	return 1;

ERROR_FREE:
	jstrDeleteFast(dest);
	return 0;
}

inline int jstrPushback(Jstr *RESTRICT dest, const char c)
{
	if (likely(dest->size >= dest->len + 1));
	else
		if (likely((dest->data = realloc(dest->data, (dest->size *= 2)))));
		else goto ERROR_FREE;
	dest->data[dest->len] = c;
	dest->data[dest->len + 1] = '\0';
	return 1;

ERROR_FREE:
	jstrDeleteFast(dest);
	return 0;
}

ALWAYS_INLINE void jstrPopback(Jstr *RESTRICT dest)
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

ALWAYS_INLINE void jstrSwapStr(Jstr *RESTRICT dest, char **RESTRICT src, size_t *RESTRICT srcLen, size_t *RESTRICT srcSize)
{
	char *RESTRICT tmpSrc = *src;
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

inline int jstrReplace(Jstr *RESTRICT dest, char *RESTRICT src, const size_t srcLen)
{
	if (dest->size > srcLen + 1);
	else
		if (likely((dest->data = realloc(dest->data, srcLen + 1))));
		else goto ERROR_FREE;
	memcpy(dest->data, src, srcLen + 1);
	return 1;

ERROR_FREE:
	jstrDeleteFast(dest);
	return 0;
}

inline int jstrReserve(Jstr *RESTRICT dest, size_t size)
{
	if (dest->size) {
		if (likely((dest->data = realloc(dest->data, (dest->size = size) * sizeof dest->data[0]))));
		else goto ERROR_FREE;
	} else {
		if (likely((dest->data = malloc((dest->size = size) * sizeof dest->data[0]))));
		else goto ERROR;
	}
	return 1;

ERROR_FREE:
	jstrDeleteFast(dest);
ERROR:
	return 0;
}

inline int jstrShrink(Jstr *RESTRICT dest)
{
	if (likely((dest->data = realloc(dest->data, dest->len + 1)))) {
		dest->size = dest->len;
		return 1;
	}
	jstrDeleteFast(dest);
	return 0;
}
