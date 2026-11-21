

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
#		include "internal/inplace.h"
#		include "string.h"
#		define JSTR_IMPLEMENTATION 1
#	endif

#	define jstr_re_chkcomp(errcode) jstr_unlikely((errcode) != JSTR_RE_RET_NOERROR)
#	define jstr_re_chkexec(errcode) (jstr_re_chkcomp(errcode) && jstr_unlikely((errcode) != JSTR_RE_RET_NOMATCH))
#	define jstr_re_chk(errcode)     jstr_unlikely((errcode) < 0)

#	define R JSTR_RESTRICT

#	define JSTR_RE_CF_EXTENDED REG_EXTENDED
#	define JSTR_RE_CF_ICASE    REG_ICASE
#	define JSTR_RE_CF_NEWLINE  REG_NEWLINE

#	define JSTR_RE_EF_NOSUB  REG_NOSUB
#	define JSTR_RE_EF_NOTBOL REG_NOTBOL
#	define JSTR_RE_EF_NOTEOL REG_NOTEOL

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

/** Maximum number of match slots used by the backref replace helpers
 *  (backreferences are limited to \0..\9). */
#	define JSTR_RE_NMATCH_MAX 10

#	define JSTR_INTERNAL_RE_ERR_EXEC_HANDLE_LOOP(errcode, do_on_error) \
		if (jstr_likely(errcode == JSTR_RE_RET_NOERROR)) {          \
			;                                                   \
		} else if (errcode == JSTR_RE_RET_NOMATCH) {                \
			break;                                              \
		} else {                                                    \
			do_on_error;                                        \
		}

#	if JSTR_PANIC
#		define JSTR_RE_RETURN_ERR(errcode, preg)               \
			do {                                            \
				jstr_err("%s", "");                     \
				jstr_re_err(errcode, preg, "%s", "\n"); \
				return -(errcode);                      \
			} while (0)
#	else
#		define JSTR_RE_RETURN_ERR(errcode, preg) return -(errcode)
#	endif

/**
 * @brief jstr_internal_re_notbol
 *
 * Determine whether the search position `str + curr_idx` should be treated as
 * "not beginning of line" for anchored (`^`) matching, based on `cflags`
 * (JSTR_RE_CF_NEWLINE).
 *
 * @param str const char *str Base string (non-NULL).
 * @param curr_idx size_t curr_idx Byte offset of the current search position.
 * @param cflags int cflags Compilation flags (JSTR_RE_CF_*).
 *
 * @return int 0 if beginning of line/string, else JSTR_RE_EF_NOTBOL.
 *
 * @note Internal helper; assumes `str` non-NULL and `curr_idx` in bounds.
 */
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

/**
 * @brief jstr_internal_re_notbol_inloop
 *
 * In-loop variant of jstr_internal_re_notbol: assumes the search cursor has
 * already advanced; returns NOTBOL unless the preceding byte is a newline and
 * JSTR_RE_CF_NEWLINE is set.
 *
 * @param str const char *str Pointer at the current search position (non-NULL).
 * @param curr_idx size_t curr_idx Byte offset of the current search position.
 * @param cflags int cflags Compilation flags (JSTR_RE_CF_*).
 *
 * @return int 0 if beginning of line, else JSTR_RE_EF_NOTBOL.
 *
 * @note Internal helper; `str` must be non-NULL.
 */
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

#	define IS_NOTBOL(str, curr_idx, cflags)        jstr_internal_re_notbol(str, curr_idx, cflags)
#	define IS_NOTBOL_INLOOP(str, curr_idx, cflags) jstr_internal_re_notbol_inloop(str, curr_idx, cflags)

typedef regoff_t jstr_re_off_ty;
typedef struct jstr_re_ty {
	regex_t reg;
	int cflags;
} jstr_re_ty;

/**
 * @brief jstr_re_free
 *
 * Release resources held by a compiled regex (`regfree`). The object may be
 * reused after recompilation.
 *
 * @param preg jstr_re_ty *preg Compiled regex (non-NULL).
 *
 *
 * @note `preg` must be non-NULL; passing an uninitialized object is undefined.
 */
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

