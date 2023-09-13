#ifndef JSTR_REGEX_H
#define JSTR_REGEX_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-pp-va-args-macros.h"
#include "jstr-replace.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#ifndef JSTR_REG_DEBUG
#	define JSTR_REG_DEBUG 0
#endif

#if JSTR_REG_DEBUG
#	define PJSTR_REG_LOG(x) jstr_pp_cout(x)
#else
#	define PJSTR_REG_LOG(x)
#endif /* PJSTR_REG_LOG */

/* POSIX cflags */
#define JSTR_REG_CF_EXTENDED REG_EXTENDED
#define JSTR_REG_CF_ICASE    REG_ICASE
#define JSTR_REG_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#define JSTR_REG_EF_NOSUB  REG_NOSUB
#define JSTR_REG_EF_NOTBOL REG_NOTBOL
#define JSTR_REG_EF_NOTEOL REG_NOTEOL

/* BSD extension */
#ifdef REG_STARTEND
#	define JSTR_REG_EF_STARTEND REG_STARTEND
#endif /* REG_STARTEND */

typedef enum {
	/* Cannot allocate memory */
	JSTR_REG_RET_ENOMEM,
#ifdef REG_ENOSYS
	JSTR_REG_RET_ENOSYS = REG_ENOSYS,
#endif
	JSTR_REG_RET_NOERROR = REG_NOERROR,
	JSTR_REG_RET_NOMATCH = REG_NOMATCH,
	/* POSIX regcomp return values */
	JSTR_REG_RET_BADPAT = REG_BADPAT,
	JSTR_REG_RET_ECOLLATE = REG_ECOLLATE,
	JSTR_REG_RET_ECTYPE = REG_ECTYPE,
	JSTR_REG_RET_EESCAPE = REG_EESCAPE,
	JSTR_REG_RET_ESUBREG = REG_ESUBREG,
	JSTR_REG_RET_EBRACK = REG_EBRACK,
	JSTR_REG_RET_EPAREN = REG_EPAREN,
	JSTR_REG_RET_EBRACE = REG_EBRACE,
	JSTR_REG_RET_BADBR = REG_BADBR,
	JSTR_REG_RET_ERANGE = REG_ERANGE,
	JSTR_REG_RET_ESPACE = REG_ESPACE,
	JSTR_REG_RET_BADRPT = REG_BADRPT,
/* GNU regcomp returns */
#ifdef REG_RET_EEND
	JSTR_REG_EEND = REG_RET_EEND,
#endif
#ifdef REG_RET_ESIZE
	JSTR_REG_ESIZE = REG_RET_ESIZE,
#endif
#ifdef REG_RET_ERPAREN
	JSTR_REG_ERPAREN = REG_RET_ERPAREN,
#endif
} jstr_reg_errcode_ty;

#define JSTR_REG_HAVE_RET_ENOMEM  1
#define JSTR_REG_HAVE_RET_NOERROR 1
#define JSTR_REG_HAVE_RET_NOMATCH 1

/* POSIX regcomp return values */
#define JSTR_REG_HAVE_RET_BADPAT   1
#define JSTR_REG_HAVE_RET_ECOLLATE 1
#define JSTR_REG_HAVE_RET_ECTYPE   1
#define JSTR_REG_HAVE_RET_EESCAPE  1
#define JSTR_REG_HAVE_RET_ESUBREG  1
#define JSTR_REG_HAVE_RET_EBRACK   1
#define JSTR_REG_HAVE_RET_EPAREN   1
#define JSTR_REG_HAVE_RET_EBRACE   1
#define JSTR_REG_HAVE_RET_BADBR	   1
#define JSTR_REG_HAVE_RET_ERANGE   1
#define JSTR_REG_HAVE_RET_ESPACE   1
#define JSTR_REG_HAVE_RET_BADRPT   1

