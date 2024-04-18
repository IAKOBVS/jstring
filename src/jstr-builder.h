/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"
#include "jstr-struct.h"

JSTR__BEGIN_DECLS
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
JSTR__END_DECLS

#include "jstr-config.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-stdstring.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define JSTR_INIT       \
	{               \
		0, 0, 0 \
	}

#define jstr_foreach(j, p) \
	for (char *p = ((j)->data), *const _jstr__foreach_end_##p = ((j)->data) + ((j)->size); p < _jstr__foreach_end_##p; ++p)
#define jstr_foreachi(j, i) \
	for (size_t i = 0, const _jstr__foreachi_end_##i = ((j)->size); i < _jstr__foreachi_end_##i; ++i)
#define jstr_index(j, curr) JSTR_DIFF(curr, (j)->data)
#define jstr__at(j, i)      ((j)->data + (i))
#define jstr_start(j)       ((j)->data)
#define jstr_end(j)         ((j)->data + (j)->size)
#ifdef JSTR_DEBUG
#	define jstr_at(j, i)                                                   \
		((i <= (j)->size) ? ((j)->data + (i)) : assert(i <= (j)->size), \
		 ((j)->data))
#else
#	define jstr_at(j, i) ((j)->data + (i))
#endif

JSTR__BEGIN_DECLS

/* Set first char to NUL and size to zero. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_empty(char *R s, size_t *R sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(s != NULL))
		*s = '\0';
	*sz = 0;
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static size_t
jstr__grow(size_t cap, size_t new_cap)
JSTR_NOEXCEPT
{
	while ((cap = (size_t)(cap * JSTR_GROWTH)) < new_cap) {}
	return JSTR_ALIGN_UP(cap, JSTR_MALLOC_ALIGNMENT);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_debug(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	int ret;
	ret = fprintf(stderr, "size       :%zu.\n"
	                  "capacity     :%zu.\n",
	          j->size,
	          j->capacity);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	const char *data;
	size_t size;
	if (jstr_likely(j->data != NULL)) {
		data = j->data;
		size = strlen(data);
	} else {
		data = "(null)";
		size = 0;
	}
	ret = fprintf(stderr, "strlen()   :%zu.\n", size);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	ret = fprintf(stderr, "data puts():%s.\n", data);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	ret = fputs("data       :", stderr);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	jstr_io_fwrite(j->data, 1, j->size, stderr);
	if (jstr_unlikely(ferror(stderr)))
		goto err;
	ret = fputs(".\n", stderr);
	if (jstr_unlikely(ret < 0))
		goto err;
	return JSTR_RET_SUCC;
err_set_errno:
	errno = ret;
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* free(p) and set p to NULL. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_free(char *R *R s, size_t *R sz, size_t *R cap)
JSTR_NOEXCEPT
{
	free(*s);
	*s = NULL;
	*cap = 0;
	*sz = 0;
}

/* free(p) and set p to NULL. */
JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
static void
jstr_free_noinline(char *R *R s, size_t *R sz, size_t *R cap)
JSTR_NOEXCEPT
{
	jstr_free(s, sz, cap);
}

/* free(p) and set p to NULL. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_free_j(jstr_ty *R j)
JSTR_NOEXCEPT
{
	jstr_free(&j->data, &j->size, &j->capacity);
}

/* Return JSTR_RET_ERR on malloc error.
 * NEW_CAP must include NUL. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reservealways(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap)
JSTR_NOEXCEPT
{
	*cap = jstr__grow(JSTR_MAX(*cap, JSTR_MIN_CAP), new_cap);
	char *tmp = (char *)realloc(*s, *cap);
	if (jstr_nullchk(tmp))
		goto err;
	*s = tmp;
	return JSTR_RET_SUCC;
err:
	jstr_free_noinline(s, sz, cap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Do nothing if new_cap < cap.
 * Return JSTR_RET_ERR on malloc error.
 * NEW_CAP must include NUL. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserve(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap <= *cap)
		return JSTR_RET_SUCC;
	return jstr_reservealways(s, sz, cap, new_cap);
}

/* Return JSTR_RET_ERR on malloc error.
 * NEW_CAP must include NUL. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserveexactalways(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap)
JSTR_NOEXCEPT
{
	*cap = new_cap;
	char *tmp = (char *)realloc(*s, *cap);
	if (jstr_nullchk(tmp))
		goto err;
	*s = tmp;
	return JSTR_RET_SUCC;
err:
	jstr_free_noinline(s, sz, cap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Do nothing if new_cap < cap.
 * Return JSTR_RET_ERR on malloc error.
 * NEW_CAP must include NUL. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserveexact(char *R *R s, size_t *R sz, size_t *R cap, size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return JSTR_RET_SUCC;
	return jstr_reserveexactalways(s, sz, cap, new_cap);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_shrink_to_fit(char *R *R s, size_t *R sz, size_t *R cap)
{
	return (jstr_chk(jstr_reserveexactalways(s, sz, cap, *sz + 1))) ? JSTR_RET_ERR : JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_print(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	return jstr_io_fwrite(j->data, 1, j->size, stdout) != j->size ? JSTR_RET_SUCC : JSTR_RET_ERR;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_io_println(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(jstr_io_fwrite(j->data, 1, j->size, stdout) != j->size))
		return JSTR_RET_ERR;
	if (jstr_unlikely(jstr_io_putchar('\n') == EOF))
		return JSTR_RET_ERR;
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr__cat(char *R *R s, size_t *R sz, size_t *R cap, va_list ap, size_t arg_len)
JSTR_NOEXCEPT
{
	char *p;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + arg_len + 1)))
		return JSTR_RET_ERR;
	p = *s + *sz;
	*sz += arg_len;
	const char *R arg;
	for (; (arg = va_arg(ap, const char *)); p = jstr_stpcpy(p, arg)) {}
	return JSTR_RET_SUCC;
}

/* Last arg must be NULL.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_ATTR_SENTINEL
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_NONNULL((3))
static jstr_ret_ty
jstr_cat(char *R *R s, size_t *R sz, size_t *R cap, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, cap);
	size_t arg_len = 0;
	const char *R arg;
	for (; (arg = va_arg(ap, const char *)); arg_len += strlen(arg)) {}
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return JSTR_RET_SUCC;
	va_start(ap, cap);
	const jstr_ret_ty ret = jstr__cat(s, sz, cap, ap, arg_len);
	va_end(ap);
	return ret;
}

/* Last arg must be NULL.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_ATTR_SENTINEL
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
static jstr_ret_ty
jstr_cat_j(jstr_ty *R j, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, j);
	size_t arg_len = 0;
	const char *R arg;
	for (; (arg = va_arg(ap, const char *)); arg_len += strlen(arg)) {}
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return JSTR_RET_SUCC;
	va_start(ap, j);
	const jstr_ret_ty ret = jstr__cat(&j->data, &j->size, &j->capacity, ap, arg_len);
	va_end(ap);
	return ret;
}

/* Return ptr to '\0' in S. */
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_append_len_unsafe_p(char *R s, size_t sz, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s + sz, src, src_len);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_append_unsafe(char *R s, size_t sz, const char *R src)
JSTR_NOEXCEPT
{
	strcpy(s + sz, src);
}

/* Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_append_unsafe_p(char *R s, size_t sz, const char *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy(s + sz, src);
}

/* Append SRC to DST.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_append_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_append_len_unsafe_p(*s, *sz, src, src_len), *s);
	return JSTR_RET_SUCC;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strset(char *R s, int c)
JSTR_NOEXCEPT
{
	memset(s, c, strlen(s));
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_assignnchr_unsafe_p(char *R s, size_t sz, int c, size_t n)
JSTR_NOEXCEPT
{
	memset(s, c, n);
	if (n > sz) {
		*(s + n) = '\0';
		return s + n;
	}
	return s + sz;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_assignnchr(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n)
JSTR_NOEXCEPT
{
	if (n > *sz) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, n + 1)))
			return JSTR_RET_ERR;
		*(*s + n) = '\0';
		*sz = n;
	}
	memset(*s, c, n);
	return JSTR_RET_SUCC;
}

/* Append N Cs to end of S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_pushbackn_len_unsafe_p(char *R s, size_t sz, int c, size_t n)
JSTR_NOEXCEPT
{
	return (char *)memset(s + sz, c, n) + n;
}

/* Append N Cs to end of S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushbackn(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + n + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_pushbackn_len_unsafe_p(*s, *sz, c, n), *s);
	return JSTR_RET_SUCC;
}

/* Prepend N Cs to S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_pushfrontn_len_unsafe_p(char *R s, size_t sz, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0))
		jstr_strmove_len(s + n, s, sz);
	else
		*(s + n) = '\0';
	return (char *)memset(s, c, n) + n;
}

/* Prepend N Cs to S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushfrontn(char *R *R s, size_t *R sz, size_t *R cap, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + n + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_pushfrontn_len_unsafe_p(*s, *sz, c, n), *s);
	return JSTR_RET_SUCC;
}

/* Only prepend NUL if S is empty.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_prepend_len_unsafe_p(char *R s, size_t sz, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s != 0))
		jstr_strmove_len(s + src_len, s, sz);
	else
		*(s + src_len) = '\0';
	memcpy(s, src, src_len);
	return s + sz + src_len;
}

/* Only prepend NUL if S is empty.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_prepend_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_prepend_len_unsafe_p(*s, *sz, src, src_len), *s);
	return JSTR_RET_SUCC;
}

/* Assign SRC to DST.
 * S is NUL terminated.
 * Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_assign_len_unsafe_p(char *R s, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s, src, src_len);
}

/* Assign SRC to DST.
 * S is NUL terminated.
 * Return value:
 * JSTR_RET_ERR on malloc error
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_assign_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		return JSTR_RET_ERR;
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return JSTR_RET_SUCC;
}

JSTR_ATTR_INLINE
JSTR_FUNC
static char *
jstr_pushback_unsafe_p(char *R s, size_t sz, char c)
JSTR_NOEXCEPT
{
	s += sz;
	*s = c;
	*++s = '\0';
	return s;
}

/* Push C to end of S.
 * S is NUL terminated.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_ATTR_INLINE
JSTR_FUNC
static jstr_ret_ty
jstr_pushback(char *R *R s, size_t *R sz, size_t *R cap, char c)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + 1 + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_pushback_unsafe_p(*s, *sz, c), *s);
	return JSTR_RET_SUCC;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_pushfront_unsafe_p(char *R s, size_t sz, char c)
JSTR_NOEXCEPT
{
	jstr_strmove_len(s + 1, s, sz);
	*s = c;
	return s + sz + 1;
}

/* Push C to end of S.
 * S is NUL terminated.
 * Return value:
 * JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushfront(char *R *R s, size_t *R sz, size_t *R cap, char c)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + 1 + 1)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_pushfront_unsafe_p(*s, *sz, c), *s);
	return JSTR_RET_SUCC;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_popback_p(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0)) {
		*(s + sz - 1) = '\0';
		return s + sz - 1;
	}
	return s;
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_popfront_p(char *R s, size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0)) {
		memmove(s, s + 1, sz);
		return s + sz - 1;
	}
	return s;
}

#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) \
|| (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 200112L)
#	define JSTR_HAVE_SNPRINTF_STRLEN 1
#else
#	define JSTR_HAVE_SNPRINTF_STRLEN 0
#endif

/* Return maximum size of allocation needed for sprintf.
 * Return value:
 * size of allocation.
 * -1 on error and errno is set. */
JSTR_FUNC
#if JSTR_HAVE_SNPRINTF_STRLEN && !JSTR_TEST
JSTR_ATTR_INLINE
#endif
static int
jstr_vsprintfstrlenmax(va_list ap, const char *R fmt)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_SNPRINTF_STRLEN && !JSTR_TEST
	const int ret = vsnprintf(NULL, 0, fmt, ap);
	if (jstr_likely(ret > 0))
		return ret + 1;
	if (ret == 0)
		return 0;
	errno = ret;
	return -1;
#else
#	define JSTR__COUNTDIGITS(lflag, base)               \
		if (lflag == L_INT)                          \
			arg_len += INT / base;               \
		else {                                       \
			if (lflag == L_LONG)                 \
				arg_len += LONG / base;      \
			else if (lflag == L_LONG_LONG)       \
				arg_len += LONG_LONG / base; \
			else                                 \
				goto einval;                 \
			lflag = L_INT;                       \
		}
#	define JSTR__GETMAXDIGITS(length)                                         \
		do {                                                               \
			if (base == B_DEC)                                         \
				arg_len                                            \
				+= (is_thousep) ? DEC_##length : 2 * DEC_##length; \
			else if (base == B_HEX)                                    \
				arg_len                                            \
				+= (is_thousep) ? HEX_##length : 2 * DEC_##length; \
			else                                                       \
				arg_len                                            \
				+= (is_thousep) ? OCT_##length : 2 * DEC_##length; \
		} while (0)
	typedef enum {
		B_DEC = 10,
		B_OCT = 8,
		B_HEX = 16,
	} base_ty; /* base */
	enum {
		L_INT = 0,
		L_LONG,
		L_LONG_LONG,
	}; /* lflag */
	enum {
		WINT = CHAR_BIT * sizeof(wint_t),
		INT = CHAR_BIT * sizeof(int),
		LONG = CHAR_BIT * sizeof(long),
		LONG_LONG = CHAR_BIT * sizeof(long long),
		SIZE = CHAR_BIT * sizeof(size_t),
		UINTPTR = CHAR_BIT * sizeof(uintptr_t),
		FLOAT = 1 + CHAR_BIT * sizeof(float),
		DOUBLE = 1 + CHAR_BIT * sizeof(double),
		DOUBLE_LONG = 1 + CHAR_BIT * sizeof(long double),
		DEC_WINT = 1 + WINT / 10,
		DEC_INT = 1 + INT / 10,
		DEC_LONG = 1 + LONG / 10,
		DEC_LONG_LONG = 1 + LONG_LONG / 10,
		DEC_SIZE = 1 + SIZE / 10,
		DEC_UINTPTR = 1 + UINTPTR / 10,
		OCT_WINT = 1 + WINT / 8,
		OCT_INT = 1 + INT / 8,
		OCT_LONG = 1 + LONG / 8,
		OCT_LONG_LONG = 1 + LONG_LONG / 8,
		OCT_SIZE = 1 + SIZE / 8,
		OCT_UINTPTR = 1 + UINTPTR / 8,
		HEX_WINT = 1 + WINT / 16,
		HEX_INT = 1 + INT / 16,
		HEX_LONG = 1 + LONG / 16,
		HEX_LONG_LONG = 1 + LONG_LONG / 16,
		HEX_SIZE = 1 + SIZE / 16,
		HEX_UINTPTR = 1 + UINTPTR / 16,
	}; /* sizeof */
	/* Use size_t because we are calculating the length for the
	 * buffer conservatively and it may be much larger than the
	 * actual length. */
	const char *arg;
	size_t arg_len = 0;
	unsigned int errno_len = 0;
	int lflag = L_INT;
	int is_pad = 0;
	int is_thousep = 0;
	unsigned int pad_len;
	base_ty base;
	for (;; ++fmt) {
		if (*fmt == '%') {
cont_switch:
			switch (*++fmt) {
			case 's':
				arg = va_arg(ap, const char *);
				arg_len += jstr_likely(arg != NULL) ? strlen(arg) : sizeof("(null)") - 1;
				break;
			case 'c':
				if (jstr_likely(lflag == L_INT)) {
					++arg_len;
				} else if (lflag == L_LONG) {
					arg_len += INT;
					lflag = L_INT;
				} else {
					goto einval;
				}
				goto get_arg;
				/* int */
			case 'u':
			case 'd':
			case 'i': base = B_DEC; goto check_integer;
			case 'x':
			case 'X': base = B_HEX; goto check_integer;
			case 'o':
				base = B_OCT;
				goto check_integer;
check_integer:
				if (lflag == L_INT) {
					JSTR__GETMAXDIGITS(INT);
				} else {
					if (lflag == L_LONG)
						JSTR__GETMAXDIGITS(LONG);
					else if (lflag == L_LONG_LONG)
						JSTR__GETMAXDIGITS(LONG_LONG);
					else
						goto einval;
					lflag = L_INT;
				}
				goto get_arg;
				/* ptr */
			case 'p':
				/* max + 2 to make room for the 0x at
				 * the start of the number. */
				arg_len += jstr_likely(va_arg(ap, const void *) != NULL) ? (HEX_UINTPTR + sizeof("0x") - 1) : sizeof("(nil)") - 1;
				break;
				/* chars written */
			case 'n':
				arg_len += INT;
				break;
				/* double */
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				if (lflag == L_INT) {
					arg_len += (is_thousep) ? DOUBLE * 2 : DOUBLE;
				} else if (lflag == L_LONG) {
					arg_len += (is_thousep) ? DOUBLE_LONG * 2 : DOUBLE_LONG;
					lflag = L_INT;
				} else {
					goto einval;
				}
				goto get_arg;
			case '%': arg_len += 2; break;
			case 'm':
				if (errno_len == 0)
					arg_len += strlen(strerror(errno));
				else
					arg_len += errno_len;
				break;
			/* padding */
			case '*':
				/* Get length of padding from argument. */
				arg_len += (size_t)va_arg(ap, int);
				goto cont_switch;
			case '-': is_pad = 1; pad_len = 1;
			/* flags */
			case '+':
			case '#':
			case '0':
			case ' ': goto cont_switch;
			case '\'': is_thousep = 1; goto cont_switch;
			/* precision */
			case '.':
				is_pad = 1;
				pad_len = 1;
				goto cont_switch;
			/* length modifier */
			case 'l': ++lflag; goto cont_switch;
			case 't': lflag = UINTPTR; goto cont_switch;
			case 'z': lflag = SIZE; goto cont_switch;
			case 'h':
			case 'j': goto cont_switch;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (is_pad) {
					const unsigned char *p = (const unsigned char *)fmt;
					if (!jstr_isdigit(*p)) {
						pad_len = 0;
						goto cont_switch;
					}
					pad_len = *p++ - '0';
					for (; jstr_isdigit(*p); ++p)
						pad_len = (pad_len * 10)
						          + (*p - '0');
					fmt = (char *)p - 1;
					if (jstr_unlikely(pad_len > INT_MAX)) {
						errno = EOVERFLOW;
						return -1;
					}
					arg_len += pad_len;
				}
				goto cont_switch;
			/* case '\0': */
			default:
einval:
				errno = EINVAL;
				return -1;
get_arg:
				va_arg(ap, void *);
				break;
			}
			lflag = L_INT;
			is_pad = 0;
			is_thousep = 0;
		} else {
			++arg_len;
			if (jstr_unlikely(*fmt == '\0'))
				break;
		}
	}
	++arg_len;
	if (jstr_unlikely(arg_len > INT_MAX))
		arg_len = INT_MAX;
	return arg_len;
#endif
#undef JSTR__COUNTDIGITS
#undef JSTR__GETMAXDIGITS
}

#if JSTR_HAVE_SNPRINTF_STRLEN

/* Return JSTR_RET_ERR on error.
 * Referencing arguments with $ is not supported.
 * Supported conversions:
   %s - string.
   %c - char.
   %% - %.
   %d, %i, %u, %o, %x, %X - int.
   %a, %A, %e, %E, %f, %F, %g, %G - float/double.
   %zu - size_t.
   %l - long.
   %ll - long long.
   %lu - unsigned long.
   %llu - unsigned long long.
   %p - ptr.
   %n - chars written.
   %m - errno string.
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int arg_len = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err;
	if (jstr_chk(jstr_reserveexact(s, sz, cap, (size_t)arg_len * JSTR_ALLOC_MULTIPLIER)))
		goto err_free;
	va_start(ap, fmt);
	arg_len = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free_set_errno;
	*sz = (size_t)arg_len;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = arg_len;
err_free:
	jstr_free_noinline(s, sz, cap);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Return JSTR_RET_ERR on error.
 * Supports only some conversions: see jstr_asprintf().
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_j(jstr_ty *R j, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	if (jstr_chk(jstr_reserveexact(&j->data, &j->size, &j->capacity, (size_t)ret * JSTR_ALLOC_MULTIPLIER)))
		goto err_free;
	va_start(ap, fmt);
	ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = (size_t)ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
err_free:
	jstr_free_noinline(&j->data, &j->size, &j->capacity);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Append ... to end of S.
 * Return JSTR_RET_ERR on error.
 * Supports only some conversions: see jstr_asprintf().
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_append(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + (size_t)ret)))
		goto err_free;
	va_start(ap, fmt);
	ret = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz += (size_t)ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
err_free:
	jstr_free_noinline(s, sz, cap);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Append ... to end of S.
 * Return JSTR_RET_ERR on error.
 * Supports only some conversions: see jstr_asprintf().
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_append_j(jstr_ty *R j, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	if (jstr_chk(jstr_reserve(&j->data, &j->size, &j->capacity, j->size + (size_t)ret)))
		goto err_free;
	va_start(ap, fmt);
	ret = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size += (size_t)ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
err_free:
	jstr_free_noinline(&j->data, &j->size, &j->capacity);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Return JSTR_RET_ERR on error.
 * Supports only some conversions: see jstr_asprintf().
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	if (jstr_chk(jstr_reserve(s, sz, cap, start_idx + (size_t)ret)))
		goto err_free;
	va_start(ap, fmt);
	ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
err_free:
	jstr_free_noinline(s, sz, cap);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Return JSTR_RET_ERR on error.
 * Supports only some conversions: see jstr_asprintf().
 * Otherwise, JSTR_RET_ERR and set errno to EINVAL. */
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_from_j(jstr_ty *R j, size_t start_idx, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlenmax(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	if (jstr_chk(jstr_reserve(&j->data, &j->size, &j->capacity, start_idx + (size_t)ret)))
		goto err_free;
	va_start(ap, fmt);
	ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
err_free:
	jstr_free_noinline(&j->data, &j->size, &j->capacity);
err:
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#endif

/* Assume that S has enough space.
 * Use jstr_asprintf() to grow S. */
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf(char *R *R s, size_t *R sz, size_t *R cap, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz = (size_t)ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	jstr_free_noinline(s, sz, cap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Assume that S has enough space.
 * Use jstr_asprintf() to grow S. */
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf_j(jstr_ty *R j, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = (size_t)ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	jstr_free_noinline(&j->data, &j->size, &j->capacity);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Assume that S has enough space.
 * Use jstr_asprintf() to grow S. */
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	jstr_free_noinline(s, sz, cap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Assume that S has enough space.
 * Use jstr_asprintf() to grow S. */
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf_from_j(jstr_ty *R j, size_t start_idx, const char *R fmt, ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = (size_t)ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	jstr_free_noinline(&j->data, &j->size, &j->capacity);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR__END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
