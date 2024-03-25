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

#ifndef JSTR_REPLACE_H
#define JSTR_REPLACE_H 1

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
JSTR__END_DECLS

#include "jstr-builder.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

JSTR__BEGIN_DECLS

typedef struct jstr__inplace_ty {
	char *dst;
	const char *src;
	char *src_e;
} jstr__inplace_ty;

#define JSTR__INPLACE_INIT(str) \
	{                       \
		str, str, str   \
	}

#define JSTR__INPLACE_RMALL(i, find_len)                         \
	do {                                                     \
		const size_t _n = JSTR_DIFF((i).src_e, (i).src); \
		if (jstr_likely((i).dst != (i).src))             \
			memmove((i).dst, (i).src, _n);           \
		(i).dst += _n;                                   \
		(i).src += _n + find_len;                        \
		(i).src_e += find_len;                           \
	} while (0)

#define JSTR__INPLACE_RPLCALL(i, rplc, rplc_len, find_len)                                \
	do {                                                                              \
		const size_t _n = JSTR_DIFF((i).src_e, (i).src);                          \
		if (jstr_likely(find_len != rplc_len) && jstr_likely((i).dst != (i).src)) \
			memmove((i).dst, (i).src, _n);                                    \
		(i).dst = (char *)jstr_mempcpy((i).dst + _n, rplc, rplc_len);             \
		(i).src += _n + find_len;                                                 \
		(i).src_e += find_len;                                                    \
	} while (0)

/* Insert SRC into DST[AT].
 * Return value:
 * ptr to '\0' in S.
 * Assume that S have enough space for SRC. */
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jstr_insert_unsafe(char *R s, size_t sz, size_t at, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	memmove(s + at + src_len, s + at, sz - at + 1);
	memcpy(s + at, src, src_len);
}

/* Insert SRC into DST[AT].
 * Return JSTR_RET_ERR on malloc error.
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_insert_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len)))
		return JSTR_RET_ERR;
	jstr_insert_unsafe(*s, *sz, at, src, src_len);
	*sz += src_len;
	return JSTR_RET_SUCC;
}

/* Replace RPLC in S with FIND.
    Return value:
    ptr to RPLC in S + RPLCLEN;
    NULL on error. */
JSTR_FUNC
static char *
jstr_rplcat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R rplc, size_t rplc_len, size_t find_len)
JSTR_NOEXCEPT
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + rplc_len - find_len)))
		return NULL;
	if (jstr_likely(rplc_len != find_len))
		jstr_strmove_len(*s + at + rplc_len, *s + at + find_len, *sz - (at + find_len));
	*sz += rplc_len - find_len;
	return (char *)jstr_mempcpy(*s + at, rplc, rplc_len);
}

