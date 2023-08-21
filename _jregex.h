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

#include "_builder.h"
#include "_config.h"
#include "_macros.h"
#include "_pp_va_args_macros.h"
#include "_replace.h"
#include "_string.h"

#if JREG_DEBUG
#	define JREG_DEB_PRINT(x) jstr_pp_cout(x)
#else
#	define JREG_DEB_PRINT(x)
#endif /* JREG_DEB_PRINT */

#define JSTR_RST JSTR_RESTRICT

/* POSIX cflags */
#define JREG_CF_EXTENDED REG_EXTENDED
#define JREG_CF_ICASE	 REG_ICASE
#define JREG_CF_NEWLINE	 REG_NEWLINE

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
} jreg_errcode_ty;

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
JSTR_PURE
static jreg_errcode_ty
jreg_comp(regex_t *JSTR_RST const _preg,
	  const char *JSTR_RST const _ptn,
	  const int _cflags) JSTR_NOEXCEPT
{
	const jreg_errcode_ty ret = (jreg_errcode_ty)regcomp(_preg, _ptn, _cflags);
	switch (ret) {
	default:
#if JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#endif /* JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR */
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
JSTR_PURE
static jreg_errcode_ty
jreg_exec(const regex_t *JSTR_RST _preg,
	  const char *JSTR_RST const _s,
	  const size_t _nmatch,
	  regmatch_t *JSTR_RST const _pmatch,
	  const int _eflags)
{
	const jreg_errcode_ty ret = (jreg_errcode_ty)regexec(_preg, _s, _nmatch, _pmatch, _eflags);
	switch (ret) {
	default:
#if JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#endif /* JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR */
	case JREG_RET_NOMATCH:
	case JREG_RET_NOERROR:
		break;
	}
	return ret;
}

#ifdef JREG_EF_STARTEND

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_exec_mem(const regex_t *JSTR_RST _preg,
	      const char *JSTR_RST const _s,
	      const size_t _sz,
	      const size_t _nmatch,
	      regmatch_t *JSTR_RST const _pmatch,
	      const int _eflags)
{
	_pmatch->rm_so = 0;
	_pmatch->rm_eo = _sz;
	const jreg_errcode_ty ret = (jreg_errcode_ty)regexec(_preg, _s, _nmatch, _pmatch, _eflags | REG_STARTEND);
	switch (ret) {
	default:
#	if JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR
		jreg_error(ret, _preg);
#	endif /* JSTR_CFG_PRINT_ERR_MSG_ON_REGEX_ERROR */
	case JREG_RET_NOMATCH:
	case JREG_RET_NOERROR:
		break;
	}
	return ret;
}

#endif /* JREG_EF_ REG_STARTEND */

#ifdef JREG_EF_STARTEND
#	define PRIV_JREG_EXEC(_preg, _s, _sz, _nmatch, _pmatch, _eflags) \
		jreg_exec_mem(_preg, _s, _sz, _nmatch, _pmatch, _eflags)
#else
#	define PRIV_JREG_EXEC(_preg, _s, _sz, _nmatch, _pmatch, _eflags) \
		jreg_exec(_preg, _s, _nmatch, _pmatch, _eflags)
#endif /* JREG_EF_STARTEND */

/*
   Checks if S matches precompiled regex.
   Returns return value of regexec.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_match(const char *JSTR_RST const _s,
	   regex_t *JSTR_RST const _preg,
	   const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec(_preg, _s, 0, NULL, _eflags | JREG_EF_NOSUB);
}

/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_match_now(const char *JSTR_RST const _s,
	       const char *JSTR_RST const _ptn,
	       regex_t *JSTR_RST const _preg,
	       const int _cflags,
	       const int _eflags) JSTR_NOEXCEPT
{
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_match(_s, _preg, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_search(const char *JSTR_RST const _s,
	    regex_t *JSTR_RST const _preg,
	    regmatch_t *JSTR_RST const _pmatch,
	    const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec(_preg, _s, 1, _pmatch, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec or regcomp if it fails.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_search_now(const char *JSTR_RST const _s,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	const jreg_errcode_ty ret = jreg_comp(_preg, _s, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_search(_s, _preg, _pmatch, _eflags);
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
static jreg_errcode_ty
jreg_search_now_mem(const char *JSTR_RST const _s,
		    const size_t _sz,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _eflags) JSTR_NOEXCEPT
{
	return jreg_exec_mem(_preg, _s, _sz, 1, _pmatch, _eflags | JREG_EF_NOSUB);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_search_mem(const char *JSTR_RST const _s,
		const char *JSTR_RST const _ptn,
		const size_t _sz,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_search_now_mem(_s, _sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_match_mem(const char *JSTR_RST const _s,
	       const size_t _sz,
	       regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	return jreg_exec_mem(_preg, _s, _sz, 0, &rm, _eflags | JREG_EF_NOSUB | JREG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_match_now_mem(const char *JSTR_RST const _s,
		   const char *JSTR_RST const _ptn,
		   const size_t _sz,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_match_mem(_s, _sz, _preg, _eflags);
}

#endif /* JREG_EF_STARTEND */

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static jreg_errcode_ty
jreg_rm(char *JSTR_RST const _s,
	    size_t *JSTR_RST const _sz,
	    const regex_t *JSTR_RST const _preg,
	    const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const jreg_errcode_ty ret = PRIV_JREG_EXEC(_preg, _s, *_sz, 1, &rm, _eflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	memmove(_s + rm.rm_so,
		_s + rm.rm_eo,
		*_sz - (rm.rm_eo - rm.rm_so) + 1);
	*_sz -= (rm.rm_eo - rm.rm_so);
	return ret;
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static jreg_errcode_ty
jreg_rm_now(char *JSTR_RST const _s,
		size_t *JSTR_RST const _sz,
		const char *JSTR_RST const _ptn,
		regex_t *JSTR_RST const _preg,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rm(_s, _sz, _preg, _eflags);
}

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static jreg_errcode_ty
priv_jreg_base_rmall(const priv_jstr_flag_use_n_ty _flag,
			 char *JSTR_RST const _s,
			 size_t *JSTR_RST const _sz,
			 size_t _n,
			 const regex_t *JSTR_RST const _preg,
			 const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	unsigned char *dst = (unsigned char *)_s;
	const unsigned char *p = dst;
	const unsigned char *old = dst;
	const unsigned char *const end = dst + *_sz;
	size_t ptnlen;
	jreg_errcode_ty ret = JREG_RET_NOMATCH;
	while ((_flag & PRIV_JSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && PRIV_JREG_EXEC(_preg, (char *)p, end - p, 1, &rm, _eflags) == JREG_RET_NOERROR) {
		ret = JREG_RET_NOERROR;
		ptnlen = rm.rm_eo - rm.rm_so;
		p = p + rm.rm_so;
		if (jstr_unlikely(ptnlen == 0))
			++p;
		else
			priv_jstr_rmall_in_place(&dst, &old, &p, ptnlen);
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (jstr_likely(dst != old))
		memmove(dst, old, end - old + 1);
	*_sz = (char *)dst + (end - old) - _s;
	return ret;
}

JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static jreg_errcode_ty
jreg_rmall(char *JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       const regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	return priv_jreg_base_rmall(PRIV_JSTR_FLAG_USE_NOT_N, _s, _sz, 0, _preg, _eflags);
}

JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static jreg_errcode_ty
jreg_rmn(char *JSTR_RST const _s,
	     size_t *JSTR_RST const _sz,
	     const size_t _n,
	     const regex_t *JSTR_RST const _preg,
	     const int _eflags) JSTR_NOEXCEPT
{
	return priv_jreg_base_rmall(PRIV_JSTR_FLAG_USE_N, _s, _sz, _n, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static jreg_errcode_ty
jreg_rmn_now(char *JSTR_RST const _s,
		 const char *JSTR_RST const _ptn,
		 size_t *JSTR_RST const _sz,
		 const size_t _n,
		 regex_t *JSTR_RST const _preg,
		 const int _cflags,
		 const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rmn(_s, _sz, _n, _preg, _eflags);
}

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static jreg_errcode_ty
jreg_rmall_now(char *JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   const char *JSTR_RST const _ptn,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rmall(_s, _sz, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jreg_errcode_ty
priv_jreg_base_rplcall_mem(const priv_jstr_flag_use_n_ty _flag,
			   char **JSTR_RST const _s,
			   size_t *JSTR_RST const _sz,
			   size_t *JSTR_RST const _cap,
			   const char *JSTR_RST const _rplc,
			   size_t _n,
			   const size_t _rplclen,
			   const regex_t *JSTR_RST const _preg,
			   const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_rplclen == 0)) {
		return jreg_rmall(*_s, _sz, _preg, _eflags);
	}
	regmatch_t rm;
	size_t _ptnlen;
	unsigned char *_tmp;
	jreg_errcode_ty ret = JREG_RET_NOMATCH;
	typedef unsigned char uc;
	unsigned char *dst = *(uc **)_s;
	unsigned char *p = dst;
	const unsigned char *old = dst;
#if JREG_HAVE_REALLOC_MREMAP
	const int is_mmap = JSTR_IS_MMAP(*_cap);
#endif /* JREG_HAVE_REALLOC_MREMAP */
	while ((_flag & PRIV_JSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && PRIV_JREG_EXEC(_preg, (char *)p, (*(uc **)_s + *_sz) - p, 1, &rm, _eflags) == JREG_RET_NOERROR) {
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
			priv_jstr_rplcall_in_place(&dst, &old, (const uc **)&p, _rplc, _rplclen, _ptnlen);
			if (jstr_unlikely(*p == '\0'))
				break;
			continue;
		}
		if (*_cap > *_sz + _rplclen - _ptnlen) {
			JREG_DEB_PRINT("*_cap > *_sz + _rplclen - _ptnlen");
			if (dst != old) {
				JREG_DEB_PRINT("dst != old");
				memmove(dst, old, p - old);
				dst += (p - old);
				memmove(dst + _rplclen,
					p + _ptnlen,
					(*(uc **)_s + *_sz) - (p + _ptnlen) + 1);
				memcpy(dst, _rplc, _rplclen);
				dst += _rplclen;
				old = dst;
			} else {
				JREG_DEB_PRINT("dst == old");
				memmove(p + _rplclen,
					p + _ptnlen,
					(*(uc **)_s + *_sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
			}
		} else {
			JREG_DEB_PRINT("_cap <= *_sz + _rplclen - _ptnlen");
#if JREG_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(is_mmap)) {
				if (dst != old)
					memmove(dst, old, p - old);
				_tmp = *(uc **)_s;
				PRIV_JSTR_REALLOC(*_s, *_cap, *_sz + _rplclen - _ptnlen, return JREG_RET_MALLOC_ERROR);
				memmove(p + _rplclen,
					p + _ptnlen,
					(_tmp + *_sz) - (p + _ptnlen) + 1);
				memcpy(p, _rplc, _rplclen);
				p = *(uc **)_s + (p - _tmp);
				dst = *(uc **)_s + (dst - _tmp) + _rplclen;
				old = dst;
			} else
#endif /* JREG_HAVE_REALLOC_MREMAP */
			{
				PRIV_JSTR_GROW(*_cap, *_sz + _rplclen - _ptnlen);
				_tmp = (uc *)malloc(*_cap);
				PRIV_JSTR_MALLOC_ERR(_tmp, return JREG_RET_MALLOC_ERROR);
				if (dst != old) {
					memcpy(_tmp, *(uc **)_s, dst - *(uc **)_s);
					memcpy(_tmp + (dst - *(uc **)_s),
					       old,
					       p - old);
					dst = _tmp + (dst - *(uc **)_s);
					memcpy(dst, _rplc, _rplclen);
					dst += _rplclen;
					memcpy(dst,
					       p,
					       (*(uc **)_s + *_sz) - (p + _ptnlen) + 1);
					old = dst;
				} else {
#ifdef __clang__
#	pragma clang diagnostic ignored "-Wunknown-warning-option"
#	pragma clang diagnostic push
#elif defined __GNUC__
#	pragma GCC diagnostic ignored "-Wanalyzer-use-of-uninitialized-value"
#	pragma GCC diagnostic push
#endif
					memcpy(_tmp, *(uc **)_s, p - *(uc **)_s);
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#elif defined __clang__
#	pragma clang diagnostic pop
#endif
					memcpy(_tmp + (p - *(uc **)_s), _rplc, _rplclen);
					memcpy(_tmp + (p - *(uc **)_s) + _rplclen,
					       p + _ptnlen,
					       (*(uc **)_s + *_sz) - (p + _ptnlen) + 1);
				}
				p = _tmp + (p - *(uc **)_s);
				free(*_s);
				*_s = (char *)_tmp;
			}
		}
		*_sz += _rplclen - _ptnlen;
		p += _rplclen;
		if (jstr_unlikely(*p == '\0'))
			break;
	}
	if (dst != old) {
		memmove(dst, old, (*(uc **)_s + *_sz) - old + 1);
		*_sz = (dst + (*(uc **)_s + *_sz - old)) - *(uc **)_s;
	}
	return ret;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplcall_mem(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _rplc,
		 const size_t _rplclen,
		 const regex_t *JSTR_RST const _preg,
		 const int _eflags) JSTR_NOEXCEPT
{
	return priv_jreg_base_rplcall_mem(PRIV_JSTR_FLAG_USE_NOT_N, _s, _sz, _cap, _rplc, 0, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplcn_mem(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _rplc,
	       const size_t _n,
	       const size_t _rplclen,
	       const regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	return priv_jreg_base_rplcall_mem(PRIV_JSTR_FLAG_USE_N, _s, _sz, _cap, _rplc, _n, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplcall_now_mem(char **JSTR_RST const _s,
		     size_t *JSTR_RST const _sz,
		     size_t *JSTR_RST const _cap,
		     const char *JSTR_RST const _ptn,
		     const char *JSTR_RST const _rplc,
		     const size_t _rplclen,
		     regex_t *JSTR_RST const _preg,
		     const int _cflags,
		     const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplcall_mem(_s, _sz, _cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplcn_now_mem(char **JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   size_t *JSTR_RST const _cap,
		   const char *JSTR_RST const _ptn,
		   const char *JSTR_RST const _rplc,
		   const size_t _n,
		   const size_t _rplclen,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplcn_mem(_s, _sz, _cap, _rplc, _n, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplc_mem(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen,
	      const regex_t *JSTR_RST const _preg,
	      const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t rm;
	const jreg_errcode_ty ret = PRIV_JREG_EXEC(_preg, *_s, *_sz, 1, &rm, _eflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	if (jstr_unlikely(rm.rm_eo == rm.rm_so))
		return ret;
	jstr_rplcat_mem(_s, _sz, _cap, rm.rm_so, _rplc, _rplclen, rm.rm_eo - rm.rm_so);
	return ret;
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static jreg_errcode_ty
jreg_rplc_now_mem(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _ptn,
		  const char *JSTR_RST const _rplc,
		  const size_t _rplclen,
		  regex_t *JSTR_RST const _preg,
		  const int _cflags,
		  const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JREG_RET_NOERROR;
	const jreg_errcode_ty ret = jreg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(ret != JREG_RET_NOERROR))
		return ret;
	return jreg_rplc_mem(_s, _sz, _cap, _rplc, _rplclen, _preg, _eflags);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#undef PRIV_JREG_EXEC
#undef JREG_DEB_PRINT

#endif /* JREGEX_H_DEF */
