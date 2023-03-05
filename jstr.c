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

ALWAYS_INLINE void jstr_delete_nocheck(jstring_t *RESTRICT dest)
{
	free(dest->data);
	jstr_init(dest);
}

ALWAYS_INLINE void jstr_delete(jstring_t *RESTRICT dest)
{
	if (dest->data)
		jstr_delete_nocheck(dest);
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
	jstr_delete_nocheck(dest);
	return 0;
}

int jstr_append(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	const size_t total_size = dest->size + src_size;
	if (dest->capacity < total_size) {
		size_t tmp_cap = dest->capacity;
		do {
			tmp_cap *= 2;
		} while (tmp_cap < total_size);
		if (unlikely((dest->data = realloc(dest->data, (dest->capacity = tmp_cap)))))
			return 0;
	}
	memcpy(dest->data + dest->size, src, src_size + 1);
	dest->size = total_size;
	return 1;
}

ALWAYS_INLINE int jstr_new(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t src_size)
{
	return (likely((dest->data = malloc((dest->capacity = MAX(JSTR_MIN_CAP, JARR_NEAR_POW2(2 * src_size)))))))
		? 
			(dest->size = src_size,
			memcpy(dest->data, src, src_size + 1), 1)
		:
			(jstr_init(dest), 0);
}

ALWAYS_INLINE int jstr_new_alloc(jstring_t *RESTRICT dest, const size_t cap)
{
	return (likely((dest->data = malloc((dest->capacity = MAX(JSTR_MIN_CAP, cap))))))
		? 
			(dest->capacity = cap, 1)
		:
			(jstr_init(dest), 0);
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

ALWAYS_INLINE int jstr_cmp_nocheck(jstring_t *RESTRICT dest, jstring_t *RESTRICT src)
{
	return memcmp(dest->data, src->data, dest->size);
}

ALWAYS_INLINE void jstr_replace_nocheck(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t src_size)
{
	memcpy(dest->data, src, src_size + 1);
	dest->data[src_size] = '\0';
	dest->size = src_size;
}

ALWAYS_INLINE int jstr_reserve_nocheck(jstring_t *RESTRICT dest, size_t capacity)
{
	char *tmp;
	return (likely((tmp = realloc(dest->data, (dest->capacity = capacity) * sizeof *dest->data))))
		? (dest->data = tmp, 1)
		: 0;
}

ALWAYS_INLINE int jstr_replace(jstring_t *RESTRICT dest, char *RESTRICT src, const size_t src_size)
{
	if (dest->capacity < src_size)
		if (!(unlikely(jstr_reserve_nocheck(dest, src_size))))
			return 0;
	jstr_replace_nocheck(dest, src, src_size);
	return 1;
}

ALWAYS_INLINE int jstr_reserve(jstring_t *RESTRICT dest, size_t capacity)
{
	return (likely(capacity > dest->capacity))
		? jstr_reserve_nocheck(dest, capacity)
		: 1;
}

ALWAYS_INLINE int jstr_shrink_nocheck(jstring_t *RESTRICT dest)
{
	char *tmp;
	return (likely((tmp = realloc(dest->data, (dest->capacity = dest->size) + 1))))
		? (dest->data = tmp, 1)
		: 0;
}

ALWAYS_INLINE int jstr_shrink(jstring_t *RESTRICT dest)
{
	return (likely(dest->capacity != dest->size))
		? jstr_shrink_nocheck(dest)
		: 1;
}

ALWAYS_INLINE void jstr_push_back_noalloc(jstring_t *RESTRICT dest, const char c)
{
	dest->data[dest->size] = c;
	dest->data[++dest->size] = '\0';
}

ALWAYS_INLINE int jstr_push_back_nocheck(jstring_t *RESTRICT dest, const char c)
{
	return (jstr_reserve_nocheck(dest, dest->size + 1))
		? (jstr_push_back_noalloc(dest, c), 1)
		: 0;
}

inline int jstr_push_back(jstring_t *RESTRICT dest, const char c)
{
	return (likely(dest->size == dest->capacity))
		? (jstr_push_back_nocheck(dest, c))
		: (jstr_push_back_noalloc(dest, c), 0);
}

#ifdef JSTR_DEBUG

#include <assert.h>
static ALWAYS_INLINE int debug()
{
	char a[100];
	jstring_t s;
	jstr_init(&s);
	assert(jstr_new(&s, "aaa", 2));
	assert(jstr_append(&s, a, 10));
	assert(jstr_cat(&s, 1, 3, 4));
	assert(jstr_push_back(&s, 3));
	assert(jstr_reserve(&s, 100));
	assert(jstr_shrink(&s));
	(jstr_delete(&s), 0);
	return 1;
}

int main()
{
	assert(debug);
	return 0;
}

#endif
