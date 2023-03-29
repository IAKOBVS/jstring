#include "jstr.h"
#include "macros.h"

#define JSTR_MIN_CAP 8

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define JSTR_MAX_STACK 2048

ALWAYS_INLINE void jstr_init(jstring_t *RESTRICT this_)
{
	this_->capacity = 0;
	this_->size = 0;
	this_->data = NULL;
}

ALWAYS_INLINE void jstr_delete(jstring_t *RESTRICT this_)
{
	free(this_->data);
	jstr_init(this_);
}

ALWAYS_INLINE int jstr_reserve_nocheck_exact(jstring_t *RESTRICT this_, const size_t cap)
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_->data, cap))))
		return 0;
	this_->data = tmp;
	this_->capacity = cap;
	return 1;
}

ALWAYS_INLINE int jstr_reserve_nocheck(jstring_t *RESTRICT this_, const size_t cap, ...)
{
	size_t tmp_cap = this_->capacity * 2;
	while (tmp_cap < cap)
		tmp_cap *= 2;
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_->data, tmp_cap))))
		return 0;
	this_->data = tmp;
	this_->capacity = tmp_cap;
	return 1;
}

int private_jstr_cat(jstring_t *RESTRICT this_, const size_t len, ...)
{
	if (unlikely(!jstr_reserve(this_, this_->size + len + 1)))
		return 0;
	this_->size += len;
	char *RESTRICT tmp = this_->data + len;
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

ALWAYS_INLINE void private_jstr_append_noalloc(jstring_t *RESTRICT this_, const char *RESTRICT const other, const size_t otherlen, ...)
{
	memcpy(this_->data + this_->size, other, otherlen + 1);
	this_->size = this_->size + otherlen;
}

ALWAYS_INLINE int private_jstr_append(jstring_t *RESTRICT this_, const char *RESTRICT const other, const size_t otherlen, ...)
{
	if (unlikely(!jstr_reserve(this_, this_->size + otherlen)))
		return 0;
	private_jstr_append_noalloc(this_, other, otherlen);
	return 1;
}

ALWAYS_INLINE int jstr_new_append(jstring_t *RESTRICT this_, const size_t otherlen, const char *RESTRICT const other, ...)
{
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * otherlen));
	this_->data = malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		jstr_init(this_);
		return 0;
	}
	this_->size = otherlen;
	memcpy(this_->data, other, otherlen + 1);
	return 1;
}

int private_jstr_new_cat(jstring_t *RESTRICT this_, const size_t arglen, ...)
{
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * arglen));
	this_->data = malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		jstr_init(this_);
		return 0;
	}
	this_->size = arglen;
	char *tmp = this_->data;
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

ALWAYS_INLINE int jstr_new_alloc(jstring_t *RESTRICT this_, const size_t size)
{
	this_->size = 0;
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size));
	this_->data = malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->data = NULL;
		return 0;
	}
	return 1;
}

ALWAYS_INLINE void jstr_swap(jstring_t *RESTRICT this_, jstring_t *RESTRICT other)
{
	const jstring_t tmp = *other;
	*other = *this_;
	*this_ = tmp;
}

ALWAYS_INLINE void jstr_swap_str(jstring_t *RESTRICT this_, char **RESTRICT other, size_t *RESTRICT otherlen, size_t *RESTRICT other_capacity)
{
	char *RESTRICT const tmp_other = *other;
	const size_t tmp_otherlen = *otherlen;
	const size_t tmp_other_cap = *other_capacity;
	*other = this_->data;
	*otherlen = this_->size;
	*other_capacity = this_->capacity;
	this_->data = tmp_other;
	this_->size = tmp_otherlen;
	this_->capacity = tmp_other_cap;
}

ALWAYS_INLINE int jstr_cmp_nocheck(jstring_t *RESTRICT this_, jstring_t *RESTRICT other)
{
	return memcmp(this_->data, other->data, this_->size);
}

ALWAYS_INLINE int jstr_cmp(jstring_t *RESTRICT this_, jstring_t *RESTRICT other)
{
	return (this_->size != other->size) ? 1 : jstr_cmp_nocheck(this_, other);
}

ALWAYS_INLINE int jstr_case_cmp_nocheck(jstring_t *RESTRICT this_, jstring_t *RESTRICT other)
{
	return strcasecmp(this_->data, other->data);
}

ALWAYS_INLINE int jstr_case_cmp(jstring_t *RESTRICT this_, jstring_t *RESTRICT other)
{
	return (this_->size != other->size) ? 1 : jstr_case_cmp_nocheck(this_, other);
}

ALWAYS_INLINE void jstr_replace_noalloc(jstring_t *RESTRICT dest, char *RESTRICT other, const size_t srclen)
{
	memcpy(dest->data, other, srclen + 1);
	dest->size = srclen;
}

ALWAYS_INLINE void jstr_replace_jstr_noalloc(jstring_t *RESTRICT dest, jstring_t *RESTRICT src)
{
	memcpy(dest->data, src->data, src->size);
	dest->size = src->size;
}

ALWAYS_INLINE int jstr_replace_nocheck(jstring_t *RESTRICT dest, char *RESTRICT other, const size_t srclen)
{
	if (unlikely(!jstr_reserve_nocheck(dest, srclen + 1)))
		return 0;
	jstr_replace_noalloc(dest, other, srclen);
	return 1;
}

ALWAYS_INLINE int jstr_replace_jstr_nocheck(jstring_t *RESTRICT dest, jstring_t *RESTRICT src)
{
	if (unlikely(!jstr_reserve_nocheck(dest, src->size + 1)))
		return 0;
	jstr_replace_jstr_noalloc(dest, src);
	return 1;
}

