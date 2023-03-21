#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"
#include "macros.h"

#define JSTR_MIN_CAP 8

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

CONST ALWAYS_INLINE static size_t private_jstr_next_pow2_32(size_t x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
}

CONST ALWAYS_INLINE static size_t private_jstr_next_pow2_64(size_t x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return ++x;
}

ALWAYS_INLINE void jstr_init(jstring_t *RESTRICT this_jstr)
{
	this_jstr->data = NULL;
	this_jstr->size = 0;
	this_jstr->capacity = 0;
}

ALWAYS_INLINE void jstr_delete_nocheck(jstring_t *RESTRICT this_jstr)
{
	free(this_jstr->data);
	jstr_init(this_jstr);
}

ALWAYS_INLINE void jstr_delete(jstring_t *RESTRICT this_jstr)
{
	if (this_jstr->data)
		jstr_delete_nocheck(this_jstr);
}

static ALWAYS_INLINE int jstr_cap_grow(jstring_t *RESTRICT this_jstr, const size_t size)
{
	if (this_jstr->capacity < size) {
		size_t cap = this_jstr->capacity * 2;
		while (cap < size)
			cap *= 2;
		if (unlikely(!(jstr_reserve_nocheck(this_jstr, cap))))
			return 0;
	}
	return 1;
}

