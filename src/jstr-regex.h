#ifndef JSTR_REEX_H
#define JSTR_REEX_H 1

#include "_jstr-macros.h"

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

#define jstr_re_chk(errcode) jstr_unlikely(errcode != JSTR_RE_RET_NOERROR && errcode != JSTR_RE_RET_NOMATCH)

#define PJSTR_RE_COMP_NOW()                                                     \
	do {                                                                    \
		const jstr_re_errcode_ty ret = jstr_re_comp(preg, ptn, cflags); \
		if (jstr_unlikely(ret != JSTR_RE_RET_NOERROR))                  \
			return ret;                                             \
	} while (0)

/* POSIX cflags */
#define JSTR_RE_CF_EXTENDED REG_EXTENDED
#define JSTR_RE_CF_ICASE    REG_ICASE
#define JSTR_RE_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#define JSTR_RE_EF_NOSUB  REG_NOSUB
#define JSTR_RE_EF_NOTBOL REG_NOTBOL
#define JSTR_RE_EF_NOTEOL REG_NOTEOL

/* BSD extension */
#ifdef REG_STARTEND
#	define JSTR_RE_EF_STARTEND REG_STARTEND
#endif /* REG_STARTEND */

PJSTR_BEGIN_DECLS

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
	JSTR_RE_RET_BADRPT = REG_BADRPT,
#define JSTR_RE_RET_BADRPT JSTR_RE_RET_BADRPT
/* GNU regcomp returns */
#ifdef REG_RET_EEND
	JSTR_RE_EEND = REG_RET_EEND,
#	define JSTR_RE_EEND JSTR_RE_EEND
#endif
#ifdef REG_RET_ESIZE
	JSTR_RE_ESIZE = REG_RET_ESIZE,
#	define JSTR_RE_ESIZE JSTR_RE_ESIZE
#endif
#ifdef REG_RET_ERPAREN
	JSTR_RE_ERPAREN = REG_RET_ERPAREN
#	define JSTR_RE_ERPAREN JSTR_RE_ERPAREN
#endif
} jstr_re_errcode_ty;

JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_re_free(regex_t *R preg)
JSTR_NOEXCEPT
{
	regfree(preg);
}

JSTR_COLD
JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_re_err_print(const int errcode,
		   const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, sizeof(buf));
	fprintf(stderr, "%s\n", buf);
}

JSTR_COLD
JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_re_err_exit_print(const int errcode,
			const regex_t *R preg)