/* Insert SRC after C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_ATTR_INLINE
JSTR_FUNC
static jstr_ret_ty
jstr_insertafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + 1), src, src_len);
	return JSTR_RET_SUCC;
}

/* Insert SRC after all C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_insertafterallchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		off = JSTR_DIFF(p, *s);
		if (jstr_chk(jstr_insert_len(s, sz, cap, off, src, src_len)))
			return JSTR_RET_ERR;
		off += src_len + 1;
	}
	return JSTR_RET_SUCC;
}

/* Insert SRC after end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_insertafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_insertafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *const p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len);
	return JSTR_RET_SUCC;
}

/* Insert SRC after all end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_insertafterall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_insertafterallchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	size_t off = 0;
	const char *p;
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	while ((p = (const char *)jstr_memmem_exec(&t, *s + off, *sz - off, find))) {
		off = JSTR_DIFF(p, *s);
		if (jstr_chk(jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len)))
			return JSTR_RET_ERR;
		off += find_len + src_len;
	}
	return JSTR_RET_SUCC;
}

/* Replace first group of REJECT in S with RPLC.
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
static size_t
jstr_rplcspn(char *R s, const char *R reject, char rplc)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s) && (*(s += strcspn(s, reject)))) {
		const size_t changed = strspn(s, reject);
		memset(s, rplc, changed);
		return changed;
	}
	return 0;
}

/* Replace all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_rplcallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject, char rplc)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	char *p = s + start_idx;
	size_t changed = 0;
	size_t i;
	while (*p && (*(p += strcspn(p, reject)))) {
		i = strspn(p, reject);
		changed += i;
		p = jstr_stpset_len(p, rplc, i);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}

/* Replace all REJECT in S with RPLC.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_rplcallspn(char *R s, size_t *R sz, const char *R reject, char rplc)
JSTR_NOEXCEPT
{
	return jstr_rplcallspn_from(s, sz, 0, reject, rplc);
}

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_rmspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, strlen(p));
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}

/* Remove all REJECT in S with RPLC
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmspn(char *R s, size_t *R sz, const char *R reject)
JSTR_NOEXCEPT
{
	return jstr_rmspn_from(s, sz, 0, reject);
}

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
static size_t
jstr_rmspn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, JSTR_DIFF(s + *sz, p));
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}

/* Remove all REJECT in S with RPLC
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmspn_len(char *R s, size_t *R sz, const char *R reject)
JSTR_NOEXCEPT
{
	return jstr_rmspn_len_from(s, sz, 0, reject);
}

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_rmallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	jstr__inplace_ty i = JSTR__INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(*i.src_e == '\0')
	    || !(*(i.src_e += strcspn(i.src_e, reject))))
		return 0;
	size_t find_len;
	size_t changed = 0;
	size_t j = JSTR_DIFF(i.src_e, i.src);
	find_len = strspn(i.src_e, reject);
	changed += find_len;
	goto start;
	while (*i.src_e && (*(i.src_e += strcspn(i.src_e, reject)))) {
		find_len = strspn(i.src_e, reject);
		changed += find_len;
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + find_len;
		i.src_e += find_len;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(i.src_e, i.src)), s);
	return changed;
}

/* Remove all REJECT in S with RPLC.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmallspn(char *R s, size_t *R sz, const char *R reject)
JSTR_NOEXCEPT
{
	return jstr_rmallspn_from(s, sz, 0, reject);
}

/* Remove N Cs in S from START_IDX.
 * Return value:
 * Number of Cs replaced. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
static size_t
jstr_rmnchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	const char *const end = s + *sz;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*i.src_e == '\0')
	    || !(i.src_e = (char *)memchr(i.src_e, c, JSTR_DIFF(end, i.src_e))))
		return 0;
	size_t changed = 0;
	size_t j = JSTR_DIFF(i.src_e, i.src);
	goto start;
	for (; n && (i.src_e = (char *)memchr(i.src_e, c, JSTR_DIFF(end, i.src_e))); --n, ++changed) {
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + 1;
		++i.src_e;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), s);
	return changed;
}

/* Remove N Cs in S.
 * Return value:
 * Number of Cs replaced. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmnchr_len(char *R s, size_t *R sz, int c, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_len_from(s, sz, 0, c, n);
}

/* Remove all C in S from START_IDX.
 * Return value:
 * Ptr to '\0' in S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmallchr_len_from(char *R s, size_t *R sz, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_len_from(s, sz, 0, c, (size_t)-1);
}

/* Remove all C in S.
 * Return value:
 * Ptr to '\0' in S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmallchr_len(char *R s, size_t *R sz, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_len(s, sz, c, (size_t)-1);
}

/* Remove N C in S from START_IDX.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_rmnchr_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	jstr__inplace_ty i = JSTR__INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*i.src_e == '\0')
	    || !*(i.src_e = jstr_strchrnul((char *)i.src_e, c)))
		return 0;
	size_t changed = 0;
	size_t j = JSTR_DIFF(i.src_e, i.src);
	goto start;
	for (; n && *(i.src_e = jstr_strchrnul((char *)i.src_e, c)); --n, ++changed) {
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + 1;
		++i.src_e;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(i.src_e, i.src)), s);
	return changed;
}

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmnchr(char *R s, size_t *R sz, int c, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_from(s, sz, 0, c, n);
}

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmallchr_from(char *R s, size_t *R sz, size_t start_idx, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_from(s, sz, start_idx, c, (size_t)-1);
}

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmallchr(char *R s, size_t *R sz, int c)
JSTR_NOEXCEPT
{
	return jstr_rmallchr_from(s, sz, 0, c);
}

/* Remove first C in S from START_IDX.
 * Return value:
 * Cs replaced. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_len_from(s, sz, start_idx, c, 1);
}

/* Remove first C in S.
 * Return value:
 * Cs replaced. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmchr_len(char *R s, size_t *R sz, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_len_from(s, sz, 0, c, 1);
}

/* Remove first C in S from START_IDX.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmchr_from(char *R s, size_t *R sz, size_t start_idx, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_from(s, sz, start_idx, c, 1);
}

/* Remove first C in S.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_ATTR_ACCESS((__read_write__, 1, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmchr(char *R s, size_t *R sz, int c)
JSTR_NOEXCEPT
{
	return jstr_rmnchr_from(s, sz, 0, c, 1);
}

/* Remove chars in REJECT in S.
 * Return value:
 * chars replaced.
 * SZ is only used to store the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_stripspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject)
JSTR_NOEXCEPT
{
	jstr__inplace_ty i = JSTR__INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(*i.src_e == '\0')
	    || !*(i.src_e += strcspn(i.src_e, reject)))
		return 0;
	size_t changed = 0;
	size_t j = JSTR_DIFF(i.src_e, i.src);
	goto start;
	for (; *(i.src_e += strcspn(i.src_e, reject)); ++changed) {
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + 1;
		++i.src_e;
	}
	if (changed)
		*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(i.src_e, i.src)), s);
	return changed;
}

/* Remove chars in REJECT in S.
 * Return value:
 * chars replaced.
 * SZ is only used to store the length of S. */
