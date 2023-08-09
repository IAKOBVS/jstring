#ifndef JSTR_REGEX_H_DEF
#define JSTR_REGEX_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_config.h"
#include "_jstr_macros.h"
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

/* POSIX regexec return values */
#ifdef REG_ENOSYS
#	define JSTR_REG_RET_ENOSYS REG_ENOSYS
#endif /* REG_ENOSYS */
#define JSTR_REG_RET_NOERROR REG_NOERROR
#define JSTR_REG_RET_NOMATCH REG_NOMATCH

/* POSIX regcomp return values */
#define JSTR_REG_RET_BADPAT   REG_BADPAT
#define JSTR_REG_RET_ECOLLATE REG_ECOLLATE
#define JSTR_REG_RET_ECTYPE   REG_ECTYPE
#define JSTR_REG_RET_EESCAPE  REG_EESCAPE
#define JSTR_REG_RET_ESUBREG  REG_ESUBREG
#define JSTR_REG_RET_EBRACK   REG_EBRACK
#define JSTR_REG_RET_EPAREN   REG_EPAREN
#define JSTR_REG_RET_EBRACE   REG_EBRACE
#define JSTR_REG_RET_BADBR    REG_BADBR
#define JSTR_REG_RET_ERANGE   REG_ERANGE
#define JSTR_REG_RET_ESPACE   REG_ESPACE
#define JSTR_REG_RET_BADRPT   REG_BADRPT

/* GNU regcomp returns */
#ifdef REG_RET_EEND
#	define JSTR_REG_EEND REG_RET_EEND
#endif /* REG_RET_EEND */
#ifdef REG_RET_ESIZE
#	define JSTR_REG_ESIZE REG_RET_ESIZE
#endif /* REG_RET_ESIZE */
#ifdef REG_RET_ERPAREN
#	define JSTR_REG_ERPAREN REG_RET_ERPAREN
#endif /* REG_RET_ERPAREN */

