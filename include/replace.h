/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#ifndef JSTR_REPLACE_H
#	define JSTR_REPLACE_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#ifdef JSTR_IMPLEMENTATION
#undef JSTR_IMPLEMENTATION
#	include "builder.h"
#	include "ctype.h"
#	include "macros.h"
#	include "stdstring.h"
#	include "string.h"
#define JSTR_IMPLEMENTATION 1
#endif

#	define R JSTR_RESTRICT

JSTR_INTERNAL_BEGIN_DECLS

typedef struct jstr_internal_inplace_ty {
	char *dst;
	const char *src;
	char *src_e;
} jstr_internal_inplace_ty;

#	define JSTR_INTERNAL_INPLACE_INIT(str) \
		{                       \
			str,            \
			str,            \
			str             \
		}

#	define JSTR_INTERNAL_INPLACE_RMALL(i, find_len)                         \
		do {                                                     \
			const size_t _n = JSTR_DIFF((i).src_e, (i).src); \
			if (jstr_likely((i).dst != (i).src))             \
				memmove((i).dst, (i).src, _n);           \
			(i).dst += _n;                                   \
			(i).src += _n + find_len;                        \
			(i).src_e += find_len;                           \
		} while (0)

#	define JSTR_INTERNAL_INPLACE_RPLCALL(i, rplc, rplc_len, find_len)                                \
		do {                                                                              \
			const size_t _n = JSTR_DIFF((i).src_e, (i).src);                          \
			if (jstr_likely(find_len != rplc_len) && jstr_likely((i).dst != (i).src)) \
				memmove((i).dst, (i).src, _n);                                    \
			(i).dst = (char *)jstr_mempcpy((i).dst + _n, rplc, rplc_len);             \
			(i).src += _n + find_len;                                                 \
			(i).src_e += find_len;                                                    \
		} while (0)

/* TODO: optimize memory allocation for *all* functions. */

/* Insert SRC into DST[AT].
 * Return value:
 * ptr to '\0' in S.
 * Assume that S have enough space for SRC. */
JSTR_ATTR_ACCESS((__read_only__, 4, 5))
JSTR_FUNC_VOID
void
jstr_insert_unsafe(char *R s, size_t sz, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	memmove(s + at + src_len, s + at, sz - at + 1);
	memcpy(s + at, src, src_len);
}
#else
;
#endif

/* Insert SRC into DST[AT].
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_insert_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + src_len + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	jstr_insert_unsafe(*s, *sz, at, src, src_len);
	*sz += src_len;
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Replace RPLC in S with FIND.
 * Return value:
 * ptr to RPLC in S + RPLCLEN.
 * NULL on error. */
JSTR_FUNC
char *
jstr_rplcat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R rplc, size_t rplc_len, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz + rplc_len - find_len + 1)))
		JSTR_RETURN_ERR_P(NULL);
	if (jstr_likely(rplc_len != find_len))
		jstr_strmove_len(*s + at + rplc_len, *s + at + find_len, *sz - (at + find_len));
	*sz += rplc_len - find_len;
	return (char *)jstr_mempcpy(*s + at, rplc, rplc_len);
}
#else
;
#endif