ALWAYS_INLINE int private_jstr_replace(jstring_t *RESTRICT dest, char *RESTRICT other, const size_t srclen, ...)
{
	if (dest->capacity < srclen)
		return jstr_replace_nocheck(dest, other, srclen);
	jstr_replace_noalloc(dest, other, srclen);
	return 1;
}

ALWAYS_INLINE int jstr_replace_jstr(jstring_t *RESTRICT dest, jstring_t *RESTRICT src, ...)
{
	if (dest->capacity < src->capacity)
		return jstr_replace_nocheck(dest, src->data, src->capacity);
	jstr_replace_jstr_noalloc(dest, src);
	return 1;
}

ALWAYS_INLINE int jstr_reserve(jstring_t *RESTRICT this_, const size_t cap)
{
	if (cap > this_->capacity)
		return jstr_reserve_nocheck(this_, cap + 1);
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_)
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_->data, this_->size + 1))))
		return 0;
	this_->capacity = this_->size + 1;
	this_->data = tmp;
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to_fit(jstring_t *RESTRICT this_)
{
	if (unlikely(this_->capacity != this_->size))
		return 1;
	return jstr_shrink_to_fit_nocheck(this_);
}

ALWAYS_INLINE int jstr_shrink_to_nocheck(jstring_t *RESTRICT this_, const size_t cap)
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = realloc(this_->data, cap))))
		return 0;
	this_->data = tmp;
	this_->data[this_->size = cap] = '\0';
	this_->capacity = cap;
	return 1;
}

ALWAYS_INLINE int jstr_shrink_to(jstring_t *RESTRICT this_, const size_t cap)
{
	if (cap > this_->capacity)
		return 1;
	return jstr_shrink_to_nocheck(this_, cap);
}

ALWAYS_INLINE void jstr_shrink_to_size_nocheck(jstring_t *RESTRICT this_, const size_t size)
{
	this_->data[this_->size = size] = '\0';
}

ALWAYS_INLINE void jstr_shrink_to_size(jstring_t *RESTRICT this_, const size_t size)
{
	if (size < this_->size)
		jstr_shrink_to_size_nocheck(this_, size);
}

ALWAYS_INLINE void jstr_push_back_noalloc(jstring_t *RESTRICT this_, const char c)
{
	this_->data[this_->size] = c;
	this_->data[++this_->size] = '\0';
}

ALWAYS_INLINE int jstr_push_back_nocheck(jstring_t *RESTRICT this_, const char c)
{
	if (unlikely(!jstr_reserve_nocheck(this_, this_->size * 2)))
		return 0;
	jstr_push_back_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE int jstr_push_back(jstring_t *RESTRICT this_, const char c)
{
	if (unlikely(this_->size == this_->capacity))
		return jstr_push_back_nocheck(this_, c);
	jstr_push_back_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE void jstr_pop_back(jstring_t *RESTRICT this_)
{
	this_->data[--this_->size] = '\0';
}

ALWAYS_INLINE void jstr_push_front_noalloc(jstring_t *RESTRICT this_, const char c)
{
	memmove(this_->data + 1, this_->data, ++this_->size);
	*this_->data = c;
}

ALWAYS_INLINE int jstr_push_front_nocheck(jstring_t *RESTRICT this_, const char c)
{
	if (unlikely(!jstr_reserve_nocheck(this_, this_->size * 2)))
		return 0;
	jstr_push_front_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE int jstr_push_front(jstring_t *RESTRICT this_, const char c)
{
	if (unlikely(this_->capacity == this_->size))
		return jstr_push_front_nocheck(this_, c);
	jstr_push_front_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE void jstr_pop_front(jstring_t *RESTRICT this_)
{
	memmove(this_->data, this_->data + 1, this_->size--);
}

ALWAYS_INLINE CONST char *jstr_rchr(jstring_t *RESTRICT this_, int c)
{
	const char *const begin = this_->data;
	char *RESTRICT end = this_->data + this_->size - 1;
	for ( ; end != begin; --end)
		if (*end == c)
			return end;
	return NULL;
}

ALWAYS_INLINE void jstr_rev(jstring_t *RESTRICT this_)
{
	char *RESTRICT begin = this_->data;
	char *RESTRICT end = this_->data + this_->size - 1;
	for (char tmp_begin; begin < end; ++begin, --end) {
		tmp_begin = *begin;
		*begin = *end;
		*end = tmp_begin;
	}
}

ALWAYS_INLINE void jstr_rev_noalloc(jstring_t *RESTRICT this_, char *buf)
{
	memcpy(buf, this_->data, this_->size + 1);
	char *RESTRICT end = this_->data + this_->size - 1;
	while ((*end-- = *buf++));
}

ALWAYS_INLINE int jstr_rev_dup(jstring_t *RESTRICT this_)
{
	if (this_->size < JSTR_MAX_STACK) {
		char s[this_->size + 1];
		jstr_rev_noalloc(this_, s);
	} else {
		char *s = malloc(this_->size + 1);
		if (unlikely(!s))
			return 0;
		jstr_rev_noalloc(this_, s);
		free(s);
	}
	return 1;
}

ALWAYS_INLINE char *jstr_chr(jstring_t *RESTRICT this_, int c)
{
	return memchr(this_->data, c, this_->size);
}

ALWAYS_INLINE int jstr_dup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other_)
{
	if (unlikely(!this_->capacity))
		return 0;
	other_->data = malloc(this_->capacity);
	if (unlikely(!other_))
		return 0;
	memcpy(other_->data, this_->data, this_->size + 1);
	other_->capacity = this_->capacity;
	other_->size = this_->size;
	return 1;
}