/**
 * @brief jstr_re_verr
 *
 * Print a POSIX regex error message to stderr using `regerror` and a
 * caller-supplied format string. Returns 0 on success, -1 on I/O failure.
 *
 * @param errcode jstr_re_ret_ty errcode Regex error code.
 * @param preg const jstr_re_ty *preg Compiled regex (non-NULL).
 * @param fmt const char *fmt printf-style format string (non-NULL).
 * @param args va_list args Variadic argument list.
 *
 * @return int 0 on success, -1 on I/O failure.
 *
 * @note Internal helper; `preg` and `fmt` must be non-NULL.
 */
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

/**
 * @brief jstr_re_err
 *
 * Print a POSIX regex error message to stderr. Returns 0 on success, -1 on
 * failure. Does not terminate the process.
 *
 * @param errcode jstr_re_ret_ty errcode Regex error code.
 * @param preg const jstr_re_ty *preg Compiled regex (non-NULL).
 * @param fmt const char *R fmt printf-style format string (non-NULL).
 * @param ... Variadic arguments for `fmt`.
 *
 * @return int 0 on success, -1 on I/O failure.
 *
 * @note `preg` and `fmt` must be non-NULL.
 */
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

/**
 * @brief jstr_re_errdie
 *
 * Print a POSIX regex error message to stderr and terminate the process with
 * EXIT_FAILURE. Does not return.
 *
 * @param errcode jstr_re_ret_ty errcode Regex error code.
 * @param preg const jstr_re_ty *preg Compiled regex (non-NULL).
 * @param fmt const char *R fmt printf-style format string (non-NULL).
 * @param ... Variadic arguments for `fmt`.
 *
 *
 * @note `preg` and `fmt` must be non-NULL.
 */
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

/**
 * @brief jstr_re_comp
 *
 * Compile a POSIX regular expression pattern into `preg`. Stores `cflags`.
 * Under JSTR_PANIC, compilation errors are printed and abort. Returns the
 * regcomp result code (0 on success).
 *
 * @param preg jstr_re_ty *R preg Destination compiled regex (non-NULL).
 * @param ptn const char *R ptn NUL-terminated pattern string (non-NULL).
 * @param cflags int cflags Compilation flags (JSTR_RE_CF_* / REG_*).
 *
 * @return jstr_re_ret_ty 0 (JSTR_RE_RET_NOERROR) on success, error code otherwise.
 *
 * @note `preg` and `ptn` must be non-NULL.
 */
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

/**
 * @brief jstr_re_exec
 *
 * Execute a compiled regex against NUL-terminated string `s`, optionally
 * filling up to `nmatch` entries in `pmatch`. `pmatch` may be NULL when
 * `nmatch` is 0. Returns match/no-match/error code.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s NUL-terminated haystack (non-NULL).
 * @param nmatch size_t nmatch Number of regmatch_t slots available.
 * @param pmatch regmatch_t *R pmatch Match result array (may be NULL if nmatch == 0).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg` and `s` must be non-NULL; `pmatch` must be non-NULL if nmatch > 0.
 */
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

/**
 * @brief jstr_re_exec_len
 *
 * Like jstr_re_exec but bounds the match to `sz` bytes using REG_STARTEND
 * (when available). Handles empty haystacks safely.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s Buffer (non-NULL, not necessarily NUL-terminated within sz).
 *                   When sz == 0, s must be NUL-terminated (strlen(s) == 0)
 *                   per the `char *` convention.
 * @param sz size_t sz Length in bytes of `s`.
 * @param nmatch size_t nmatch Number of regmatch_t slots.
 * @param pmatch regmatch_t *R pmatch Match result array (may be NULL if nmatch == 0).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg` and `s` must be non-NULL.
 */
