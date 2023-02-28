#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"
#include "macros.h"

#define JSTR_MIN_CAP 8
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

ALWAYS_INLINE void jstr_init(jstring_t *RESTRICT dest)
{
	dest->data = NULL;
	dest->size = 0;
	dest->capacity = 0;
}

ALWAYS_INLINE void jstr_delete_fast(jstring_t *RESTRICT dest)
{
	free(dest->data);
	jstr_init(dest);
}

ALWAYS_INLINE void jstr_delete(jstring_t *RESTRICT dest)
{
	if (dest->data) {
		free(dest->data);
		jstr_init(dest);
	}
}

ALWAYS_INLINE int jstr_new(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	if (likely((dest->data = malloc((dest->capacity = MAX(JSTR_MIN_CAP, 2 * src_size)))))) {
		dest->size = src_size;
		memcpy(dest->data, src, src_size + 1);
		return 1;
	}
	jstr_init(dest);
	return 0;
}

int private_jstr_cat(jstring_t *RESTRICT dest, ...)
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
		if (unlikely(!(dest->data = realloc(dest->data, (tmp_cap = dest->capacity)))))
			goto ERROR_FREE;
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

inline int jstr_append(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	const size_t total_size = dest->size + src_size;
	if (dest->capacity < total_size) {
		size_t tmp_cap = dest->capacity;
		do {
			tmp_cap *= 2;
		} while (tmp_cap < total_size);
		if (unlikely((dest->data = realloc(dest->data, (dest->capacity = tmp_cap)))))
			goto ERROR_FREE;
	}
	memcpy(dest->data + dest->size, src, src_size + 1);
	dest->size = total_size;
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

inline int jstr_push_back(jstring_t *RESTRICT dest, const char c)
{
	if (unlikely(dest->capacity != dest->size))
		if (unlikely(!(dest->data = realloc(dest->data, (dest->capacity *= 2)))))
			goto ERROR_FREE;
	dest->data[dest->size] = c;
	dest->data[dest->size + 1] = '\0';
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

ALWAYS_INLINE void jstr_pop_back(jstring_t *RESTRICT dest)
{
	dest->data[--dest->size] = '\0';
}

ALWAYS_INLINE void jstr_swap(jstring_t *RESTRICT dest, jstring_t *RESTRICT src)
{
	char *RESTRICT tmp_src = src->data;
	const size_t src_cap = src->capacity;
	const size_t src_size = src->size;
	src->data = dest->data;
	src->capacity = dest->capacity;
	src->size = dest->size;
	dest->data = tmp_src;
	dest->capacity = src_cap;
	dest->size = src_size;
}

ALWAYS_INLINE void jstr_swap_str(jstring_t *RESTRICT dest, char **RESTRICT src, size_t *RESTRICT src_size, size_t *RESTRICT src_capacity)
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

ALWAYS_INLINE int jstr_cmp(jstring_t *RESTRICT dest, jstring_t *RESTRICT src)
{
	return (dest->size != src->size) ? 1 : memcmp(dest->data, src->data, dest->size);
}

ALWAYS_INLINE int jstr_replace(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t src_size)
{
	if ((dest->capacity < src_size) && (unlikely(!(dest->data = realloc(dest->data, src_size + 1)))))
		goto ERROR_FREE;
	memcpy(dest->data, src, src_size + 1);
	return 1;

ERROR_FREE:
	jstr_delete_fast(dest);
	return 0;
}

ALWAYS_INLINE int jstr_reserve(jstring_t *RESTRICT dest, size_t capacity)
{
	if (likely(capacity > dest->capacity)) {
		if (likely((dest->data = realloc(dest->data, (dest->capacity = capacity) * sizeof dest->data[0]))))
			return 1;
		jstr_delete_fast(dest);
	}
	return 0;
}

ALWAYS_INLINE int jstr_reserve_fast(jstring_t *RESTRICT dest, size_t capacity)
{
	if (likely((dest->data = realloc(dest->data, (dest->capacity = capacity) * sizeof *dest->data))))
		return 1;
	jstr_delete_fast(dest);
	return 0;
}

ALWAYS_INLINE int jstr_shrink(jstring_t *RESTRICT dest)
{
	if (likely((dest->data = realloc(dest->data, (dest->capacity = dest->size) + 1))))
		return 1;
	jstr_delete_fast(dest);
	return 0;
}