JSTR_FUNC_VOID
static size_t
jstr_stripspn(char *R s, size_t *R sz, const char *R reject)
JSTR_NOEXCEPT
{
	return jstr_stripspn_from(s, sz, 0, reject);
}

/* Remove first HS in S.
 * Return value:
 * Ptr to '\0' in S. */
JSTR_FUNC_RET_NONNULL
JSTR_ATTR_INLINE
static char *
jstr_rmat_len_p(char *R s, size_t sz, size_t at, size_t find_len)
JSTR_NOEXCEPT
{
	jstr_strmove_len(s + at, s + at + find_len, JSTR_DIFF(s + sz, s + at + find_len));
	return s + sz - find_len;
}

/* Remove first HS in S from START_IDX.
 * Return value:
 * Ptr to '\0' in S.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
static int
jstr_rm_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *const p = (char *)jstr_memmem_exec(t, s + start_idx, *sz - start_idx, find);
	if (p == NULL)
		return 0;
	*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, *s), find_len), *s);
	return 1;
}

/* Remove first HS in S.
 * Return value:
 * Ptr to '\0' in S.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static int
jstr_rm_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rm_len_from_exec(t, s, sz, 0, find, find_len);
}

/* Remove first HS in S from START_IDX.
 * Return value:
 * Ptr to '\0' in S. */
JSTR_FUNC_VOID
static int
jstr_rm_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rm_len_from_exec(&t, s, sz, start_idx, find, find_len);
}

/* Remove first HS in S.
 * Return value:
 * Ptr to '\0' in S. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static int
jstr_rm_len(char *R s, size_t *R sz, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rm_len_from(s, sz, 0, find, find_len);
}

/* Replace all SEARCH in REPLACE. */
JSTR_FUNC_VOID
static void
jstr_rplcallchr(char *R s, char find, char rplc)
JSTR_NOEXCEPT
{
	for (; (s = strchr(s, find)); *s++ = rplc) {}
}

