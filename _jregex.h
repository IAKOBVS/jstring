#ifndef JREGEX_H_DEF
#define JREGEX_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <assert.h>
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
#include "_jstr_pp_va_args_macros.h"
#include "_jstr_replace.h"
#include "_jstr_string.h"

#if JREG_DEBUG
#	define JREG_DEB_PRINT(x) jstr_pp_cout(x)
#else
#	define JREG_DEB_PRINT(x)
#endif /* JREG_DEB_PRINT */

#define JSTR_RST JSTR_RESTRICT

/* POSIX cflags */
#define JREG_CF_EXTENDED REG_EXTENDED
#define JREG_CF_ICASE    REG_ICASE
#define JREG_CF_NEWLINE  REG_NEWLINE

/* POSIX eflags */
#define JREG_EF_NOSUB  REG_NOSUB
#define JREG_EF_NOTBOL REG_NOTBOL
#define JREG_EF_NOTEOL REG_NOTEOL

/* BSD extension */
#ifdef REG_STARTEND
#	define JREG_EF_STARTEND REG_STARTEND
#endif /* REG_STARTEND */

typedef enum {
	JREG_RET_MALLOC_ERROR,
#ifdef REG_ENOSYS
	JREG_RET_ENOSYS = REG_ENOSYS,
#endif
	JREG_RET_NOERROR = REG_NOERROR,
	JREG_RET_NOMATCH = REG_NOMATCH,
	/* POSIX regcomp return values */
	JREG_RET_BADPAT = REG_BADPAT,
	JREG_RET_ECOLLATE = REG_ECOLLATE,
	JREG_RET_ECTYPE = REG_ECTYPE,
	JREG_RET_EESCAPE = REG_EESCAPE,
	JREG_RET_ESUBREG = REG_ESUBREG,
	JREG_RET_EBRACK = REG_EBRACK,
	JREG_RET_EPAREN = REG_EPAREN,
	JREG_RET_EBRACE = REG_EBRACE,
	JREG_RET_BADBR = REG_BADBR,
	JREG_RET_ERANGE = REG_ERANGE,
	JREG_RET_ESPACE = REG_ESPACE,
	JREG_RET_BADRPT = REG_BADRPT,
/* GNU regcomp returns */
#ifdef REG_RET_EEND
	JREG_EEND = REG_RET_EEND,
#endif
#ifdef REG_RET_ESIZE
	JREG_ESIZE = REG_RET_ESIZE,
#endif
#ifdef REG_RET_ERPAREN
	JREG_ERPAREN = REG_RET_ERPAREN,
#endif
} Jstr_reg_errcode;

#define JREG_HAVE_REG_RET_MALLOC_ERROR 1
#define JREG_HAVE_REG_RET_NOERROR      1
#define JREG_HAVE_REG_RET_NOMATCH      1

/* POSIX regcomp return values */
#define JREG_HAVE_REG_RET_BADPAT   1
#define JREG_HAVE_REG_RET_ECOLLATE 1
#define JREG_HAVE_REG_RET_ECTYPE   1
#define JREG_HAVE_REG_RET_EESCAPE  1
#define JREG_HAVE_REG_RET_ESUBREG  1
#define JREG_HAVE_REG_RET_EBRACK   1
#define JREG_HAVE_REG_RET_EPAREN   1
#define JREG_HAVE_REG_RET_EBRACE   1
#define JREG_HAVE_REG_RET_BADBR	   1
#define JREG_HAVE_REG_RET_ERANGE   1
#define JREG_HAVE_REG_RET_ESPACE   1
#define JREG_HAVE_REG_RET_BADRPT   1

/* GNU regcomp returns */
#ifdef JREG_RET_RET_EEND
#	define JREG_HAVE_REG_EEND 1
#endif
#ifdef JREG_RET_RET_ESIZE
#	define JREG_HAVE_REG_ESIZE 1
#endif
#ifdef JREG_RET_RET_ERPAREN
#	define JREG_HAVE_REG_ERPAREN 1
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