JSTR_FUNC
jstr_re_ret_ty
jstr_re_exec_len(const jstr_re_ty *R preg, const char *R s, size_t sz, size_t nmatch, regmatch_t *R pmatch, int eflags) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
#		ifdef JSTR_RE_EF_STARTEND
	/* A zero-width REG_STARTEND window makes glibc return NOMATCH even for
	 * epsilon-matching patterns (".*"). Match the NUL-terminated string
	 * without STARTEND instead; S always points at a terminated buffer. */
	if (jstr_unlikely(sz == 0)) {
		/* eflags may carry REG_STARTEND (set by callers like
		 * jstr_re_match_len) but there is no pmatch window to scope the
		 * match to. A stale/uninitialized pmatch would make glibc read
		 * bogus rm_so/rm_eo bounds (SEGV on empty haystacks). Match the
		 * NUL-terminated string with STARTEND cleared. */
		return (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags & ~JSTR_RE_EF_STARTEND);
	}
	/* REG_STARTEND takes its window from pmatch[0], so a local window is
	 * required whenever the caller asked for no match slots (the API
	 * documents pmatch as NULL when nmatch == 0). */
	regmatch_t local;
	regmatch_t *m = (nmatch == 0 || pmatch == NULL) ? &local : pmatch;
	const size_t nout = (nmatch == 0 || pmatch == NULL) ? 1 : nmatch;
	m->rm_so = 0;
	m->rm_eo = sz;
#		endif
#		ifdef JSTR_RE_EF_STARTEND
	return (jstr_re_ret_ty)regexec(&preg->reg, s, nout, m, eflags | JSTR_RE_EF_STARTEND);
#		else
	return (jstr_re_ret_ty)regexec(&preg->reg, s, nmatch, pmatch, eflags);
#		endif
}
#	else
;
#	endif

/**
 * @brief jstr_re_match
 *
 * Check whether the entire (or anchored) pattern matches string `s`.
 * Returns regexec result code.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s NUL-terminated haystack (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg` and `s` must be non-NULL.
 */
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

/**
 * @brief jstr_re_search
 *
 * Search for the first match of the pattern in NUL-terminated string `s`,
 * storing the match offset in `pmatch`. Returns regexec result code.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s NUL-terminated haystack (non-NULL).
 * @param pmatch regmatch_t *R pmatch Match result (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg`, `s`, and `pmatch` must be non-NULL.
 */
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

/**
 * @brief jstr_re_search_len
 *
 * Search for the first match within the first `sz` bytes of `s`, storing the
 * match offset in `pmatch`.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s Buffer (non-NULL).
 * @param sz size_t sz Length in bytes of `s`.
 * @param pmatch regmatch_t *R pmatch Match result (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg`, `s`, and `pmatch` must be non-NULL.
 */
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

/**
 * @brief jstr_re_match_len
 *
 * Check whether the pattern matches the first `sz` bytes of `s`.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s const char *R s Buffer (non-NULL).
 * @param sz size_t sz Length in bytes of `s`.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_ret_ty JSTR_RE_RET_NOERROR on match, JSTR_RE_RET_NOMATCH, or error code.
 *
 * @note `preg` and `s` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rm_from_exec
 *
 * Remove the first regex match starting at `start_idx` from the in-place
 * string `*s` of length `*sz` with capacity `*cap`, updating `*sz`. Returns 1
 * if removed, 0 if no match, negative error code on failure. Do not use
 * anchored patterns (`^`/`$`); use jstr_re_rm_exec instead.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty 1 if removed, 0 if no match, negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL; assumes `*cap > *sz`.
 *       `start_idx` must be <= `*sz` (with empty-string exception).
 */
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

/**
 * @brief jstr_re_rm_exec
 *
 * Remove the first regex match from the entire string. Equivalent to
 * jstr_re_rm_from_exec with start_idx = 0.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty 1 if removed, 0 if no match, negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rmn_from_exec
 *
 * Remove up to `n` regex matches starting at `start_idx` from the in-place
 * string, updating `*sz`. Returns number removed, negative on error. Do not use
 * anchored patterns.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param n size_t n Maximum number of matches to remove (0 returns 0).
 *
 * @return jstr_re_off_ty Number of matches removed (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL; `n == 0` returns 0.
 */