/* Replace N SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
static size_t
jstr_rplcnchr_len(char *R s, size_t sz, char find, char rplc, size_t n)
JSTR_NOEXCEPT
{
	const char *R end = s + sz;
	size_t changed = 0;
	for (; n-- && (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc, ++changed) {}
	return changed;
}

/* Replace N SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
static size_t
jstr_rplcnchr(char *R s, char find, char rplc, size_t n)
JSTR_NOEXCEPT
{
	size_t changed = 0;
	for (; n-- && (s = strchr(s, find)); *s++ = rplc, ++changed) {}
	return changed;
}

/* Replace all SEARCH in REPLACE. */
JSTR_FUNC_VOID
static void
jstr_rplcallchr_len(char *R s, size_t sz, char find, char rplc)
JSTR_NOEXCEPT
{
	const char *R end = s + sz;
	for (; (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc) {}
}

/* Replace last SEARCH in S with REPLACE.
 * Return value:
 * number of number of FINDs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static int
jstr_rmlast_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	const char *const p
	= jstr_strrstr_len(s + start_idx, *sz - start_idx, find, find_len);
	if (p) {
		*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, s), find_len), s);
		return 1;
	}
	return 0;
}

/* Replace last SEARCH in S with REPLACE.
 * Return value:
 * number of number of FINDs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static int
jstr_rmlast_len(char *R s, size_t *R sz, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rmlast_len_from(s, sz, 0, find, find_len);
}

/* Replace last SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static int
jstr_rplclast_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	const char *const p = jstr_strrstr_len(*s, *sz, find, find_len);
	if (p) {
		if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
			return -1;
		return 1;
	}
	return 0;
}

/* Replace N SEARCH in S with REPLACE from START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
static size_t
jstr_rmn_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (find_len == 1)
		return jstr_rmnchr_len(s, sz, *find, n);
	if (jstr_unlikely(find_len == 0))
		return 0;
	const char *const end = s + *sz;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0)
	    || !(i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find)))
		return 0;
	size_t changed = 0;
	size_t j = JSTR_DIFF(i.src_e, i.src);
	goto start;
	for (; n && (i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find)); --n, ++changed) {
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + find_len;
		i.src_e += find_len;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), s);
	return changed;
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmn_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rmn_len_from_exec(t, s, sz, 0, find, find_len, n);
}

/* Replace all SEARCH in S.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmall_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rmn_len_exec(t, s, sz, find, find_len, (size_t)-1);
}

/* Replace N SEARCH in S with REPLACE from START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
static size_t
jstr_rmn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n)
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rmn_len_from_exec(&t, s, sz, start_idx, find, find_len, n);
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmn_len(char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rmn_len_from(s, sz, 0, find, find_len, n);
}

/* Replace all SEARCH in S.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr_rmall_len(char *R s, size_t *R sz, const char *R find, size_t find_len)
JSTR_NOEXCEPT
{
	return jstr_rmn_len(s, sz, find, find_len, (size_t)-1);
}

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
static int
jstr_rplc_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *p = (char *)jstr_memmem_exec(t, *s + start_idx, *sz - start_idx, find);
	if (p == NULL)
		return 0;
	if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
		return -1;
	return 1;
}

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
JSTR_ATTR_INLINE
static int
jstr_rplc_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplc_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len);
}

/* Replace N SEARCH in S with REPLACE from S + START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp.
 * The current implementation is O(n) where n is the length
 * of S.
 * It does 6 * n operations:
 * 2 * 2 n operations for memmem,
 * n operations for malloc,
 * n operations for replacements. */
JSTR_FUNC
static size_t
jstr_rplcn_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (n == 1)
		return (size_t)jstr_rplc_len_from_exec(t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len);
	if (jstr_unlikely(n == 0))
		return 0;
	if (jstr_unlikely(rplc_len == 0))
		return jstr_rmn_len_from_exec(t, *s, sz, start_idx, find, find_len, n);
	if (find_len == 1) {
		if (rplc_len == 1)
			return jstr_rplcnchr_len(*s + start_idx, *sz - start_idx, *find, *rplc, n);
	} else if (jstr_unlikely(find_len == 0)) {
		return 0;
	}
	jstr__inplace_ty i = JSTR__INPLACE_INIT(*s + start_idx);
	size_t changed = 0;
	const char *end = *s + *sz;
	if (rplc_len <= find_len) {
		if (!(i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find)))
			return 0;
		size_t j = JSTR_DIFF(i.src_e, i.src);
		goto start;
		for (; n && (i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find)); --n, ++changed) {
			j = JSTR_DIFF(i.src_e, i.src);
			if (jstr_likely(find_len != rplc_len))
				memmove(i.dst, i.src, j);
start:
			i.dst = (char *)jstr_mempcpy(i.dst + j, rplc, rplc_len);
			i.src += j + find_len;
			i.src_e += find_len;
		}
		if (jstr_likely(rplc_len != find_len))
			*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), *s);
	} else {
		char *first = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find);
		if (jstr_nullchk(first))
			return 0;
		i.src_e = first;
		const char *last;
		goto loop1;
		while (n && (i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find))) {
loop1:
			--n;
			++changed;
			last = i.src_e;
			i.src_e += find_len;
		}
		if (!changed)
			return 0;
		if (changed == 1) {
			if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(first, *s), rplc, rplc_len, find_len)))
				goto err;
			return 1;
		}
		/* Currently last points to the last match. We are going to make it point
		 * to the end of the last match. */
		last += find_len;
		char *dst_s;
		const size_t new_size = *sz + changed * (rplc_len - find_len) + 1;
		const size_t first_len = *sz - JSTR_DIFF(first, *s);
		enum { MAX_STACK = 1024 }; /* Past this size, don't use a stack buffer */
		enum { USE_MALLOC = 0,
		       USE_MOVE,
		       USE_STACK };
		int mode = USE_MALLOC;
		/* Avoid malloc if we can fit the source and destination string. */
		if (*cap >= new_size + first_len)
			mode = USE_MOVE;
