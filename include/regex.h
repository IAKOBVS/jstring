/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* For jstr_re_rm_exec*, jstr_re_rplc_exec*, a negative number will be returned
 * indicating the negated value of the regex error code. To print an
 * error message, pass the negation of the returned error code. */

#ifndef JSTR_REGEX_H
#	define JSTR_REGEX_H 1

#	include "macros.h"
#	include "struct.h"

JSTR_INTERNAL_BEGIN_DECLS
#	include <regex.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <string.h>
JSTR_INTERNAL_END_DECLS

#	ifdef JSTR_IMPLEMENTATION
#		undef JSTR_IMPLEMENTATION
#		include "ctype.h"
#		include "stdstring.h"
#		include "builder.h"
#		include "config.h"
#		include "replace.h"
#		include "string.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

#	define jstr_re_chkcomp(errcode) jstr_unlikely((errcode) != JSTR_RE_RET_NOERROR)
#	define jstr_re_chkexec(errcode) (jstr_re_chkcomp(errcode) && jstr_unlikely((errcode) != JSTR_RE_RET_NOMATCH))
#	define jstr_re_chk(errcode)     jstr_unlikely((errcode) < 0)

#	define R JSTR_RESTRICT

/* POSIX cflags */
#	define JSTR_RE_CF_EXTENDED REG_EXTENDED
#	define JSTR_RE_CF_ICASE    REG_ICASE
#	define JSTR_RE_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#	define JSTR_RE_EF_NOSUB  REG_NOSUB
#	define JSTR_RE_EF_NOTBOL REG_NOTBOL
#	define JSTR_RE_EF_NOTEOL REG_NOTEOL

/* BSD eflags */
#	ifdef REG_STARTEND
#		define JSTR_RE_EF_STARTEND REG_STARTEND
#	endif

JSTR_INTERNAL_BEGIN_DECLS

typedef enum {
#	ifdef REG_ENOSYS
	JSTR_RE_RET_ENOSYS = REG_ENOSYS,
#	endif
#	if defined REG_NOERROR
	JSTR_RE_RET_NOERROR = REG_NOERROR,
#	else
	JSTR_RE_RET_NOERROR = 0,
#	endif
#	define JSTR_RE_RET_NOERROR JSTR_RE_RET_NOERROR
	JSTR_RE_RET_NOMATCH = REG_NOMATCH,
#	define JSTR_RE_RET_NOMATCH JSTR_RE_RET_NOMATCH
	/* POSIX regcomp return values */
	JSTR_RE_RET_BADPAT = REG_BADPAT,
#	define JSTR_RE_RET_BADPAT JSTR_RE_RET_BADPAT
	JSTR_RE_RET_ECOLLATE = REG_ECOLLATE,
#	define JSTR_RE_RET_ECOLLATE JSTR_RE_RET_ECOLLATE
	JSTR_RE_RET_ECTYPE = REG_ECTYPE,
#	define JSTR_RE_RET_ECTYPE JSTR_RE_RET_ECTYPE
	JSTR_RE_RET_EESCAPE = REG_EESCAPE,
#	define JSTR_RE_RET_EESCAPE JSTR_RE_RET_EESCAPE
	JSTR_RE_RET_ESUBREG = REG_ESUBREG,
#	define JSTR_RE_RET_ESUBREG JSTR_RE_RET_ESUBREG
	JSTR_RE_RET_EBRACK = REG_EBRACK,
#	define JSTR_RE_RET_EBRACK JSTR_RE_RET_EBRACK
	JSTR_RE_RET_EPAREN = REG_EPAREN,
#	define JSTR_RE_RET_EPAREN JSTR_RE_RET_EPAREN
	JSTR_RE_RET_EBRACE = REG_EBRACE,
#	define JSTR_RE_RET_EBRACE JSTR_RE_RET_EBRACE
	JSTR_RE_RET_BADBR = REG_BADBR,
#	define JSTR_RE_RET_BADBR JSTR_RE_RET_BADBR
	JSTR_RE_RET_ERANGE = REG_ERANGE,
#	define JSTR_RE_RET_ERANGE JSTR_RE_RET_ERANGE
	JSTR_RE_RET_ESPACE = REG_ESPACE,
#	define JSTR_RE_RET_ESPACE JSTR_RE_RET_ESPACE
	JSTR_RE_RET_BADRPT = REG_BADRPT
#	define JSTR_RE_RET_BADRPT JSTR_RE_RET_BADRPT
/* GNU regcomp returns */
#	ifdef REG_EEND
	,
	JSTR_RE_RET_EEND = REG_EEND
#		define JSTR_RE_RET_EEND JSTR_RE_RET_EEND
#	endif
#	ifdef REG_ESIZE
	,
	JSTR_RE_RET_ESIZE = REG_ESIZE
#		define JSTR_RE_RET_ESIZE REG_ESIZE
#	endif
#	ifdef REG_ERPAREN
	,
	JSTR_RE_RET_ERPAREN = REG_ERPAREN
#		define JSTR_RE_RET_ERPAREN REG_ERPAREN
#	endif
} jstr_re_ret_ty;

