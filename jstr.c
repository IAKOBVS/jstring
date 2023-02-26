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

ALWAYS_INLINE void jstr_init(Jstring *RESTRICT dest)
{
	dest->data = NULL;
	dest->size = 0;
	dest->capacity = 0;
}

ALWAYS_INLINE void jstr_delete_fast(Jstring *RESTRICT dest)
{
	free(dest->data);
	jstr_init(dest);
}

ALWAYS_INLINE void jstr_delete(Jstring *RESTRICT dest)
{
	if (dest->data) {
		free(dest->data);
		jstr_init(dest);
	}
}

ALWAYS_INLINE int jstr_new(Jstring *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	if ((dest->data = malloc((dest->capacity = MAX(JSTR_MIN_SIZE, 2 * src_size)))));
	else goto ERROR;
	dest->size = src_size;
	memcpy(dest->data, src, src_size + 1);
	return 1;

ERROR:
	jstr_init(dest);
	return 0;
}

int private_jstr_cat(Jstring *RESTRICT dest, ...)
{
	size_t total_size = dest->size;
	char *RESTRICT tmp_dest = dest->data + total_size;
	va_list ap;
	va_start(ap, dest);
	for (char *RESTRICT argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		total_size += strlen(argv);
	va_end(ap);
	if (dest->capacity < total_size) {
		size_t tmp_cap = dest->capacity;
		do {
			tmp_cap *= 2;
		} while (tmp_cap < total_size);
		if (likely((dest->data = realloc(dest->data, (tmp_cap = dest->capacity)))));
		else goto ERROR_FREE;
	}
	va_start(ap, dest);
	for (char *RESTRICT argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		do {
			*tmp_dest++ = *argv++;
		} while (*argv);
	va_end(ap);
	*tmp_dest = '\0';
	dest->size = total_size;
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

inline int jstr_append(Jstring *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	const size_t total_size = dest->size + src_size;
	if (dest->capacity < total_size) {
		size_t tmp_cap = dest->capacity;
		do {
			tmp_cap *= 2;
		} while (tmp_cap < total_size);
		if (likely((dest->data = realloc(dest->data, (dest->capacity = tmp_cap)))));
		else goto ERROR_FREE;
	}
	memcpy(dest->data + dest->size, src, src_size + 1);
	dest->size = total_size;
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

inline int jstr_push_back(Jstring *RESTRICT dest, const char c)
{
	if (likely((dest->capacity > dest->size)));
	else
		if (likely((dest->data = realloc(dest->data, (dest->capacity *= 2)))));
		else goto ERROR_FREE;
	dest->data[dest->size] = c;
	dest->data[dest->size + 1] = '\0';
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

ALWAYS_INLINE void jstr_pop_back(Jstring *RESTRICT dest)
{
	dest->data[--dest->size] = '\0';
}

ALWAYS_INLINE void jstr_swap(Jstring *RESTRICT dest, Jstring *RESTRICT src)
{
	char *RESTRICT tmp_src = src->data;
	const size_t src_cap = src->capacity;
	const size_t src_size = src->size;
	src->data = dest->data;
	src->capacity = dest->capacity;
	src->size = dest->size;
	dest->data = tmp_src;
	dest->capacity = src_size;
	dest->size = src_size;
}

ALWAYS_INLINE void jstr_swap_str(Jstring *RESTRICT dest, char **RESTRICT src, size_t *RESTRICT src_size, size_t *RESTRICT src_capacity)
{
	char *RESTRICT tmp_src = *src;
	const size_t tmp_src_size = *src_size;
	const size_t tmp_src_cap = *src_capacity;
	*src = dest->data;
	*src_size = dest->size;
	*src_capacity = dest->capacity;
	dest->data = tmp_src;
	dest->size = tmp_src_size;
	dest->capacity = tmp_src_cap;
}

ALWAYS_INLINE int jstr_cmp(Jstring *RESTRICT dest, Jstring *RESTRICT src)
{
	return (dest->size != src->size) ? 1 : memcmp(dest->data, src->data, dest->size);
}

inline int jstr_replace(Jstring *RESTRICT dest, char *RESTRICT src, const size_t src_size)
{
	if (dest->capacity > src_size);
	else
		if (likely((dest->data = realloc(dest->data, src_size + 1))));
		else goto ERROR_FREE;
	memcpy(dest->data, src, src_size + 1);
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

inline int jstr_reserve(Jstring *RESTRICT dest, size_t capacity)
{
	if (dest->capacity) {
		if (likely((dest->data = realloc(dest->data, (dest->capacity = capacity) * sizeof dest->data[0]))));
		else goto ERROR_FREE;
	} else {
		if (likely((dest->data = malloc((dest->capacity = capacity) * sizeof dest->data[0]))));
		else goto ERROR;
	}
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
ERROR:
	return 0;
}

inline int jstr_shrink(Jstring *RESTRICT dest)
{
	if (likely((dest->data = realloc(dest->data, dest->size + 1)))) {
		dest->capacity = dest->size;
		return 1;
	}
	jstr_delete_fast(dest);
	return 0;
}