/* Insert SRC after C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + 1), src, src_len);
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Insert SRC after all C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterallchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	size_t off = 0;
	const char *p;
	while ((p = (char *)memchr(*s + off, c, *sz - off))) {
		off = JSTR_DIFF(p, *s);
		if (jstr_chk(jstr_insert_len(s, sz, cap, off, src, src_len)))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		off += src_len + 1;
	}
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Insert SRC after end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_insertafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_insertafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len);
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Insert SRC after all end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_insertafterall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_insertafterallchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	size_t off = 0;
	const char *p;
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	while ((p = (const char *)jstr_memmem_exec(&t, *s + off, *sz - off, find, find_len))) {
		off = JSTR_DIFF(p, *s);
		if (jstr_chk(jstr_insert_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len)))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		off += find_len + src_len;
	}
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Replace first group of REJECT in S with RPLC.
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
size_t
jstr_rplcspn(char *R s, const char *R reject, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_likely(*s) && (*(s += strcspn(s, reject)))) {
		const size_t changed = strspn(s, reject);
		memset(s, rplc, changed);
		return changed;
	}
	return 0;
}
#else
;
#endif

/* Replace all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rplcallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
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
#else
;
#endif

/* Replace all REJECT in S with RPLC.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rplcallspn(char *R s, size_t *R sz, const char *R reject, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcallspn_from(s, sz, 0, reject, rplc);
}
#else
;
#endif

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, strlen(p) - changed);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}
#else
;
#endif

/* Remove all REJECT in S with RPLC
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmspn_from(s, sz, 0, reject);
}
#else
;
#endif

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmspn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	char *p = s + start_idx;
	size_t changed = 0;
	if (jstr_likely(*p) && (*(p += strcspn(p, reject)))) {
		changed = strspn(p, reject);
		p = jstr_stpmove_len(p, p + changed, JSTR_DIFF(s + *sz, p) - changed);
	}
	*sz = JSTR_DIFF(p, s);
	return changed;
}
#else
;
#endif

/* Remove all REJECT in S with RPLC
 * Return value:
 * Characters replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmspn_len(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmspn_len_from(s, sz, 0, reject);
}
#else
;
#endif

/* Remove all REJECT in S with RPLC from START_IDX.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmallspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(*i.src_e == '\0') || !(*(i.src_e += strcspn(i.src_e, reject))))
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
#else
;
#endif

/* Remove all REJECT in S with RPLC.
 * Return value:
 * Characters replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmallspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmallspn_from(s, sz, 0, reject);
}
#else
;
#endif

/* Remove N Cs in S from START_IDX.
 * Return value:
 * Number of Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	const char *end = s + *sz;
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0) || jstr_unlikely(*i.src_e == '\0') || !(i.src_e = (char *)memchr(i.src_e, c, JSTR_DIFF(end, i.src_e))))
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
#else
;
#endif

/* Remove N Cs in S.
 * Return value:
 * Number of Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_len(char *R s, size_t *R sz, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, n);
}
#else
;
#endif

/* Remove all C in S from START_IDX.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_len_from(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, (size_t)-1);
}
#else
;
#endif

/* Remove all C in S.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_len(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len(s, sz, c, (size_t)-1);
}
#else
;
#endif

/* Remove N C in S from START_IDX.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmnchr_from(char *R s, size_t *R sz, size_t start_idx, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0) || jstr_unlikely(*i.src_e == '\0') || !*(i.src_e = jstr_strchrnul((char *)i.src_e, c)))
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
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, (size_t)((s + *sz) - i.src)), s);
	return changed;
}
#else
;
#endif

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmnchr(char *R s, size_t *R sz, int c, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, 0, c, n);
}
#else
;
#endif

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmallchr_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, start_idx, c, (size_t)-1);
}
#else
;
#endif

/* Remove all C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmallchr(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmallchr_from(s, sz, 0, c);
}
#else
;
#endif

/* Remove first C in S from START_IDX.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmchr_len_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, start_idx, c, 1);
}
#else
;
#endif

/* Remove first C in S.
 * Return value:
 * Cs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmchr_len(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_len_from(s, sz, 0, c, 1);
}
#else
;
#endif

/* Remove first C in S from START_IDX.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmchr_from(char *R s, size_t *R sz, size_t start_idx, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, start_idx, c, 1);
}
#else
;
#endif

/* Remove first C in S.
 * Return value:
 * Cs replaced.
 * SZ is only used to save the length of S. */
JSTR_FUNC_VOID
size_t
jstr_rmchr(char *R s, size_t *R sz, int c) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmnchr_from(s, sz, 0, c, 1);
}
#else
;
#endif

/* Remove chars in REJECT in S.
 * Return value:
 * chars replaced.
 * SZ is only used to store the length of S. */
JSTR_FUNC_VOID
size_t
jstr_stripspn_from(char *R s, size_t *R sz, size_t start_idx, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(*i.src_e == '\0') || !*(i.src_e += strcspn(i.src_e, reject)))
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
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, (size_t)((s + *sz) - i.src)), s);
	return changed;
}
#else
;
#endif

/* Remove chars in REJECT in S.
 * Return value:
 * chars replaced.
 * SZ is only used to store the length of S. */
