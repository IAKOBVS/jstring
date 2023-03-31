#include "jstr.h"

#include "macros.h"

#ifndef __cplusplus
#	include <stdlib.h>
#	include <string.h>
#	include <stdarg.h>
#else
#	include <cstdlib>
#	include <cstring>
#	include <cstdarg>
#endif

#define JSTR_MAX_STACK 2048

#ifdef __cplusplus
#	define JSTR_CAST(T) (T)
#else
#	define JSTR_CAST(T)
#endif // __cplusplus

#define ALWAYS_INLINE_

#ifdef __cplusplus

ALWAYS_INLINE void private_jstr_constructor_cap(jstring_t *RESTRICT this_, const std::size_t cap, const char *RESTRICT s, const std::size_t slen) JSTR_NOEXCEPT__
{
	this_->capacity = MAX(JSTR_NEXT_POW2(cap), JSTR_MIN_CAP);
	this_->data = (char *)std::malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->size = 0;
		return;
	}
	std::memcpy(this_->data, s, slen + 1);
	*(this_->data + slen) = '\0';
	this_->size = slen;
}

ALWAYS_INLINE_ void private_jstr_new_append_void(jstring_t *RESTRICT dest, const size_t srclen, const char *RESTRICT const src, ...) JSTR_NOEXCEPT__
{
	dest->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * srclen));
	dest->data = JSTR_CAST(char *)malloc(dest->capacity);
	if (unlikely(!dest->data)) {
		jstr_init(dest);
		return;
	}
	dest->size = srclen;
	memcpy(dest->data, src, srclen + 1);
}

ALWAYS_INLINE_ void private_jstr_new_alloc_void(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__
{
	this_->size = 0;
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size));
	this_->data = JSTR_CAST(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->data = NULL;
		return;
	}
}

#endif // __cplusplus

ALWAYS_INLINE_ void jstr_init(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	this_->capacity = 0;
	this_->size = 0;
	this_->data = NULL;
}

ALWAYS_INLINE_ void jstr_delete(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	free(this_->data);
	jstr_init(this_);
}

ALWAYS_INLINE_ int jstr_reserve_nocheck_exact(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = JSTR_CAST(char *)realloc(this_->data, cap))))
		return 0;
	this_->data = tmp;
	this_->capacity = cap;
	return 1;
}

ALWAYS_INLINE_ int jstr_reserve_nocheck(jstring_t *RESTRICT this_, const size_t cap, ...) JSTR_NOEXCEPT__
{
	size_t tmp_cap = this_->capacity * JSTR_MULTIPLIER;
	while (tmp_cap < cap)
		tmp_cap *= 2;
	char *RESTRICT tmp;
	if (unlikely(!(tmp = JSTR_CAST(char *)realloc(this_->data, tmp_cap))))
		return 0;
	this_->data = tmp;
	this_->capacity = tmp_cap;
	return 1;
}

ALWAYS_INLINE_ int jstr_reserve(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__
{
	if (cap > this_->capacity)
		return jstr_reserve_nocheck(this_, cap);
	return 1;
}

int private_jstr_cat(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve(this_, this_->size + len + 1)))
		return 0;
	char *RESTRICT tmp = this_->data + len;
	this_->size += len;
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

int private_jstr_cat_s(jstring_t *RESTRICT this_, const size_t len, ...) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->capacity))
		return 0;
	if (unlikely(!jstr_reserve(this_, this_->size + len + 1)))
		return 0;
	char *RESTRICT tmp = this_->data + len;
	this_->size += len;
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

ALWAYS_INLINE_ void private_jstr_append_noalloc(jstring_t *RESTRICT dest, const char *RESTRICT const src, const size_t srclen) JSTR_NOEXCEPT__
{
	memcpy(dest->data + dest->size, src, srclen + 1);
	dest->size = dest->size + srclen;
}

ALWAYS_INLINE_ int private_jstr_append(jstring_t *RESTRICT dest, const char *RESTRICT const src, const size_t srclen) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve(dest, dest->size + srclen)))
		return 0;
	private_jstr_append_noalloc(dest, src, srclen);
	return 1;
}