JSTR_FUNC_VOID
jstr_re_off_ty
jstr_re_rmn_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, int eflags, size_t n) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	/* Allow start_idx == 0 when *sz == 0 to handle empty string haystacks. */
	if (jstr_unlikely(start_idx > *sz || (start_idx == *sz && start_idx != 0)))
		return 0;
	regmatch_t rm;
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, *s, sz, cap, start_idx, n, JSTR_INPLACE_ZERO_ADVANCE);
	if (jstr_unlikely(n == 0))
		return 0;
	if (jstr_unlikely(*sz == 0))
		it.n = 1;
	if (n == 1)
		return jstr_re_rm_from_exec(preg, s, sz, cap, start_idx, eflags);
	for (;;) {
		/* Explicit SCAN state: boundary and NOTBOL guards. */
		it.state = JSTR_INPLACE_SCAN;
		const int matched_at_end = (it.src_e == it.end);
		if (matched_at_end) {
			if (!it.prev_zero)
				break;
		} else if (it.src_e > it.end) {
			break;
		}
		const int eflags_curr = eflags | IS_NOTBOL(*s, JSTR_DIFF(it.src_e, *s), preg->cflags);
		const int ret = jstr_re_search_len(preg, it.src_e, JSTR_DIFF(it.end, it.src_e), &rm, eflags_curr);
		if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
			const size_t find_len = (size_t)(rm.rm_eo - rm.rm_so);
			/* Explicit COMMIT state. */
			jstr_internal_inplace_stage(&it, it.src_e + rm.rm_so, find_len);
			jstr_internal_inplace_commit_manual(&it, 0);
			--it.n;
			++it.changed;
			if (matched_at_end)
				break;
			it.prev_zero = (find_len == 0);
			if (!it.n)
				break;
		} else if (ret == JSTR_RE_RET_NOMATCH) {
			break;
		} else {
			JSTR_RE_RETURN_ERR(ret, preg);
		}
	}
	return (jstr_re_off_ty)jstr_internal_inplace_finalize(&it);
}
#	else
;
#	endif

/**
 * @brief jstr_re_rmn_exec
 *
 * Remove up to `n` regex matches from the entire string. Equivalent to
 * jstr_re_rmn_from_exec with start_idx = 0.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param n size_t n Maximum number of matches to remove.
 *
 * @return jstr_re_off_ty Number of matches removed (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rmall_exec
 *
 * Remove all regex matches from the entire string. Equivalent to
 * jstr_re_rmn_exec with n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty Number of matches removed (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rmall_from_exec
 *
 * Remove all regex matches starting at `start_idx`. Equivalent to
 * jstr_re_rmn_from_exec with n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty Number of matches removed (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplc_len_from_exec
 *
 * Replace the first regex match starting at `start_idx` with `rplc` of length
 * `rplc_len`, updating `*s`/`*sz`/`*cap`. Returns 1 if replaced, 0 if no
 * match, negative on error. Do not use anchored patterns.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement bytes (non-NULL unless rplc_len == 0).
 * @param rplc_len size_t rplc_len Replacement length in bytes.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty 1 if replaced, 0 if no match, negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplc_len_exec
 *
 * Replace the first regex match in the entire string with `rplc`. Equivalent
 * to jstr_re_rplc_len_from_exec with start_idx = 0.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement bytes.
 * @param rplc_len size_t rplc_len Replacement length in bytes.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty 1 if replaced, 0 if no match, negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

#	define NMATCH_PARAM , size_t nmatch
#	define NMATCH_ARG   , nmatch
#	define NMATCH       nmatch

/**
 * @brief jstr_internal_re_rplcbackrefstrlen
 *
 * Compute the resulting length after expanding backreferences (`\0`..`\9`)
 * in a replacement template of length `rplc_len`. Returns `(size_t)-1` on a
 * trailing lone backslash.
 *
 * @param rm const regmatch_t *R rm Match groups (non-NULL).
 * @param rplc const unsigned char *rplc Replacement template start (non-NULL).
 * @param rplc_e const unsigned char *rplc_e Replacement template end (non-NULL).
 * @param rplc_len size_t rplc_len Base replacement length.
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @return size_t Expanded length, or (size_t)-1 on error.
 *
 * @note Internal helper; pointers must be non-NULL.
 */
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