JSTR_FUNC_VOID
size_t
jstr_stripspn(char *R s, size_t *R sz, const char *R reject) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_stripspn_from(s, sz, 0, reject);
}
#else
;
#endif

/* Remove first HS in S.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_RET_NONNULL
char *
jstr_rmat_len_p(char *s, size_t sz, size_t at, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_strmove_len(s + at, s + at + find_len, JSTR_DIFF(s + sz, s + at + find_len));
	return s + sz - find_len;
}
#else
;
#endif

/* Remove first HS in S from START_IDX.
 * Return value:
 * ptr to '\0' in S.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
int
jstr_rm_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *p = (char *)jstr_memmem_exec(t, s + start_idx, *sz - start_idx, find, find_len);
	if (p == NULL)
		return 0;
	*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, s), find_len), s);
	return 1;
}
#else
;
#endif

/* Remove first HS in S.
 * Return value:
 * ptr to '\0' in S.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
int
jstr_rm_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rm_len_from_exec(t, s, sz, 0, find, find_len);
}
#else
;
#endif

/* Remove first HS in S from START_IDX.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_VOID
int
jstr_rm_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rm_len_from_exec(&t, s, sz, start_idx, find, find_len);
}
#else
;
#endif

/* Remove first HS in S.
 * Return value:
 * ptr to '\0' in S. */
JSTR_FUNC_VOID
int
jstr_rm_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rm_len_from(s, sz, 0, find, find_len);
}
#else
;
#endif

/* Replace all SEARCH in REPLACE. */
JSTR_FUNC_VOID
void
jstr_rplcallchr(char *s, char find, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	for (; (s = strchr(s, find)); *s++ = rplc) {}
}
#else
;
#endif

/* Replace N SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rplcnchr_len(char *s, size_t sz, char find, char rplc, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *end = s + sz;
	size_t changed = 0;
	for (; n-- && (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc, ++changed) {}
	return changed;
}
#else
;
#endif

/* Replace N SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rplcnchr(char *s, char find, char rplc, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	size_t changed = 0;
	for (; n-- && (s = strchr(s, find)); *s++ = rplc, ++changed) {}
	return changed;
}
#else
;
#endif

/* Replace all SEARCH in REPLACE. */
JSTR_FUNC_VOID
void
jstr_rplcallchr_len(char *s, size_t sz, char find, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *end = s + sz;
	for (; (s = (char *)memchr(s, find, JSTR_DIFF(end, s))); *s++ = rplc) {}
}
#else
;
#endif

/* Replace last SEARCH in S with REPLACE.
 * Return value:
 * number of number of FINDs replaced. */
JSTR_FUNC_VOID
int
jstr_rmlast_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	const char *p = jstr_strrstr_len(s + start_idx, *sz - start_idx, find, find_len);
	if (p) {
		*sz = JSTR_DIFF(jstr_rmat_len_p(s, *sz, JSTR_DIFF(p, s), find_len), s);
		return 1;
	}
	return 0;
}
#else
;
#endif

/* Replace last SEARCH in S with REPLACE.
 * Return value:
 * number of number of FINDs replaced. */
JSTR_FUNC_VOID
int
jstr_rmlast_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmlast_len_from(s, sz, 0, find, find_len);
}
#else
;
#endif

/* Replace last SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
int
jstr_rplclast_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *p = jstr_strrstr_len(*s, *sz, find, find_len);
	if (p) {
		if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
			JSTR_RETURN_ERR(-1);
		return 1;
	}
	return 0;
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE from START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_from_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	if (find_len == 1)
		return jstr_rmnchr_len_from(s, sz, start_idx, *find, n);
	if (jstr_unlikely(find_len == 0))
		return 0;
	const char *end = s + *sz;
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(s + start_idx);
	if (jstr_unlikely(n == 0) || !(i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find, find_len)))
		return 0;
	size_t changed = 0;
	size_t prev_len = JSTR_DIFF(i.src_e, i.src);
	goto start;
	for (; n && (i.src_e = (char *)jstr_memmem_exec(t, i.src_e, JSTR_DIFF(end, i.src_e), find, find_len)); --n, ++changed) {
		/* Length of previous SRC that needs to be copied to DST. */
		prev_len = JSTR_DIFF(i.src_e, i.src);
		/* Copy to DST the previous SRC. */
		memmove(i.dst, i.src, prev_len);