ALWAYS_INLINE_ int private_jstr_append_s(jstring_t *RESTRICT dest, const char *RESTRICT const src, const size_t srclen) JSTR_NOEXCEPT__
{
	if (unlikely(!dest->capacity))
		return 0;
	return private_jstr_append(dest, src, srclen);
}

ALWAYS_INLINE_ int jstr_new_append(jstring_t *RESTRICT dest, const size_t srclen, const char *RESTRICT const src, ...) JSTR_NOEXCEPT__
{
	dest->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * srclen));
	dest->data = JSTR_CAST(char *)malloc(dest->capacity);
	if (unlikely(!dest->data)) {
		jstr_init(dest);
		return 0;
	}
	dest->size = srclen;
	memcpy(dest->data, src, srclen + 1);
	return 1;
}

int private_jstr_new_cat(jstring_t *RESTRICT this_, const size_t arglen, ...) JSTR_NOEXCEPT__
{
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * arglen));
	this_->data = JSTR_CAST(char *)malloc(this_->capacity);
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

ALWAYS_INLINE_ int jstr_new_alloc(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__
{
	this_->size = 0;
	this_->capacity = MAX(JSTR_MIN_CAP, JSTR_NEXT_POW2(2 * size));
	this_->data = JSTR_CAST(char *)malloc(this_->capacity);
	if (unlikely(!this_->data)) {
		this_->capacity = 0;
		this_->data = NULL;
		return 0;
	}
	return 1;
}

ALWAYS_INLINE_ void jstr_swap(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_NOEXCEPT__
{
	const jstring_t tmp = *other;
	*other = *this_;
	*this_ = tmp;
}

ALWAYS_INLINE_ void jstr_swap_str(jstring_t *RESTRICT this_, char **RESTRICT other, size_t *RESTRICT otherlen, size_t *RESTRICT othercapacity) JSTR_NOEXCEPT__
{
	char *RESTRICT const tmp_other = *other;
	const size_t tmp_otherlen = *otherlen;
	const size_t tmp_othercap = *othercapacity;
	*other = this_->data;
	*otherlen = this_->size;
	*othercapacity = this_->capacity;
	this_->data = tmp_other;
	this_->size = tmp_otherlen;
	this_->capacity = tmp_othercap;
}

ALWAYS_INLINE_ int jstr_cmp_nocheck(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
{
	return memcmp(this_->data, other->data, this_->size);
}

ALWAYS_INLINE_ int jstr_cmp(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
{
	if (this_->size != other->size)
		return 1;
	return jstr_cmp_nocheck(this_, other);
}

ALWAYS_INLINE_ int jstr_cmp_str(const jstring_t *RESTRICT const this_, const char *RESTRICT const s, const size_t slen) JSTR_NOEXCEPT__
{
	if (this_->size != slen)
		return 1;
	return memcmp(this_, s, slen);
}

#ifdef __USE_GNU

ALWAYS_INLINE_ int jstr_case_cmp_nocheck(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
{
	return strcasecmp(this_->data, other->data);
}

ALWAYS_INLINE_ int jstr_case_cmp_str(const jstring_t *RESTRICT const this_, const char *RESTRICT const s) JSTR_NOEXCEPT__
{
	return strcasecmp(this_->data, s);
}

ALWAYS_INLINE_ int jstr_case_cmp(const jstring_t *RESTRICT const this_, const jstring_t *RESTRICT const other) JSTR_NOEXCEPT__
{
	if (this_->size != other->size)
		return 1;
	return jstr_case_cmp_nocheck(this_, other);
}

#endif

ALWAYS_INLINE_ void jstr_replace_noalloc(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__
{
	memcpy(dest->data, src, srclen + 1);
	dest->size = srclen;
}

ALWAYS_INLINE_ void jstr_replace_jstr_noalloc(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src) JSTR_NOEXCEPT__
{
	memcpy(dest->data, src->data, src->size + 1);
	dest->size = src->size;
}

ALWAYS_INLINE_ int jstr_replace_nocheck(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve_nocheck(dest, srclen)))
		return 0;
	jstr_replace_noalloc(dest, src, srclen);
	return 1;
}

ALWAYS_INLINE_ int jstr_replace_jstr_nocheck(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve_nocheck(dest, src->size)))
		return 0;
	jstr_replace_jstr_noalloc(dest, src);
	return 1;
}


ALWAYS_INLINE_ int private_jstr_replace(jstring_t *RESTRICT dest, const char *RESTRICT src, const size_t srclen, ...) JSTR_NOEXCEPT__
{
	if (dest->capacity < srclen)
		return jstr_replace_nocheck(dest, src, srclen);
	jstr_replace_noalloc(dest, src, srclen);
	return 1;
}

ALWAYS_INLINE_ int jstr_replace_jstr(jstring_t *RESTRICT dest, const jstring_t *RESTRICT const src, ...) JSTR_NOEXCEPT__
{
	if (dest->capacity < src->capacity)
		return jstr_replace_nocheck(dest, src->data, src->capacity);
	jstr_replace_jstr_noalloc(dest, src);
	return 1;
}

ALWAYS_INLINE_ int jstr_reserve_s(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->capacity))
		return 0;
	return jstr_reserve(this_, cap);
}

