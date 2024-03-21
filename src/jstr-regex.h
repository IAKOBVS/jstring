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

/* For jstr_re_rm*(), jstr_re_rplc*(), a negative number will be returned
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
#define jstr_re_chkexec(errcode) \
	(                        \
	jstr_re_chkcomp(errcode) \
	&& jstr_unlikely((errcode) != JSTR_RE_RET_NOMATCH))
#define jstr_re_chk(errcode) jstr_unlikely((errcode) < 0)

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
	return (jstr_re_ret_ty)regcomp(preg, ptn, cflags);
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

/* Estimate the length of a regex pattern. If the pattern contains
 * special characters, return (size_t)-1. */
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static size_t
jstr_re_patternlenmax(const char *pattern)
JSTR_NOEXCEPT
{
	int c;
	const unsigned char *p = (const unsigned char *)pattern;
	int minus = *p == '^';
	/* Quickly find a special character. */
	p = (const unsigned char *)strcspn((const char *)p, "\\*{}");
	if (*p != '\0')
		for (;; ++p)
			switch (c = *p) {
			case '*':
			/* Extended regex. */
			case '{':
			case '}':
				return (size_t)-1;
			case '\\':
				c = *(p + 1);
				if (jstr_isdigit(c) || c == '{' || c == '}')
					return (size_t)-1;
				++p;
			case '\0':
				if (*(p - 1) == '$')
					++minus;
				break;
			}
	return JSTR_PTR_DIFF(p, pattern) - (unsigned int)minus;
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

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmn_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	regmatch_t rm;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(*s + start_idx);
	const char *const end = *s + *sz;
	size_t find_len;
	int ret;
	jstr_re_off_ty changed = 0;
	for (; n && i.src_e < end;) {
		ret = jstr_re_search_len(preg, i.src_e, JSTR_PTR_DIFF(end, i.src_e), &rm, eflags);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err_free);
		find_len = (size_t)(rm.rm_eo - rm.rm_so);
		i.src_e += rm.rm_so;
		JSTR__INPLACE_RMALL(i, find_len);
		++changed;
		--n;
		if (jstr_unlikely(find_len == 0))
			++i.src_e;
	}
	if (i.dst != i.src)
		*sz = JSTR_PTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_PTR_DIFF(end, i.src)), *s);
	return changed;
err_free:
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rmn(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, cap, 0, eflags, n);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rm(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, cap, 0, eflags, 1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rm_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, cap, start_idx, eflags, 1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rmall(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, cap, 0, eflags, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rmall_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, cap, start_idx, eflags, (size_t)-1);
}

/* Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr__rplcallsmallerrplc(char *s, size_t *R sz, char **dst, const char **src, char **src_e, const char *R rplc, size_t rplc_len, size_t find_len)
{
	if (*dst != *src) {
		*dst = (char *)
		jstr_mempmove(*dst, *src, JSTR_PTR_DIFF(*src_e, *src));
		jstr_strmove_len(*dst + rplc_len, *src_e + find_len, JSTR_PTR_DIFF(s + *sz, *src_e + find_len));
		*dst = (char *)jstr_mempcpy(*dst, rplc, rplc_len);
		*src = *dst;
	} else {
		jstr_strmove_len(*src_e + rplc_len, *src_e + find_len, JSTR_PTR_DIFF(s + *sz, *src_e + find_len));
		memcpy(*src_e, rplc, rplc_len);
	}
	*sz += rplc_len - find_len;
	*src_e += rplc_len;
}

/* Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr__rplcallbiggerrplc(char *R *R s, size_t *R sz, size_t *R cap, char **dst, const char **src, char **src_e, const char *R rplc, size_t rplc_len, size_t find_len)
{
	if (*dst != *src)
		memmove(*dst, *src, JSTR_PTR_DIFF(*src_e, *src));
	if (*cap <= *sz + rplc_len - find_len) {
		char *tmp = *s;
		if (jstr_chk(jstr_reservealways(&tmp, sz, cap, *sz + rplc_len - find_len)))
			return JSTR_RET_ERR;
		*src_e = tmp + (*src_e - *s);
		*dst = tmp + (*dst - *s);
		*s = tmp;
	}
	jstr_strmove_len(*src_e + rplc_len, *src_e + find_len, JSTR_PTR_DIFF(*s + *sz, *src_e + find_len));
	*src_e = (char *)jstr_mempcpy(*src_e, rplc, rplc_len);
	*dst += rplc_len;
	*src = *dst;
	*sz += rplc_len - find_len;
	return JSTR_RET_SUCC;
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from(preg, s, sz, cap, start_idx, eflags, n);
	jstr_re_off_ty find_len;
	regmatch_t rm;
	int ret;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(*s + start_idx);
	jstr_re_off_ty changed = 0;
	for (; n-- && i.src_e < *s + *sz; ++changed) {
		ret = jstr_re_search_len(preg, i.src_e, JSTR_PTR_DIFF(*s + *sz, i.src_e), &rm, eflags);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err);
		find_len = rm.rm_eo - rm.rm_so;
		i.src_e += rm.rm_so;
		if (rplc_len <= (size_t)find_len) {
			JSTR__INPLACE_RPLCALL(i, rplc, rplc_len, (size_t)find_len);
		} else {
			if (*cap > *sz + rplc_len - (size_t)find_len) {
				jstr__rplcallsmallerrplc(*s, sz, &i.dst, &i.src, &i.src_e, rplc, rplc_len, (size_t)find_len);
			} else if (jstr_chk(jstr__rplcallbiggerrplc(s, sz, cap, &i.dst, &i.src, &i.src_e, rplc, rplc_len, (size_t)find_len))) {
				ret = JSTR_RE_RET_ESPACE;
				goto err;
			}
			if (jstr_unlikely(find_len == 0))
				++i.src_e;
		}
	}
	if (i.dst != i.src)
		*sz = JSTR_PTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_PTR_DIFF(*s + *sz, i.src)), *s);
	return changed;
err:
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcn_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, n);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcall_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcall_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_ATTR_INLINE
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, 1);
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_ATTR_INLINE
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, 1);
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
 * on error, -errcode (negative);
 * length of backreference. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
jstr__re_brefstrlen(const regmatch_t *R rm, const char *R rplc, size_t rplc_len NMATCH_PARAM)
JSTR_NOEXCEPT
{
	const char *const rplc_e = rplc + rplc_len;
	int c;
	for (; (rplc = (char *)memchr(rplc, '\\', JSTR_PTR_DIFF(rplc_e, rplc))); rplc += 2) {
		c = *(unsigned char *)(rplc + 1);
		if (jstr_likely(jstr_isdigit(c))) {
			c -= '0';
			JSTR_ASSERT_DEBUG((size_t)c < nmatch, "Using a backreference higher than nmatch.");
			rplc_len += (size_t)(rm[c].rm_eo - rm[c].rm_so - 2);
			/* We don't need this because we've checked
			 * that the pattern does not end with a backslash. */
		} /* else if (c == '\0')
		     return (size_t)-1; */
	}
	return rplc_len;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr__re_breffirst(const char *bref, size_t bref_len)
{
	/* We've checked that the pattern
	 * does not end with a backslash. */
	for (const char *bref_e = bref + bref_len; (bref = (const char *)memchr(bref, '\\', JSTR_PTR_DIFF(bref_e, bref))) && !jstr_isdigit(*(bref + 1)); bref += 2) {}
	return (char *)bref;
}

#if 0 /* unused */

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr__re_breflast(const char *bref, size_t bref_len)
{
	if (bref_len >= 4) {
		bref += 2;
		bref_len -= 2;
		const char *p;
		const char *end = bref + bref_len - 1;
		for (;; --end) {
			p = (const char *)jstr_memrchr(bref, '\\', JSTR_PTR_DIFF(end, bref));
			if (jstr_unlikely(p == NULL))
				break;
			if (jstr_isdigit(*(p + 1)))
				return (char *)p + 2;
		}
	}
	return NULL;
}

#endif

/* Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr__re_brefcreat(const char *R mtc, const regmatch_t *R rm, char *R bref, const char *R rplc, size_t rplc_len)
JSTR_NOEXCEPT
{
	int c0, c1;
	for (;;) {
		c0 = *(unsigned char *)rplc;
		if (c0 == '\\') {
			c1 = *(unsigned char *)(rplc + 1);
			if (jstr_isdigit(c1)) {
				c1 -= '0';
				bref = (char *)jstr_mempcpy(bref, mtc + rm[c1].rm_so, rm[c1].rm_eo - rm[c1].rm_so);
				rplc += 2;
			} else {
				/* We don't need this because we've checked
				 * that the pattern does not end with a backslash.
				if (c == '\0')
				        return JSTR_RET_ERR; */
				*bref = c0;
				*(bref + 1) = c1;
				bref += 2;
				rplc += 2;
			}
		} else {
			if (jstr_unlikely(c0 == '\0'))
				break;
			*bref++ = c0;
			++rplc;
		}
	}
	return JSTR_RET_SUCC;
	(void)rplc_len;
}