start:
		/* Advance DST after the copy. */
		i.dst += prev_len;
		/* Advance SRC and SRC_E to the next SRC to find. */
		i.src += prev_len + find_len;
		i.src_e += find_len;
	}
	/* Copy to DST the remaining SRC. */
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), s);
	return changed;
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_from_exec(t, s, sz, 0, find, find_len, n);
}
#else
;
#endif

/* Replace all SEARCH in S.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC_VOID
size_t
jstr_rmall_len_exec(const jstr_twoway_ty *R t, char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_exec(t, s, sz, find, find_len, (size_t)-1);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE from START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmn_len_from(char *R s, size_t *R sz, size_t start_idx, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rmn_len_from_exec(&t, s, sz, start_idx, find, find_len, n);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmn_len(char *R s, size_t *R sz, const char *R find, size_t find_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len_from(s, sz, 0, find, find_len, n);
}
#else
;
#endif

/* Replace all SEARCH in S.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC_VOID
size_t
jstr_rmall_len(char *R s, size_t *R sz, const char *R find, size_t find_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rmn_len(s, sz, find, find_len, (size_t)-1);
}
#else
;
#endif

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
int
jstr_rplc_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
	if (jstr_unlikely(find_len == 0))
		return 0;
	char *p = (char *)jstr_memmem_exec(t, *s + start_idx, *sz - start_idx, find, find_len);
	if (p == NULL)
		return 0;
	if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(p, *s), rplc, rplc_len, find_len)))
		return -1;
	return 1;
}
#else
;
#endif

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
int
jstr_rplc_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplc_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len);
}
#else
;
#endif

#ifdef JSTR_IMPLEMENTATION
static inline size_t
j_rplc_grow_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap,
                 char *first_match, const char *R find, size_t find_len,
                 const char *R rplc, size_t rplc_len, size_t n, size_t changed,
                 const char *last_match) JSTR_NOEXCEPT
{
	/* Calculate where the last match ends in the original string. */
	const char *last_match_end = last_match + find_len;
	/* Calculate final string size after all replacements (each match grows the string by rplc_len - find_len). */
	const size_t new_size = *sz + changed * (rplc_len - find_len);
	/* suffix_len is the length from the first match to the very end of the original string. */
	const size_t suffix_len = *sz - JSTR_DIFF(first_match, *s);
	/* Store the old buffer address to handle potential pointer adjustments if jstr_reserve reallocates. */
	const uintptr_t s_old = (uintptr_t)*s;
	/* Reserve space for final string and moved suffix + NUL terminator.
	 * The safety zone must contain both the rebuilt string (new_size) and the temporary moved suffix (suffix_len). */
	if (jstr_chk(jstr_reserve(s, sz, cap, new_size + suffix_len + 1))) {
		JSTR_RETURN_ERR_ZU((size_t)-1);
	}
	/* write_ptr starts at the location of the first match in the (potentially reallocated) buffer. */
	char *write_ptr = *s + JSTR_DIFF(first_match, s_old);
	/* Move original suffix to end of buffer (suffix_start).
	 * Since rplc_len > find_len, writing replacements from left to right would overwrite unprocessed characters.
	 * Shifting the suffix to suffix_start (at s + new_size) isolates the source characters in a safe non-overlapping zone. */
	char *suffix_start = *s + new_size;
	memmove(suffix_start, write_ptr, suffix_len);
	/* Rebase pointers to be relative to the shifted suffix inside the temporary safe zone. */
	const char *last_match_end_in_suffix = suffix_start + (last_match_end - first_match);
	const char *suffix_end = suffix_start + suffix_len;
	const char *source_ptr = suffix_start;
	const char *search_ptr = suffix_start;
	size_t prev_len;
	/* Second pass: perform replacements from left to right. */
	do {
		/* Calculate unmatched segment length before the current match. */
		prev_len = JSTR_DIFF(search_ptr, source_ptr);
		/* Copy the unmatched segment from the safe zone to our destination. */
		memmove(write_ptr, source_ptr, prev_len);
		/* Write the replacement string and advance the write cursor. */
		write_ptr = (char *)jstr_mempmove(write_ptr + prev_len, rplc, rplc_len);
		/* Advance source pointer past the match in the safe zone. */
		source_ptr += prev_len + find_len;
		search_ptr += find_len;
	} while (--n && (search_ptr = (char *)jstr_memmem_exec(t, search_ptr, JSTR_DIFF(last_match_end_in_suffix, search_ptr), find, find_len)));
	/* Copy the remaining unmatched tail from the safe zone to the destination. */
	*sz = JSTR_DIFF(jstr_stpmove_len(write_ptr, source_ptr, JSTR_DIFF(suffix_end, source_ptr)), *s);
	return changed;
}
#endif

