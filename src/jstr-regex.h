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

/* For jstr_re_rm_exec*(), jstr_re_rplc_exec*(), a negative number will be returned
 * indicating the negated value of the regex error code. To print an
 * error message, pass the negation of the returned error code. */

#ifndef JSTR_REGEX_H
#define JSTR_REGEX_H 1

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
JSTR__END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-replace.h"
#include "jstr-string.h"

#define jstr_re_chkcomp(errcode) jstr_unlikely((errcode) != JSTR_RE_RET_NOERROR)
#define jstr_re_chkexec(errcode) (jstr_re_chkcomp(errcode) && jstr_unlikely((errcode) != JSTR_RE_RET_NOMATCH))
#define jstr_re_chk(errcode)     jstr_unlikely((errcode) < 0)

#define R JSTR_RESTRICT

/* POSIX cflags */
#define JSTR_RE_CF_EXTENDED REG_EXTENDED
#define JSTR_RE_CF_ICASE    REG_ICASE
#define JSTR_RE_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#define JSTR_RE_EF_NOSUB  REG_NOSUB
#define JSTR_RE_EF_NOTBOL REG_NOTBOL
#define JSTR_RE_EF_NOTEOL REG_NOTEOL

/* BSD eflags */
#ifdef REG_STARTEND
#	define JSTR_RE_EF_STARTEND REG_STARTEND
#endif

JSTR__BEGIN_DECLS

typedef enum {
#ifdef REG_ENOSYS
	JSTR_RE_RET_ENOSYS = REG_ENOSYS,
#endif
#if defined REG_NOERROR
	JSTR_RE_RET_NOERROR = REG_NOERROR,
#else
	JSTR_RE_RET_NOERROR = 0,
#endif
#define JSTR_RE_RET_NOERROR JSTR_RE_RET_NOERROR
	JSTR_RE_RET_NOMATCH = REG_NOMATCH,
#define JSTR_RE_RET_NOMATCH JSTR_RE_RET_NOMATCH
	/* POSIX regcomp return values */
	JSTR_RE_RET_BADPAT = REG_BADPAT,
#define JSTR_RE_RET_BADPAT JSTR_RE_RET_BADPAT
	JSTR_RE_RET_ECOLLATE = REG_ECOLLATE,
#define JSTR_RE_RET_ECOLLATE JSTR_RE_RET_ECOLLATE
	JSTR_RE_RET_ECTYPE = REG_ECTYPE,
#define JSTR_RE_RET_ECTYPE JSTR_RE_RET_ECTYPE
	JSTR_RE_RET_EESCAPE = REG_EESCAPE,
#define JSTR_RE_RET_EESCAPE JSTR_RE_RET_EESCAPE
	JSTR_RE_RET_ESUBREG = REG_ESUBREG,
#define JSTR_RE_RET_ESUBREG JSTR_RE_RET_ESUBREG
	JSTR_RE_RET_EBRACK = REG_EBRACK,
#define JSTR_RE_RET_EBRACK JSTR_RE_RET_EBRACK
	JSTR_RE_RET_EPAREN = REG_EPAREN,
#define JSTR_RE_RET_EPAREN JSTR_RE_RET_EPAREN
	JSTR_RE_RET_EBRACE = REG_EBRACE,
#define JSTR_RE_RET_EBRACE JSTR_RE_RET_EBRACE
	JSTR_RE_RET_BADBR = REG_BADBR,
#define JSTR_RE_RET_BADBR JSTR_RE_RET_BADBR
	JSTR_RE_RET_ERANGE = REG_ERANGE,
#define JSTR_RE_RET_ERANGE JSTR_RE_RET_ERANGE
	JSTR_RE_RET_ESPACE = REG_ESPACE,
#define JSTR_RE_RET_ESPACE JSTR_RE_RET_ESPACE
	JSTR_RE_RET_BADRPT = REG_BADRPT
#define JSTR_RE_RET_BADRPT JSTR_RE_RET_BADRPT
/* GNU regcomp returns */
#ifdef REG_EEND
	,
	JSTR_RE_RET_EEND = REG_EEND
#	define JSTR_RE_RET_EEND JSTR_RE_RET_EEND
#endif
#ifdef REG_ESIZE
	,
	JSTR_RE_RET_ESIZE = REG_ESIZE
#	define JSTR_RE_RET_ESIZE REG_ESIZE
#endif
#ifdef REG_ERPAREN
	,
	JSTR_RE_RET_ERPAREN = REG_ERPAREN
#	define JSTR_RE_RET_ERPAREN REG_ERPAREN
#endif
} jstr_re_ret_ty;