#if JSTR_HAVE_VLA || JSTR_HAVE_ALLOCA
		/* We can fit the source string in the stack buffer
		 * and the destination string doesn't need realloc'ing. */
		else if (first_len <= MAX_STACK && *cap >= new_size)
			mode = USE_STACK;
#	if JSTR_HAVE_VLA
		char stack_buf[mode & USE_STACK ? first_len : 1];
#	endif
#endif
		/* If the source string has enough capacity to fit both
		 * itself and the destination string, avoid allocation by pushing
		 * back the source string to make room for the destination string. */
		if (mode == USE_MOVE) {
			/* SRC is the source string + NEW_SIZE. */
			i.src = *s + new_size;
			/* DST is source string. */
			i.dst = first;
			/* Move back the source string so we have enough
			 * space for the destination string. */
			memmove((void *)i.src, i.dst, first_len);
			/* Update the ptrs to point to SRC. */
			last = i.src + (last - first);
			end = i.src + (end - first);
			first = (char *)i.src;
			dst_s = *s;
#if JSTR_HAVE_VLA || JSTR_HAVE_ALLOCA /* Maybe use a stack buffer. */
		} else if (mode == USE_STACK) { /* NEW_SIZE is small enough. */
			/* DST is the source string. */
			i.dst = first;
			/* SRC is the stack buffer. */
#	if JSTR_HAVE_VLA
			i.src = stack_buf;
#	else
			i.src = (const char *)alloca(first_len);
#	endif
			/* Copy the source string to SRC. */
			memcpy((void *)i.src, i.dst, first_len);
			/* Update the ptrs to point to SRC. */
			last = i.src + (last - first);
			end = i.src + (end - first);
			first = (char *)i.src;
			dst_s = *s;
#endif
		} else { /* Capacity is too small or we can't use a stack buffer. */
			i.dst = NULL;
			if (jstr_chk(jstr_reserveexactalways(&i.dst, sz, cap, new_size)))
				goto err;
			/* Currently, i.src = *s + start_idx */
			i.src = *s;
			dst_s = i.dst;
		}
		size_t j;
		n = changed;
		/* Cache first match. */
		i.src_e = first;
		goto loop2;
		while (n && (i.src_e = (char *)jstr_memmem_exec(t, i.src, JSTR_DIFF(last, i.src), find))) {
loop2:
			--n;
			j = JSTR_DIFF(i.src_e, i.src);
			/* We must use memmove because DST and SRC may overlap if CAN_FIT is true. */
			if (jstr_likely(find_len != rplc_len))
				memmove(i.dst, i.src, j);
			i.dst += j;
			i.dst = (char *)jstr_mempmove(i.dst, rplc, rplc_len);
			i.src = i.src_e + find_len;
		}
		/* Don't free if we didn't malloc. */
		if (mode == USE_MALLOC) {
			free(*s);
			/* *S is currently the source string. */
			*s = dst_s;
		}
		if (jstr_likely(find_len != rplc_len))
			*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), dst_s);
	}
	return changed;