/* Replace N SEARCH in S with REPLACE from S + START_IDX.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp.
 * The current implementation is O(n). */
JSTR_FUNC
size_t
jstr_rplcn_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(start_idx >= *sz))
		return 0;
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
	size_t changed = 0;
	const char *end = *s + *sz;
	if (rplc_len <= find_len) {
		/* Do an in-place replacement with no reallocation. */
		char *match = (char *)jstr_memmem_exec(t, *s + start_idx, *sz - start_idx, find, find_len);
		if (!match)
			return 0;
		char *write_ptr = *s + start_idx;
		const char *source_ptr = *s + start_idx;
		const char *search_ptr = match;
		/* 1. Process first match (no memmove needed as write_ptr and source_ptr are identical). */
		size_t prev_len = JSTR_DIFF(search_ptr, source_ptr);
		write_ptr = (char *)jstr_mempcpy(write_ptr + prev_len, rplc, rplc_len);
		source_ptr += prev_len + find_len;
		search_ptr += find_len;
		changed = 1;
		--n;
		/* 2. Process subsequent matches. */
		while (n && (match = (char *)jstr_memmem_exec(t, search_ptr, JSTR_DIFF(end, search_ptr), find, find_len))) {
			prev_len = JSTR_DIFF(match, source_ptr);
			if (find_len != rplc_len)
				memmove(write_ptr, source_ptr, prev_len);
			write_ptr = (char *)jstr_mempcpy(write_ptr + prev_len, rplc, rplc_len);
			source_ptr += prev_len + find_len;
			search_ptr = match + find_len;
			++changed;
			--n;
		}
		/* 3. Copy remaining unmatched tail. */
		if (jstr_likely(rplc_len != find_len))
			*sz = JSTR_DIFF(jstr_stpmove_len(write_ptr, source_ptr, JSTR_DIFF(end, source_ptr)), *s);
	} else {
		/* May need to allocate/grow. */
		char *first = (char *)jstr_memmem_exec(t, *s + start_idx, *sz - start_idx, find, find_len);
		if (!first)
			return 0;
		/* First pass: count matches and track last match. */
		const char *search_ptr = first;
		const char *last;
		do {
			++changed;
			last = search_ptr;
			search_ptr += find_len;
		} while (--n && (search_ptr = (char *)jstr_memmem_exec(t, search_ptr, JSTR_DIFF(end, search_ptr), find, find_len)));
		if (changed == 1) {
			if (jstr_nullchk(jstr_rplcat_len(s, sz, cap, JSTR_DIFF(first, *s), rplc, rplc_len, find_len))) {
				JSTR_RETURN_ERR_ZU((size_t)-1);
			}
			return 1;
		}
		return j_rplc_grow_exec(t, s, sz, cap, first, find, find_len, rplc, rplc_len, changed, changed, last);
	}
	return changed;
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
size_t
jstr_rplcn_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}
#else
;
#endif

/* Replace all SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
size_t
jstr_rplcall_len_from_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
size_t
jstr_rplcall_len_exec(const jstr_twoway_ty *R t, char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from_exec(t, s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}
#else
;
#endif

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced.
 * T must be precompiled with jstr_memmem_comp. */
JSTR_FUNC
int
jstr_rplc_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplc_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len);
}
#else
;
#endif