#define JSTR__RE_ERR_EXEC_HANDLE(errcode, do_on_error)     \
	if (jstr_likely(errcode == JSTR_RE_RET_NOERROR)) { \
		;                                          \
	} else if (errcode == JSTR_RE_RET_NOMATCH) {       \
		break;                                     \
	} else {                                           \
		do_on_error;                               \
	}

#if JSTR_PANIC
#	define JSTR_RE_RETURN_ERR(errcode, preg)   \
		do {                                \
			jstr_re_err(errcode, preg); \
			jstr_errdie("");            \
			return -(errcode);          \
		} while (0)
#else
#	define JSTR_RE_RETURN_ERR(errcode, preg) return -(errcode)
#endif

/* Check if *s + start_idx is the beginning of a string or beginning of a line. */
#define IS_NOTBOL ((start_idx && *(*s + start_idx - 1) != '\n') ? JSTR_RE_EF_NOTBOL : 0)
#define IS_NOTBOL_LOOP (*(*s - 1) != '\n' ? JSTR_RE_EF_NOTBOL : 0)

typedef regoff_t jstr_re_off_ty;

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jstr_re_free(regex_t *R preg)
JSTR_NOEXCEPT
{
	regfree(preg);
}

JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
static void
jstr_re_errdie(jstr_re_ret_ty errcode, const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, 32);
	fprintf(stderr, "%s\n", buf);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
static void
jstr_re_err(jstr_re_ret_ty errcode, const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, sizeof(buf));
	fprintf(stderr, "%s\n", buf);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_comp(regex_t *R preg, const char *R ptn, int cflags)
JSTR_NOEXCEPT
{
#if JSTR_PANIC
	const int ret = (jstr_re_ret_ty)regcomp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR)) {
		jstr_re_err(-ret, preg);
		jstr_errdie("");
	}
	return ret;
#else
	return (jstr_re_ret_ty)regcomp(preg, ptn, cflags);
#endif
}

JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_ATTR_INLINE
JSTR_ATTR_WARN_UNUSED
JSTR_FUNC_PURE_MAY_NULL
JSTR_ATTR_NOTHROW
static jstr_re_ret_ty
jstr_re_exec(const regex_t *R preg, const char *R s, size_t nmatch, regmatch_t *R pmatch, int eflags)
JSTR_NOEXCEPT
{
	return (jstr_re_ret_ty)regexec(preg, s, nmatch, pmatch, eflags);
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_exec_len(const regex_t *R preg, const char *R s, size_t sz, size_t nmatch, regmatch_t *R pmatch, int eflags)
JSTR_NOEXCEPT
{
#ifdef JSTR_RE_EF_STARTEND
	pmatch->rm_so = 0;
	pmatch->rm_eo = sz;
#endif
	return (jstr_re_ret_ty)regexec(preg, s, nmatch, pmatch, eflags | JSTR_RE_EF_STARTEND);
}

/* Check if S matches precompiled regex.
 * Return return value of regexec. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_match(const regex_t *R preg, const char *R s, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec(preg, s, 0, NULL, eflags);
}

/* Search pattern in S.
 * Return return value of regexec.
 * Store offset of matched pattern in pmatch. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_search(const regex_t *R preg, const char *R s, regmatch_t *R pmatch, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec(preg, s, 1, pmatch, eflags);
}

/* Search pattern in S.
 * Return return value of regexec.
 * Store offset of matched pattern in pmatch. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_search_len(const regex_t *R preg, const char *R s, size_t sz, regmatch_t *R pmatch, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec_len(preg, s, sz, 1, pmatch, eflags);
}

/* Check if S matches PTN.
 * Return return value of regexec or regcomp if it fails. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstr_re_ret_ty
jstr_re_match_len(const regex_t *R preg, const char *R s, size_t sz, int eflags)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstr_re_exec_len(preg, s, sz, 0, &rm, eflags | JSTR_RE_EF_STARTEND);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative); */
JSTR_FUNC_VOID
static jstr_re_off_ty
jstr_re_rm_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	int ret = jstr_re_search_len(preg, *s + start_idx, *sz - start_idx, &rm, eflags | IS_NOTBOL);
	if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
		*sz = JSTR_DIFF(jstr_rmat_len_p(*s, *sz, start_idx + (size_t)rm.rm_so, (size_t)(rm.rm_eo - rm.rm_so)), *s);
		return 1;
	} else if (ret == JSTR_RE_RET_NOMATCH) {
		return 0;
	}
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rm_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rm_from_exec(preg, s, sz, cap, 0, eflags);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
static jstr_re_off_ty
jstr_re_rmn_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	regmatch_t rm;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(*s + start_idx);
	const char *const end = *s + *sz;
	if (n == 1)
		return jstr_re_rm_from_exec(preg, s, sz, cap, start_idx, eflags);
	if (jstr_unlikely(n == 0))
		return 0;
	int ret = jstr_re_search_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), &rm, eflags | IS_NOTBOL);
	jstr_re_off_ty changed = 0;
	size_t j;
	size_t find_len;
	if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
		find_len = (size_t)(rm.rm_eo - rm.rm_so);
		i.src_e += rm.rm_so;
		j = JSTR_DIFF(i.src_e, i.src);
		goto start;
	} else if (ret == JSTR_RE_RET_NOMATCH) {
		return 0;
	} else {
		goto err_free;
	}
	for (; n && i.src_e < end; --n, ++changed) {
		ret = jstr_re_search_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), &rm, eflags | IS_NOTBOL_LOOP);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err_free);
		find_len = (size_t)(rm.rm_eo - rm.rm_so);
		i.src_e += rm.rm_so;
		j = JSTR_DIFF(i.src_e, i.src);
		memmove(i.dst, i.src, j);
start:
		i.dst += j;
		i.src += j + find_len;
		i.src_e += find_len;
		if (jstr_unlikely(find_len == 0))
			++i.src_e;
	}
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), *s);
	return changed;
