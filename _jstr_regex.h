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
	_JSTR_REG_RET_MALLOC_ERROR,
#ifdef REG_ENOSYS
	JSTR_REG_RET_ENOSYS = REG_ENOSYS,
#endif
	_JSTR_REG_RET_NOERROR = REG_NOERROR,
	_JSTR_REG_RET_NOMATCH = REG_NOMATCH,
	/* POSIX regcomp return values */
	_JSTR_REG_RET_BADPAT = REG_BADPAT,
	_JSTR_REG_RET_ECOLLATE = REG_ECOLLATE,
	_JSTR_REG_RET_ECTYPE = REG_ECTYPE,
	_JSTR_REG_RET_EESCAPE = REG_EESCAPE,
	_JSTR_REG_RET_ESUBREG = REG_ESUBREG,
	_JSTR_REG_RET_EBRACK = REG_EBRACK,
	_JSTR_REG_RET_EPAREN = REG_EPAREN,
	_JSTR_REG_RET_EBRACE = REG_EBRACE,
	_JSTR_REG_RET_BADBR = REG_BADBR,
	_JSTR_REG_RET_ERANGE = REG_ERANGE,
	_JSTR_REG_RET_ESPACE = REG_ESPACE,
	_JSTR_REG_RET_BADRPT = REG_BADRPT,
/* GNU regcomp returns */
#ifdef REG_RET_EEND
	_JSTR_REG_EEND = REG_RET_EEND,
#endif
#ifdef REG_RET_ESIZE
	_JSTR_REG_ESIZE = REG_RET_ESIZE,
#endif
#ifdef REG_RET_ERPAREN
	_JSTR_REG_ERPAREN = REG_RET_ERPAREN,
#endif
} Jstr_reg_errcode;

#define JSTR_REG_RET_MALLOC_ERROR _JSTR_REG_RET_MALLOC_ERROR
#define JSTR_REG_RET_NOERROR	  REG_NOERROR
#define JSTR_REG_RET_NOMATCH	  REG_NOMATCH

/* POSIX regcomp return values */
#define JSTR_REG_RET_BADPAT   _JSTR_REG_RET_BADPAT
#define JSTR_REG_RET_ECOLLATE _JSTR_REG_RET_ECOLLATE
#define JSTR_REG_RET_ECTYPE   _JSTR_REG_RET_ECTYPE
#define JSTR_REG_RET_EESCAPE  _JSTR_REG_RET_EESCAPE
#define JSTR_REG_RET_ESUBREG  _JSTR_REG_RET_ESUBREG
#define JSTR_REG_RET_EBRACK   _JSTR_REG_RET_EBRACK
#define JSTR_REG_RET_EPAREN   _JSTR_REG_RET_EPAREN
#define JSTR_REG_RET_EBRACE   _JSTR_REG_RET_EBRACE
#define JSTR_REG_RET_BADBR    _JSTR_REG_RET_BADBR
#define JSTR_REG_RET_ERANGE   _JSTR_REG_RET_ERANGE
#define JSTR_REG_RET_ESPACE   _JSTR_REG_RET_ESPACE
#define JSTR_REG_RET_BADRPT   _JSTR_REG_RET_BADRPT