JSTR_NONNULL_ALL
JSTR_INLINE
static void
jreg_free(regex_t *JSTR_RST const _preg)
{
	regfree(_preg);
}

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
static void
jreg_error(const int _reg_errcode,
	       const regex_t *JSTR_RST const _preg) JSTR_NOEXCEPT
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
jreg_comp(regex_t *JSTR_RST const _preg,
	      const char *JSTR_RST const _ptn,
	      const int _cflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode ret = (Jstr_reg_errcode)regcomp(_preg, _ptn, _cflags);
	switch (ret) {
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	case JREG_RET_NOMATCH:
	case JREG_RET_NOERROR:
		break;
	}
	return ret;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_CONST
static Jstr_reg_errcode
jreg_exec(const regex_t *JSTR_RST _preg,
	      const char *JSTR_RST const s,
	      const size_t _nmatch,
	      regmatch_t *JSTR_RST const _pmatch,
	      const int _eflags)
{
	const Jstr_reg_errcode ret = (Jstr_reg_errcode)regexec(_preg, s, _nmatch, _pmatch, _eflags);
	switch (ret) {
	default:
#if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	case JREG_RET_NOMATCH:
	case JREG_RET_NOERROR:
		break;
	}
	return ret;
}

#ifdef JREG_EF_STARTEND

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_CONST
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_exec_mem(const regex_t *JSTR_RST _preg,
		  const char *JSTR_RST const s,
		  const size_t sz,
		  const size_t _nmatch,
		  regmatch_t *JSTR_RST const _pmatch,
		  const int _eflags)
{
	_pmatch->rm_so = 0;
	_pmatch->rm_eo = sz;
	const Jstr_reg_errcode ret = (Jstr_reg_errcode)regexec(_preg, s, _nmatch, _pmatch, _eflags | REG_STARTEND);
	switch (ret) {
	default:
#	if JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#	endif /* JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR */
	case JREG_RET_NOMATCH:
	case JREG_RET_NOERROR:
		break;
	}
	return ret;
}

#endif /* JREG_EF_ REG_STARTEND */

#ifdef JREG_EF_STARTEND
#	define PRIVATE_JREG_EXEC(_preg, s, sz, _nmatch, _pmatch, _eflags) \
		jreg_exec_mem(_preg, s, sz, _nmatch, _pmatch, _eflags)
#else
#	define PRIVATE_JREG_EXEC(_preg, s, sz, _nmatch, _pmatch, _eflags) \
		jreg_exec(_preg, s, _nmatch, _pmatch, _eflags)
#endif /* JREG_EF_STARTEND */

/*
   Checks if S matches precompiled regex.
   Returns return value of regexec.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_match(const char *JSTR_RST const s,
	       regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec(_preg, s, 0, NULL, _eflags | JREG_EF_NOSUB);
}

/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_match_now(const char *JSTR_RST const s,
		   const char *JSTR_RST const _ptn,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_match(s, _preg, _eflags);
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
jreg_search(const char *JSTR_RST const s,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec(_preg, s, 1, _pmatch, _eflags);
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
jreg_search_now(const char *JSTR_RST const s,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode ret = jreg_comp(_preg, s, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_search(s, _preg, _pmatch, _eflags);
}

#ifdef JREG_EF_STARTEND

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_search_now_mem(const char *JSTR_RST const s,
			const size_t sz,
			regex_t *JSTR_RST const _preg,
			regmatch_t *JSTR_RST const _pmatch,
			const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec_mem(_preg, s, sz, 1, _pmatch, _eflags | JREG_EF_NOSUB);
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
jreg_search_mem(const char *JSTR_RST const s,
		    const char *JSTR_RST const _ptn,
		    const size_t sz,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_search_now_mem(s, sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_match_mem(const char *JSTR_RST const s,
		   const size_t sz,
		   regex_t *JSTR_RST const _preg,
		   const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jreg_exec_mem(_preg, s, sz, 0, &rm, _eflags | JREG_EF_NOSUB | JREG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_match_now_mem(const char *JSTR_RST const s,
		       const char *JSTR_RST const _ptn,
		       const size_t sz,
		       regex_t *JSTR_RST const _preg,
		       const int _cflags,
		       const int _eflags) JSTR_NOEXCEPT
{
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_match_mem(s, sz, _preg, _eflags);
}

#endif /* JREG_EF_STARTEND */

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jreg_rm_mem(char *JSTR_RST const s,
		size_t *JSTR_RST const sz,
		const regex_t *JSTR_RST const _preg,
		const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const Jstr_reg_errcode ret = PRIVATE_JREG_EXEC(_preg, s, *sz, 1, &rm, _eflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	memmove(s + rm.rm_so,
		s + rm.rm_eo,
		*sz - (rm.rm_eo - rm.rm_so) + 1);
	*sz -= (rm.rm_eo - rm.rm_so);
	return ret;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jreg_rm_now_mem(char *JSTR_RST const s,
		    size_t *JSTR_RST const sz,
		    const char *JSTR_RST const _ptn,
		    regex_t *JSTR_RST const _preg,
		    const int _cflags,
		    const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rm_mem(s, sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static Jstr_reg_errcode
private_jreg_base_rmall_mem(private_jstr_flag_use_n flag,
				char *JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t n,
				const regex_t *JSTR_RST const _preg,
				const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *p = dst;
	const unsigned char *old = dst;
	const unsigned char *const end = dst + *sz;
	size_t ptnlen;
	Jstr_reg_errcode ret = JREG_RET_NOMATCH;
	while ((flag & PRIVATE_JSTR_FLAG_USE_N ? jstr_likely(n--) : 1)
	       && PRIVATE_JREG_EXEC(_preg, (char *)p, end - p, 1, &rm, _eflags) == JREG_RET_NOERROR) {
		ret = JREG_RET_NOERROR;
		ptnlen = rm.rm_eo - rm.rm_so;
		p = p + rm.rm_so;
		if (jstr_unlikely(ptnlen == 0))
			++p;
		else
			private_jstr_rmall_in_place(&dst, &old, &p, ptnlen);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != old))
		memmove(dst, old, end - old + 1);
	*sz = (char *)dst + (end - old) - s;
	return ret;
}

JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static Jstr_reg_errcode
jreg_rmall_mem(char *JSTR_RST const s,
		   size_t *JSTR_RST const sz,
		   const regex_t *JSTR_RST const _preg,
		   const int _eflags) JSTR_NOEXCEPT
{
	return private_jreg_base_rmall_mem(PRIVATE_JSTR_FLAG_USE_NOT_N, s, sz, 0, _preg, _eflags);
}

JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static Jstr_reg_errcode
jreg_rmn_mem(char *JSTR_RST const s,
		 size_t *JSTR_RST const sz,
		 const size_t n,
		 const regex_t *JSTR_RST const _preg,
		 const int _eflags) JSTR_NOEXCEPT
{
	return private_jreg_base_rmall_mem(PRIVATE_JSTR_FLAG_USE_N, s, sz, n, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jreg_rmn_now_mem(char *JSTR_RST const s,
		     const char *JSTR_RST const _ptn,
		     size_t *JSTR_RST const sz,
		     const size_t n,
		     regex_t *JSTR_RST const _preg,
		     const int _cflags,
		     const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rmn_mem(s, sz, n, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static Jstr_reg_errcode
jreg_rmall_now_mem(char *JSTR_RST const s,
		       size_t *JSTR_RST const sz,
		       const char *JSTR_RST const _ptn,
		       regex_t *JSTR_RST const _preg,
		       const int _cflags,
		       const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rmall_mem(s, sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
private_jreg_base_rplcall_mem(private_jstr_flag_use_n flag,
				  char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const _rplc,
				  size_t n,
				  const size_t _rplclen,
				  const regex_t *JSTR_RST const _preg,
				  const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		return jreg_rmall_mem(*s, sz, _preg, _eflags);
	}
	regmatch_t rm;
	size_t _ptnlen;
	unsigned char *tmp;
	Jstr_reg_errcode ret = JREG_RET_NOMATCH;
	typedef unsigned char uc;
	unsigned char *dst = *(uc **)s;
	unsigned char *p = dst;
	const unsigned char *old = dst;
#if JREG_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*cap);
#endif /* JREG_HAVE_REALLOC_MREMAP */
	while ((flag & PRIVATE_JSTR_FLAG_USE_N ? jstr_likely(n--) : 1)
	       && PRIVATE_JREG_EXEC(_preg, (char *)p, (*(uc **)s + *sz) - p, 1, &rm, _eflags) == JREG_RET_NOERROR) {
		ret = JREG_RET_NOERROR;
		_ptnlen = rm.rm_eo - rm.rm_so;
		p += rm.rm_so;
		if (jstr_unlikely(_ptnlen == 0)) {
			JREG_DEB_PRINT("jstr_unlikely(_ptnlen == 0)");
			++p;
			if (jstr_unlikely(*p == '\0'))
				break;
			continue;
		}
		if (_rplclen <= _ptnlen) {
			JREG_DEB_PRINT("_rplclen <= _ptnlen");
			if (_rplclen != _ptnlen)
				if (jstr_likely(dst != old))
					memmove(dst, old, p - old);
			dst += (p - old);
			old += (p - old);
			old += _ptnlen;
			p += _ptnlen;
			memcpy(dst, _rplc, _rplclen);
			dst += _rplclen;
			if (jstr_unlikely(*p == '\0'))
				break;
			continue;
		}
		if (*cap > *sz + _rplclen - _ptnlen) {
			JREG_DEB_PRINT("*cap > *sz + _rplclen - _ptnlen");
			if (dst != old) {
				JREG_DEB_PRINT("dst != old");
				memmove(dst, old, p - old);
				dst += (p - old);
				memmove(dst + _rplclen,
					p + _ptnlen,
					(*(uc **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(dst, _rplc, _rplclen);
				dst += _rplclen;
				old = dst;
			} else {
				JREG_DEB_PRINT("dst == old");
				memmove(p + _rplclen,
					p + _ptnlen,
					(*(uc **)s + *sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
			}
		} else {
			JREG_DEB_PRINT("cap <= *sz + _rplclen - _ptnlen");
#if JREG_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(is_mmap)) {
				if (dst != old)
					memmove(dst, old, p - old);
				tmp = *(uc **)s;
				JSTR_REALLOC(*s, *cap, *sz + _rplclen - _ptnlen, return JREG_RET_MALLOC_ERROR);
				*(uc **)s = *(uc **)s;
				memmove(p + _rplclen,
					p + _ptnlen,
					(tmp + *sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
				p = *(uc **)s + (p - tmp);
				dst = *(uc **)s + (dst - tmp) + _rplclen;
				old = dst;
			} else {
#endif /* JREG_HAVE_REALLOC_MREMAP */
				JSTR_GROW(*cap, *sz + _rplclen - _ptnlen);
				tmp = (uc *)malloc(*cap);
				JSTR_MALLOC_ERR(tmp, return JREG_RET_MALLOC_ERROR);
				if (dst != old) {
					memcpy(tmp, *(uc **)s, dst - *(uc **)s);
					memcpy(tmp + (dst - *(uc **)s),
					       old,
					       p - old);
					dst = tmp + (dst - *(uc **)s);
					memcpy(dst, _rplc, _rplclen);
					dst += _rplclen;
					memcpy(dst,
					       p,
					       (*(uc **)s + *sz) - (p + _ptnlen) + 1);
					old = dst;
				} else {
					memcpy(tmp, *(uc **)s, p - *(uc **)s);
					memcpy(tmp + (p - *(uc **)s), _rplc, _rplclen);
					memcpy(tmp + (p - *(uc **)s) + _rplclen,
					       p + _ptnlen,
					       (*(uc **)s + *sz) - (p + _ptnlen) + 1);
				}
				p = tmp + (p - *(uc **)s);
				free(*s);
				*s = (char *)tmp;
			}
#if JREG_HAVE_REALLOC_MREMAP
		}
#endif /* JREG_HAVE_REALLOC_MREMAP */
		*sz += _rplclen - _ptnlen;
		p += _rplclen;
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (dst != old) {
		memmove(dst, old, (*(uc **)s + *sz) - old + 1);
		*sz = (dst + (*(uc **)s + *sz - old)) - *(uc **)s;
	}
	return ret;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplcall_mem(char **JSTR_RST const s,
		     size_t *JSTR_RST const sz,
		     size_t *JSTR_RST const cap,
		     const char *JSTR_RST const _rplc,
		     const size_t _rplclen,
		     const regex_t *JSTR_RST const _preg,
		     const int _eflags) JSTR_NOEXCEPT
{
	return private_jreg_base_rplcall_mem(PRIVATE_JSTR_FLAG_USE_NOT_N, s, sz, cap, _rplc, 0, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplcn_mem(char **JSTR_RST const s,
		   size_t *JSTR_RST const sz,
		   size_t *JSTR_RST const cap,
		   const char *JSTR_RST const _rplc,
		   const size_t n,
		   const size_t _rplclen,
		   const regex_t *JSTR_RST const _preg,
		   const int _eflags) JSTR_NOEXCEPT
{
	return private_jreg_base_rplcall_mem(PRIVATE_JSTR_FLAG_USE_N, s, sz, cap, _rplc, n, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplcall_now_mem(char **JSTR_RST const s,
			 size_t *JSTR_RST const sz,
			 size_t *JSTR_RST const cap,
			 const char *JSTR_RST const _ptn,
			 const char *JSTR_RST const _rplc,
			 const size_t _rplclen,
			 regex_t *JSTR_RST const _preg,
			 const int _cflags,
			 const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplcall_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplcn_now_mem(char **JSTR_RST const s,
		       size_t *JSTR_RST const sz,
		       size_t *JSTR_RST const cap,
		       const char *JSTR_RST const _ptn,
		       const char *JSTR_RST const _rplc,
		       const size_t n,
		       const size_t _rplclen,
		       regex_t *JSTR_RST const _preg,
		       const int _cflags,
		       const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplcn_mem(s, sz, cap, _rplc, n, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplc_mem(char **JSTR_RST const s,
		  size_t *JSTR_RST const sz,
		  size_t *JSTR_RST const cap,
		  const char *JSTR_RST const _rplc,
		  const size_t _rplclen,
		  const regex_t *JSTR_RST const _preg,
		  const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const Jstr_reg_errcode ret = PRIVATE_JREG_EXEC(_preg, *s, *sz, 1, &rm, _eflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	jstr_rplcat_mem(s, sz, cap, rm.rm_so, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
	return ret;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static Jstr_reg_errcode
jreg_rplc_now_mem(char **JSTR_RST const s,
		      size_t *JSTR_RST const sz,
		      size_t *JSTR_RST const cap,
		      const char *JSTR_RST const _ptn,
		      const char *JSTR_RST const _rplc,
		      const size_t _rplclen,
		      regex_t *JSTR_RST const _preg,
		      const int _cflags,
		      const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const Jstr_reg_errcode ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplc_mem(s, sz, cap, _rplc, _rplclen, _preg, _eflags);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#undef PRIVATE_JREG_EXEC

#endif /* JREGEX_H_DEF */