err_free:
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rmn_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, 0, eflags, n);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rmall_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, 0, eflags, (size_t)-1);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rmall_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, start_idx, eflags, (size_t)-1);
}

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr__rplcallsmallerrplc(char *s, size_t *R sz, jstr__inplace_ty *R const i, const char *R rplc, size_t rplc_len, size_t find_len)
{
	if (i->dst != i->src) {
		i->dst = (char *)
		jstr_mempmove(i->dst, i->src, JSTR_DIFF(i->src_e, i->src));
		jstr_strmove_len(i->dst + rplc_len, i->src_e + find_len, JSTR_DIFF(s + *sz, i->src_e + find_len));
		i->dst = (char *)jstr_mempcpy(i->dst, rplc, rplc_len);
		i->src = i->dst;
	} else {
		jstr_strmove_len(i->src_e + rplc_len, i->src_e + find_len, JSTR_DIFF(s + *sz, i->src_e + find_len));
		memcpy(i->src_e, rplc, rplc_len);
	}
	*sz += rplc_len - find_len;
	i->src_e += rplc_len;
}

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr__rplcallbiggerrplc(char *R *R s, size_t *R sz, size_t *R cap, jstr__inplace_ty *R const i, const char *R rplc, size_t rplc_len, size_t find_len)
{
	if (i->dst != i->src)
		memmove(i->dst, i->src, JSTR_DIFF(i->src_e, i->src));
	if (*cap <= *sz + rplc_len - find_len) {
		const uintptr_t tmp = (uintptr_t)*s;
		if (jstr_chk(jstr_reservealways(s, sz, cap, *sz + rplc_len - find_len + 1)))
			return JSTR_RET_ERR;
		i->src_e = *s + JSTR_DIFF(i->src_e, tmp);
		i->dst = *s + JSTR_DIFF(i->dst, tmp);
	}
	jstr_strmove_len(i->src_e + rplc_len, i->src_e + find_len, JSTR_DIFF(*s + *sz, i->src_e + find_len));
	i->src_e = (char *)jstr_mempcpy(i->src_e, rplc, rplc_len);
	i->dst += rplc_len;
	i->src = i->dst;
	*sz += rplc_len - find_len;
	return JSTR_RET_SUCC;
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_ATTR_INLINE
JSTR_FUNC
static jstr_re_off_ty
jstr_re_rplc_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	int ret = jstr_re_search_len(preg, *s + start_idx, *sz - start_idx, &rm, eflags | IS_NOTBOL);
	if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
		if (jstr_likely(jstr_rplcat_len(s, sz, cap, start_idx + (size_t)rm.rm_so, rplc, rplc_len, (size_t)(rm.rm_eo - rm.rm_so)) != NULL))
			return 1;
		ret = JSTR_RE_RET_ESPACE;
	} else if (ret == JSTR_RE_RET_NOMATCH) {
		return 0;
	}
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_ATTR_INLINE
JSTR_FUNC
static jstr_re_off_ty
jstr_re_rplc_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplc_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
static jstr_re_off_ty
jstr_re_rplcn_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from_exec(preg, s, sz, cap, start_idx, eflags, n);
	if (n == 1)
		return jstr_re_rplc_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags);
	if (jstr_unlikely(n == 0))
		return 0;
	jstr__inplace_ty i;
	i.src_e = *s + start_idx;
	regmatch_t rm;
	const char *end = *s + *sz;
	int ret = jstr_re_search_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), &rm, eflags | IS_NOTBOL);
	if (jstr_unlikely(ret == JSTR_RE_RET_NOMATCH))
		return 0;
	if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR)) {
err:
		jstr_free_noinline(s, sz, cap);
		JSTR_RE_RETURN_ERR(ret, preg);
	}
	jstr_re_off_ty find_len = rm.rm_eo - rm.rm_so;
	i.src_e += rm.rm_so;
	size_t j;
	jstr_re_off_ty changed = 0;
	/* DST and SRC exist in the same buffer *S, where SRC + NUL is followed by DST.
	 * Allocate enough memory for all of them and move back DST. */
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * 2 + rplc_len - (size_t)find_len + 1)))
		goto err;
	memmove(*s + *sz + 1, *s, *sz);
	i.dst = *s + *sz + 1;
	const char *dst_s = i.dst;
	i.src = *s;
	i.src_e = *s + start_idx + rm.rm_so;
	end = *s + *sz;
	goto start;
	for (; n && i.src_e < end; --n, ++changed) {
		ret = jstr_re_search_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), &rm, eflags | IS_NOTBOL_LOOP);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err);
		find_len = rm.rm_eo - rm.rm_so;
		i.src_e += rm.rm_so;
start:
		j = JSTR_DIFF(i.src_e, i.src);
		if (jstr_unlikely(*cap < *sz * 2 + rplc_len - (size_t)find_len + 1)) {
			const uintptr_t tmp = (uintptr_t)*s;
			if (jstr_chk(jstr_reservealways(s, sz, cap, *sz * 2 + rplc_len - (size_t)find_len + 1))) {
				ret = JSTR_RE_RET_ESPACE;
				goto err;
			}
			i.src = *s + JSTR_DIFF(i.src, tmp);
			i.src_e = *s + JSTR_DIFF(i.src_e, tmp);
			i.dst = *s + JSTR_DIFF(i.dst, tmp);
			dst_s = *s + JSTR_DIFF(dst_s, tmp);
			end = *s + JSTR_DIFF(end, tmp);
			dst_s = *s + JSTR_DIFF(dst_s, tmp);
		}
		memmove(i.dst, i.src, j);
		i.dst = (char *)jstr_mempcpy(i.dst + j, rplc, rplc_len);
		i.src += j + (size_t)find_len;
		i.src_e += find_len;
		if (jstr_unlikely(find_len == 0))
			++i.src_e;
	}
	*sz = JSTR_DIFF(jstr_mempmove(i.dst, i.src, JSTR_DIFF(end, i.src)), dst_s);
	/* Move back DST to the start of the string since we don't need SRC
	 * anymore. */
	jstr_strmove_len(*s, dst_s, *sz);
	return changed;
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcn_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, n);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcall_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, (size_t)-1);
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcall_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, (size_t)-1);
}