/* GNU regcomp returns */
#ifdef JSTR_REG_RET_RET_EEND
#	define JSTR_REG_EEND _JSTR_REG_RET_RET_EEND
#endif
#ifdef JSTR_REG_RET_RET_ESIZE
#	define JSTR_REG_ESIZE _JSTR_REG_RET_RET_ESIZE
#endif
#ifdef JSTR_REG_RET_RET_ERPAREN
#	define JSTR_REG_ERPAREN _JSTR_REG_RET_RET_ERPAREN
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_reg_free(regex_t *JSTR_RST const _preg)
{
	regfree(_preg);
}

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
static void jstr_reg_error(const int _reg_errcode,
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
static Jstr_reg_errcode jstr_reg_comp(regex_t *JSTR_RST const _preg,
				      const char *JSTR_RST const _ptn,
				      const int _cflags) JSTR_NOEXCEPT
{
	const int ret = regcomp(_preg, _ptn, _cflags);
	switch (ret) {
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	}
	return (Jstr_reg_errcode)ret;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_CONST
static Jstr_reg_errcode jstr_reg_exec(const regex_t *JSTR_RST _preg,
				      const char *JSTR_RST const s,
				      size_t nmatch,
				      regmatch_t *JSTR_RST const _pmatch,
				      const int _eflags)
{
	const int ret = regexec(_preg, s, nmatch, _pmatch, _eflags);
	switch (ret) {
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
		return (Jstr_reg_errcode)ret;
	case JSTR_REG_RET_NOMATCH: return (Jstr_reg_errcode)JSTR_REG_RET_NOMATCH;
	case JSTR_REG_RET_NOERROR: return (Jstr_reg_errcode)JSTR_REG_RET_NOERROR;
	}
}

#ifdef JSTR_REG_EF_STARTEND

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_exec_mem(const regex_t *JSTR_RST _preg,
					  const char *JSTR_RST const s,
					  const size_t sz,
					  size_t nmatch,
					  regmatch_t *JSTR_RST const _pmatch,
					  const int _eflags)
{
	_pmatch->rm_so = 0;
	_pmatch->rm_eo = sz;
	const int ret = regexec(_preg, s, nmatch, _pmatch, _eflags | REG_STARTEND);
	switch (ret) {
	default:
#	if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(ret, _preg);
#	endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
		return (Jstr_reg_errcode)ret;
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
static Jstr_reg_errcode jstr_reg_match(const char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_match_now(const char *JSTR_RST const s,
					   const char *JSTR_RST const _ptn,
					   regex_t *JSTR_RST const _preg,
					   const int _cflags,
					   const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
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
static Jstr_reg_errcode jstr_reg_search(const char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_search_now(const char *JSTR_RST const s,
					    regex_t *JSTR_RST const _preg,
					    regmatch_t *JSTR_RST const _pmatch,
					    const int _cflags,
					    const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, s, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
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
static Jstr_reg_errcode jstr_reg_search_now_mem(const char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_search_mem(const char *JSTR_RST const s,
					    const char *JSTR_RST const _ptn,
					    const size_t sz,
					    regex_t *JSTR_RST const _preg,
					    regmatch_t *JSTR_RST const _pmatch,
					    const int _cflags,
					    const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_search_now_mem(s, sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_match_mem(const char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_match_now_mem(const char *JSTR_RST const s,
					       const char *JSTR_RST const _ptn,
					       const size_t sz,
					       regex_t *JSTR_RST const _preg,
					       const int _cflags,
					       const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_match_mem(s, sz, _preg, _eflags);
}

#endif /* JSTR_REG_EF_STARTEND */

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode jstr_reg_rm_mem(char *JSTR_RST const s,
					size_t *JSTR_RST sz,
					const regex_t *JSTR_RST const _preg,
					const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const int ret = PRIVATE_JSTR_REG_EXEC(_preg, s, *sz, 1, &rm, _eflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	if (unlikely(rm.rm_eo == rm.rm_so))
		return (Jstr_reg_errcode)ret;
	memmove(s + rm.rm_so,
		s + rm.rm_eo,
		*sz - (rm.rm_eo - rm.rm_so) + 1);
	*sz -= (rm.rm_eo - rm.rm_so);
	return (Jstr_reg_errcode)ret;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode jstr_reg_rm_now_mem(char *JSTR_RST const s,
					    size_t *JSTR_RST sz,
					    const char *JSTR_RST const _ptn,
					    regex_t *JSTR_RST const _preg,
					    const int _cflags,
					    const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_rm_mem(s, sz, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode jstr_reg_rm(char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_rm_now(char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_rmall_mem(char *JSTR_RST const s,
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
	int ret;
	while ((ret = PRIVATE_JSTR_REG_EXEC(_preg, (char *)p, end - p, 1, &rm, _eflags)) == JSTR_REG_RET_NOERROR) {
		ptnlen = rm.rm_eo - rm.rm_so;
		if (unlikely(!ptnlen)) {
			p += ptnlen + 1;
			if (unlikely(!*p))
				break;
			continue;
		}
		p = p + rm.rm_so;
		if (likely(dst != old))
			memmove(dst, old, p - old);
		dst += (p - old);
		old += (p - old);
		old += ptnlen;
		p += ptnlen + 1;
	}
	if (likely(dst != old))
		memmove(dst, old, end - old + 1);
	*sz = (char *)dst + (end - old) - s;
	return (Jstr_reg_errcode)ret;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode jstr_reg_rmall_now_mem(char *JSTR_RST const s,
					       size_t *JSTR_RST sz,
					       const char *JSTR_RST const _ptn,
					       regex_t *JSTR_RST const _preg,
					       const int _cflags,
					       const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_rmall_now(char *JSTR_RST const s,
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
static Jstr_reg_errcode jstr_reg_rmall(char *JSTR_RST const s,
				       size_t *JSTR_RST sz,
				       regex_t *JSTR_RST const _preg,
				       const int _eflags) JSTR_NOEXCEPT
{
	*sz = strlen(s);
	return jstr_reg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_rplcall_mem(char **JSTR_RST const s,
					     size_t *JSTR_RST const sz,
					     size_t *JSTR_RST const cap,
					     const char *JSTR_RST const _rplc,
					     const size_t _rplclen,
					     const regex_t *JSTR_RST const _preg,
					     const int _eflags) JSTR_NOEXCEPT
{
	if (unlikely(_rplclen == 0)) {
		return jstr_reg_rmall_mem(*s, sz, _preg, _eflags);
	}
	regmatch_t rm;
	size_t _ptnlen;
	unsigned char *tmp;
	unsigned char *dst = (unsigned char *)*s;
	unsigned char *p = dst;
	const unsigned char *old = dst;
	int ret;
#if JSTR_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JSTR_HAVE_REALLOC_MREMAP */
	while ((ret = PRIVATE_JSTR_REG_EXEC(_preg, (char *)p, (*(unsigned char **)s + *sz) - p, 1, &rm, _eflags)) == JSTR_REG_RET_NOERROR) {
		_ptnlen = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (unlikely(!_ptnlen)) {
			if (unlikely(!*++p))
				break;
			continue;
		}
		if (_rplclen <= _ptnlen) {
			if (likely(dst != old))
				memmove(dst, old, p - old);
			dst += (p - old);
			old += (p - old);
			memcpy(dst, _rplc, _rplclen);
			dst += _rplclen;
			old += _ptnlen;
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
#if JSTR_HAVE_REALLOC_MREMAP
			if (unlikely(is_mmap)) {
				JSTR_REALLOC(*s, *cap, *sz + _rplclen - _ptnlen, return JSTR_REG_RET_MALLOC_ERROR);
				memmove(p + _rplclen,
					p + _ptnlen,
					(*(unsigned char **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
				if (dst != old) {
					memcpy(*s + (dst - *(unsigned char **)s),
					       old,
					       p - old);
					dst += _rplclen;
					old = dst;
				} else {
					memcpy(*s, *s, p - *(unsigned char **)s);
				}
			} else {
#endif /* JSTR_HAVE_REALLOC_MREMAP */
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
#if JSTR_HAVE_REALLOC_MREMAP
		}
#endif /* JSTR_HAVE_REALLOC_MREMAP */
		*sz += _rplclen - _ptnlen;
		p += _rplclen;
	}
	if (dst != old)
		memmove(dst, old, (*(unsigned char **)s + *sz) - old + 1);
	return (Jstr_reg_errcode)ret;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_rplcall_now_mem(char **JSTR_RST const s,
						 size_t *JSTR_RST const sz,
						 size_t *JSTR_RST const cap,
						 const char *JSTR_RST const _ptn,
						 const char *JSTR_RST const _rplc,
						 const size_t _rplclen,
						 regex_t *JSTR_RST const _preg,
						 const int _cflags,
						 const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_rplcall_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_rplc_mem(char **JSTR_RST const s,
					  size_t *JSTR_RST const sz,
					  size_t *JSTR_RST const cap,
					  const char *JSTR_RST const _rplc,
					  const size_t _rplclen,
					  const regex_t *JSTR_RST const _preg,
					  const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const int ret = PRIVATE_JSTR_REG_EXEC(_preg, *s, *sz, 1, &rm, _eflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	if (unlikely(rm.rm_eo == rm.rm_so))
		return (Jstr_reg_errcode)ret;
	jstr_rplcat_mem(s, sz, cap, rm.rm_so, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
	return (Jstr_reg_errcode)ret;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode jstr_reg_rplc_now_mem(char **JSTR_RST const s,
					      size_t *JSTR_RST const sz,
					      size_t *JSTR_RST const cap,
					      const char *JSTR_RST const _ptn,
					      const char *JSTR_RST const _rplc,
					      const size_t _rplclen,
					      regex_t *JSTR_RST const _preg,
					      const int _cflags,
					      const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return (Jstr_reg_errcode)ret;
	return jstr_reg_rplc_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#undef PRIVATE_JSTR_REG_EXEC

#endif /* JSTR_REGEX_H_DEF */