/**
 * @brief jstr_internal_re_rplcbackreffirst
 *
 * Return a pointer to the first backreference (`\digit`) in `backref`, or
 * NULL if none or length < 2.
 *
 * @param backref const char *backref Replacement template (non-NULL).
 * @param backref_len size_t backref_len Template length in bytes.
 *
 * @return char * Pointer to first backref, or NULL.
 *
 * @note `backref` must be non-NULL.
 */
JSTR_FUNC
char *
jstr_internal_re_rplcbackreffirst(const char *backref, size_t backref_len) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(backref_len < 2))
		return NULL;
	const char *const end = backref + backref_len;
	while (backref + 1 < end) {
		const char *p = (const char *)memchr(backref, '\\', (size_t)(end - 1 - backref));
		if (p == NULL)
			return NULL;
		if (jstr_isdigit(*(p + 1)))
			return (char *)p;
		backref = p + 2;
	}
	return NULL;
}
#	else
;
#	endif

/**
 * @brief jstr_internal_re_rplcbackreflast
 *
 * Return a pointer just past the last backreference in `backref`, or NULL.
 *
 * @param backref const unsigned char *backref Replacement template (non-NULL).
 * @param backref_len size_t backref_len Template length in bytes.
 *
 * @return char * Pointer past last backref, or NULL.
 *
 * @note `backref` must be non-NULL.
 */
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

/**
 * @brief jstr_internal_re_rplcbackrefcpy
 *
 * Copy the replacement template into `mtc_dst`, expanding backreferences from
 * `rm` against source `mtc_src`. Does not NUL-terminate.
 *
 * @param rm const regmatch_t *R rm Match groups (non-NULL).
 * @param mtc_src unsigned char *mtc_src Source string base (non-NULL).
 * @param mtc_dst unsigned char *R mtc_dst Destination buffer (non-NULL, sufficient space).
 * @param rplc const unsigned char *R rplc Replacement template start (non-NULL).
 * @param rplc_e const unsigned char *rplc_e Replacement template end (non-NULL).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 *
 * @note Internal helper; all pointers must be non-NULL.
 */
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