#if JSTR_DEBUG
#	define NMATCH_PARAM , size_t nmatch
#	define NMATCH_ARG   , nmatch
#	define NMATCH       nmatch
#else
#	define NMATCH_PARAM
#	define NMATCH_ARG
#	define NMATCH
#endif

/* Return value:
 * length of backref.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr__re_rplcbackrefstrlen(const regmatch_t *R rm, const unsigned char *rplc, const unsigned char *rplc_e, size_t rplc_len NMATCH_PARAM)
JSTR_NOEXCEPT
{
	int c;
	for (; (rplc = (unsigned char *)memchr(rplc, '\\', JSTR_DIFF(rplc_e, rplc))); rplc += 2) {
		/* Trailing backslash error. */
		if (jstr_unlikely(rplc == rplc_e - 1))
			return (size_t)-1;
		c = *(rplc + 1);
		if (jstr_likely(jstr_isdigit(c))) {
			c -= '0';
			JSTR_ASSERT_DEBUG((size_t)c < nmatch, "Using a backref higher than nmatch.");
			rplc_len += (size_t)(rm[c].rm_eo - rm[c].rm_so - 2);
		}
	}
	return rplc_len;
}

/* Return ptr to first backref. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr__re_rplcbackreffirst(const char *backref, size_t backref_len)
{
	if (jstr_unlikely(backref_len < 2))
		return NULL;
	for (const char *backref_e = backref + backref_len - 1; (backref = (const char *)memchr(backref, '\\', JSTR_DIFF(backref_e, backref))) && !jstr_isdigit(*(backref + 1)); backref += 2) {}
	return (char *)backref;
}

/* Return ptr to the end of the last backref. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr__re_rplcbackreflast(const unsigned char *backref, size_t backref_len)
{
	if (backref_len >= 4) {
		backref += 2;
		const unsigned char *p;
		const unsigned char *end = backref + backref_len - 1;
		for (;; --end) {
			p = (const unsigned char *)jstr_memrchr(backref, '\\', JSTR_DIFF(end, backref));
			if (jstr_unlikely(p == NULL))
				break;
			if (jstr_isdigit(*(p + 1)))
				return (char *)p + 2;
		}
	}
	return NULL;
}

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr__re_rplcbackrefcreat(const unsigned char *R mtc, const regmatch_t *R rm, unsigned char *R backref, const unsigned char *R rplc, const unsigned char *rplc_e)
JSTR_NOEXCEPT
{
	int c;
	const unsigned char *rplc_o = rplc;
	for (;; rplc += 2) {
		rplc_o = rplc;
		rplc = (unsigned char *)memchr(rplc, '\\', JSTR_DIFF(rplc_e, rplc));
		if (jstr_nullchk(rplc))
			break;
		/* We've checked that there's no trailing backslash. */
		/* if (rplc == rplc_e - 1)
		     return (size_t)-1; */
		backref = (unsigned char *)jstr_mempcpy(backref, rplc_o, JSTR_DIFF(rplc, rplc_o));
		c = *(rplc + 1);
		if (jstr_likely(jstr_isdigit(c))) {
			c -= '0';
			backref = (unsigned char *)jstr_mempmove(backref, mtc + rm[c].rm_so, (size_t)(rm[c].rm_eo - rm[c].rm_so));
		} else {
			*backref = '\\';
			*(backref + 1) = c;
			backref += 2;
		}
	}
	memcpy(backref, rplc_o, JSTR_DIFF(rplc_e, rplc_o));
}