#ifdef __cplusplus
namespace jstr {
#endif /* JSTR_NAMESPACE */

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
static void jstr_reg_error(const int _reg_errcode,
			   const regex_t *_preg) JSTR_NOEXCEPT
{
	char buf[32];
	regerror(_reg_errcode, _preg, buf, 32);
	fputs(buf, stderr);
	fputc('\n', stderr);
}

JSTR_NONNULL_ALL
JSTR_INLINE
static int jstr_reg_comp(regex_t *JSTR_RST const _preg,
			 const char *JSTR_RST const _ptn,
			 const int _cflags) JSTR_NOEXCEPT
{
	const int ret = regcomp(_preg, _ptn, _cflags);
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		jstr_reg_error(ret, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	return ret;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
static int jstr_reg_exec(const regex_t *JSTR_RST _preg,
			 const char *JSTR_RST const s,
			 size_t nmatch,
			 regmatch_t *JSTR_RST const _pmatch,
			 const int _eflags)
{
	const int ret = regexec(_preg, s, nmatch, _pmatch, _eflags);
	switch (ret) {
	default:
		jstr_reg_error(ret, _preg);
		return ret;
	case JSTR_REG_RET_NOMATCH: return JSTR_REG_RET_NOMATCH;
	case JSTR_REG_RET_NOERROR: return JSTR_REG_RET_NOERROR;
	}
}

#if JSTR_REG_EF_STARTEND

JSTR_INLINE
static int jstr_reg_exec_mem(const regex_t *JSTR_RST _preg,
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
		jstr_reg_error(ret, _preg);
		return ret;
	case JSTR_REG_RET_NOMATCH: return JSTR_REG_RET_NOMATCH;
	case JSTR_REG_RET_NOERROR: return JSTR_REG_RET_NOERROR;
	}
}

#endif /* JSTR_REG_EF_ REG_STARTEND */

#if JSTR_REG_EF_STARTEND
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
static int jstr_reg_match(const char *JSTR_RST const s,
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
static int jstr_reg_match_now(const char *JSTR_RST const s,
			      const char *JSTR_RST const _ptn,
			      regex_t *JSTR_RST const _preg,
			      const int _cflags,
			      const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_match(s, _preg, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static int jstr_reg_search(const char *JSTR_RST const s,
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
static int jstr_reg_search_now(const char *JSTR_RST const s,
			       regex_t *JSTR_RST const _preg,
			       regmatch_t *JSTR_RST const _pmatch,
			       const int _cflags,
			       const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, s, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
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
static int jstr_reg_search_now_mem(const char *JSTR_RST const s,
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
static int jstr_reg_search_mem(const char *JSTR_RST const s,
			       const char *JSTR_RST const _ptn,
			       const size_t sz,
			       regex_t *JSTR_RST const _preg,
			       regmatch_t *JSTR_RST const _pmatch,
			       const int _cflags,
			       const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_search_now_mem(s, sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
static int jstr_reg_match_mem(const char *JSTR_RST const s,
			      const size_t sz,
			      regex_t *JSTR_RST const _preg,
			      const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jstr_reg_exec_mem(_preg, s, sz, 0, &rm, _eflags | JSTR_REG_EF_NOSUB | JSTR_REG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
static int jstr_reg_match_now_mem(const char *JSTR_RST const s,
				  const char *JSTR_RST const _ptn,
				  const size_t sz,
				  regex_t *JSTR_RST const _preg,
				  const int _cflags,
				  const int _eflags) JSTR_NOEXCEPT
{
	const int ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (unlikely(ret != JSTR_REG_RET_NOERROR))
		return ret;
	return jstr_reg_match_mem(s, sz, _preg, _eflags);
}

#endif /* JSTR_REG_EF_STARTEND */

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm_mem(char *JSTR_RST const s,
			     size_t sz,
			     const regex_t *JSTR_RST const _preg,
			     const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	if (unlikely(PRIVATE_JSTR_REG_EXEC(_preg, s, sz, 1, &rm, _eflags) == JSTR_REG_RET_NOERROR))
		return s + sz;
	memmove(s + rm.rm_so,
		s + rm.rm_eo,
		sz - (rm.rm_eo - rm.rm_so) + 1);
	return s + sz - (rm.rm_eo - rm.rm_so);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm_now_mem(char *JSTR_RST const s,
				 const char *JSTR_RST const _ptn,
				 size_t sz,
				 regex_t *JSTR_RST const _preg,
				 const int _cflags,
				 const int _eflags) JSTR_NOEXCEPT
{
	if (unlikely(jstr_reg_comp(_preg, _ptn, _cflags) != JSTR_REG_RET_NOERROR))
		return s + sz;
	return jstr_reg_rm_mem(s, sz, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm_j(Jstring *JSTR_RST const j,
			   const regex_t *JSTR_RST const _preg,
			   const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rm_mem(j->data, j->size, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm_now_j(Jstring *JSTR_RST const j,
			       const char *JSTR_RST const _ptn,
			       regex_t *JSTR_RST const _preg,
			       const int _cflags,
			       const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rm_now_mem(j->data, _ptn, j->size, _preg, _cflags, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm(char *JSTR_RST const s,
			 regex_t *JSTR_RST const _preg,
			 const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rm_mem(s, strlen(s), _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *jstr_reg_rm_now(char *JSTR_RST const s,
			     const char *JSTR_RST const _ptn,
			     regex_t *JSTR_RST const _preg,
			     const int _cflags,
			     const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rm_now_mem(s, _ptn, strlen(s), _preg, _cflags, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_RETURNS_NONNULL
static char *jstr_reg_rmall_mem(char *JSTR_RST const s,
				size_t sz,
				const regex_t *JSTR_RST const _preg,
				const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *p = dst;
	const unsigned char *old = dst;
	const unsigned char *const end = dst + sz;
	while (PRIVATE_JSTR_REG_EXEC(_preg, (char *)p, end - p, 1, &rm, _eflags) == JSTR_REG_RET_NOERROR) {
		p = p + rm.rm_so;
		if (likely(dst != old))
			memmove(dst, old, p - old);
		dst += (p - old);
		old += (p - old);
		old += (rm.rm_eo - rm.rm_so);
		p += (rm.rm_eo - rm.rm_so);
	}
	if (likely(dst != old))
		memmove(dst, old, end - old + 1);
	return (char *)dst + (end - old);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *jstr_reg_rmall_now_mem(char *JSTR_RST const s,
				    const char *JSTR_RST const _ptn,
				    const size_t sz,
				    regex_t *JSTR_RST const _preg,
				    const int _cflags,
				    const int _eflags) JSTR_NOEXCEPT
{
	if (unlikely(jstr_reg_comp(_preg, _ptn, _cflags) != JSTR_REG_RET_NOERROR))
		return s + sz;
	return jstr_reg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_reg_rmall_now_j(Jstring *JSTR_RST const j,
				 const char *JSTR_RST const _ptn,
				 regex_t *JSTR_RST const _preg,
				 const int _cflags,
				 const int _eflags) JSTR_NOEXCEPT
{
	j->size = jstr_reg_rmall_now_mem(j->data, _ptn, j->size, _preg, _cflags, _eflags) - j->data;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_reg_rmall_j(Jstring *JSTR_RST const j,
			     regex_t *JSTR_RST const _preg,
			     const int _eflags) JSTR_NOEXCEPT
{
	j->size = jstr_reg_rmall_mem(j->data, j->size, _preg, _eflags) - j->data;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_reg_rmall_now(char *JSTR_RST const s,
				const char *JSTR_RST const _ptn,
				regex_t *JSTR_RST const _preg,
				const int _cflags,
				const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rmall_now_mem(s, _ptn, strlen(s), _preg, _cflags, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_reg_rmall(char *JSTR_RST const s,
			    regex_t *JSTR_RST const _preg,
			    const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_rmall_mem(s, strlen(s), _preg, _eflags);
}

JSTR_NONNULL_ALL
static void jstr_reg_rplcall_mem(char **JSTR_RST const s,
				 size_t *JSTR_RST const sz,
				 size_t *JSTR_RST const cap,
				 const char *JSTR_RST const _rplc,
				 const size_t _rplclen,
				 const regex_t *JSTR_RST const _preg,
				 const int _eflags) JSTR_NOEXCEPT
{
	if (unlikely(_rplclen == 0)) {
		*sz = jstr_reg_rmall_mem(*s, *sz, _preg, _eflags) - *s;
		return;
	}
	regmatch_t rm;
	size_t off = 0;
	size_t ptnlen;
	char *tmp;
	while (PRIVATE_JSTR_REG_EXEC(_preg, *s + off, *sz - off, 1, &rm, _eflags) == JSTR_REG_RET_NOERROR) {
		ptnlen = rm.rm_eo - rm.rm_so;
		rm.rm_so += off;
		rm.rm_eo += off;
		if (_rplclen <= ptnlen || *cap > *sz + _rplclen - ptnlen) {
			memmove(*s + rm.rm_so + _rplclen,
				*s + rm.rm_eo,
				(*s + *sz + 1) - *s + rm.rm_eo);
			memcpy(*s + rm.rm_so, _rplc, _rplclen);
		} else {
			JSTR_GROW(*cap, *sz + _rplclen + 1);
			tmp = (char *)malloc(*cap);
			JSTR_MALLOC_ERR(tmp, return);
			memcpy(tmp, *s, rm.rm_so);
			memcpy(tmp + rm.rm_so, _rplc, _rplclen);
			memcpy(tmp + rm.rm_so + _rplclen,
			       *s + rm.rm_so,
			       (*s + *sz + 1) - (*s + rm.rm_eo + ptnlen));
			free(*s);
			*s = tmp;
		}
		off = rm.rm_eo + _rplclen - ptnlen;
		if (unlikely((*sz += _rplclen - ptnlen) == _rplclen))
			return;
	}
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_reg_rplcall_now_mem(char **JSTR_RST const s,
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
		return;
	jstr_reg_rplcall_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
static void jstr_reg_rplc_mem(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const _rplc,
			      const size_t _rplclen,
			      const regex_t *JSTR_RST const _preg,
			      const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	if (unlikely(PRIVATE_JSTR_REG_EXEC(_preg, *s, *sz, 1, &rm, _eflags) == JSTR_REG_RET_NOERROR))
		return;
	const size_t ptnlen = rm.rm_eo - rm.rm_so;
	if (_rplclen <= ptnlen || *cap > *sz + _rplclen - ptnlen) {
		memmove(*s + rm.rm_so + _rplclen,
			*s + rm.rm_so + ptnlen,
			(*s + *sz + 1) - *s + rm.rm_so + ptnlen);
		memcpy(*s + rm.rm_so, _rplc, _rplclen);
	} else {
		JSTR_GROW(*cap, *sz + _rplclen + 1);
		char *JSTR_RST tmp = (char *)malloc(*cap);
		JSTR_MALLOC_ERR(tmp, return);
		memcpy(tmp, s, rm.rm_so);
		memcpy(tmp + rm.rm_so, _rplc, _rplclen);
		memcpy(tmp + rm.rm_so + _rplclen,
		       s + rm.rm_so,
		       (*s + *sz + 1) - (*s + rm.rm_eo));
		free(*s);
		*s = tmp;
	}
	*sz += _rplclen - ptnlen;
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_reg_rplc_now_mem(char **JSTR_RST const s,
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
		return;
	jstr_reg_rplc_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_INLINE
static void jstr_reg_free(regex_t *JSTR_RST const _preg)
{
	regfree(_preg);
}

#ifdef __cplusplus
} /* namespace jstr */
#endif /* __cpluslus */

#undef PRIVATE_JSTR_REG_EXEC

#endif /* JSTR_REGEX_H_DEF */