/**
 * @brief jstr_internal_re_rplcn_backref_len_from_exec
 *
 * Core implementation for replacing up to `n` matches with optional
 * backreference expansion. `backref` enables `\digit` expansion. Returns
 * number replaced, negative on error. Do not use anchored patterns.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement template (non-NULL).
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 * @param n size_t n Maximum replacements (SIZE_MAX for all).
 * @param backref int backref Enable backreference expansion.
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note Internal helper; `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
JSTR_FUNC
jstr_re_off_ty
jstr_internal_re_rplcn_backref_len_from_exec(const jstr_re_ty *R preg, char *R *R s, size_t *R sz, size_t *R cap, size_t start_idx, const char *R rplc, size_t rplc_len, int eflags, size_t nmatch, size_t n, int backref) JSTR_NOEXCEPT
#	ifdef JSTR_IMPLEMENTATION
{
	if (jstr_unlikely(n == 0))
		return 0;
	if (jstr_unlikely(nmatch > JSTR_RE_NMATCH_MAX))
		nmatch = JSTR_RE_NMATCH_MAX;
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
	regmatch_t rm[JSTR_RE_NMATCH_MAX];
	jstr_internal_inplace_it_ty it;
	jstr_internal_inplace_init(&it, *s, sz, cap, start_idx, n, JSTR_INPLACE_ZERO_ADVANCE);
	const size_t initial_cap = *sz * 2 + 2;
	if (jstr_chk(jstr_reserve(s, sz, cap, initial_cap))) {
		const int err = JSTR_RE_RET_ESPACE;
		JSTR_RE_RETURN_ERR(err, preg);
	}
	memmove(*s + *sz + 1, *s, *sz);
	it.dst_base = *s + *sz + 1;
	char *dst_s = it.dst_base;
	it.dst = dst_s;
	it.src = *s;
	it.src_e = *s + start_idx;
	it.end = *s + *sz;
	for (;;) {
		it.state = JSTR_INPLACE_SCAN;
		const int matched_at_end = (it.src_e == it.end);
		if (matched_at_end) {
			if (!it.prev_zero)
				break;
		} else if (it.src_e > it.end) {
			break;
		}
		const int eflags_curr = eflags | IS_NOTBOL(*s, JSTR_DIFF(it.src_e, *s), preg->cflags);
		int ret = jstr_re_exec_len(preg, it.src_e, JSTR_DIFF(it.end, it.src_e), nmatch, rm, eflags_curr);
		if (jstr_likely(ret == JSTR_RE_RET_NOERROR)) {
			const size_t find_len = (size_t)(rm[0].rm_eo - rm[0].rm_so);
			size_t emit_len;
			if (backref) {
				emit_len = jstr_internal_re_rplcbackrefstrlen(rm, rplc_backref1, rplc_backref1_e, rplc_len NMATCH_ARG);
				if (jstr_unlikely(emit_len == (size_t)-1)) {
					ret = JSTR_RE_RET_ESPACE;
					JSTR_RE_RETURN_ERR(ret, preg);
				}
			} else {
				emit_len = rplc_len;
			}
			it.state = JSTR_INPLACE_GROW;
			size_t need = JSTR_DIFF(it.dst, *s) + JSTR_DIFF(it.end, it.src) + 2;
			if (emit_len > find_len)
				need += emit_len - find_len;
			if (jstr_unlikely(*cap < need)) {
				const uintptr_t old_base = (uintptr_t)*s;
				const size_t saved_sz = *sz;
				*sz = JSTR_DIFF(it.dst, *s);
				if (jstr_chk(jstr_reserve(s, sz, cap, need))) {
					ret = JSTR_RE_RET_ESPACE;
					JSTR_RE_RETURN_ERR(ret, preg);
				}
				*sz = saved_sz;
				it.src = *s + JSTR_DIFF(it.src, old_base);
				it.src_e = *s + JSTR_DIFF(it.src_e, old_base);
				it.dst = *s + JSTR_DIFF(it.dst, old_base);
				dst_s = *s + JSTR_DIFF(dst_s, old_base);
				it.dst_base = dst_s;
				it.end = *s + JSTR_DIFF(it.end, old_base);
			}
			it.state = JSTR_INPLACE_COMMIT;
			jstr_internal_inplace_stage(&it, it.src_e + rm[0].rm_so, find_len);
			const size_t prefix = it.prefix_len;
			if (prefix)
				memmove(it.dst, it.src, prefix);
			if (backref)
				jstr_internal_re_rplcbackrefcpy(rm, (unsigned char *)it.src_e, (unsigned char *)(it.dst + prefix), (unsigned char *)rplc, (unsigned char *)rplc + rplc_len NMATCH_ARG);
			else if (rplc_len) {
				char *emitted = (char *)jstr_mempcpy(it.dst + prefix, rplc, rplc_len);
				(void)emitted;
			}
			jstr_internal_inplace_commit_manual(&it, emit_len);
			--it.n;
			++it.changed;
			if (matched_at_end)
				break;
			it.prev_zero = (find_len == 0);
			if (!it.n)
				break;
		} else if (ret == JSTR_RE_RET_NOMATCH) {
			break;
		} else {
			JSTR_RE_RETURN_ERR(ret, preg);
		}
	}
	*sz = JSTR_DIFF(jstr_mempmove(it.dst, it.src, JSTR_DIFF(it.end, it.src)), dst_s);
	jstr_strmove_len(*s, dst_s, *sz);
	return (jstr_re_off_ty)it.changed;
}
#	else
;
#	endif

#	undef NMATCH
#	undef NMATCH_ARG
#	undef NMATCH_PARAM

/**
 * @brief jstr_re_rplcn_len_from_exec
 *
 * Replace up to `n` regex matches starting at `start_idx` with `rplc`
 * (literal, no backreferences). Returns number replaced, negative on error.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement bytes.
 * @param rplc_len size_t rplc_len Replacement length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param n size_t n Maximum replacements.
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcn_len_exec
 *
 * Replace up to `n` regex matches in the entire string with `rplc`
 * (literal). Equivalent to jstr_re_rplcn_len_from_exec with start_idx = 0.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement bytes.
 * @param rplc_len size_t rplc_len Replacement length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param n size_t n Maximum replacements.
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcall_len_from_exec
 *
 * Replace all regex matches starting at `start_idx` with `rplc` (literal).
 * Equivalent to jstr_re_rplcn_len_from_exec with n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement bytes.
 * @param rplc_len size_t rplc_len Replacement length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcall_len_exec
 *
 * Replace all regex matches in the entire string with `rplc` (literal).
 * Equivalent to jstr_re_rplcn_len_exec with n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement bytes.
 * @param rplc_len size_t rplc_len Replacement length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcn_backref_len_from_exec
 *
 * Replace up to `n` regex matches starting at `start_idx` with `rplc`,
 * expanding backreferences (`\digit`) using `nmatch` groups. Returns number
 * replaced, negative on error.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 * @param n size_t n Maximum replacements.
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcall_backref_len_exec
 *
 * Replace all regex matches in the entire string with `rplc`, expanding
 * backreferences. Equivalent to jstr_re_rplcn_backref_len_exec with n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcall_backref_len_from_exec
 *
 * Replace all regex matches starting at `start_idx` with `rplc`, expanding
 * backreferences. Equivalent to jstr_re_rplcn_backref_len_from_exec with
 * n = SIZE_MAX.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplcn_backref_len_exec
 *
 * Replace up to `n` regex matches in the entire string with `rplc`, expanding
 * backreferences.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 * @param n size_t n Maximum replacements.
 *
 * @return jstr_re_off_ty Number of matches replaced (0 if none), or negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplc_backref_len_from_exec
 *
 * Replace the first regex match starting at `start_idx` with `rplc`,
 * expanding backreferences. Equivalent to jstr_re_rplcn_backref_len_from_exec
 * with n = 1.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param start_idx size_t start_idx Starting byte offset.
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 *
 * @return jstr_re_off_ty Number replaced (0 or 1), negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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

/**
 * @brief jstr_re_rplc_backref_len_exec
 *
 * Replace the first regex match in the entire string with `rplc`, expanding
 * backreferences. Equivalent to jstr_re_rplc_backref_len_from_exec with
 * start_idx = 0.
 *
 * @param preg const jstr_re_ty *R preg Compiled regex (non-NULL).
 * @param s char *R *R s Pointer to string buffer pointer (non-NULL; may be reallocated).
 * @param sz size_t *R sz Pointer to current length (non-NULL).
 * @param cap size_t *R cap Pointer to capacity (non-NULL).
 * @param rplc const char *R rplc Replacement template.
 * @param rplc_len size_t rplc_len Replacement template length.
 * @param eflags int eflags Execution flags (JSTR_RE_EF_*).
 * @param nmatch size_t nmatch Number of match groups available.
 *
 * @note nmatch is clamped to JSTR_RE_NMATCH_MAX (10).
 *
 * @return jstr_re_off_ty Number replaced (0 or 1), negative on error.
 *
 * @note `preg`, `s`, `sz`, `cap` must be non-NULL.
 */
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