err:
	JSTR_RETURN_ERR((size_t)-1);
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcn_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}

/* Replace all SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcall_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcall_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
JSTR_ATTR_INLINE
static int
jstr_rplc_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplc_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len);
}

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static int
jstr_rplc_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplc_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len);
}

/* Replace N SEARCH in S with REPLACE from S + START_IDX.
 * Return -1 on malloc error. */
JSTR_FUNC
static size_t
jstr_rplcn_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n)
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplcn_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, n);
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcn_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}

/* Replace all SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcall_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static size_t
jstr_rplcall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}

/* Replace first SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static int
jstr_rplcchr_len(char *R s, size_t sz, char find, char rplc)
JSTR_NOEXCEPT
{
	s = (char *)memchr(s, find, sz);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}

/* Replace first SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static int
jstr_rplcchr(char *R s, char find, char rplc)
JSTR_NOEXCEPT
{
	s = strchr(s, find);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}

/* Place SRC into DST[AT].
 * Assume that S have enough space for SRC.
 * Return value: */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_place_len_unsafe(char *R s, size_t at, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	memcpy(s + at, src, src_len);
}

/* Place SRC into DST[AT].
 * Return JSTR_RET_ERR on malloc error.
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_place_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (at + src_len > *sz) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, at + src_len)))
			return JSTR_RET_ERR;
		*sz = at + src_len;
		*(*s + *sz) = '\0';
	}
	jstr_place_len_unsafe(*s, at, src, src_len);
	return JSTR_RET_SUCC;
}

/* Place SRC after C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_placeafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s + 1), src, src_len);
	return JSTR_RET_SUCC;
}

/* Place SRC after end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * otherwise JSTR_RET_SUCC. */
JSTR_FUNC
static jstr_ret_ty
jstr_placeafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len)
JSTR_NOEXCEPT
{
	if (find_len == 1)
		return jstr_placeafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *const p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len);
	return JSTR_RET_SUCC;
}

/* Return value:
 * JSTR_RET_ERR on error.
 * JSTR_RET_SUCC otherwise. */
JSTR_FUNC
static jstr_ret_ty
jstr_repeat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n <= 1))
		return JSTR_RET_SUCC;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * n)))
		return JSTR_RET_ERR;
	*sz = JSTR_DIFF(jstr_repeat_len_unsafe_p(*s, *sz, n), *s);
	return JSTR_RET_SUCC;
}

JSTR__END_DECLS

#undef R

#endif /* JSTR_REPLACE_H */