/* GNU regcomp returns */
#ifdef JSTR_REG_RET_EEND
#	define JSTR_REG_HAVE_EEND 1
#endif
#ifdef JSTR_REG_RET_ESIZE
#	define JSTR_REG_HAVE_ESIZE 1
#endif
#ifdef JSTR_REG_RET_ERPAREN
#	define JSTR_REG_HAVE_ERPAREN 1
#endif

PJSTR_BEGIN_DECLS

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_reg_free(regex_t *R const preg)
{
	regfree(preg);
}

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_reg_error(const int reg_errcode,
	       const regex_t *R const preg) JSTR_NOEXCEPT
{
	if (jstr_unlikely(reg_errcode == JSTR_REG_RET_ENOMEM)) {
		fprintf(stderr, "cannot allocate memory\n");
		return;
	}
	char buf[64];
	regerror(reg_errcode, preg, buf, 32);
	fprintf(stderr, "%s\n", buf);
}

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_comp(regex_t *R const preg,
	      const char *R const ptn,
	      const int cflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = (jstr_reg_errcode_ty)regcomp(preg, ptn, cflags);
	switch (ret) {
	default:
#if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, preg);
#endif /* JSTR_ERR_MSG_ON_REGEX_ERROR */
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return ret;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_NOTHROW
static jstr_reg_errcode_ty
jstr_reg_exec(const regex_t *R preg,
	      const char *R const s,
	      const size_t nmatch,
	      regmatch_t *R const pmatch,
	      const int eflags)
{
	const jstr_reg_errcode_ty ret = (jstr_reg_errcode_ty)regexec(preg, s, nmatch, pmatch, eflags);
	switch (ret) {
	default:
#if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, preg);
#endif /* JSTR_ERR_MSG_ON_REGEX_ERROR */
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return ret;
}

#ifdef JSTR_REG_EF_STARTEND

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_exec_len(const regex_t *R preg,
		  const char *R const s,
		  const size_t sz,
		  const size_t nmatch,
		  regmatch_t *R const pmatch,
		  const int eflags)
{
	pmatch->rm_so = 0;
	pmatch->rm_eo = sz;
	const jstr_reg_errcode_ty ret = (jstr_reg_errcode_ty)regexec(preg, s, nmatch, pmatch, eflags | REG_STARTEND);
	switch (ret) {
	default:
#	if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, preg);
#	endif
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return ret;
}

#endif /* JSTR_REG_EF_ REG_STARTEND */

#ifdef JSTR_REG_EF_STARTEND
#	define PJSTR_REG_EXEC(preg, s, sz, nmatch, pmatch, eflags) \
		jstr_reg_exec_len(preg, s, sz, nmatch, pmatch, eflags)
#else
#	define PJSTR_REG_EXEC(preg, s, sz, nmatch, pmatch, eflags) \
		jstr_reg_exec(preg, s, nmatch, pmatch, eflags)
#endif /* JSTR_REG_EF_STARTEND */

/*
   Checks if S matches precompiled regex.
   Returns return value of regexec.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match(const char *R const s,
	       regex_t *R const preg,
	       const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(preg, s, 0, NULL, eflags | JSTR_REG_EF_NOSUB);
}

/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match_now(const char *R const s,
		   const char *R const ptn,
		   regex_t *R const preg,
		   const int cflags,
		   const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_match(s, preg, eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search(const char *R const s,
		regex_t *R const preg,
		regmatch_t *R const pmatch,
		const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(preg, s, 1, pmatch, eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec or regcomp if it fails.
   Stores offset of matched pattern in pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_now(const char *R const s,
		    const char *R const ptn,
		    regex_t *R const preg,
		    regmatch_t *R const pmatch,
		    const int cflags,
		    const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_search(s, preg, pmatch, eflags);
}

#ifdef JSTR_REG_EF_STARTEND

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in pmatch.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_len_now(const char *R const s,
			const size_t sz,
			regex_t *R const preg,
			regmatch_t *R const pmatch,
			const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec_len(preg, s, sz, 1, pmatch, eflags | JSTR_REG_EF_NOSUB);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in pmatch.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_len(const char *R const s,
		    const char *R const ptn,
		    const size_t sz,
		    regex_t *R const preg,
		    regmatch_t *R const pmatch,
		    const int cflags,
		    const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_search_len_now(s, sz, preg, pmatch, eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match_len(const char *R const s,
		   const size_t sz,
		   regex_t *R const preg,
		   const int eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstr_reg_exec_len(preg, s, sz, 0, &rm, eflags | JSTR_REG_EF_NOSUB | JSTR_REG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static jstr_reg_errcode_ty
jstr_reg_match_len_now(const char *R const s,
		       const char *R const ptn,
		       const size_t sz,
		       regex_t *R const preg,
		       const int cflags,
		       const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_match_len(s, sz, preg, eflags);
}

#endif /* JSTR_REG_EF_STARTEND */

#ifdef __clang__
#	pragma clang diagnostic ignored "-Wunknown-warning-option"
#	pragma clang diagnostic push
#elif defined __GNUC__
#	pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#	pragma GCC diagnostic push
#endif

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rm(char *R const s,
	    size_t *R const sz,
	    const regex_t *R const preg,
	    const int eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const jstr_reg_errcode_ty ret = PJSTR_REG_EXEC(preg, s, *sz, 1, &rm, eflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR)
	    || jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	memmove(s + rm.rm_so,
		s + rm.rm_eo,
		*sz - (rm.rm_eo - rm.rm_so) + 1);
	*sz -= (rm.rm_eo - rm.rm_so);
	return ret;
}

#ifdef __clang__
#	pragma clang diagnostic pop
#elif defined __GNUC__
#	pragma GCC diagnostic pop
#endif

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rm_now(char *R const s,
		size_t *R const sz,
		const char *R const ptn,
		regex_t *R const preg,
		const int cflags,
		const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rm(s, sz, preg, eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
pjstr_reg_base_rmall(const pjstr_flag_use_n_ty flag,
		     char *R const s,
		     size_t *R const sz,
		     size_t n,
		     const regex_t *R const preg,
		     const int eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *p = dst;
	const unsigned char *old = dst;
	const unsigned char *const end = dst + *sz;
	size_t findlen;
	jstr_reg_errcode_ty ret = JSTR_REG_RET_NOMATCH;
	while ((flag & PJSTR_FLAG_USE_N ? n-- : 1)
	       && PJSTR_REG_EXEC(preg, (char *)p, end - p, 1, &rm, eflags) == JSTR_REG_RET_NOERROR) {
		ret = JSTR_REG_RET_NOERROR;
		findlen = rm.rm_eo - rm.rm_so;
		p = p + rm.rm_so;
		if (jstr_unlikely(findlen == 0))
			++p;
		else
			PJSTR_RMALL_IN_PLACE(dst, old, p, findlen);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != old)) {
		memmove(dst, old, end - old);
		dst[end - old] = '\0';
	}
	*sz = (char *)dst + (end - old) - s;
	return ret;
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rmall(char *R const s,
	       size_t *R const sz,
	       const regex_t *R const preg,
	       const int eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rmall(PJSTR_FLAG_USE_NOT_N, s, sz, 0, preg, eflags);
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rmn(char *R const s,
	     size_t *R const sz,
	     const size_t n,
	     const regex_t *R const preg,
	     const int eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rmall(PJSTR_FLAG_USE_N, s, sz, n, preg, eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rmn_now(char *R const s,
		 const char *R const ptn,
		 size_t *R const sz,
		 const size_t n,
		 regex_t *R const preg,
		 const int cflags,
		 const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rmn(s, sz, n, preg, eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rmall_now(char *R const s,
		   size_t *R const sz,
		   const char *R const ptn,
		   regex_t *R const preg,
		   const int cflags,
		   const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rmall(s, sz, preg, eflags);
}

JSTR_FUNC
static jstr_reg_errcode_ty
pjstr_reg_base_rplcall_len(const pjstr_flag_use_n_ty flag,
			   char *R *R const s,
			   size_t *R const sz,
			   size_t *R const cap,
			   const char *R const rplc,
			   size_t n,
			   const size_t rplclen,
			   const regex_t *R const preg,
			   const int eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == 0)) {
		if (flag & PJSTR_FLAG_USE_N)
			return jstr_reg_rmn(*s, sz, n, preg, eflags);
		return jstr_reg_rmall(*s, sz, preg, eflags);
	}
	regmatch_t rm;
	size_t findlen;
	unsigned char *tmp;
	jstr_reg_errcode_ty ret = JSTR_REG_RET_NOMATCH;
	typedef unsigned char u;
	unsigned char *dst = *(u **)s;
	unsigned char *p = dst;
	const unsigned char *old = dst;
	while ((flag & PJSTR_FLAG_USE_N ? n-- : 1)
	       && *p
	       && PJSTR_REG_EXEC(preg, (char *)p, (*(u **)s + *sz) - p, 1, &rm, eflags) == JSTR_REG_RET_NOERROR) {
		ret = JSTR_REG_RET_NOERROR;
		findlen = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(findlen == 0)) {
			PJSTR_REG_LOG("jstr_unlikely(findlen == 0)");
			if (jstr_unlikely(*++p == '\0'))
				break;
			continue;
		}
#define PJSTR_REG_RPLCALL_SMALL_RPLC(dst, old, p, rplc, rplclen, findlen, tmp)     \
	do {                                                                       \
		PJSTR_REG_LOG("*cap > *sz + rplclen - findlen");                   \
		if (dst != old) {                                                  \
			PJSTR_REG_LOG("dst != old");                               \
			memmove(dst, old, p - old);                                \
			dst += (p - old);                                          \
			memmove(dst + rplclen,                                     \
				p + findlen,                                       \
				(*(unsigned char **)s + *sz) - (p + findlen) + 1); \
			memcpy(dst, rplc, rplclen);                                \
			dst += rplclen;                                            \
			old = dst;                                                 \
		} else {                                                           \
			PJSTR_REG_LOG("dst == old");                               \
			memmove(p + rplclen,                                       \
				p + findlen,                                       \
				(*(unsigned char **)s + *sz) - (p + findlen) + 1); \
			memcpy(p, rplc, rplclen);                                  \
		}                                                                  \
		*sz += rplclen - findlen;                                          \
		p += rplclen;                                                      \
	} while (0)
#define PJSTR_REG_RPLCALL_BIG_RPLC(dst, old, p, rplc, rplclen, findlen, tmp, malloc_fail) \
	do {                                                                              \
		PJSTR_REG_LOG("cap <= *sz + rplclen - findlen");                          \
		if (dst != old)                                                           \
			memmove(dst, old, p - old);                                       \
		tmp = *(unsigned char **)s;                                               \
		PJSTR_REALLOC(*s, *cap, *sz + rplclen - findlen, malloc_fail);            \
		memmove(p + rplclen,                                                      \
			p + findlen,                                                      \
			(tmp + *sz) - (p + findlen) + 1);                                 \
		memcpy(p, rplc, rplclen);                                                 \
		p = *(unsigned char **)s + (p - tmp);                                     \
		dst = *(unsigned char **)s + (dst - tmp) + rplclen;                       \
		old = dst;                                                                \
		*sz += rplclen - findlen;                                                 \
		p += rplclen;                                                             \
	} while (0)
		if (rplclen <= findlen) {
			PJSTR_REG_LOG("rplclen <= findlen");
			PJSTR_RPLCALL_IN_PLACE(dst, old, p, rplc, rplclen, findlen);
		} else if (*cap > *sz + rplclen - findlen) {
			PJSTR_REG_LOG("*cap > *sz + rplclen - findlen");
			PJSTR_REG_RPLCALL_SMALL_RPLC(dst, old, p, rplc, rplclen, findlen, tmp);
		} else {
			PJSTR_REG_LOG("else");
			PJSTR_REG_RPLCALL_BIG_RPLC(dst, old, p, rplc, rplclen, findlen, tmp, return JSTR_REG_RET_ENOMEM);
		}
	}
	if (dst != old) {
		memmove(dst, old, (*(u **)s + *sz) - old);
		dst[(*(u **)s + *sz) - old] = '\0';
		*sz = (dst + (*(u **)s + *sz - old)) - *(u **)s;
	}
	return ret;
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_len(char *R *R const s,
		     size_t *R const sz,
		     size_t *R const cap,
		     const char *R const rplc,
		     const size_t rplclen,
		     const regex_t *R const preg,
		     const int eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len(PJSTR_FLAG_USE_NOT_N, s, sz, cap, rplc, 0, rplclen, preg, eflags);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcall(char *R *R const s,
		 size_t *R const sz,
		 size_t *R const cap,
		 const char *R const rplc,
		 const regex_t *R const preg,
		 const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplcall_len(s, sz, cap, rplc, strlen(rplc), preg, eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_len(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const rplc,
		   const size_t n,
		   const size_t rplclen,
		   const regex_t *R const preg,
		   const int eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len(PJSTR_FLAG_USE_N, s, sz, cap, rplc, n, rplclen, preg, eflags);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcn(char *R *R const s,
	       size_t *R const sz,
	       size_t *R const cap,
	       const char *R const rplc,
	       const size_t n,
	       const regex_t *R const preg,
	       const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplcn_len(s, sz, cap, rplc, n, strlen(rplc), preg, eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_len_now(char *R *R const s,
			 size_t *R const sz,
			 size_t *R const cap,
			 const char *R const ptn,
			 const char *R const rplc,
			 const size_t rplclen,
			 regex_t *R const preg,
			 const int cflags,
			 const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplcall_len(s, sz, cap, rplc, rplclen, preg, eflags);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcall_now(char *R *R const s,
		     size_t *R const sz,
		     size_t *R const cap,
		     const char *R const ptn,
		     const char *R const rplc,
		     regex_t *R const preg,
		     const int cflags,
		     const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplcall_len_now(s, sz, cap, ptn, rplc, strlen(rplc), preg, cflags, eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_len_now(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const ptn,
		       const char *R const rplc,
		       const size_t n,
		       const size_t rplclen,
		       regex_t *R const preg,
		       const int cflags,
		       const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplcn_len(s, sz, cap, rplc, n, rplclen, preg, eflags);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcn_now(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const ptn,
		   const char *R const rplc,
		   const size_t n,
		   regex_t *R const preg,
		   const int cflags,
		   const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplcn_len_now(s, sz, cap, ptn, rplc, n, strlen(rplc), preg, cflags, eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplc_len(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const rplc,
		  const size_t rplclen,
		  const regex_t *R const preg,
		  const int eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const jstr_reg_errcode_ty ret = PJSTR_REG_EXEC(preg, *s, *sz, 1, &rm, eflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR)
	    || jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	if (jstr_unlikely(pjstr_rplcat_len(s, sz, cap, rm.rm_so, rplc, rplclen, rm.rm_eo - rm.rm_so) == NULL))
		return JSTR_REG_RET_ENOMEM;
	return ret;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplc(char *R *R const s,
	      size_t *R const sz,
	      size_t *R const cap,
	      const char *R const rplc,
	      const regex_t *R const preg,
	      const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplc_len(s, sz, cap, rplc, strlen(rplc), preg, eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rplc_len_now(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const ptn,
		      const char *R const rplc,
		      const size_t rplclen,
		      regex_t *R const preg,
		      const int cflags,
		      const int eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplc_len(s, sz, cap, rplc, rplclen, preg, eflags);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplc_now(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const ptn,
		  const char *R const rplc,
		  regex_t *R const preg,
		  const int cflags,
		  const int eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rplc_len_now(s, sz, cap, ptn, rplc, strlen(rplc), preg, cflags, eflags);
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplc_len_bref(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const rplc,
		       size_t rplclen,
		       const regex_t *R const preg,
		       const int eflags,
		       const size_t nmatch) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == 0))
		return jstr_reg_rm(*s, sz, preg, eflags);
	regmatch_t rm[10];
	jstr_reg_errcode_ty ret = PJSTR_REG_EXEC(preg, *s, *sz, nmatch, rm, eflags);
	const size_t findlen = rm[0].rm_eo - rm[0].rm_so;
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR)
	    || jstr_unlikely(findlen == 0))
		return ret;
	const unsigned char *const rend = (unsigned char *)rplc + rplclen;
	size_t rdstlen = rplclen;
	unsigned char no_bref = 1;
	for (const unsigned char *rsrc = (unsigned char *)rplc;
	     (rsrc = (unsigned char *)memchr(rsrc, '\\', rend - rsrc));
	     ++rsrc) {
		if (jstr_likely(jstr_isdigit(*++rsrc)))
			rdstlen = rdstlen + ((rm[*rsrc - '0'].rm_eo - rm[*rsrc - '0'].rm_so)) - 2;
		else if (jstr_unlikely(*rsrc == '\0')) {
			no_bref = 0;
			break;
		}
	}
	if (jstr_unlikely(no_bref))
		return jstr_reg_rplc_len(s, sz, cap, rplc, rplclen, preg, eflags);
	unsigned char *rdst;
	unsigned char rdst_stack[256];
	if (jstr_unlikely(rdstlen > 256)) {
		rdst = (unsigned char *)malloc(rdstlen);
		PJSTR_MALLOC_ERR(rdst, return JSTR_REG_RET_ENOMEM);
	} else {
		rdst = rdst_stack;
	}
#define PJSTR_CREAT_RPLC_BREF(ptr_passed_to_reg, rplc_dst, rplc_len, rplc, rplc_end)   \
	do {                                                                           \
		unsigned char *rp = rplc_dst;                                          \
		const unsigned char *rold;                                             \
		const unsigned char *rsrc = (unsigned char *)rplc;                     \
		for (;; ++rsrc) {                                                      \
			rold = rsrc;                                                   \
			rsrc = (unsigned char *)memchr(rsrc, '\\', rplc_end - rsrc);   \
			if (jstr_unlikely(rsrc == NULL)) {                             \
				memcpy(rp, rold, rplc_end - rold);                     \
				break;                                                 \
			}                                                              \
			if (jstr_likely(jstr_isdigit(*++rsrc))) {                      \
				if (jstr_likely(rsrc != rold)) {                       \
					memmove(rp, rold, (rsrc - 1) - rold);          \
					rp += (rsrc - 1) - rold;                       \
					rold += (rsrc - 1) - rold;                     \
					rsrc += (rsrc - 1) - rold;                     \
				}                                                      \
				memcpy(rp,                                             \
				       ptr_passed_to_reg + rm[*rsrc - '0'].rm_so,      \
				       rm[*rsrc - '0'].rm_eo - rm[*rsrc - '0'].rm_so); \
				rp += rm[*rsrc - '0'].rm_eo - rm[*rsrc - '0'].rm_so;   \
			} else if (jstr_unlikely(*rsrc == '\0')) {                     \
				break;                                                 \
			} else {                                                       \
				rp[0] = rsrc[-1];                                      \
				rp[1] = rsrc[0];                                       \
				rp += 2;                                               \
			}                                                              \
		}                                                                      \
	} while (0)
	PJSTR_CREAT_RPLC_BREF(*s, rdst, rdstlen, rplc, rend);
	if (jstr_unlikely(pjstr_rplcat_len(s, sz, cap, rm[0].rm_so, (char *)rdst, rdstlen, findlen) == NULL))
		ret = JSTR_REG_RET_ENOMEM;
	if (rdst != rdst_stack)
		free(rdst);
	return ret;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplc_bref(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const rplc,
		   const regex_t *R const preg,
		   const int eflags,
		   const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplc_len_bref(s, sz, cap, rplc, strlen(rplc), preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
pjstr_reg_base_rplcall_len_bref(const pjstr_flag_use_n_ty nflag,
				char *R *R const s,
				size_t *R const sz,
				size_t *R const cap,
				const char *R const rplc,
				size_t n,
				size_t rplclen,
				const regex_t *R const preg,
				const int eflags,
				const size_t nmatch) JSTR_NOEXCEPT
{
	if (jstr_unlikely(rplclen == 0)) {
		if (nflag & PJSTR_FLAG_USE_N)
			return jstr_reg_rmn(*s, sz, n, preg, eflags);
		return jstr_reg_rmall(*s, sz, preg, eflags);
	}
	regmatch_t rm[10];
	jstr_reg_errcode_ty ret = JSTR_REG_RET_NOERROR;
	size_t rdstlen = rplclen;
	size_t rdstcap = 0;
	unsigned char rdst_stack[256];
	unsigned char *rdst = NULL;
	unsigned char *rdstp;
	typedef unsigned char u;
	unsigned char *dst = *(u **)s;
	const unsigned char *old = dst;
	unsigned char *p = dst;
	const unsigned char *const rend = (u *)rplc + rplclen;
	unsigned char *tmp;
	size_t findlen;
	while (((nflag & PJSTR_FLAG_USE_N) ? n-- : 1)
	       && *p
	       && PJSTR_REG_EXEC(preg, (char *)p, (*(u **)s + *sz) - p, nmatch, rm, eflags) != JSTR_REG_RET_NOERROR) {
		ret = JSTR_REG_RET_NOERROR;
		findlen = rm[0].rm_eo - rm[0].rm_so;
		if (jstr_unlikely(findlen == 0)) {
			++p;
			continue;
		}
		p += rm[0].rm_so;
		rdstlen = rplclen;
		for (const unsigned char *rsrc = (u *)rplc;
		     *rsrc
		     && (rsrc = (u *)memchr(rsrc, '\\', rend - rsrc));
		     ++rsrc)
			if (jstr_likely(jstr_isdigit(*++rsrc)))
				rdstlen = rdstlen + (rm[*rsrc - '0'].rm_eo - rm[*rsrc - '0'].rm_so) - 2;
		if (jstr_unlikely(rdstlen > 256)) {
			if (jstr_unlikely(rdst == NULL)) {
				rdstcap = PJSTR_PTR_ALIGN_UP(rdstlen, PJSTR_MALLOC_ALIGNMENT);
				rdst = (u *)malloc(rdstcap);
				PJSTR_MALLOC_ERR(rdst, return JSTR_REG_RET_NOERROR);
			} else if (rdstcap < rdstlen) {
				PJSTR_GROW(rdstcap, rdstlen);
				rdst = (u *)realloc(rdst, rdstcap);
				PJSTR_MALLOC_ERR(rdst, return JSTR_REG_RET_NOERROR);
			}
			rdstp = rdst;
		} else {
			rdstp = rdst_stack;
		}
		PJSTR_CREAT_RPLC_BREF(p - rm[0].rm_so, rdstp, rdstlen, rplc, rend);
		if (rdstlen <= findlen)
			PJSTR_RPLCALL_IN_PLACE(dst, old, p, rdstp, rdstlen, findlen);
		else if (*cap > *sz + rdstlen - findlen)
			PJSTR_REG_RPLCALL_SMALL_RPLC(dst, old, p, rdstp, rdstlen, findlen, tmp);
		else
			PJSTR_REG_RPLCALL_BIG_RPLC(dst, old, p, rdstp, rdstlen, findlen, tmp, ret = JSTR_REG_RET_ENOMEM; goto cleanup);
	}
	if (dst != old) {
		memmove(dst, old, (*(u **)s + *sz) - old);
		dst[(*(u **)s + *sz) - old] = '\0';
		*sz = (dst + (*(u **)s + *sz - old)) - *(u **)s;
	}
cleanup:
	if (rdst)
		free(rdst);
	return ret;
#undef PJSTR_CREAT_RPLC_BREF
#undef PJSTR_RPLCALL_BREF
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_len_bref(char *R *R const s,
			  size_t *R const sz,
			  size_t *R const cap,
			  const char *R const rplc,
			  size_t rplclen,
			  const regex_t *R const preg,
			  const int eflags,
			  const size_t nmatch) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len_bref(PJSTR_FLAG_USE_NOT_N, s, sz, cap, rplc, 0, rplclen, preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcall_bref(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const rplc,
		      const regex_t *R const preg,
		      const int eflags,
		      const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplcall_len_bref(s, sz, cap, rplc, strlen(rplc), preg, eflags, nmatch);
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_len_bref(char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const char *R const rplc,
			size_t n,
			size_t rplclen,
			const regex_t *R const preg,
			const int eflags,
			const size_t nmatch) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len_bref(PJSTR_FLAG_USE_N, s, sz, cap, rplc, n, rplclen, preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcn_bref(char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const char *R const rplc,
		    size_t n,
		    const regex_t *R const preg,
		    const int eflags,
		    const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplcn_len_bref(s, sz, cap, rplc, n, strlen(rplc), preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_len_bref_now(char *R *R const s,
			    size_t *R const sz,
			    size_t *R const cap,
			    const char *R const ptn,
			    const char *R const rplc,
			    size_t n,
			    size_t rplclen,
			    regex_t *R const preg,
			    const int cflags,
			    const int eflags,
			    const size_t nmatch) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplcn_len_bref(s, sz, cap, rplc, n, rplclen, preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcn_bref_now(char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const char *R const ptn,
			const char *R const rplc,
			size_t n,
			regex_t *R const preg,
			const int cflags,
			const int eflags,
			const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplcn_len_bref_now(s, sz, cap, ptn, rplc, n, strlen(rplc), preg, cflags, eflags, nmatch);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_len_bref_now(char *R *R const s,
			      size_t *R const sz,
			      size_t *R const cap,
			      const char *R const ptn,
			      const char *R const rplc,
			      size_t rplclen,
			      regex_t *R const preg,
			      const int cflags,
			      const int eflags,
			      const size_t nmatch) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplcall_len_bref(s, sz, cap, rplc, rplclen, preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplcall_bref_now(char *R *R const s,
			  size_t *R const sz,
			  size_t *R const cap,
			  const char *R const ptn,
			  const char *R const rplc,
			  regex_t *R const preg,
			  const int cflags,
			  const int eflags,
			  const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplcall_len_bref_now(s, sz, cap, ptn, rplc, strlen(rplc), preg, cflags, eflags, nmatch);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplc_len_bref_now(char *R *R const s,
			   size_t *R const sz,
			   size_t *R const cap,
			   const char *R const ptn,
			   const char *R const rplc,
			   size_t rplclen,
			   regex_t *R const preg,
			   const int cflags,
			   const int eflags,
			   const size_t nmatch) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty ret = jstr_reg_comp(preg, ptn, cflags);
	if (jstr_unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_rplc_len_bref(s, sz, cap, rplc, rplclen, preg, eflags, nmatch);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static jstr_reg_errcode_ty
jstr_reg_rplc_bref_now(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const ptn,
		       const char *R const rplc,
		       regex_t *R const preg,
		       const int cflags,
		       const int eflags,
		       const size_t nmatch) JSTR_NOEXCEPT
{
	return jstr_reg_rplc_len_bref_now(s, sz, cap, ptn, rplc, strlen(rplc), preg, cflags, eflags, nmatch);
}

PJSTR_END_DECLS

#undef R

#undef PJSTR_REG_EXEC
#undef PJSTR_REG_LOG
#undef JSTR_REG_DEBUG

#undef PJSTR_REG_RPLCALL_SMALL_RPLC
#undef PJSTR_REG_RPLCALL_BIG_RPLC

#endif /* JSTR_REGEX_H */