/* Replace first SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
int
jstr_rplc_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplc_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE from S + START_IDX.
 * Return -1 on malloc error. */
JSTR_FUNC
size_t
jstr_rplcn_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	jstr_twoway_ty t;
	jstr_memmem_comp(&t, find, find_len);
	return jstr_rplcn_len_from_exec(&t, s, sz, cap, start_idx, find, find_len, rplc, rplc_len, n);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
size_t
jstr_rplcn_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, n);
}
#else
;
#endif

/* Replace all SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
size_t
jstr_rplcall_len_from(char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, start_idx, find, find_len, rplc, rplc_len, (size_t)-1);
}
#else
;
#endif

/* Replace N SEARCH in S with REPLACE.
 * Return -1 on malloc error.
 * Otherwise, number of FINDs replaced. */
JSTR_FUNC
size_t
jstr_rplcall_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R rplc, size_t rplc_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	return jstr_rplcn_len_from(s, sz, cap, 0, find, find_len, rplc, rplc_len, (size_t)-1);
}
#else
;
#endif

/* Replace first SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
int
jstr_rplcchr_len(char *s, size_t sz, char find, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = (char *)memchr(s, find, sz);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}
#else
;
#endif

/* Replace first SEARCH in REPLACE.
 * Return value:
 * number of FINDs replaced. */
JSTR_FUNC_VOID
int
jstr_rplcchr(char *s, char find, char rplc) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	s = strchr(s, find);
	if (s != NULL) {
		*s = rplc;
		return 1;
	}
	return 0;
}
#else
;
#endif

/* Place SRC into DST[AT].
 * Assume that S have enough space for SRC.
 * Return value: */
JSTR_FUNC_VOID
void
jstr_place_len_unsafe(char *R s, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	memcpy(s + at, src, src_len);
}
#else
;
#endif

/* Place SRC into DST[AT].
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_place_len(char *R *R s, size_t *R sz, size_t *R cap, size_t at, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	size_t old_sz = *sz;
	if (at + src_len > *sz) {
		if (jstr_chk(jstr_reservealways(s, sz, cap, at + src_len)))
			JSTR_RETURN_ERR(JSTR_RET_ERR);
		*sz = at + src_len;
		*(*s + *sz) = '\0';
		if (at > old_sz)
			memset(*s + old_sz, 0, at - old_sz);
	}
	jstr_place_len_unsafe(*s, at, src, src_len);
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Place SRC after C in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_placeafterchr_len(char *R *R s, size_t *R sz, size_t *R cap, int c, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	const char *p = (char *)memchr(*s, c, *sz);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s + 1), src, src_len);
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Place SRC after end of NE in DST.
 * Return JSTR_RET_ERR on malloc error.
 * Otherwise, JSTR_RET_SUCC. */
JSTR_FUNC
jstr_ret_ty
jstr_placeafter_len(char *R *R s, size_t *R sz, size_t *R cap, const char *R find, size_t find_len, const char *R src, size_t src_len) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (find_len == 1)
		return jstr_placeafterchr_len(s, sz, cap, *find, src, src_len);
	if (jstr_unlikely(find_len == 0))
		return JSTR_RET_SUCC;
	const char *p = (char *)jstr_memmem(*s, *sz, find, find_len);
	if (p != NULL)
		return jstr_place_len(s, sz, cap, JSTR_DIFF(p, *s + find_len), src, src_len);
	return JSTR_RET_SUCC;
}
#else
;
#endif

/* Return value:
 * JSTR_RET_ERR on error.
 * JSTR_RET_SUCC otherwise. */
JSTR_FUNC
jstr_ret_ty
jstr_repeat_len(char *R *R s, size_t *R sz, size_t *R cap, size_t n) JSTR_NOEXCEPT
#ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(n <= 1))
		return JSTR_RET_SUCC;
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * n + 1)))
		JSTR_RETURN_ERR(JSTR_RET_ERR);
	*sz = JSTR_DIFF(jstr_repeat_len_unsafe_p(*s, *sz, n), *s);
	return JSTR_RET_SUCC;
}
#else
;
#endif

JSTR_INTERNAL_END_DECLS

#	undef R

#endif /* JSTR_REPLACE_H */