#	define JSTR_INTERNAL_RE_ERR_EXEC_HANDLE_LOOP(errcode, do_on_error) \
		if (jstr_likely(errcode == JSTR_RE_RET_NOERROR)) {          \
			;                                                   \
		} else if (errcode == JSTR_RE_RET_NOMATCH) {                \
			break;                                              \
		} else {                                                    \
			do_on_error;                                        \
		}

#	if JSTR_PANIC
#		define JSTR_RE_RETURN_ERR(errcode, preg)             \
			do {                                          \
				jstr_err("%s", "");                   \
				jstr_re_err(errcode, preg, "%s", "\n"); \
				return -(errcode);                    \
			} while (0)
#	else
#		define JSTR_RE_RETURN_ERR(errcode, preg) return -(errcode)
#	endif

JSTR_FUNC_PURE
int
jstr_internal_re_notbol(const char *str, size_t curr_idx, int cflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (curr_idx) {
		if (cflags & JSTR_RE_CF_NEWLINE)
			if (*(str + curr_idx - 1) == '\n')
				return 0;
		return JSTR_RE_EF_NOTBOL;
	}
	return 0;
}
#	else
;
#	endif

JSTR_FUNC_PURE
int
jstr_internal_re_notbol_inloop(const char *str, size_t curr_idx, int cflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (cflags & JSTR_RE_CF_NEWLINE && curr_idx > 0)
		if (*(str - 1) == '\n')
			return 0;
	return JSTR_RE_EF_NOTBOL;
}
#	else
;
#	endif

/* Check if *s + start_idx is the beginning of a string or beginning of a line. */
#	define IS_NOTBOL(str, curr_idx, cflags)        jstr_internal_re_notbol(str, curr_idx, cflags)
#	define IS_NOTBOL_INLOOP(str, curr_idx, cflags) jstr_internal_re_notbol_inloop(str, curr_idx, cflags)

typedef regoff_t jstr_re_off_ty;
typedef struct jstr_re_ty {
	regex_t reg;
	int cflags;
} jstr_re_ty;

JSTR_FUNC_VOID
void
jstr_re_free(jstr_re_ty *preg) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	regfree(&preg->reg);
}
#	else
;
#	endif

#	ifdef JSTR_IMPLEMENTATION
JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
static int
jstr_re_verr(jstr_re_ret_ty errcode, const jstr_re_ty *preg, const char *fmt, va_list args) JSTR_NOEXCEPT
{
	char buf[128];
	regerror(errcode, &preg->reg, buf, sizeof(buf));
	int ret;
	ret = fprintf(stderr, "POSIX regex error (%d): %s: ", errcode, buf);
	if (ret < 0)
		return -1;
	ret = vfprintf(stderr, fmt, args);
	return ret > 0 ? 0 : -1;
}
#	endif

JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
JSTR_ATTR_FORMAT(printf, 3, 4)
int
jstr_re_err(jstr_re_ret_ty errcode, const jstr_re_ty *R preg, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list args;
	va_start(args, fmt);
	const int ret = jstr_re_verr(errcode, preg, fmt, args);
	va_end(args);
	return ret < 0 ? -1 : 0;
}
#	else
;
#	endif

JSTR_FUNC_VOID
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
JSTR_ATTR_FORMAT(printf, 3, 4)
void
jstr_re_errdie(jstr_re_ret_ty errcode, const jstr_re_ty *R preg, const char *R fmt, ...) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	va_list args;
	va_start(args, fmt);
	jstr_re_verr(errcode, preg, fmt, args);
	va_end(args);
	exit(EXIT_FAILURE);
}
#	else
;
#	endif

