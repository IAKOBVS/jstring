/* See LICENSE file for copyright and license details. */

#ifndef JSTRRE_H
#define JSTRRE_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-replace.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

/* POSIX cflags */
#define JSTRRE_CF_EXTENDED REG_EXTENDED
#define JSTRRE_CF_ICASE    REG_ICASE
#define JSTRRE_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#define JSTRRE_EF_NOSUB  REG_NOSUB
#define JSTRRE_EF_NOTBOL REG_NOTBOL
#define JSTRRE_EF_NOTEOL REG_NOTEOL

/* BSD extension */
#ifdef REG_STARTEND
#	define JSTRRE_EF_STARTEND REG_STARTEND
#endif /* REG_STARTEND */

PJSTR_BEGIN_DECLS

typedef enum {
#ifdef REG_ENOSYS
	JSTRRE_RET_ENOSYS = REG_ENOSYS,
#endif
#if defined REG_NOERROR
	JSTRRE_RET_NOERROR = REG_NOERROR,
#else
	JSTRRE_RET_NOERROR = 0,
#endif
#define JSTRRE_RET_NOERROR JSTRRE_RET_NOERROR
	JSTRRE_RET_NOMATCH = REG_NOMATCH,
#define JSTRRE_RET_NOMATCH JSTRRE_RET_NOMATCH
	/* POSIX regcomp return values */
	JSTRRE_RET_BADPAT = REG_BADPAT,
#define JSTRRE_RET_BADPAT JSTRRE_RET_BADPAT
	JSTRRE_RET_ECOLLATE = REG_ECOLLATE,
#define JSTRRE_RET_ECOLLATE JSTRRE_RET_ECOLLATE
	JSTRRE_RET_ECTYPE = REG_ECTYPE,
#define JSTRRE_RET_ECTYPE JSTRRE_RET_ECTYPE
	JSTRRE_RET_EESCAPE = REG_EESCAPE,
#define JSTRRE_RET_EESCAPE JSTRRE_RET_EESCAPE
	JSTRRE_RET_ESUBREG = REG_ESUBREG,
#define JSTRRE_RET_ESUBREG JSTRRE_RET_ESUBREG
	JSTRRE_RET_EBRACK = REG_EBRACK,
#define JSTRRE_RET_EBRACK JSTRRE_RET_EBRACK
	JSTRRE_RET_EPAREN = REG_EPAREN,
#define JSTRRE_RET_EPAREN JSTRRE_RET_EPAREN
	JSTRRE_RET_EBRACE = REG_EBRACE,
#define JSTRRE_RET_EBRACE JSTRRE_RET_EBRACE
	JSTRRE_RET_BADBR = REG_BADBR,
#define JSTRRE_RET_BADBR JSTRRE_RET_BADBR
	JSTRRE_RET_ERANGE = REG_ERANGE,
#define JSTRRE_RET_ERANGE JSTRRE_RET_ERANGE
	JSTRRE_RET_ESPACE = REG_ESPACE,
#define JSTRRE_RET_ESPACE JSTRRE_RET_ESPACE
	JSTRRE_RET_BADRPT = REG_BADRPT,
#define JSTRRE_RET_BADRPT JSTRRE_RET_BADRPT
/* GNU regcomp returns */
#ifdef REG_RET_EEND
	JSTRRE_EEND = REG_RET_EEND,
#	define JSTRRE_EEND JSTRRE_EEND
#endif
#ifdef REG_RET_ESIZE
	JSTRRE_ESIZE = REG_RET_ESIZE,
#	define JSTRRE_ESIZE JSTRRE_ESIZE
#endif
#ifdef REG_RET_ERPAREN
	JSTRRE_ERPAREN = REG_RET_ERPAREN
#	define JSTRRE_ERPAREN JSTRRE_ERPAREN
#endif
} jstrre_ret_ty;

