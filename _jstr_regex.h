#ifndef JSTR_REGEX_H_DEF
#define JSTR_REGEX_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_config.h"
#include "_jstr_macros.h"
#include "_jstr_replace.h"
#include "_jstr_string.h"

#define JSTR_RST JSTR_RESTRICT

/* POSIX _cflags */
#define JSTR_REG_CF_EXTENDED REG_EXTENDED
#define JSTR_REG_CF_ICASE    REG_ICASE
#define JSTR_REG_CF_NEWLINE  REG_NEWLINE

/* POSIX _eflags */
#define JSTR_REG_EF_NOSUB  REG_NOSUB
#define JSTR_REG_EF_NOTBOL REG_NOTBOL
#define JSTR_REG_EF_NOTEOL REG_NOTEOL

/* BSD extension */
#ifdef REG_STARTEND
#	define JSTR_REG_EF_STARTEND REG_STARTEND
#endif /* REG_STARTEND */

typedef enum {
	JSTR_REG_RET_MALLOC_ERROR,
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
} Jstr_reg_errcode;

#define JSTR_HAVE_REG_RET_MALLOC_ERROR 1
#define JSTR_HAVE_REG_RET_NOERROR      1
#define JSTR_HAVE_REG_RET_NOMATCH      1

/* POSIX regcomp return values */
#define JSTR_HAVE_REG_RET_BADPAT   1
#define JSTR_HAVE_REG_RET_ECOLLATE 1
#define JSTR_HAVE_REG_RET_ECTYPE   1
#define JSTR_HAVE_REG_RET_EESCAPE  1
#define JSTR_HAVE_REG_RET_ESUBREG  1
#define JSTR_HAVE_REG_RET_EBRACK   1
#define JSTR_HAVE_REG_RET_EPAREN   1
#define JSTR_HAVE_REG_RET_EBRACE   1
#define JSTR_HAVE_REG_RET_BADBR	   1
#define JSTR_HAVE_REG_RET_ERANGE   1
#define JSTR_HAVE_REG_RET_ESPACE   1
#define JSTR_HAVE_REG_RET_BADRPT   1