JSTR_FUNC
jstr_re_ret_ty
jstr_re_comp(jstr_re_ty *R preg, const char *R ptn, int cflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	preg->cflags = cflags;
#		if JSTR_PANIC
	const jstr_re_ret_ty ret = (jstr_re_ret_ty)regcomp(&preg->reg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR) && ret != JSTR_RE_RET_NOMATCH)
		jstr_re_errdie(ret, preg, "regcomp(preg, pattern: \"%s\", cflags: %d) failed\n", ptn ? ptn : "(null)", cflags);
	return ret;
#		else
	return (jstr_re_ret_ty)regcomp(&preg->reg, ptn, cflags);
#		endif
}
#	else
;
#	endif

#	ifdef JSTR_IMPLEMENTATION
#		define JSTR_IMPLEMENTATION 1
#	endif

JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_ATTR_WARN_UNUSED
JSTR_FUNC_MAY_NULL
JSTR_ATTR_NOTHROW
jstr_re_ret_ty
jstr_re_exec(const jstr_re_ty *R preg, const char *R s, size_t nmatch, regmatch_t *R pmatch, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		if JSTR_PANIC
	const jstr_re_ret_ty ret = (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags);
	if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR) && ret != JSTR_RE_RET_NOMATCH)
		jstr_re_errdie(ret, preg, "regexec(preg, string: \"%.128s\", nmatch: %zu, pmatch, cflags: %d) failed\n", s ? s : "(null)", nmatch, eflags);
	return ret;
#		else
	return (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags);
#		endif
}
#	else
;
#	endif

JSTR_FUNC
jstr_re_ret_ty
jstr_re_exec_len(const jstr_re_ty *R preg, const char *R s, size_t sz, size_t nmatch, regmatch_t *R pmatch, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		ifdef JSTR_RE_EF_STARTEND
	/* A zero-width REG_STARTEND window makes glibc return NOMATCH even for
	 * epsilon-matching patterns (".*"). Match the NUL-terminated string
	 * without STARTEND instead; S always points at a terminated buffer. */
	if (jstr_unlikely(sz == 0))
		return (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags);
	pmatch->rm_so = 0;
	pmatch->rm_eo = sz;
#		endif
	return (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags | JSTR_RE_EF_STARTEND);
}
#	else
;
#	endif

/* Check if S matches precompiled regex.
 * Return return value of regexec. */
JSTR_FUNC_PURE
jstr_re_ret_ty
jstr_re_match(const jstr_re_ty *R preg, const char *R s, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_exec(preg, s, 0, NULL, eflags);
}
#	else
;
#	endif

/* Search pattern in S.
 * Return return value of regexec.
 * Store offset of matched pattern in pmatch. */
JSTR_FUNC
jstr_re_ret_ty
jstr_re_search(const jstr_re_ty *R preg, const char *R s, regmatch_t *R pmatch, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_exec(preg, s, 1, pmatch, eflags);
}
#	else
;
#	endif

/* Search pattern in S.
 * Return return value of regexec.
 * Store offset of matched pattern in pmatch. */
JSTR_FUNC
jstr_re_ret_ty
jstr_re_search_len(const jstr_re_ty *R preg, const char *R s, size_t sz, regmatch_t *R pmatch, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_exec_len(preg, s, sz, 1, pmatch, eflags);
}
#	else
;
#	endif

/* Check if S matches PTN.
 * Return return value of regexec or regcomp if it fails. */