ALWAYS_INLINE_ int jstr_shrink_to_fit_nocheck(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = JSTR_CAST(char *)realloc(this_->data, this_->size + 1))))
		return 0;
	this_->capacity = this_->size + 1;
	this_->data = tmp;
	return 1;
}

ALWAYS_INLINE_ int jstr_shrink_to_fit(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	if (unlikely(this_->capacity == this_->size))
		return 1;
	return jstr_shrink_to_fit_nocheck(this_);
}

ALWAYS_INLINE_ int jstr_shrink_to_nocheck(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__
{
	char *RESTRICT tmp;
	if (unlikely(!(tmp = JSTR_CAST(char *)realloc(this_->data, cap))))
		return 0;
	this_->data = tmp;
	this_->data[this_->size = cap] = '\0';
	this_->capacity = cap;
	return 1;
}

ALWAYS_INLINE_ int jstr_shrink_to(jstring_t *RESTRICT this_, const size_t cap) JSTR_NOEXCEPT__
{
	if (cap > this_->capacity)
		return 1;
	return jstr_shrink_to_nocheck(this_, cap);
}

ALWAYS_INLINE_ void jstr_shrink_to_size_nocheck(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__
{
	this_->data[this_->size = size] = '\0';
}

ALWAYS_INLINE_ void jstr_shrink_to_size(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__
{
	if (size < this_->size)
		jstr_shrink_to_size_nocheck(this_, size);
}

ALWAYS_INLINE_ void jstr_shrink_to_size_s(jstring_t *RESTRICT this_, const size_t size) JSTR_NOEXCEPT__
{
	if (!!this_->size & (size < this_->size))
		jstr_shrink_to_size_nocheck(this_, size);
}

ALWAYS_INLINE_ void jstr_push_back_noalloc(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	this_->data[this_->size] = c;
	this_->data[++this_->size] = '\0';
}

ALWAYS_INLINE_ int jstr_push_back_nocheck(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve_nocheck(this_, this_->size * JSTR_MULTIPLIER)))
		return 0;
	jstr_push_back_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE_ int jstr_push_back(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(this_->size == this_->capacity))
		return jstr_push_back_nocheck(this_, c);
	jstr_push_back_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE_ int jstr_push_back_s(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->capacity))
		return 0;
	return jstr_push_back(this_, c);
}

ALWAYS_INLINE_ void jstr_pop_back(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	this_->data[--this_->size] = '\0';
}

ALWAYS_INLINE_ void jstr_pop_back_s(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->size))
		return;
	jstr_pop_back(this_);
}

ALWAYS_INLINE_ void jstr_push_front_noalloc(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	memmove(this_->data + 1, this_->data, ++this_->size);
	*this_->data = c;
}

ALWAYS_INLINE_ int jstr_push_front_nocheck(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(!jstr_reserve_nocheck(this_, this_->size * JSTR_MULTIPLIER)))
		return 0;
	jstr_push_front_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE_ int jstr_push_front(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(this_->capacity == this_->size))
		return jstr_push_front_nocheck(this_, c);
	jstr_push_front_noalloc(this_, c);
	return 1;
}