/* Use rplc/rm instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
static jstr_re_off_ty
jstr_re_rplcn_backref_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from_exec(preg, s, sz, cap, start_idx, eflags, n);
	if (jstr_unlikely(n == 0))
		return 0;
	/* Check if we have backrefs in RPLC. */
	const unsigned char *rplc_backref1 = (const unsigned char *)jstr__re_rplcbackreffirst(rplc, rplc_len); /* Cache the first backref. */
	/* If not, fallback to re_rplcn_len. */
	if (jstr_nullchk(rplc_backref1))
		return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, n);
	const unsigned char *rplc_backref1_e = (const unsigned char *)jstr__re_rplcbackreflast(rplc_backref1, rplc_len - JSTR_DIFF(rplc_backref1, rplc));
	if (rplc_backref1_e == NULL)
		rplc_backref1_e = rplc_backref1 + 2;
	regmatch_t rm[10];
	jstr__inplace_ty i;
	i.src_e = *s + start_idx;
	const char *end = *s + *sz;
	int ret = jstr_re_exec_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), nmatch, rm, eflags);
	if (jstr_unlikely(ret == JSTR_RE_RET_NOMATCH))
		return 0;
	if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR)) {
err:
		jstr_free_noinline(s, sz, cap);
		JSTR_RE_RETURN_ERR(ret, preg);
	}
	jstr_re_off_ty find_len = rm[0].rm_eo - rm[0].rm_so;
	i.src_e += rm[0].rm_so;
	size_t j;
	jstr_re_off_ty changed = 0;
	size_t rplcwbackref_len = jstr__re_rplcbackrefstrlen(rm, rplc_backref1, rplc_backref1_e, rplc_len NMATCH_ARG);
	i.src_e += rm[0].rm_so;
	/* DST and SRC exist in the same buffer *S, where SRC + NUL is followed by DST.
	 * Allocate enough memory for all of them and move back DST. */
	if (jstr_chk(jstr_reserve(s, sz, cap, *sz * 2 + rplcwbackref_len - (size_t)find_len + 1)))
		goto err;
	memmove(*s + *sz + 1, *s, *sz);
	i.dst = *s + *sz + 1;
	const char *dst_s = i.dst;
	i.src = *s;
	i.src_e = *s + start_idx + rm[0].rm_so;
	end = *s + *sz;
	goto start;
	for (; n && i.src_e < end; --n, ++changed) {
		ret = jstr_re_exec_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), nmatch, rm, eflags);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err);
		find_len = rm[0].rm_eo - rm[0].rm_so;
		i.src_e += rm[0].rm_so;
		rplcwbackref_len = jstr__re_rplcbackrefstrlen(rm, rplc_backref1, rplc_backref1_e, rplc_len NMATCH_ARG);
start:
		j = JSTR_DIFF(i.src_e, i.src);
		if (jstr_unlikely(*cap < *sz * 2 + rplcwbackref_len - (size_t)find_len + 1)) {
			const uintptr_t tmp = (uintptr_t)*s;
			if (jstr_chk(jstr_reserve(s, sz, cap, *sz * 2 + rplcwbackref_len - (size_t)find_len + 1))) {
				ret = JSTR_RE_RET_ESPACE;
				goto err;
			}
			i.src = *s + JSTR_DIFF(i.src, tmp);
			i.src_e = *s + JSTR_DIFF(i.src_e, tmp);
			i.dst = *s + JSTR_DIFF(i.dst, tmp);
			dst_s = *s + JSTR_DIFF(dst_s, tmp);
			end = *s + JSTR_DIFF(end, tmp);
		}
		memmove(i.dst, i.src, j);
		i.dst += j;
		jstr__re_rplcbackrefcreat((unsigned char *)i.src_e - rm[0].rm_so, rm, (unsigned char *)i.dst, (unsigned char *)rplc, (unsigned char *)rplc + rplc_len);
		i.dst += rplcwbackref_len;
		i.src += j + (size_t)find_len;
		i.src_e += find_len;
		if (jstr_unlikely(find_len == 0))
			++i.src_e;
	}
	*sz = JSTR_DIFF(jstr_mempmove(i.dst, i.src, JSTR_DIFF(end, i.src)), dst_s);
	/* Move back DST to the start of the string since we don't need SRC
	 * anymore. */
	jstr_strmove_len(*s, dst_s, *sz);
	return changed;
}

#undef NMATCH
#undef NMATCH_ARG
#undef NMATCH_PARAM

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcall_backref_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcall_backref_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplcn_backref_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, n);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplc_backref_len_from_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, 1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_re_off_ty
jstr_re_rplc_backref_len_exec(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, 1);
}

JSTR__END_DECLS

#undef R
#undef IS_NOTBOL
#undef IS_NOTBOL_LOOP

#endif /* JSTR_REGEX_H */