JSTR_FUNC_PURE
jstr_re_ret_ty
jstr_re_match_len(const jstr_re_ty *R preg, const char *R s, size_t sz, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	regmatch_t rm;
	return jstr_re_exec_len(preg, s, sz, 0, &rm, eflags | JSTR_RE_EF_STARTEND);
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative); */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rm_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	/* Allow start_idx == 0 when *sz == 0 to handle empty string haystacks. */
	if (jstr_unlikely(start_idx > *sz || (start_idx == *sz && start_idx != 0)))
		return 0;
	assert(strlen(*s) == *sz);
	assert(*cap > *sz);
	regmatch_t rm;
	int ret = jstr_re_search_len(preg, *s + start_idx, *sz - start_idx, &rm, eflags | IS_NOTBOL(*s, start_idx, preg->cflags));
	if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
		*sz = JSTR_DIFF(jstr_rmat_len_p(*s, *sz, start_idx + (size_t)rm.rm_so, (size_t)(rm.rm_eo - rm.rm_so)), *s);
		return 1;
	} else if (ret == JSTR_RE_RET_NOMATCH) {
		return 0;
	}
	JSTR_RE_RETURN_ERR(ret, preg);
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rm_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rm_from_exec(preg, s, sz, cap, 0, eflags);
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmn_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	/* Allow start_idx == 0 when *sz == 0 to handle empty string haystacks. */
	if (jstr_unlikely(start_idx > *sz || (start_idx == *sz && start_idx != 0)))
		return 0;
	regmatch_t rm;
	/* i holds cursors for dst (writing cursor), src (copy cursor), and src_e (search cursor). */
	jstr_internal_inplace_ty i = JSTR_INTERNAL_INPLACE_INIT(*s + start_idx);
	const char *end = *s + *sz;
	if (jstr_unlikely(n == 0))
		return 0;
	/* If empty string haystack, process at max once. */
	if (jstr_unlikely(*sz == 0))
		n = 1;
	/* Fall back to single removal if n == 1. */
	if (n == 1)
		return jstr_re_rm_from_exec(preg, s, sz, cap, start_idx, eflags);
	jstr_re_off_ty changed = 0;
	int ret;
	/* Flag tracking if the previous match was zero-length. */
	int prev_zero = 1;
	while (n) {
		/* Track if we are starting search from the end of the string. */
		int matched_at_end = (i.src_e == end);
		if (matched_at_end) {
			/* If the previous match was not zero-length, stop. */
			if (!prev_zero)
				break;
		} else if (i.src_e > end) {
			/* Stop if search cursor went past end. */
			break;
		}
		/* Compute NOTBOL flag dynamically based on current search position. */
		const int eflags_curr = eflags | IS_NOTBOL(*s, JSTR_DIFF(i.src_e, *s), preg->cflags);
		ret = jstr_re_search_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), &rm, eflags_curr);
		if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
			const size_t find_len = (size_t)(rm.rm_eo - rm.rm_so);
			/* Copy unmatched prefix up to the start of the current match. */
			const size_t prev_len_total = JSTR_DIFF(i.src_e + rm.rm_so, i.src);
			if (prev_len_total > 0) {
				memmove(i.dst, i.src, prev_len_total);
				i.dst += prev_len_total;
			}
			--n;
			++changed;
			/* Set copy pointer to end of the match. */
			i.src = i.src_e + rm.rm_eo;
			/* To avoid infinite loops on empty/zero-length matches, advance search past 1 char. */
			if (jstr_unlikely(find_len == 0)) {
				if (i.src < end) {
					*i.dst = *i.src;
					++i.dst;
					++i.src;
				}
			}
			/* Set next search pointer. */
			i.src_e = (char *)i.src;
			/* If we matched at end, stop immediately. */
			if (matched_at_end)
				break;
			prev_zero = (find_len == 0);
		} else if (ret == JSTR_RE_RET_NOMATCH) {
			break;
		} else {
			JSTR_RE_RETURN_ERR(ret, preg);
		}
	}
	/* Copy any remaining unmatched tail portion. */
	*sz = JSTR_DIFF(jstr_stpmove_len(i.dst, i.src, JSTR_DIFF(end, i.src)), *s);
	return changed;
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmn_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, 0, eflags, n);
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmall_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, 0, eflags, (size_t)-1);
}
#	else
;
#	endif

/* Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmall_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rmn_from_exec(preg, s, sz, cap, start_idx, eflags, (size_t)-1);
}
#	else
;
#	endif

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	/* Allow start_idx == 0 when *sz == 0 to handle empty string haystacks. */
	if (jstr_unlikely(start_idx > *sz || (start_idx == *sz && start_idx != 0)))
		return 0;
	regmatch_t rm;
	int ret = jstr_re_search_len(preg, *s + start_idx, *sz - start_idx, &rm, eflags | IS_NOTBOL(*s, start_idx, preg->cflags));
	if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
		if (jstr_likely(jstr_rplcat_len(s, sz, cap, start_idx + (size_t)rm.rm_so, rplc, rplc_len, (size_t)(rm.rm_eo - rm.rm_so)) != NULL))
			return 1;
		ret = JSTR_RE_RET_ESPACE;
	} else if (ret == JSTR_RE_RET_NOMATCH) {
		return 0;
	}
	JSTR_RE_RETURN_ERR(ret, preg);
}
#	else
;
#	endif

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplc_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags);
}
#	else
;
#	endif