JSTR_NOEXCEPT
{
	char buf[64];
	regerror(errcode, preg, buf, 32);
	fprintf(stderr, "%s\n", buf);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_re_err_exit(jstr_re_errcode_ty errcode,
		 const regex_t *R preg)
JSTR_NOEXCEPT
{
	if (jstr_re_chk(errcode))
		pjstr_re_err_exit_print(errcode, preg);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_re_err_print(jstr_re_errcode_ty errcode,
		  const regex_t *R preg)
JSTR_NOEXCEPT
{
	if (jstr_re_chk(errcode))
		pjstr_re_err_print(errcode, preg);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_re_err(jstr_re_errcode_ty errcode,
	    const regex_t *R preg,
	    char *R errbuf,
	    const size_t errbuf_size)
JSTR_NOEXCEPT
{
	if (jstr_re_chk(errcode))
		regerror(errcode, preg, errbuf, errbuf_size);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_comp(regex_t *R preg,
	     const char *R ptn,
	     const int cflags)
JSTR_NOEXCEPT
{
	return (jstr_re_errcode_ty)regcomp(preg, ptn, cflags);
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_FUNC_PURE_MAY_NULL
JSTR_NOTHROW
static jstr_re_errcode_ty
jstr_re_exec(const regex_t *R preg,
	     const char *R s,
	     const size_t nmatch,
	     regmatch_t *R pmatch,
	     const int eflags)
JSTR_NOEXCEPT
{
	return (jstr_re_errcode_ty)regexec(preg, s, nmatch, pmatch, eflags);
}

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_exec_len(const regex_t *R preg,
		 const char *R s,
		 const size_t sz,
		 const size_t nmatch,
		 regmatch_t *R pmatch,
		 const int eflags)
JSTR_NOEXCEPT
{
#ifdef JSTR_RE_EF_STARTEND
	pmatch->rm_so = 0;
	pmatch->rm_eo = sz;
#endif
	return (jstr_re_errcode_ty)regexec(preg, s, nmatch, pmatch, eflags | JSTR_RE_EF_STARTEND);
}

/*
   Check if S matches precompiled regex.
   Return return value of regexec.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_match(const regex_t *R preg,
	      const char *R s,
	      const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec(preg, s, 0, NULL, eflags);
}

/*
   Check if S matches PTN.
   Return return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_match_now(regex_t *R preg,
		  const char *R s,
		  const char *R ptn,
		  const int cflags,
		  const int eflags)
JSTR_NOEXCEPT
{
	PJSTR_RE_COMP_NOW();
	return jstr_re_match(preg, s, eflags);
}

/*
   Search pattern in S.
   Return return value of regexec.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_search(const regex_t *R preg,
	       const char *R s,
	       regmatch_t *R pmatch,
	       const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec(preg, s, 1, pmatch, eflags);
}

/*
   Search pattern in S.
   Return return value of regexec or regcomp if it fails.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_search_now(regex_t *R preg,
		   const char *R s,
		   const char *R ptn,
		   regmatch_t *R pmatch,
		   const int cflags,
		   const int eflags)
JSTR_NOEXCEPT
{
	PJSTR_RE_COMP_NOW();
	return jstr_re_search(preg, s, pmatch, eflags);
}

#ifdef JSTR_RE_EF_STARTEND

/*
   Search pattern in S.
   Return return value of regexec.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_search_len(const regex_t *R preg,
		   const char *R s,
		   const size_t sz,
		   regmatch_t *R pmatch,
		   const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_exec_len(preg, s, sz, 1, pmatch, eflags);
}

/*
   Search pattern in S.
   Return return value of regexec.
   Store offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_search_len_now(regex_t *R preg,
		       const char *R s,
		       const char *R ptn,
		       const size_t sz,
		       regmatch_t *R pmatch,
		       const int cflags,
		       const int eflags)
JSTR_NOEXCEPT
{
	PJSTR_RE_COMP_NOW();
	return jstr_re_search_len(preg, s, sz, pmatch, eflags);
}

/*
   Check if S matches PTN.
   Return return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_match_len(const regex_t *R preg,
		  const char *R s,
		  const size_t sz,
		  const int eflags)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstr_re_exec_len(preg, s, sz, 0, &rm, eflags | JSTR_RE_EF_STARTEND);
}

/*
   Check if S matches PTN.
   Return return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_match_len_now(regex_t *R preg,
		      const char *R s,
		      const char *R ptn,
		      const size_t sz,
		      const int cflags,
		      const int eflags)
JSTR_NOEXCEPT
{
	PJSTR_RE_COMP_NOW();
	return jstr_re_match_len(preg, s, sz, eflags);
}

#endif /* JSTR_RE_EF_STARTEND */

JSTR_FUNC
static jstr_re_errcode_ty
jstr_re_rmn_from(const regex_t *R preg,
		 char *R s,
		 size_t *R sz,
		 const size_t start_idx,
		 const int eflags,
		 size_t n)
JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)s + start_idx;
	const unsigned char *p = dst;
	const unsigned char *oldp = dst;
	const unsigned char *const end = *(unsigned char **)s + *sz;
	size_t find_len;
	jstr_re_errcode_ty ret = JSTR_RE_RET_NOMATCH;
	while (n-- && jstr_re_exec_len(preg, (char *)p, end - p, 1, &rm, eflags) == JSTR_RE_RET_NOERROR) {
		ret = JSTR_RE_RET_NOERROR;
		find_len = rm.rm_eo - rm.rm_so;
		p = p + rm.rm_so;
		if (jstr_unlikely(find_len == 0))
			++p;
		else
			pjstr_rmallinplace(&dst, &oldp, &p, find_len);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != oldp))
		*sz = jstr_stpmove_len(dst, oldp, end - oldp) - s;
	return ret;
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rm(const regex_t *R preg,
	   char *R s,
	   size_t *R sz,
	   const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, 0, eflags, 1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rm_from(const regex_t *R preg,
		char *R s,
		size_t *R sz,
		const size_t start_idx,
		const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, start_idx, eflags, 1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rmall(const regex_t *R preg,
	      char *R s,
	      size_t *R sz,
	      const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, 0, eflags, -1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rmn(const regex_t *R preg,
	    char *R s,
	    size_t *R sz,
	    const size_t n,
	    const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, n, eflags, n);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rmall_from(const regex_t *R preg,
		   char *R s,
		   size_t *R sz,
		   const size_t start_idx,
		   const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rmn_from(preg, s, sz, start_idx, eflags, -1);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstr_re_rplcall_small_rplc(unsigned char *s,
			    size_t *R sz,
			    unsigned char **dst,
			    const unsigned char **oldp,
			    unsigned char **p,
			    const unsigned char *R rplc,
			    const size_t rplc_len,
			    const size_t find_len)
{
	typedef unsigned char u;
	if (*dst != *oldp) {
		JSTR_PRINT_LOG("*dst != *oldp");
		*dst = (u *)jstr_mempmove(*dst, *oldp, *p - *oldp);
		jstr_strmove_len(*dst + rplc_len,
				 *p + find_len,
				 (s + *sz) - (*p + find_len));
		*dst = (u *)jstr_mempcpy(*dst, rplc, rplc_len);
		*oldp = *dst;
	} else {
		JSTR_PRINT_LOG("*dst == *oldp");
		jstr_strmove_len(*p + rplc_len,
				 *p + find_len,
				 s + *sz - (*p + find_len));
		memcpy(*p, rplc, rplc_len);
	}
	*sz += rplc_len - find_len;
	*p += rplc_len;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_re_rplcall_big_rplc(unsigned char **s,
			  size_t *R sz,
			  size_t *R cap,
			  unsigned char **dst,
			  const unsigned char **oldp,
			  unsigned char **p,
			  const unsigned char *R rplc,
			  const size_t rplc_len,
			  const size_t find_len)
{
	JSTR_PRINT_LOG("cap <= *sz + rplc_len - find_len");
	if (*dst != *oldp)
		memmove(*dst, *oldp, *p - *oldp);
	const unsigned char *tmp = *s;
	PJSTR_RESERVE_ALWAYS_NOMALLOC((char **)s, sz, cap, *sz + rplc_len - find_len, return 0)
	jstr_strmove_len(*p + rplc_len,
			 *p + find_len,
			 (tmp + *sz) - (*p + find_len));
	memcpy(*p, rplc, rplc_len);
	*p = *s + (*p - tmp);
	*dst = *s + (*dst - tmp) + rplc_len;
	*oldp = *dst;
	*sz += rplc_len - find_len;
	*p += rplc_len;
	return 1;
}

JSTR_FUNC
static jstr_re_errcode_ty
jstr_re_rplcn_len_from(regex_t *R preg,
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
	typedef unsigned char u;
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmall_from(preg, *s, sz, start_idx, eflags);
	unsigned char *dst = *(u **)s + start_idx;
	unsigned char *p = dst;
	const unsigned char *oldp = dst;
	size_t find_len;
	regmatch_t rm;
	while (n-- && *p
	       && jstr_re_exec_len(preg, (char *)p, (*(u **)s + *sz) - p, 1, &rm, eflags) == JSTR_RE_RET_NOERROR) {
		find_len = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(find_len == 0)) {
			JSTR_PRINT_LOG("jstr_unlikely(find_len == 0)");
			if (jstr_unlikely(*++p == '\0'))
				break;
			continue;
		}
		if (rplc_len <= find_len) {
			JSTR_PRINT_LOG("rplc_len <= find_len");
			pjstr_rplcallinplace(&dst, &oldp, (const u **)&p, (u *)rplc, rplc_len, find_len);
		} else if (*cap > *sz + rplc_len - find_len) {
			JSTR_PRINT_LOG("*cap > *sz + rplc_len - find_len");
			pjstr_re_rplcall_small_rplc(*(u **)s, sz, &dst, &oldp, &p, (u *)rplc, rplc_len, find_len);
		} else {
			JSTR_PRINT_LOG("else");
			if (jstr_unlikely(!pjstr_re_rplcall_big_rplc((u **)s, sz, cap, &dst, &oldp, &p, (u *)rplc, rplc_len, find_len)))
				goto err;
		}
	}
	if (dst != oldp)
		*sz = jstr_stpmove_len(dst, oldp, (*(u **)s + *sz) - oldp) - *s;
	return JSTR_RE_RET_NOERROR;
err:
	jstr_re_free(preg);
	jstr_free(s, sz, cap);
	return JSTR_RE_RET_ESPACE;
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcn_len(regex_t *R preg,
		  char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const char *R rplc,
		  const size_t rplc_len,
		  const int eflags,
		  const size_t n)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, n);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcall_len_from(regex_t *R preg,
			 char *R *R s,
			 size_t *R sz,
			 size_t *R cap,
			 const size_t start_idx,
			 const char *R rplc,
			 const size_t rplc_len,
			 const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, -1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcall_len(regex_t *R preg,
		    char *R *R s,
		    size_t *R sz,
		    size_t *R cap,
		    const char *R rplc,
		    const size_t rplc_len,
		    const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, -1);
}

JSTR_INLINE
JSTR_FUNC
static jstr_re_errcode_ty
jstr_re_rplc_len(regex_t *R preg,
		 char *R *R s,
		 size_t *R sz,
		 size_t *R cap,
		 const char *R rplc,
		 const size_t rplc_len,
		 const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, 1);
}

JSTR_INLINE
JSTR_FUNC
static jstr_re_errcode_ty
jstr_re_rplc_len_from(regex_t *R preg,
		      char *R *R s,
		      size_t *R sz,
		      size_t *R cap,
		      const size_t start_idx,
		      const char *R rplc,
		      const size_t rplc_len,
		      const int eflags)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, 1);
}

JSTR_FUNC_VOID
JSTR_INLINE
static int
pjstr_re_strlenrplcdst(const regmatch_t *R rm,
		       unsigned char *R rplc,
		       const size_t rplc_len,
		       size_t *R rdst_len)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	const unsigned char *const rplc_e = rplc + rplc_len;
	int has_bref = 0;
	for (*rdst_len = rplc_len; (rplc = (u *)memchr(rplc, '\\', rplc_e - rplc)); ++rplc) {
		if (jstr_likely(jstr_isdigit(*++rplc)))
			*rdst_len += (rm[*rplc - '0'].rm_eo - rm[*rplc - '0'].rm_so) - 2;
		else if (jstr_unlikely(*rplc == '\0')) {
			has_bref = 1;
			break;
		}
	}
	return has_bref;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstr_re_creatrplcbref(const unsigned char *R mtc,
		       const regmatch_t *R rm,
		       unsigned char *R rdst,
		       unsigned char *R rplc,
		       const size_t rplc_len)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	const unsigned char *rold;
	const unsigned char *const rplc_e = rplc + rplc_len;
	for (;; ++rplc) {
		rold = rplc;
		rplc = (u *)memchr(rplc, '\\', rplc_e - rplc);
		if (jstr_likely(jstr_isdigit(*++rplc))) {
			if (jstr_likely(rplc != rold)) {
				rdst = (u *)jstr_mempmove(rdst, rold, (rplc - 1) - rold);
				rplc += (rplc - 1) - rold - 1;
			}
			rdst = (u *)jstr_mempcpy(rdst, mtc + rm[*rplc - '0'].rm_so, rm[*rplc - '0'].rm_eo - rm[*rplc - '0'].rm_so);
		} else if (jstr_unlikely(*rplc == '\0')) {
			break;
		} else {
			rdst[0] = rplc[-1];
			rdst[1] = rplc[0];
			rdst += 2;
		}
	}
	if (jstr_unlikely(rplc == NULL))
		memcpy(rdst, rold, rplc_e - rold);
}

JSTR_FUNC
static jstr_re_errcode_ty
jstr_re_rplcn_bref_len_from(regex_t *R preg,
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
	typedef unsigned char u;
	unsigned char *p = *(u **)s + start_idx;
	if (jstr_unlikely(rplc_len == 0))
		return jstr_re_rmn_from(preg, *s, sz, start_idx, eflags, n);
	unsigned char *dst = p;
	const unsigned char *oldp = p;
	regmatch_t rm[10];
	size_t rdst_len;
	size_t rdstcap = 0;
	unsigned char rdst_stack[JSTR_PAGE_SIZE];
	unsigned char *rdstp = rdst_stack;
	unsigned char *rdst_heap = NULL;
	size_t find_len;
	int has_bref;
	while (n-- && *p
	       && jstr_re_exec_len(preg, (char *)p, (*(u **)s + *sz) - p, nmatch, rm, eflags) == JSTR_RE_RET_NOERROR) {
		find_len = rm[0].rm_eo - rm[0].rm_so;
		if (jstr_unlikely(find_len == 0)) {
			++p;
			continue;
		}
		has_bref = pjstr_re_strlenrplcdst(rm, (u *)rplc, rplc_len, &rdst_len);
		if (jstr_unlikely(has_bref == 0))
			return jstr_re_rplcn_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, n);
		if (jstr_unlikely(rdst_len > JSTR_PAGE_SIZE)) {
			if (jstr_unlikely(rdst_heap == NULL)) {
				rdstcap = JSTR_PTR_ALIGN_UP(rdst_len, PJSTR_MALLOC_ALIGNMENT);
				rdst_heap = (u *)malloc(rdstcap);
				PJSTR_MALLOC_ERR(rdst_heap, goto err);
				rdstp = rdst_heap;
			} else if (rdstcap < rdst_len) {
				rdstcap = pjstr_grow(rdstcap, rdst_len);
				PJSTR_REALLOC(rdst_heap, rdstcap, rdst_len, goto err);
				rdstp = rdst_heap;
			}
		}
		pjstr_re_creatrplcbref((u *)p, rm, (u *)rdstp, (u *)rplc, rplc_len);
		p += rm[0].rm_so;
#ifdef __clang__
#	pragma GCC diagnostic ignored "-Wunknown-pragmas"
#	pragma GCC diagnostic push
#elif defined __GNUC__
#	pragma GCC diagnostic ignored "-Wanalyzer-use-of-uninitialized-value"
#	pragma GCC diagnostic push
#endif
		if (rdst_len <= find_len)
			pjstr_rplcallinplace(&dst, &oldp, (const u **)&p, rdstp, rdst_len, find_len); /* false-positive? */
		else if (*cap > *sz + rdst_len - find_len)
			pjstr_re_rplcall_small_rplc(*(u **)s, sz, &dst, &oldp, &p, rdstp, rdst_len, find_len);
		else if (jstr_unlikely(!pjstr_re_rplcall_big_rplc((u **)s, sz, cap, &dst, &oldp, &p, rdstp, rdst_len, find_len)))
			goto err_free;
#ifdef __clang__
#	pragma GCC diagnostic pop
#elif defined __GNUC__
#	pragma GCC diagnostic pop
#endif
	}
	if (dst != oldp)
		*sz = jstr_stpmove_len(dst, oldp, (*(u **)s + *sz) - oldp) - *s;
	free(rdst_heap);
	return JSTR_RE_RET_NOERROR;
err_free:
	free(rdst_heap);
err:
	jstr_re_free(preg);
	jstr_free(s, sz, cap);
	return JSTR_RE_RET_ESPACE;
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcall_bref_len(regex_t *R preg,
			 char *R *R s,
			 size_t *R sz,
			 size_t *R cap,
			 const char *R rplc,
			 size_t rplc_len,
			 const int eflags,
			 const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, -1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcall_bref_len_from(regex_t *R preg,
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
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, -1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplcn_bref_len(regex_t *R preg,
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
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, n);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplc_bref_len_from(regex_t *R preg,
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
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, start_idx, rplc, rplc_len, eflags, nmatch, 1);
}

JSTR_FUNC
JSTR_INLINE
static jstr_re_errcode_ty
jstr_re_rplc_bref_len(regex_t *R preg,
		      char *R *R s,
		      size_t *R sz,
		      size_t *R cap,
		      const char *R rplc,
		      size_t rplc_len,
		      const int eflags,
		      const size_t nmatch)
JSTR_NOEXCEPT
{
	return jstr_re_rplcn_bref_len_from(preg, s, sz, cap, 0, rplc, rplc_len, eflags, nmatch, 1);
}

PJSTR_END_DECLS

#undef R

#undef JSTR_PRINT_LOG
#undef JSTR_RE_DEBUG
#undef PJSTR_RE_COMP_NOW

#endif /* JSTR_REEX_H */