int private_jstr_cat(jstring_t *RESTRICT this_jstr, ...)
{
	size_t total_size = this_jstr->size;
	char *RESTRICT tmp = this_jstr->data + total_size;
	va_list ap;
	va_start(ap, this_jstr);
	for (char *RESTRICT argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		total_size += strlen(argv);
	va_end(ap);
	if (unlikely(!jstr_cap_grow(this_jstr, total_size)))
		return 0;
	va_start(ap, this_jstr);
	for (char *RESTRICT argv = va_arg(ap, char *); argv; argv = va_arg(ap, char *))
		do { *tmp++ = *argv++; } while (*argv);
	va_end(ap);
	*tmp = '\0';
	this_jstr->size = total_size;
	return 1;
}

void jstr_append_noalloc(jstring_t *RESTRICT this_jstr, const char *RESTRICT const src, const size_t src_size)
{
	memcpy(this_jstr->data + this_jstr->size, src, src_size + 1);
	this_jstr->size = this_jstr->size + src_size;
}

int jstr_append(jstring_t *RESTRICT this_jstr, const char *RESTRICT const src, const size_t src_size)
{
	const size_t total_size = this_jstr->size + src_size;
	if (unlikely(!jstr_cap_grow(this_jstr, total_size)))
		return 0;
	memcpy(this_jstr->data + this_jstr->size, src, src_size + 1);
	this_jstr->size = total_size;
	return 1;
}

ALWAYS_INLINE int jstr_new(jstring_t *RESTRICT this_jstr, const char *RESTRICT const src, const size_t src_size)
{
	if (unlikely(!(this_jstr->data = malloc((this_jstr->capacity = MAX(JSTR_MIN_CAP, JSTR_NEAR_POW2(2 * src_size))))))) {
		jstr_init(this_jstr);
		return 0;
	}
	this_jstr->size = src_size;
	memcpy(this_jstr->data, src, src_size + 1);
	return 1;
}

ALWAYS_INLINE int jstr_new_alloc(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	this_jstr->size = 0;
	if (unlikely(!(this_jstr->data = malloc((this_jstr->capacity = MAX(JSTR_MIN_CAP, JSTR_NEAR_POW2(2 * cap))))))) {
		this_jstr->capacity = 0;
		this_jstr->data = NULL;
		return 0;
	}
	this_jstr->capacity = cap;
	return 1;
}

ALWAYS_INLINE void jstr_swap(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	char *RESTRICT const tmp_src = src->data;
	const size_t src_cap = src->capacity;
	const size_t src_size = src->size;
	src->data = this_jstr->data;
	src->capacity = this_jstr->capacity;
	src->size = this_jstr->size;
	this_jstr->data = tmp_src;
	this_jstr->capacity = src_cap;
	this_jstr->size = src_size;
}

ALWAYS_INLINE void jstr_swap_str(jstring_t *RESTRICT this_jstr, char **RESTRICT src, size_t *RESTRICT src_size, size_t *RESTRICT src_capacity)
{
	char *RESTRICT const tmp_src = *src;
	const size_t tmp_src_size = *src_size;
	const size_t tmp_src_cap = *src_capacity;
	*src = this_jstr->data;
	*src_size = this_jstr->size;
	*src_capacity = this_jstr->capacity;
	this_jstr->data = tmp_src;
	this_jstr->size = tmp_src_size;
	this_jstr->capacity = tmp_src_cap;
}

ALWAYS_INLINE int jstr_cmp(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return (this_jstr->size != src->size) ? 1 : memcmp(this_jstr->data, src->data, this_jstr->size);
}

ALWAYS_INLINE int jstr_cmp_nocheck(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return memcmp(this_jstr->data, src->data, this_jstr->size);
}

ALWAYS_INLINE void jstr_replace_noalloc(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size)
{
	memcpy(this_jstr->data, src, src_size + 1);
	this_jstr->data[src_size] = '\0';
	this_jstr->size = src_size;
}

ALWAYS_INLINE int jstr_reserve_nocheck(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	char *tmp;
	if (unlikely(!(tmp = realloc(this_jstr->data, cap))))
		return 0;
	this_jstr->data = tmp;
	this_jstr->capacity = cap;
	return 1;
}

ALWAYS_INLINE int jstr_replace_nocheck(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size)
{
	if (unlikely(!jstr_reserve_nocheck(this_jstr, src_size)))
		return 0;
	jstr_replace_noalloc(this_jstr, src, src_size);
	return 1;
}

ALWAYS_INLINE int jstr_replace(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size)
{
	if (this_jstr->capacity < src_size)
		return jstr_replace_nocheck(this_jstr, src, src_size);
	jstr_replace_noalloc(this_jstr, src, src_size);
	return 1;
}

ALWAYS_INLINE int jstr_reserve(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	if (cap > this_jstr->capacity)
		return jstr_reserve_nocheck(this_jstr, cap);
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_jstr)
{
	char *tmp;
	if (unlikely(!(tmp = realloc(this_jstr->data, this_jstr->size + 1))))
		return 0;
	this_jstr->capacity = this_jstr->size + 1;
	this_jstr->data = tmp;
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to_fit(jstring_t *RESTRICT this_jstr)
{
	if (unlikely(this_jstr->capacity != this_jstr->size))
		return 1;
	return jstr_shrink_to_fit_nocheck(this_jstr);
}

ALWAYS_INLINE void jstr_shrink_nocheck(jstring_t *RESTRICT this_jstr, const size_t size)
{
	this_jstr->data[this_jstr->size = size] = '\0';
}

ALWAYS_INLINE void jstr_shrink(jstring_t *RESTRICT this_jstr, const size_t size)
{
	if (likely(size > this_jstr->size))
		jstr_shrink_nocheck(this_jstr, size);
}

ALWAYS_INLINE void jstr_push_back_noalloc(jstring_t *RESTRICT this_jstr, const char c)
{
	this_jstr->data[this_jstr->size] = c;
	this_jstr->data[++this_jstr->size] = '\0';
}

ALWAYS_INLINE int jstr_push_back_nocheck(jstring_t *RESTRICT this_jstr, const char c)
{
	if (unlikely(!(jstr_reserve_nocheck(this_jstr, this_jstr->size * 2))))
		return 0;
	jstr_push_back_noalloc(this_jstr, c);
	return 1;
}

ALWAYS_INLINE int jstr_push_back(jstring_t *RESTRICT this_jstr, const char c)
{
	if (unlikely(this_jstr->size == this_jstr->capacity))
		return jstr_push_back_nocheck(this_jstr, c);
	jstr_push_back_noalloc(this_jstr, c);
	return 1;
}

ALWAYS_INLINE void jstr_pop_back(jstring_t *RESTRICT this_jstr)
{
	this_jstr->data[--this_jstr->size] = '\0';
}

ALWAYS_INLINE void jstr_push_front_noalloc(jstring_t *RESTRICT this_jstr, const char c)
{
	memmove(this_jstr->data + 1, this_jstr->data, this_jstr->size++);
	*this_jstr->data = c;
}

ALWAYS_INLINE int jstr_push_front_nocheck(jstring_t *RESTRICT this_jstr, const char c)
{
	if (unlikely(!jstr_reserve_nocheck(this_jstr, this_jstr->size * 2)))
		return 0;
	jstr_push_front_noalloc(this_jstr, c);
	return 1;
}

ALWAYS_INLINE int jstr_push_front(jstring_t *RESTRICT this_jstr, const char c)
{
	if (unlikely(this_jstr->capacity == this_jstr->size))
		return jstr_push_front_nocheck(this_jstr, c);
	jstr_push_front_noalloc(this_jstr, c);
	return 1;
}

ALWAYS_INLINE void jstr_pop_front(jstring_t *RESTRICT this_jstr, const char c)
{
	memmove(this_jstr->data, this_jstr->data + 1, --this_jstr->size);
}

/* #define JSTR_DEBUG */
#ifdef JSTR_DEBUG

#include <assert.h>
#include <stdio.h>
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
	assert(jstr_shrink_to_fit(&s));
	(jstr_delete(&s), 0);
	return 1;
}

int main()
{
	assert(debug);
	return 0;
}

#endif // JSTR_DEBUG