/* NMATCH is always passed: the backref engine needs it at runtime to
 * reject references beyond nmatch (not just under JSTR_DEBUG). */
#	define NMATCH_PARAM , size_t nmatch
#	define NMATCH_ARG   , nmatch
#	define NMATCH       nmatch

/* Return value:
 * length of backref.
 * On error, -errcode (negative). */
JSTR_FUNC
size_t
jstr_internal_re_rplcbackrefstrlen(const regmatch_t *R rm, const unsigned char *rplc, const unsigned char *rplc_e, size_t rplc_len NMATCH_PARAM) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	int c;
	for (; (rplc = (unsigned char *)memchr(rplc, '\\', JSTR_DIFF(rplc_e, rplc))); rplc += 2) {
		/* Trailing backslash error. */
		if (jstr_unlikely(rplc == rplc_e - 1))
			return (size_t)-1;
		c = *(rplc + 1);
		if (jstr_likely(jstr_isdigit(c))) {
			c -= '0';
			/* Invalid reference (out of nmatch range or group did not
			 * participate): the pair stays a literal 2-byte "\d". */
			if (jstr_unlikely((size_t)c >= nmatch || rm[c].rm_so < 0))
				continue;
			JSTR_ASSERT_DEBUG((size_t)c < nmatch, "Using a backref higher than nmatch.");
			rplc_len += (size_t)(rm[c].rm_eo - rm[c].rm_so - 2);
		}
	}
	return rplc_len;
}
#	else
;
#	endif

/* Return ptr to first backref. */
JSTR_FUNC
char *
jstr_internal_re_rplcbackreffirst(const char *backref, size_t backref_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(backref_len < 2))
		return NULL;
	for (const char *backref_e = backref + backref_len - 1; (backref = (const char *)memchr(backref, '\\', JSTR_DIFF(backref_e, backref))) && !jstr_isdigit(*(backref + 1)); backref += 2) {}
	return (char *)backref;
}
#	else
;
#	endif

/* Return ptr to the end of the last backref. */
JSTR_FUNC
char *
	jstr_internal_re_rplcbackreflast(const unsigned char *backref, size_t backref_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	const unsigned char *e = backref + backref_len;
	if (backref_len >= 4) {
		const unsigned char *s0 = backref + 2;
		const unsigned char *p;
		for (;;) {
			if (e <= s0)
				break;
			p = (const unsigned char *)jstr_memrchr(s0, '\\', JSTR_DIFF(e, s0));
			if (p == NULL)
				break;
			/* Only a '\digit' pair fully inside the buffer counts;
			 * never peek at *(p + 1) when p is the last byte. */
			if ((size_t)JSTR_DIFF(e, p) >= 2 && jstr_isdigit(*(p + 1)))
				return (char *)p + 2;
			e = p;
		}
	}
	return NULL;
}
#	else
;
#	endif

/* Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC_VOID
void
jstr_internal_re_rplcbackrefcpy(const regmatch_t *R rm, const unsigned char *mtc_src, unsigned char *R mtc_dst, const unsigned char *R rplc, const unsigned char *rplc_e NMATCH_PARAM) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	int c;
	const unsigned char *rplc_o = rplc;
	for (;; rplc += 2) {
		rplc_o = rplc;
		rplc = (unsigned char *)memchr(rplc, '\\', JSTR_DIFF(rplc_e, rplc));
		if (jstr_nullchk(rplc))
			break;
		mtc_dst = (unsigned char *)jstr_mempcpy(mtc_dst, rplc_o, JSTR_DIFF(rplc, rplc_o));
		/* Trailing lone backslash: emit it literally. */
		if (jstr_unlikely(rplc == rplc_e - 1)) {
			*mtc_dst++ = '\\';
			rplc_o = rplc_e;
			break;
		}
		c = *(rplc + 1);
		if (jstr_likely(jstr_isdigit(c))) {
			c -= '0';
			/* Invalid reference (out of nmatch range or group did not
			 * participate): emit the pair literally. Must match the
			 * length accounting in rplcbackrefstrlen above. */
			if (jstr_unlikely((size_t)c >= nmatch || rm[c].rm_so < 0)) {
				*mtc_dst++ = '\\';
				*mtc_dst++ = (unsigned char)c + '0';
				rplc_o = rplc;
				continue;
			}
			mtc_dst = (unsigned char *)jstr_mempmove(mtc_dst, mtc_src + rm[c].rm_so, (size_t)(rm[c].rm_eo - rm[c].rm_so));
		} else {
			*mtc_dst = '\\';
			*(mtc_dst + 1) = c;
			mtc_dst += 2;
		}
	}
	memcpy(mtc_dst, rplc_o, JSTR_DIFF(rplc_e, rplc_o));
}
#	else
;
#	endif