#define jstrre_comp_chk(errcode) jstr_unlikely(errcode != JSTRRE_RET_NOERROR)
#define jstrre_exec_chk(errcode) (jstrre_comp_chk(errcode) && jstr_unlikely(errcode != JSTRRE_RET_NOMATCH))
#define jstrre_chk(errcode)      jstr_unlikely(errcode != JSTRRE_RET_NOERROR)

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static void
jstrre_free(regex_t *R preg)
JSTR_NOEXCEPT
{
	regfree(preg);
}

JSTR_ATTR_COLD
JSTR_FUNC_VOID
JSTR_ATTR_NOINLINE
static void
pjstrre_err_print(const int errcode,
                  const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, sizeof(buf));
	fprintf(stderr, "%s\n", buf);
}

JSTR_ATTR_COLD
JSTR_FUNC_VOID
JSTR_ATTR_NOINLINE
static void
pjstrre_err_exit_print(const int errcode,
                       const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, 32);
	fprintf(stderr, "%s\n", buf);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstrre_err_exit(jstrre_ret_ty errcode,
                const regex_t *R preg)
JSTR_NOEXCEPT
{
	if (jstrre_exec_chk(errcode))
		pjstrre_err_exit_print(errcode, preg);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstrre_err_print(jstrre_ret_ty errcode,
                 const regex_t *R preg)
JSTR_NOEXCEPT
{
	if (jstrre_exec_chk(errcode))
		pjstrre_err_print(errcode, preg);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstrre_err(jstrre_ret_ty errcode,
           const regex_t *R preg,
           char *R errbuf,
           const size_t errbuf_size)
JSTR_NOEXCEPT
{
	if (jstrre_exec_chk(errcode))
		regerror(errcode, preg, errbuf, errbuf_size);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_comp(regex_t *R preg,
            const char *R ptn,
            const int cflags)
JSTR_NOEXCEPT
{
	return (jstrre_ret_ty)regcomp(preg, ptn, cflags);
}

JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_ATTR_INLINE
JSTR_ATTR_WARN_UNUSED
JSTR_FUNC_PURE_MAY_NULL
JSTR_ATTR_NOTHROW
static jstrre_ret_ty
jstrre_exec(const regex_t *R preg,
            const char *R s,
            const size_t nmatch,
            regmatch_t *R pmatch,
            const int eflags)
JSTR_NOEXCEPT
{
	return (jstrre_ret_ty)regexec(preg, s, nmatch, pmatch, eflags);
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_exec_len(const regex_t *R preg,
                const char *R s,
                const size_t sz,
                const size_t nmatch,
                regmatch_t *R pmatch,
                const int eflags)
JSTR_NOEXCEPT
{
#ifdef JSTRRE_EF_STARTEND
	pmatch->rm_so = 0;
	pmatch->rm_eo = sz;
#endif
	return (jstrre_ret_ty)regexec(preg, s, nmatch, pmatch, eflags | JSTRRE_EF_STARTEND);
}

/*
   Check if S matches precompiled regex.
   Return return value of regexec.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_match(const regex_t *R preg,
             const char *R s,
             const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_exec(preg, s, 0, NULL, eflags);
}

/*
   Search pattern in S.
   Return return value of regexec.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_search(const regex_t *R preg,
              const char *R s,
              regmatch_t *R pmatch,
              const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_exec(preg, s, 1, pmatch, eflags);
}

/*
   Search pattern in S.
   Return return value of regexec.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_search_len(const regex_t *R preg,
                  const char *R s,
                  const size_t sz,
                  regmatch_t *R pmatch,
                  const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_exec_len(preg, s, sz, 1, pmatch, eflags);
}

/*
   Check if S matches PTN.
   Return return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_match_len(const regex_t *R preg,
                 const char *R s,
                 const size_t sz,
                 const int eflags)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstrre_exec_len(preg, s, sz, 0, &rm, eflags | JSTRRE_EF_STARTEND);
}

JSTR_ATTR_ACCESS((__read_write__, 2, 3))
JSTR_FUNC_VOID
static char *
jstrre_rmn_p(const regex_t *R preg,
             char *R s,
             const size_t sz,
             const int eflags,
             size_t n)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	char *dst = s;
	const char *p = dst;
	const char *oldp = dst;
	const char *const end = s + sz;
	size_t find_len;
	while (n-- && *p && jstrre_exec_len(preg, p, end - p, 1, &rm, eflags) == JSTRRE_RET_NOERROR) {
		find_len = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(find_len == 0))
			++p;
		else
			pjstr_rmallinplace(&dst, &oldp, &p, find_len);
	}
	if (dst != oldp)
		return jstr_stpmove_len(dst, oldp, end - oldp);
	return (char *)end;
}

JSTR_ATTR_ACCESS((__read_write__, 2, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstrre_rm_p(const regex_t *R preg,
            char *R s,
            const size_t sz,
            const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rmn_p(preg, s, sz, eflags, 1);
}

JSTR_ATTR_ACCESS((__read_write__, 2, 3))
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstrre_rmall_p(const regex_t *R preg,
               char *R s,
               const size_t sz,
               const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rmn_p(preg, s, sz, eflags, -1);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjstrre_rplcallsmallerrplc(char *s,
                           size_t *R sz,
                           char **dst,
                           const char **oldp,
                           char **p,
                           const char *R rplc,
                           const size_t rplc_len,
                           const size_t find_len)
{
	if (*dst != *oldp) {
		*dst = (char *)jstr_mempmove(*dst, *oldp, *p - *oldp);
		jstr_strmove_len(*dst + rplc_len,
		                 *p + find_len,
		                 (s + *sz) - (*p + find_len));
		*dst = (char *)jstr_mempcpy(*dst, rplc, rplc_len);
		*oldp = *dst;
	} else {
		jstr_strmove_len(*p + rplc_len,
		                 *p + find_len,
		                 s + *sz - (*p + find_len));
		memcpy(*p, rplc, rplc_len);
	}
	*sz += rplc_len - find_len;
	*p += rplc_len;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
pjstrre_rplcallbiggerrplc(char *R *R s,
                          size_t *R sz,
                          size_t *R cap,
                          char **dst,
                          const char **oldp,
                          char **p,
                          const char *R rplc,
                          const size_t rplc_len,
                          const size_t find_len)
{
	if (*dst != *oldp)
		memmove(*dst, *oldp, *p - *oldp);
	if (*cap <= *sz + rplc_len - find_len) {
		const char *const tmp = *s;
		PJSTR_RESERVE_ALWAYS(s, sz, cap, *sz + rplc_len - find_len, return JSTR_RET_ERR)
		*p = *s + (*p - tmp);
		*dst = *s + (*dst - tmp);
	}
	jstr_strmove_len(*p + rplc_len,
	                 *p + find_len,
	                 (*s + *sz) - (*p + find_len));
	*p = (char *)jstr_mempcpy(*p, rplc, rplc_len);
	*dst += rplc_len;
	*oldp = *dst;
	*sz += rplc_len - find_len;
	return JSTR_RET_SUCC;
}

JSTR_FUNC
static jstrre_ret_ty
jstrre_rplcn_len_from(regex_t *R preg,
                      char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      const size_t start_idx,
                      const char *R rplc,
                      const size_t rplc_len,
                      const int eflags,
                      size_t n)
JSTR_NOEXCEPT
{
	typedef char u;
	char *dst = *s + start_idx;
	if (jstr_unlikely(rplc_len == 0)) {
		*sz = jstrre_rmn_p(preg, dst, *sz - start_idx, eflags, n) - *s;
		return JSTRRE_RET_NOERROR;
	}
	char *p = dst;
	const char *oldp = dst;
	size_t find_len;
	regmatch_t rm;
	while (n-- && *p && jstrre_exec_len(preg, p, (*s + *sz) - p, 1, &rm, eflags) == JSTRRE_RET_NOERROR) {
		find_len = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(find_len == 0))
			continue;
		if (rplc_len <= find_len)
			pjstr_rplcallinplace(&dst, &oldp, (const char **)&p, rplc, rplc_len, find_len);
		else if (*cap > *sz + rplc_len - find_len)
			pjstrre_rplcallsmallerrplc(*s, sz, &dst, &oldp, &p, rplc, rplc_len, find_len);
		else if (jstr_chk(pjstrre_rplcallbiggerrplc((u **)s, sz, cap, &dst, &oldp, &p, rplc, rplc_len, find_len)))
			goto err;
	}
	if (dst != oldp)
		*sz = jstr_stpmove_len(dst, oldp, (*s + *sz) - oldp) - *s;
	return JSTRRE_RET_NOERROR;
err:
	jstrre_free(preg);
	jstr_free(s, sz, cap);
	PJSTR_EXIT_MAYBE();
	return JSTRRE_RET_ESPACE;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcn_len(regex_t *R preg,
                 char *R *R s,
                 size_t *R sz,
                 size_t *R cap,
                 const char *R rplc,
                 const size_t rplc_len,
                 const int eflags,
                 const size_t n)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, n);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcall_len_from(regex_t *R preg,
                        char *R *R s,
                        size_t *R sz,
                        size_t *R cap,
                        const size_t start_idx,
                        const char *R rplc,
                        const size_t rplc_len,
                        const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, -1);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcall_len(regex_t *R preg,
                   char *R *R s,
                   size_t *R sz,
                   size_t *R cap,
                   const char *R rplc,
                   const size_t rplc_len,
                   const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, -1);
}

JSTR_ATTR_INLINE
JSTR_FUNC
static jstrre_ret_ty
jstrre_rplc_len(regex_t *R preg,
                char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R rplc,
                const size_t rplc_len,
                const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, 1);
}

JSTR_ATTR_INLINE
JSTR_FUNC
static jstrre_ret_ty
jstrre_rplc_len_from(regex_t *R preg,
                     char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const size_t start_idx,
                     const char *R rplc,
                     const size_t rplc_len,
                     const int eflags)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, 1);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static size_t
pjstrre_brefrplcstrlen(const regmatch_t *R rm,
                       const char *R rplc,
                       size_t rplc_len)
JSTR_NOEXCEPT
{
	const char *const rplc_e = rplc + rplc_len;
	int has_bref = 0;
	for (; (rplc = (char *)memchr(rplc, '\\', rplc_e - rplc)); ++rplc, has_bref = 1)
		if (jstr_likely(jstr_isdigit(*++rplc)))
			rplc_len += (rm[*rplc - '0'].rm_eo - rm[*rplc - '0'].rm_so) - 2;
		else if (jstr_unlikely(*rplc == '\0'))
			break;
	return has_bref ? rplc_len : 0;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjstrre_brefrplccreat(const char *R mtc,
                      const regmatch_t *R rm,
                      char *R rdst,
                      const char *R rplc,
                      const size_t rplc_len)
JSTR_NOEXCEPT
{
	const char *rold;
	const char *const rplc_e = rplc + rplc_len;
	int c;
	for (;; ++rplc) {
		rold = rplc;
		rplc = (const char *)memchr(rplc, '\\', rplc_e - rplc);
		if (rplc == NULL)
			break;
		c = *++rplc;
		if (jstr_likely(jstr_isdigit(c))) {
			if (jstr_likely(rdst == rold)) {
				rdst = (char *)jstr_mempmove(rdst, rold, (rplc - 1) - rold);
				rplc += (rplc - 1) - rold - 1;
			}
			c -= '0';
			rdst = (char *)jstr_mempcpy(rdst, mtc + rm[c].rm_so, rm[c].rm_eo - rm[c].rm_so);
		} else if (jstr_unlikely(*rplc == '\0')) {
			break;
		} else {
			rdst[0] = rplc[-1];
			rdst[1] = rplc[0];
			rdst += 2;
		}
	}
	if (rdst != rold)
		memcpy(rdst, rold, rplc_e - rold);
}

JSTR_FUNC
static jstrre_ret_ty
jstrre_rplcn_bref_len_from(regex_t *R preg,
                           char *R *R s,
                           size_t *R sz,
                           size_t *R cap,
                           const size_t start_idx,
                           const char *R rplc,
                           size_t rplc_len,
                           const int eflags,
                           const size_t nmatch,
                           size_t n)
JSTR_NOEXCEPT
{
	char *p = *s + start_idx;
	if (jstr_unlikely(rplc_len == 0)) {
		*sz = jstrre_rmn_p(preg, p, *sz - start_idx, eflags, n) - *s;
		return JSTRRE_RET_NOERROR;
	}
	char *dst = p;
	const char *oldp = (const char *)p;
	regmatch_t rm[10];
	size_t rdst_len;
	size_t rdst_cap = 0;
	enum { BUFSZ = 256 };
	char rdst_stack[BUFSZ];
	char *rdstp = rdst_stack;
	char *rdst_heap = NULL;
	size_t find_len;
	while (n-- && *p && jstrre_exec_len(preg, p, (*s + *sz) - p, nmatch, rm, eflags) == JSTRRE_RET_NOERROR) {
		find_len = rm[0].rm_eo - rm[0].rm_so;
		if (jstr_unlikely(find_len == 0)) {
			++p;
			continue;
		}
		rdst_len = pjstrre_brefrplcstrlen(rm, rplc, rplc_len);
		if (jstr_unlikely(rdst_len == 0))
			return jstrre_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, n);
		if (jstr_unlikely(rdst_len > BUFSZ))
			if (rdst_cap < rdst_len) {
				if (jstr_unlikely(rdst_cap == 0))
					rdst_cap = BUFSZ;
				rdst_cap = pjstr_grow(rdst_cap, rdst_len);
				rdst_heap = (char *)realloc(rdst_heap, rdst_cap);
				PJSTR_MALLOC_ERR(rdst_heap, goto err);
				rdstp = rdst_heap;
			}
		pjstrre_brefrplccreat(p, rm, rdstp, rplc, rplc_len);
		p += rm[0].rm_so;
		find_len = rm[0].rm_eo - rm[0].rm_so;
		if (rdst_len <= find_len)
			pjstr_rplcallinplace(&dst, &oldp, (const char **)&p, rdstp, rdst_len, find_len);
		else if (*cap > *sz + rdst_len - find_len)
			pjstrre_rplcallsmallerrplc(*s, sz, &dst, &oldp, &p, rdstp, rdst_len, find_len);
		else if (jstr_chk(pjstrre_rplcallbiggerrplc(s, sz, cap, &dst, &oldp, &p, rdstp, rdst_len, find_len)))
			goto err_free;
	}
	if (dst != oldp)
		*sz = jstr_stpmove_len(dst, oldp, (*s + *sz) - oldp) - *s;
	free(rdst_heap);
	return JSTRRE_RET_NOERROR;
err_free:
	free(rdst_heap);
err:
	jstrre_free(preg);
	jstr_free(s, sz, cap);
	PJSTR_EXIT_MAYBE();
	return JSTRRE_RET_ESPACE;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcall_bref_len(regex_t *R preg,
                        char *R *R s,
                        size_t *R sz,
                        size_t *R cap,
                        const char *R rplc,
                        size_t rplc_len,
                        const int eflags,
                        const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, -1);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcall_bref_len_from(regex_t *R preg,
                             char *R *R s,
                             size_t *R sz,
                             size_t *R cap,
                             const size_t start_idx,
                             const char *R rplc,
                             size_t rplc_len,
                             const int eflags,
                             const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, -1);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplcn_bref_len(regex_t *R preg,
                      char *R *R s,
                      size_t *R sz,
                      size_t *R cap,
                      const char *R rplc,
                      size_t n,
                      size_t rplc_len,
                      const int eflags,
                      const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, n);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplc_bref_len_from(regex_t *R preg,
                          char *R *R s,
                          size_t *R sz,
                          size_t *R cap,
                          const size_t start_idx,
                          const char *R rplc,
                          size_t rplc_len,
                          const int eflags,
                          const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, 1);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstrre_ret_ty
jstrre_rplc_bref_len(regex_t *R preg,
                     char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const char *R rplc,
                     size_t rplc_len,
                     const int eflags,
                     const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstrre_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, 1);
}

PJSTR_END_DECLS

#undef R

#endif /* JSTRRE_H */