ALWAYS_INLINE_ int jstr_push_front_s(jstring_t *RESTRICT this_, const char c) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->capacity))
		return 0;
	return jstr_push_front(this_, c);
}

ALWAYS_INLINE_ void jstr_pop_front(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	memmove(this_->data, this_->data + 1, this_->size--);
}

ALWAYS_INLINE_ void jstr_pop_front_s(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->size))
		return;
	memmove(this_->data, this_->data + 1, this_->size--);
}

#ifdef __USE_GNU

char *private_jstr_str(jstring_t *haystack, const char *RESTRICT const needle, size_t needlelen, ...) JSTR_NOEXCEPT__
{
	return JSTR_CAST(char *)memmem(haystack->data, haystack->size, needle, needlelen);
}

#else

char *jstr_str(jstring_t *haystack, const char *RESTRICT needle) JSTR_NOEXCEPT__
{
	return strstr(haystack->data, needle);
}

#endif

#ifdef __USE_GNU

ALWAYS_INLINE_ char *jstr_rchr(const jstring_t *RESTRICT const this_, int c) JSTR_NOEXCEPT__
{
	return JSTR_CAST(char *)memrchr(this_->data, c, this_->size);
}

#else

ALWAYS_INLINE_ char *jstr_rchr(const jstring_t *RESTRICT const this_, const int c) JSTR_NOEXCEPT__
{
	const char *RESTRICT const begin = this_->data;
	char *RESTRICT end = this_->data + this_->size - 1;
	for ( ; end != begin; --end)
		if (*end == c)
			return end;
	return NULL;
}

#endif // __USE_GNU

ALWAYS_INLINE_ void jstr_rev(jstring_t *RESTRICT this_) JSTR_NOEXCEPT__
{
	char *RESTRICT begin = this_->data;
	char *RESTRICT end = this_->data + this_->size - 1;
	for (char tmp_begin; begin < end; ++begin, --end) {
		tmp_begin = *begin;
		*begin = *end;
		*end = tmp_begin;
	}
}

ALWAYS_INLINE_ int jstr_rev_dup(jstring_t *RESTRICT src, char **RESTRICT dest) JSTR_NOEXCEPT__
{
	*dest = JSTR_CAST(char *)malloc(src->size + 1);
	if (unlikely(!*dest))
		return 0;
	char *RESTRICT const begin = src->data - 1;
	char *RESTRICT end = src->data + src->size - 1;
	while (begin < end)
		**dest++ = *end--;
	**dest = '\0';
	return 1;
}

ALWAYS_INLINE_ char *jstr_chr(const jstring_t *RESTRICT const this_, int c) JSTR_NOEXCEPT__
{
	return JSTR_CAST(char *)memchr(this_->data, c, this_->size);
}

ALWAYS_INLINE_ int jstr_dup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_NOEXCEPT__
{
	other->data = JSTR_CAST(char *)malloc(this_->capacity);
	if (unlikely(!other))
		return 0;
	memcpy(other->data, this_->data, this_->size + 1);
	other->capacity = this_->capacity;
	other->size = this_->size;
	return 1;
}

ALWAYS_INLINE_ int jstr_dup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other) JSTR_NOEXCEPT__
{
	if (unlikely(!this_->size))
		return 0;
	return jstr_dup(this_, other);
}

ALWAYS_INLINE_ int jstr_ndup(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_NOEXCEPT__
{
	if (n > this_->size)
		return 0;
	other->data = JSTR_CAST(char *)malloc(n + 1);
	if (unlikely(!other))
		return 0;
	memcpy(other->data, this_->data, n + 1);
	*(other->data + n) = '\0';
	other->capacity = n + 1;
	other->size = n;
	return 1;
}

ALWAYS_INLINE_ int jstr_ndup_s(jstring_t *RESTRICT this_, jstring_t *RESTRICT other, const size_t n) JSTR_NOEXCEPT__
{
	if (!this_->size
	| (n > this_->size))
		return 0;
	other->data = JSTR_CAST(char *)malloc(n + 1);
	if (unlikely(!other))
		return 0;
	memcpy(other->data, this_->data, n + 1);
	*(other->data + n) = '\0';
	other->capacity = n + 1;
	other->size = n;
	return 1;
}

#undef JSTR_CAST