/* GNU regcomp returns */
#ifdef JSTR_REG_RET_RET_EEND
#	define JSTR_HAVE_REG_EEND 1
#endif
#ifdef JSTR_REG_RET_RET_ESIZE
#	define JSTR_HAVE_REG_ESIZE 1
#endif
#ifdef JSTR_REG_RET_RET_ERPAREN
#	define JSTR_HAVE_REG_ERPAREN 1
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_NONNULL_ALL
JSTR_INLINE
static void
jstr_reg_free(regex_t *JSTR_RST const _preg)
{
	regfree(_preg);
}

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
static void
jstr_reg_error(const int _reg_errcode,
	       const regex_t *_preg) JSTR_NOEXCEPT
{
	char buf[64];
	regerror(_reg_errcode, _preg, buf, 32);
	fputs(buf, stderr);
	fputc('\n', stderr);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_CONST
static Jstr_reg_errcode
jstr_reg_comp(regex_t *JSTR_RST const _preg,
	      const char *JSTR_RST const _ptn,
	      const int _cflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = (Jstr_reg_errcode)regcomp(_preg, _ptn, _cflags);
	switch (err) {
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(err, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	}
	return err;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_CONST
static Jstr_reg_errcode
jstr_reg_exec(const regex_t *JSTR_RST _preg,
	      const char *JSTR_RST const s,
	      size_t nmatch,
	      regmatch_t *JSTR_RST const _pmatch,
	      const int _eflags)
{
	const Jstr_reg_errcode err = (Jstr_reg_errcode)regexec(_preg, s, nmatch, _pmatch, _eflags);
	switch (err) {
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(err, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
		return err;
	case JSTR_REG_RET_NOMATCH: return (Jstr_reg_errcode)JSTR_REG_RET_NOMATCH;
	case JSTR_REG_RET_NOERROR: return (Jstr_reg_errcode)JSTR_REG_RET_NOERROR;
	}
}

#ifdef JSTR_REG_EF_STARTEND

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_exec_mem(const regex_t *JSTR_RST _preg,
		  const char *JSTR_RST const s,
		  const size_t sz,
		  size_t nmatch,
		  regmatch_t *JSTR_RST const _pmatch,
		  const int _eflags)
{
	_pmatch->rm_so = 0;
	_pmatch->rm_eo = sz;
	const Jstr_reg_errcode err = (Jstr_reg_errcode)regexec(_preg, s, nmatch, _pmatch, _eflags | REG_STARTEND);
	switch (err) {
	default:
#	if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(err, _preg);
#	endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
		return err;
	case JSTR_REG_RET_NOMATCH: return (Jstr_reg_errcode)JSTR_REG_RET_NOMATCH;
	case JSTR_REG_RET_NOERROR: return (Jstr_reg_errcode)JSTR_REG_RET_NOERROR;
	}
}

#endif /* JSTR_REG_EF_ REG_STARTEND */

#ifdef JSTR_REG_EF_STARTEND
#	define PRIVATE_JSTR_REG_EXEC(_preg, s, sz, nmatch, _pmatch, _eflags) \
		jstr_reg_exec_mem(_preg, s, sz, nmatch, _pmatch, _eflags)
#else
#	define PRIVATE_JSTR_REG_EXEC(_preg, s, sz, nmatch, _pmatch, _eflags) \
		jstr_reg_exec(_preg, s, nmatch, _pmatch, _eflags)
#endif /* JSTR_REG_EF_STARTEND */

/*
   Checks if S matches precompiled regex.
   Returns return value of regexec.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_match(const char *JSTR_RST const s,
	       regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(_preg, s, 0, NULL, _eflags | JSTR_REG_EF_NOSUB);
}

/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_match_now(const char *JSTR_RST const s,
		   const char *JSTR_RST const _ptn,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_match(s, _preg, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_search(const char *JSTR_RST const s,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(_preg, s, 1, _pmatch, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec or regcomp if it fails.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_search_now(const char *JSTR_RST const s,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, s, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_search(s, _preg, _pmatch, _eflags);
}

#ifdef JSTR_REG_EF_STARTEND

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_search_now_mem(const char *JSTR_RST const s,
			const size_t sz,
			regex_t *JSTR_RST const _preg,
			regmatch_t *JSTR_RST const _pmatch,
			const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec_mem(_preg, s, sz, 1, _pmatch, _eflags | JSTR_REG_EF_NOSUB);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_search_mem(const char *JSTR_RST const s,
		    const char *JSTR_RST const _ptn,
		    const size_t sz,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_search_now_mem(s, sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_match_mem(const char *JSTR_RST const s,
		   const size_t sz,
		   regex_t *JSTR_RST const _preg,
		   const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstr_reg_exec_mem(_preg, s, sz, 0, &rm, _eflags | JSTR_REG_EF_NOSUB | JSTR_REG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_match_now_mem(const char *JSTR_RST const s,
		       const char *JSTR_RST const _ptn,
		       const size_t sz,
		       regex_t *JSTR_RST const _preg,
		       const int _cflags,
		       const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_match_mem(s, sz, _preg, _eflags);
}

#endif /* JSTR_REG_EF_STARTEND */

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jstr_reg_rm_mem(char *JSTR_RST const s,
		size_t *JSTR_RST sz,
		const regex_t *JSTR_RST const _preg,
		const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const Jstr_reg_errcode err = PRIVATE_JSTR_REG_EXEC(_preg, s, *sz, 1, &rm, _eflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return err;
	memmove(s + rm.rm_so,
		s + rm.rm_eo,
		*sz - (rm.rm_eo - rm.rm_so) + 1);
	*sz -= (rm.rm_eo - rm.rm_so);
	return err;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jstr_reg_rm_now_mem(char *JSTR_RST const s,
		    size_t *JSTR_RST sz,
		    const char *JSTR_RST const _ptn,
		    regex_t *JSTR_RST const _preg,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_rm_mem(s, sz, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jstr_reg_rm(char *JSTR_RST const s,
	    size_t *JSTR_RST sz,
	    regex_t *JSTR_RST const _preg,
	    const int _eflags) JSTR_NOEXCEPT
{
	*sz = strlen(s);
	return jstr_reg_rm_mem(s, sz, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jstr_reg_rm_now(char *JSTR_RST const s,
		size_t *JSTR_RST sz,
		const char *JSTR_RST const _ptn,
		regex_t *JSTR_RST const _preg,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	*sz = strlen(s);
	return jstr_reg_rm_now_mem(s, sz, _ptn, _preg, _cflags, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static Jstr_reg_errcode
jstr_reg_rmall_mem(char *JSTR_RST const s,
		   size_t *JSTR_RST sz,
		   const regex_t *JSTR_RST const _preg,
		   const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *p = dst;
	const unsigned char *old = dst;
	const unsigned char *const end = dst + *sz;
	size_t ptnlen;
	Jstr_reg_errcode err;
	while ((err = PRIVATE_JSTR_REG_EXEC(_preg, (char *)p, end - p, 1, &rm, _eflags)) == JSTR_REG_RET_NOERROR) {
		ptnlen = rm.rm_eo - rm.rm_so;
		if (jstr_unlikely(!ptnlen)) {
			p += ptnlen + 1;
			if (jstr_unlikely(!*p))
				break;
			continue;
		}
		p = p + rm.rm_so;
		if (jstr_likely(dst != old))
			memmove(dst, old, p - old);
		dst += (p - old);
		old += (p - old);
		old += ptnlen;
		p += ptnlen + 1;
	}
	if (jstr_likely(dst != old))
		memmove(dst, old, end - old + 1);
	*sz = (char *)dst + (end - old) - s;
	return err;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jstr_reg_rmall_now_mem(char *JSTR_RST const s,
		       size_t *JSTR_RST sz,
		       const char *JSTR_RST const _ptn,
		       regex_t *JSTR_RST const _preg,
		       const int _cflags,
		       const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rmall_now(char *JSTR_RST const s,
		   size_t *JSTR_RST sz,
		   const char *JSTR_RST const _ptn,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	*sz = strlen(s);
	return jstr_reg_rmall_now_mem(s, sz, _ptn, _preg, _cflags, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rmall(char *JSTR_RST const s,
	       size_t *JSTR_RST sz,
	       regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	*sz = strlen(s);
	return jstr_reg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rplcall_mem(char **JSTR_RST const s,
		     size_t *JSTR_RST const sz,
		     size_t *JSTR_RST const cap,
		     const char *JSTR_RST const _rplc,
		     const size_t _rplclen,
		     const regex_t *JSTR_RST const _preg,
		     const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		return jstr_reg_rmall_mem(*s, sz, _preg, _eflags);
	}
	regmatch_t rm;
#if 0
	size_t _ptnlen;
	unsigned char *tmp;
#endif
	unsigned char *dst = (unsigned char *)*s;
	unsigned char *p = dst;
	const unsigned char *old = dst;
	Jstr_reg_errcode err;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((err = PRIVATE_JSTR_REG_EXEC(_preg, (char *)p, (*(unsigned char **)s + *sz) - p, 1, &rm, _eflags)) == JSTR_REG_RET_NOERROR) {
#if 1
#	if JSTR_HAVE_REALLOC_MREMAP
		if (jstr_unlikely(is_mmap))
			p = (unsigned char *)private_jstr_rplcat_mem_realloc(s, sz, cap, p + rm.rm_so - *(unsigned char **)s, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
		else
			p = (unsigned char *)private_jstr_rplcat_mem_malloc(s, sz, cap, p + rm.rm_so - *(unsigned char **)s, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
#	else
		p = (unsigned char *)jstr_rplcat_mem(s, sz, cap, p + rm.rm_so - *(unsigned char **)s, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
#	endif /* JSTR_HAVE_REALLOC_MREMAP */
#else
		_ptnlen = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(!_ptnlen)) {
			if (jstr_unlikely(!*++p))
				break;
			continue;
		}
		if (_rplclen <= _ptnlen) {
			if (jstr_likely(dst != old))
				memmove(dst, old, p - old);
			dst += (p - old);
			old += (p - old);
			memcpy(dst, _rplc, _rplclen);
			dst += _rplclen;
			old += _ptnlen;
			p += _ptnlen;
			*sz += _rplclen - _ptnlen;
			continue;
		} else if (*cap > *sz + _rplclen - _ptnlen) {
			if (dst != old) {
				memmove(dst, old, p - old);
				dst += (p - old);
				memmove(dst + _rplclen,
					p + _ptnlen,
					(*(unsigned char **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(dst, _rplc, _rplclen);
				dst += _rplclen;
				old = dst;
			} else {
				memmove(p + _rplclen,
					p + _ptnlen,
					(*(unsigned char **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
			}
		} else {
#	if JSTR_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(is_mmap)) {
				if (dst != old) {
					memmove(dst,
						old,
						p - old);
				}
				tmp = *(unsigned char **)s;
				JSTR_REALLOC(*s, *cap, *sz + _rplclen - _ptnlen, return JSTR_REG_RET_MALLOC_ERROR);
				p = *(unsigned char **)s + (p - tmp);
				dst = *(unsigned char **)s + (dst - tmp) + _rplclen;
				old = dst;
				memmove(p + _rplclen,
					p + _ptnlen,
					(*(unsigned char **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
			} else {
#	endif /* JSTR_HAVE_REALLOC_MREMAP */
				JSTR_GROW(*cap, *sz + _rplclen - _ptnlen);
				tmp = (unsigned char *)malloc(*cap);
				JSTR_MALLOC_ERR(tmp, return JSTR_REG_RET_MALLOC_ERROR);
				memcpy(tmp + (p - *(unsigned char **)s),
				       _rplc,
				       _rplclen);
				memcpy(tmp + (p - *(unsigned char **)s) + _rplclen,
				       p + _ptnlen,
				       (*(unsigned char **)s + *sz) - (p + _ptnlen) + 1);
				if (dst != old) {
					memcpy(tmp + (dst - *(unsigned char **)s),
					       old,
					       p - old);
					dst = tmp + (dst - *(unsigned char **)s) + _rplclen;
					old = dst;
				} else {
					memcpy(tmp, *s, p - *(unsigned char **)s);
				}
				free(*s);
				p = tmp + (p - *(unsigned char **)s);
				*s = (char *)tmp;
			}
#	if JSTR_HAVE_REALLOC_MREMAP
		}
#	endif /* JSTR_HAVE_REALLOC_MREMAP */
		*sz += _rplclen - _ptnlen;
		p += _rplclen;
#endif
	}
	if (dst != old)
		memmove(dst, old, (*(unsigned char **)s + *sz) - old + 1);
	return err;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rplcall_now_mem(char **JSTR_RST const s,
			 size_t *JSTR_RST const sz,
			 size_t *JSTR_RST const cap,
			 const char *JSTR_RST const _ptn,
			 const char *JSTR_RST const _rplc,
			 const size_t _rplclen,
			 regex_t *JSTR_RST const _preg,
			 const int _cflags,
			 const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_rplcall_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rplc_mem(char **JSTR_RST const s,
		  size_t *JSTR_RST const sz,
		  size_t *JSTR_RST const cap,
		  const char *JSTR_RST const _rplc,
		  const size_t _rplclen,
		  const regex_t *JSTR_RST const _preg,
		  const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const Jstr_reg_errcode err = PRIVATE_JSTR_REG_EXEC(_preg, *s, *sz, 1, &rm, _eflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return err;
	jstr_rplcat_mem(s, sz, cap, rm.rm_so, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
	return err;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jstr_reg_rplc_now_mem(char **JSTR_RST const s,
		      size_t *JSTR_RST const sz,
		      size_t *JSTR_RST const cap,
		      const char *JSTR_RST const _ptn,
		      const char *JSTR_RST const _rplc,
		      const size_t _rplclen,
		      regex_t *JSTR_RST const _preg,
		      const int _cflags,
		      const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode err = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(err != JSTR_REG_RET_NOERROR))
		return err;
	return jstr_reg_rplc_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#undef PRIVATE_JSTR_REG_EXEC

#endif /* JSTR_REGEX_H_DEF */