/* Do not pass an anchored pattern (with ^ or $) to rplcn/rplcall/rmn/rmall.
 * Use rplc/rm instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_bref_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(start_idx == 0 || start_idx < *sz, "");
	JSTR_ASSERT_DEBUG(nmatch >= 1, "");
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from(preg, s, sz, cap, start_idx, eflags, n);
	if (jstr_unlikely(n == 0))
		return 0;
	/* Pattern cannot end with a backslash. */
	if (jstr_unlikely(*(rplc + rplc_len - 1) == '\\'))
		JSTR_RE_RETURN_ERR(JSTR_RE_RET_BADPAT, preg);
	/* Check if we have backreferences in RPLC. */
	if (jstr_nullchk(jstr__re_breffirst(rplc, rplc_len)))
		return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, n);
	int ret;
	regmatch_t rm[10];
	size_t bref_len;
	size_t bref_cap = 0;
	enum { BUFSZ = 256 };
	char bref_stack[BUFSZ];
	char *brefp = bref_stack;
	char *bref_heap = NULL;
	jstr_re_off_ty find_len;
	jstr_re_off_ty changed = 0;
	jstr__inplace_ty i = JSTR__INPLACE_INIT(*s + start_idx);
	for (; n-- && i.src_e < *s + *sz; ++changed) {
		ret = jstr_re_exec_len(preg, i.src_e, JSTR_PTR_DIFF(*s + *sz, i.src_e), (size_t)nmatch, rm, eflags);
		JSTR__RE_ERR_EXEC_HANDLE(ret, goto err_free_bref);
		find_len = rm[0].rm_eo - rm[0].rm_so;
		bref_len = jstr__re_brefstrlen(rm, rplc, rplc_len NMATCH_ARG);
		if (jstr_unlikely(bref_len > BUFSZ)) {
			if (bref_cap < bref_len) {
				if (jstr_unlikely(bref_cap == 0))
					bref_cap = BUFSZ;
				bref_cap = jstr__grow(bref_cap, bref_len);
				bref_heap = (char *)realloc(bref_heap, bref_cap);
				if (jstr_nullchk(bref_heap)) {
					ret = JSTR_RE_RET_ESPACE;
					goto err_free;
				}
				brefp = bref_heap;
			}
		}
		if (jstr_chk(jstr__re_brefcreat(i.src_e, rm, brefp, rplc, rplc_len))) {
			ret = JSTR_RE_RET_BADPAT;
			goto err_free_bref;
		}
		i.src_e += rm[0].rm_so;
		find_len = (size_t)(rm[0].rm_eo - rm[0].rm_so);
		if (bref_len <= (size_t)find_len) {
			JSTR__INPLACE_RPLCALL(i, brefp, bref_len, (size_t)find_len);
		} else {
			if (*cap > *sz + bref_len - (size_t)find_len) {
				jstr__rplcallsmallerrplc(*s, sz, &i.dst, &i.src, &i.src_e, brefp, bref_len, (size_t)find_len);
			} else if (jstr_chk(jstr__rplcallbiggerrplc(s, sz, cap, &i.dst, &i.src, &i.src_e, brefp, bref_len, (size_t)find_len))) {
				ret = JSTR_RE_RET_ESPACE;
				goto err_free_bref;
			}
			if (jstr_unlikely(find_len == 0))
				++i.src;
		}
	}
	if (i.dst != i.src)
		*sz = JSTR_PTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_PTR_DIFF(*s + *sz, i.src)), *s);
	free(bref_heap);
	return changed;
err_free_bref:
	free(bref_heap);
err_free:
	jstr_free_noinline(s, sz, cap);
	JSTR_RE_RETURN_ERR(ret, preg);
}

#undef NMATCH
#undef NMATCH_ARG
#undef NMATCH_PARAM

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcall_bref_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcall_bref_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplcn_bref_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, n);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplc_bref_len_from(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, 1);
}

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * on error, -errcode (negative);
 * number of substrings replaced. */
JSTR_FUNC
JSTR_ATTR_INLINE
jstr_re_off_ty
jstr_re_rplc_bref_len(const regex_t *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, 1);
}

JSTR__END_DECLS

#undef R

#endif /* JSTR_REGEX_H */