/* Avoid doing O(n) replacements as does rplcn, since doing O(2 * n) regex
 * matching, which is used to find the new size of the string, is likely to
 * dominate over O(n^2) replacements. Given the growth factor, the allocation
 * should be amortized. */
JSTR_FUNC
jstr_re_off_ty
jstr_internal_re_rplcn_backref_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n, int backref) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(n == 0))
		return 0;
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from_exec(preg, s, sz, cap, start_idx, eflags, n);
	/* Allow start_idx == 0 when *sz == 0 to handle empty string haystacks. */
	if (jstr_unlikely(start_idx > *sz || (start_idx == *sz && start_idx != 0)))
		return 0;
	/* If empty string, process at max once. */
	if (jstr_unlikely(*sz == 0))
		n = 1;
	const unsigned char *rplc_backref1;
	const unsigned char *rplc_backref1_e;
	if (backref) {
		rplc_backref1 = (const unsigned char *)jstr_internal_re_rplcbackreffirst(rplc, rplc_len);
		if (jstr_nullchk(rplc_backref1)) {
			backref = 0;
			goto check;
		} else {
			rplc_backref1_e = (const unsigned char *)jstr_internal_re_rplcbackreflast(rplc_backref1, rplc_len - JSTR_DIFF(rplc_backref1, rplc));
			if (rplc_backref1_e == NULL)
				rplc_backref1_e = rplc_backref1 + 2;
		}
	} else {
check:
		if (n == 1 || *sz == 0)
			return jstr_re_rplc_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags);
	}
	regmatch_t rm[10];
	/* i holds dst (built destination cursor), src (source cursor), and src_e (search cursor). */
	jstr_internal_inplace_ty i;
	size_t new_cap = *sz * 2 + 2;
	if (jstr_chk(jstr_reserve(s, sz, cap, new_cap))) {
		JSTR_RE_RETURN_ERR(JSTR_RE_RET_ESPACE, preg);
	}
	/* Move source string to higher part of buffer so we can build destination safely in lower part. */
	memmove(*s + *sz + 1, *s, *sz);
	i.dst = *s + *sz + 1;
	const char *dst_s = i.dst;
	i.src = *s;
	i.src_e = *s + start_idx;
	const char *end = *s + *sz;
	jstr_re_off_ty changed = 0;
	size_t rplcwbackref_len;
	int ret;
	/* Flag tracking if the previous match was zero-length. */
	int prev_zero = 1;
	while (n) {
		/* Track if we are starting search from the end of the string. */
		int matched_at_end = (i.src_e == end);
		if (matched_at_end) {
			/* If the previous match was not zero-length, stop. */
			if (!prev_zero)
				break;
		} else if (i.src_e > end) {
			/* Stop if search cursor went past end. */
			break;
		}
		/* Compute NOTBOL flag dynamically based on current search position. */
		const int eflags_curr = eflags | IS_NOTBOL(*s, JSTR_DIFF(i.src_e, *s), preg->cflags);
		ret = jstr_re_exec_len(preg, i.src_e, JSTR_DIFF(end, i.src_e), nmatch, rm, eflags_curr);
		if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
			const size_t find_len = (size_t)(rm[0].rm_eo - rm[0].rm_so);
			if (backref)
				rplcwbackref_len = jstr_internal_re_rplcbackrefstrlen(rm, rplc_backref1, rplc_backref1_e, rplc_len NMATCH_ARG);
			else
				rplcwbackref_len = rplc_len;
			/* A length error must never flow into the capacity math below. */
			if (jstr_unlikely(rplcwbackref_len == (size_t)-1)) {
				ret = JSTR_RE_RET_ESPACE;
				JSTR_RE_RETURN_ERR(ret, preg);
			}
			/* Check and reserve capacity if needed. */
			new_cap = JSTR_DIFF(i.dst, *s) + JSTR_DIFF(end, i.src) + 2;
			if (rplcwbackref_len > find_len)
				new_cap += rplcwbackref_len - find_len;
			if (jstr_unlikely(*cap < new_cap)) {
				const uintptr_t tmp = (uintptr_t)*s;
				const size_t saved_sz = *sz;
				*sz = JSTR_DIFF(i.dst, *s);
				if (jstr_chk(jstr_reserve(s, sz, cap, new_cap))) {
					ret = JSTR_RE_RET_ESPACE;
					JSTR_RE_RETURN_ERR(ret, preg);
				}
				*sz = saved_sz;
				i.src = *s + JSTR_DIFF(i.src, tmp);
				i.src_e = *s + JSTR_DIFF(i.src_e, tmp);
				i.dst = *s + JSTR_DIFF(i.dst, tmp);
				dst_s = *s + JSTR_DIFF(dst_s, tmp);
				end = *s + JSTR_DIFF(end, tmp);
			}
			/* Copy unmatched prefix up to the start of the current match. */
			const size_t prev_len = JSTR_DIFF(i.src_e + rm[0].rm_so, i.src);
			if (prev_len > 0) {
				memmove(i.dst, i.src, prev_len);
				i.dst += prev_len;
			}
			/* Copy replacement string (handling backreferences). */
			if (backref) {
				jstr_internal_re_rplcbackrefcpy(rm, (unsigned char *)i.src_e, (unsigned char *)i.dst, (unsigned char *)rplc, (unsigned char *)rplc + rplc_len NMATCH_ARG);
				i.dst += rplcwbackref_len;
			} else {
				i.dst = (char *)jstr_mempcpy(i.dst, rplc, rplc_len);
			}
			--n;
			++changed;
			/* Set copy pointer to end of the match. */
			i.src = i.src_e + rm[0].rm_eo;
			/* To avoid infinite loops on empty/zero-length matches, advance search past 1 char. */
			if (jstr_unlikely(find_len == 0)) {
				if (i.src < end) {
					*i.dst = *i.src;
					++i.dst;
					++i.src;
				}
			}
			/* Set next search pointer. */
			i.src_e = (char *)i.src;
			/* If we matched at end, stop immediately. */
			if (matched_at_end)
				break;
			prev_zero = (find_len == 0);
		} else if (ret == JSTR_RE_RET_NOMATCH) {
			break;
		} else {
			JSTR_RE_RETURN_ERR(ret, preg);
		}
	}
	/* Copy any remaining unmatched tail portion. */
	*sz = JSTR_DIFF(jstr_mempmove(i.dst, i.src, JSTR_DIFF(end, i.src)), dst_s);
	/* Move built string back to the start of s. */
	jstr_strmove_len(*s, dst_s, *sz);
	return changed;
}
#	else
;
#	endif

#	undef NMATCH
#	undef NMATCH_ARG
#	undef NMATCH_PARAM

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, 1, n, 0);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, n);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcall_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, (size_t)-1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcall_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, (size_t)-1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_backref_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_internal_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, n, 1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcall_backref_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcall_backref_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, (size_t)-1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplcn_backref_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, n);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_backref_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, 1);
}
#	else
;
#	endif

/* PREG must be precompiled with jstr_re_comp.
 * Do not pass an anchored pattern (with ^ or $) to rmn/rmall/rplcn/rplcall.
 * Use rm/rplc instead.
 * Return value:
 * number of substrings replaced.
 * On error, -errcode (negative). */
JSTR_FUNC
jstr_re_off_ty
jstr_re_rplc_backref_len_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	return jstr_re_rplcn_backref_len_from_exec(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, 1);
}
#	else
;
#	endif

JSTR_INTERNAL_END_DECLS

#	undef R
#	undef IS_NOTBOL

#endif /* JSTR_REGEX_H */
