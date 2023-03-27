#include "jstr.h"
#include "macros.h"

#define JSTR_MIN_CAP 8

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

ALWAYS_INLINE void jstr_init(jstring_t *RESTRICT this_jstr)
{
	this_jstr->capacity = 0;
	this_jstr->size = 0;
	this_jstr->data = NULL;
}

ALWAYS_INLINE void jstr_delete(jstring_t *RESTRICT this_jstr)
{
	free(this_jstr->data);
	jstr_init(this_jstr);
}

ALWAYS_INLINE int jstr_reserve_nocheck_exact(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_jstr->data, cap))))
		return 0;
	this_jstr->data = tmp;
	this_jstr->capacity = cap;
	return 1;
}

ALWAYS_INLINE int jstr_reserve_nocheck(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	size_t tmp_cap = this_jstr->capacity * 2;
	while (tmp_cap < cap)
		tmp_cap *= 2;
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_jstr->data, tmp_cap))))
		return 0;
	this_jstr->data = tmp;
	this_jstr->capacity = tmp_cap;
	return 1;
}

int private_jstr_cat(jstring_t *RESTRICT this_jstr, const size_t len, ...)
{
	if (unlikely(!jstr_reserve(this_jstr, this_jstr->size + len + 1)))
		return 0;
	this_jstr->size += len;
	char *RESTRICT tmp = this_jstr->data + len;
	va_list ap;
	va_start(ap, len);
	for (const char *RESTRICT argv = va_arg(ap, const char *); argv; argv = va_arg(ap, const char *))
		do {
			*tmp++ = *argv++;
		} while (*argv);
	*tmp = '\0';
	va_end(ap);
	return 1;
}

ALWAYS_INLINE void private_jstr_append_noalloc(jstring_t *RESTRICT this_jstr, const char *RESTRICT const src, const size_t src_size, ...)
{
	memcpy(this_jstr->data + this_jstr->size, src, src_size + 1);
	this_jstr->size = this_jstr->size + src_size;
}

ALWAYS_INLINE int private_jstr_append(jstring_t *RESTRICT this_jstr, const char *RESTRICT const src, const size_t src_size, ...)
{
	if (unlikely(!jstr_reserve(this_jstr, this_jstr->size + src_size)))
		return 0;
	private_jstr_append_noalloc(this_jstr, src, src_size);
	return 1;
}

ALWAYS_INLINE int private_jstr_new_append(jstring_t *RESTRICT this_jstr, const size_t src_size, const char *RESTRICT const src, ...)
{
	this_jstr->capacity = MAX(JSTR_MIN_CAP, JSTR_NEAR_POW2(2 * src_size));
	this_jstr->data = malloc(this_jstr->capacity);
	if (unlikely(!this_jstr->data)) {
		jstr_init(this_jstr);
		return 0;
	}
	this_jstr->size = src_size;
	memcpy(this_jstr->data, src, src_size + 1);
	return 1;
}

int private_jstr_new_cat(jstring_t *RESTRICT this_jstr, const size_t arglen, ...)
{
	this_jstr->capacity = MAX(JSTR_MIN_CAP, JSTR_NEAR_POW2(2 * arglen));
	this_jstr->data = malloc(this_jstr->capacity);
	if (unlikely(!this_jstr->data)) {
		jstr_init(this_jstr);
		return 0;
	}
	this_jstr->size = arglen;
	char *tmp = this_jstr->data;
	va_list ap;
	va_start(ap, arglen);
	for (const char *RESTRICT argv = va_arg(ap, const char *); argv; argv = va_arg(ap, const char *))
		do {
			*tmp++ = *argv++;
		} while (*argv);
	*tmp = '\0';
	va_end(ap);
	return 1;
}

ALWAYS_INLINE int private_jstr_new_alloc(jstring_t *RESTRICT this_jstr, const size_t size)
{
	this_jstr->size = 0;
	this_jstr->capacity = MAX(JSTR_MIN_CAP, JSTR_NEAR_POW2(2 * size));
	this_jstr->data = malloc(this_jstr->capacity);
	if (unlikely(!this_jstr->data)) {
		this_jstr->capacity = 0;
		this_jstr->data = NULL;
		return 0;
	}
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

ALWAYS_INLINE int jstr_cmp_nocheck(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return memcmp(this_jstr->data, src->data, this_jstr->size);
}

ALWAYS_INLINE int jstr_cmp(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return (this_jstr->size != src->size) ? 1 : jstr_cmp_nocheck(this_jstr, src);
}

ALWAYS_INLINE int jstr_case_cmp_nocheck(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return strcasecmp(this_jstr->data, src->data);
}

ALWAYS_INLINE int jstr_case_cmp(jstring_t *RESTRICT this_jstr, jstring_t *RESTRICT src)
{
	return (this_jstr->size != src->size) ? 1 : jstr_case_cmp_nocheck(this_jstr, src);
}

ALWAYS_INLINE void jstr_replace_noalloc(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size)
{
	memcpy(this_jstr->data, src, src_size + 1);
	this_jstr->size = src_size;
}

ALWAYS_INLINE int jstr_replace_nocheck(jstring_t *RESTRICT this_jstr, char *RESTRICT src, const size_t src_size)
{
	if (unlikely(!jstr_reserve_nocheck(this_jstr, src_size + 1)))
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
		return jstr_reserve_nocheck(this_jstr, cap + 1);
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_jstr)
{
	char *RESTRICT tmp;
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

ALWAYS_INLINE int jstr_shrink_to_nocheck(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_jstr->data, cap))))
		return 0;
	this_jstr->data = tmp;
	this_jstr->data[this_jstr->size = cap] = '\0';
	this_jstr->capacity = cap;
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to(jstring_t *RESTRICT this_jstr, const size_t cap)
{
	if (cap > this_jstr->capacity)
		return 1;
	return jstr_shrink_to_nocheck(this_jstr, cap);
}

ALWAYS_INLINE void jstr_shrink_to_size_nocheck(jstring_t *RESTRICT this_jstr, const size_t size)
{
	this_jstr->data[this_jstr->size = size] = '\0';
}

ALWAYS_INLINE void jstr_shrink_to_size(jstring_t *RESTRICT this_jstr, const size_t size)
{
	if (size < this_jstr->size)
		jstr_shrink_to_size_nocheck(this_jstr, size);
}

ALWAYS_INLINE void jstr_push_back_noalloc(jstring_t *RESTRICT this_jstr, const char c)
{
	this_jstr->data[this_jstr->size] = c;
	this_jstr->data[++this_jstr->size] = '\0';
}

ALWAYS_INLINE int jstr_push_back_nocheck(jstring_t *RESTRICT this_jstr, const char c)
{
	if (unlikely(!jstr_reserve_nocheck(this_jstr, this_jstr->size * 2)))
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
	memmove(this_jstr->data + 1, this_jstr->data, ++this_jstr->size);
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
	memmove(this_jstr->data, this_jstr->data + 1, this_jstr->size--);
}
