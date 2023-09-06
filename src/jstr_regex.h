#ifndef JSTR_REGEX_DEF_H
#define JSTR_REGEX_DEF_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr_builder.h"
#include "jstr_config.h"
#include "jstr_string.h"
#include "jstr_pp_va_args_macros.h"
#include "jstr_replace.h"

#define JSTR_REG_DEBUG 0

#if JSTR_REG_DEBUG
#	define PJSTR_REG_DEB(x) jstr_pp_cout(x)
#else
#	define PJSTR_REG_DEB(x)
#endif /* PJSTR_REG_DEB */

/* POSIX cflags */
#define JSTR_REG_CF_EXTENDED REG_EXTENDED
#define JSTR_REG_CF_ICASE	 REG_ICASE
#define JSTR_REG_CF_NEWLINE	 REG_NEWLINE

/* POSIX eflags */
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
} jstr_reg_errcode_ty;

#define JSTR_REG_HAVE_RET_MALLOC_ERROR 1
#define JSTR_REG_HAVE_RET_NOERROR	   1
#define JSTR_REG_HAVE_RET_NOMATCH	   1

/* POSIX regcomp return values */
#define JSTR_REG_HAVE_RET_BADPAT   1
#define JSTR_REG_HAVE_RET_ECOLLATE 1
#define JSTR_REG_HAVE_RET_ECTYPE   1
#define JSTR_REG_HAVE_RET_EESCAPE  1
#define JSTR_REG_HAVE_RET_ESUBREG  1
#define JSTR_REG_HAVE_RET_EBRACK   1
#define JSTR_REG_HAVE_RET_EPAREN   1
#define JSTR_REG_HAVE_RET_EBRACE   1
#define JSTR_REG_HAVE_RET_BADBR    1
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
jstr_reg_free(regex_t *JSTR_RST const _preg)
{
	regfree(_preg);
}

JSTR_NOINLINE
JSTR_COLD
JSTR_MAYBE_UNUSED
JSTR_NOTHROW
static void
jstr_reg_error(const int _reg_errcode,
	   const regex_t *JSTR_RST const _preg) JSTR_NOEXCEPT
{
	char buf[64];
	regerror(_reg_errcode, _preg, buf, 32);
	fputs(buf, stderr);
	fputc('\n', stderr);
}

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_comp(regex_t *JSTR_RST const _preg,
	  const char *JSTR_RST const _ptn,
	  const int _cflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty _ret = (jstr_reg_errcode_ty)regcomp(_preg, _ptn, _cflags);
	switch (_ret) {
	default:
#if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(_ret, _preg);
#endif /* JSTR_ERR_MSG_ON_REGEX_ERROR */
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return _ret;
}

JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_NOTHROW
static jstr_reg_errcode_ty
jstr_reg_exec(const regex_t *JSTR_RST _preg,
	  const char *JSTR_RST const _s,
	  const size_t _nmatch,
	  regmatch_t *JSTR_RST const _pmatch,
	  const int _eflags)
{
	const jstr_reg_errcode_ty _ret = (jstr_reg_errcode_ty)regexec(_preg, _s, _nmatch, _pmatch, _eflags);
	switch (_ret) {
	default:
#if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(_ret, _preg);
#endif /* JSTR_ERR_MSG_ON_REGEX_ERROR */
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return _ret;
}

#ifdef JSTR_REG_EF_STARTEND

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_exec_len(const regex_t *JSTR_RST _preg,
	      const char *JSTR_RST const _s,
	      const size_t _sz,
	      const size_t _nmatch,
	      regmatch_t *JSTR_RST const _pmatch,
	      const int _eflags)
{
	_pmatch->rm_so = 0;
	_pmatch->rm_eo = _sz;
	const jstr_reg_errcode_ty _ret = (jstr_reg_errcode_ty)regexec(_preg, _s, _nmatch, _pmatch, _eflags | REG_STARTEND);
	switch (_ret) {
	default:
#	if JSTR_ERR_MSG_ON_REGEX_ERROR
		jstr_reg_error(_ret, _preg);
#	endif /* JSTR_ERR_MSG_ON_REGEX_ERROR */
	case JSTR_REG_RET_NOMATCH:
	case JSTR_REG_RET_NOERROR:
		break;
	}
	return _ret;
}

#endif /* JSTR_REG_EF_ REG_STARTEND */

#ifdef JSTR_REG_EF_STARTEND
#	define PJSTR_REG_EXEC(_preg, _s, _sz, _nmatch, _pmatch, _eflags) \
		jstr_reg_exec_len(_preg, _s, _sz, _nmatch, _pmatch, _eflags)
#else
#	define PJSTR_REG_EXEC(_preg, _s, _sz, _nmatch, _pmatch, _eflags) \
		jstr_reg_exec(_preg, _s, _nmatch, _pmatch, _eflags)
#endif /* JSTR_REG_EF_STARTEND */

/*
   Checks if S matches precompiled regex.
   Returns return value of regexec.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match(const char *JSTR_RST const _s,
	   regex_t *JSTR_RST const _preg,
	   const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(_preg, _s, 0, NULL, _eflags | JSTR_REG_EF_NOSUB);
}

/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match_now(const char *JSTR_RST const _s,
	       const char *JSTR_RST const _ptn,
	       regex_t *JSTR_RST const _preg,
	       const int _cflags,
	       const int _eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_match(_s, _preg, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search(const char *JSTR_RST const _s,
	    regex_t *JSTR_RST const _preg,
	    regmatch_t *JSTR_RST const _pmatch,
	    const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec(_preg, _s, 1, _pmatch, _eflags);
}

/*
   Searches pattern in S.
   Returns return value of regexec or regcomp if it fails.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_now(const char *JSTR_RST const _s,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _s, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_search(_s, _preg, _pmatch, _eflags);
}

#ifdef JSTR_REG_EF_STARTEND

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_now_len(const char *JSTR_RST const _s,
		    const size_t _sz,
		    regex_t *JSTR_RST const _preg,
		    regmatch_t *JSTR_RST const _pmatch,
		    const int _eflags) JSTR_NOEXCEPT
{
	return jstr_reg_exec_len(_preg, _s, _sz, 1, _pmatch, _eflags | JSTR_REG_EF_NOSUB);
}

/*
   Searches pattern in S.
   Returns return value of regexec.
   Stores offset of matched pattern in _pmatch.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_search_len(const char *JSTR_RST const _s,
		const char *JSTR_RST const _ptn,
		const size_t _sz,
		regex_t *JSTR_RST const _preg,
		regmatch_t *JSTR_RST const _pmatch,
		const int _cflags,
		const int _eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_search_now_len(_s, _sz, _preg, _pmatch, _eflags);
}
/*
   Checks if S matches PTN.
   Returns return value of regexec or regcomp if it fails.
*/
JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_match_len(const char *JSTR_RST const _s,
	       const size_t _sz,
	       regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t _rm;
	return jstr_reg_exec_len(_preg, _s, _sz, 0, &_rm, _eflags | JSTR_REG_EF_NOSUB | JSTR_REG_EF_STARTEND);
}

JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static jstr_reg_errcode_ty
jstr_reg_match_now_len(const char *JSTR_RST const _s,
		   const char *JSTR_RST const _ptn,
		   const size_t _sz,
		   regex_t *JSTR_RST const _preg,
		   const int _cflags,
		   const int _eflags) JSTR_NOEXCEPT
{
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_match_len(_s, _sz, _preg, _eflags);
}

#endif /* JSTR_REG_EF_STARTEND */

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rm(char *JSTR_RST const _s,
	size_t *JSTR_RST const _sz,
	const regex_t *JSTR_RST const _preg,
	const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t _rm;
	const jstr_reg_errcode_ty _ret = PJSTR_REG_EXEC(_preg, _s, *_sz, 1, &_rm, _eflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR)
	    || jstr_unlikely(_rm.rm_eo == _rm.rm_so))
		return _ret;
	memmove(_s + _rm.rm_so,
		_s + _rm.rm_eo,
		*_sz - (_rm.rm_eo - _rm.rm_so) + 1);
	*_sz -= (_rm.rm_eo - _rm.rm_so);
	return _ret;
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rm_now(char *JSTR_RST const _s,
	    size_t *JSTR_RST const _sz,
	    const char *JSTR_RST const _ptn,
	    regex_t *JSTR_RST const _preg,
	    const int _cflags,
	    const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rm(_s, _sz, _preg, _eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
pjstr_reg_base_rmall(const pjstr_flag_use_n_ty _flag,
		 char *JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t _n,
		 const regex_t *JSTR_RST const _preg,
		 const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t _rm;
	unsigned char *_dst = (unsigned char *)_s;
	const unsigned char *_p = _dst;
	const unsigned char *_old = _dst;
	const unsigned char *const _end = _dst + *_sz;
	size_t _ptnlen;
	jstr_reg_errcode_ty _ret = JSTR_REG_RET_NOMATCH;
	while ((_flag & PJSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && PJSTR_REG_EXEC(_preg, (char *)_p, _end - _p, 1, &_rm, _eflags) == JSTR_REG_RET_NOERROR) {
		_ret = JSTR_REG_RET_NOERROR;
		_ptnlen = _rm.rm_eo - _rm.rm_so;
		_p = _p + _rm.rm_so;
		if (jstr_unlikely(_ptnlen == 0))
			++_p;
		else
			PJSTR_RMALL_IN_PLACE(_dst, _old, _p, _ptnlen);
		if (jstr_unlikely(*_p == '\0'))
			break;
	}
	if (jstr_likely(_dst != _old))
		memmove(_dst, _old, _end - _old + 1);
	*_sz = (char *)_dst + (_end - _old) - _s;
	return _ret;
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rmall(char *JSTR_RST const _s,
	   size_t *JSTR_RST const _sz,
	   const regex_t *JSTR_RST const _preg,
	   const int _eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rmall(PJSTR_FLAG_USE_NOT_N, _s, _sz, 0, _preg, _eflags);
}

JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rmn(char *JSTR_RST const _s,
	 size_t *JSTR_RST const _sz,
	 const size_t _n,
	 const regex_t *JSTR_RST const _preg,
	 const int _eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rmall(PJSTR_FLAG_USE_N, _s, _sz, _n, _preg, _eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rmn_now(char *JSTR_RST const _s,
	     const char *JSTR_RST const _ptn,
	     size_t *JSTR_RST const _sz,
	     const size_t _n,
	     regex_t *JSTR_RST const _preg,
	     const int _cflags,
	     const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rmn(_s, _sz, _n, _preg, _eflags);
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rmall_now(char *JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       const char *JSTR_RST const _ptn,
	       regex_t *JSTR_RST const _preg,
	       const int _cflags,
	       const int _eflags) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ptn == '\0'))
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rmall(_s, _sz, _preg, _eflags);
}

JSTR_FUNC
static jstr_reg_errcode_ty
pjstr_reg_base_rplcall_len(const pjstr_flag_use_n_ty _flag,
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
		return jstr_reg_rmall(*_s, _sz, _preg, _eflags);
	}
	regmatch_t _rm;
	size_t _ptnlen;
	unsigned char *_tmp;
	jstr_reg_errcode_ty _ret = JSTR_REG_RET_NOMATCH;
	typedef unsigned char uc;
	unsigned char *_dst = *(uc **)_s;
	unsigned char *_p = _dst;
	const unsigned char *_old = _dst;
#if JSTR_HAVE_REALLOC_MREMAP
	const int _is_mmap = PJSTR_IS_MMAP(*_cap);
#endif /* HAVE_REALLOC_MREMAP */
	while ((_flag & PJSTR_FLAG_USE_N ? jstr_likely(_n--) : 1)
	       && PJSTR_REG_EXEC(_preg, (char *)_p, (*(uc **)_s + *_sz) - _p, 1, &_rm, _eflags) == JSTR_REG_RET_NOERROR) {
		_ret = JSTR_REG_RET_NOERROR;
		_ptnlen = _rm.rm_eo - _rm.rm_so;
		_p += _rm.rm_so;
		if (jstr_unlikely(_ptnlen == 0)) {
			PJSTR_REG_DEB("jstr_unlikely(_ptnlen == 0)");
			++_p;
			if (jstr_unlikely(*_p == '\0'))
				break;
			continue;
		}
		if (_rplclen <= _ptnlen) {
			PJSTR_REG_DEB("_rplclen <= _ptnlen");
			PJSTR_RPLCALL_IN_PLACE(_dst, _old, _p, _rplc, _rplclen, _ptnlen);
			if (jstr_unlikely(*_p == '\0'))
				break;
			continue;
		}
		if (*_cap > *_sz + _rplclen - _ptnlen) {
			PJSTR_REG_DEB("*_cap > *_sz + _rplclen - _ptnlen");
			if (_dst != _old) {
				PJSTR_REG_DEB("dst != old");
				memmove(_dst, _old, _p - _old);
				_dst += (_p - _old);
				memmove(_dst + _rplclen,
					_p + _ptnlen,
					(*(uc **)_s + *_sz) - (_p + _ptnlen) + 1);
				memcpy(_dst, _rplc, _rplclen);
				_dst += _rplclen;
				_old = _dst;
			} else {
				PJSTR_REG_DEB("dst == old");
				memmove(_p + _rplclen,
					_p + _ptnlen,
					(*(uc **)_s + *_sz) - (_p + _ptnlen) + 1);
				memcpy(_p, _rplc, _rplclen);
			}
		} else {
			PJSTR_REG_DEB("_cap <= *_sz + _rplclen - _ptnlen");
#if JSTR_HAVE_REALLOC_MREMAP
			if (jstr_unlikely(_is_mmap)) {
				if (_dst != _old)
					memmove(_dst, _old, _p - _old);
				_tmp = *(uc **)_s;
				PJSTR_REALLOC(*_s, *_cap, *_sz + _rplclen - _ptnlen, return JSTR_REG_RET_MALLOC_ERROR);
				memmove(_p + _rplclen,
					_p + _ptnlen,
					(_tmp + *_sz) - (_p + _ptnlen) + 1);
				memcpy(_p, _rplc, _rplclen);
				_p = *(uc **)_s + (_p - _tmp);
				_dst = *(uc **)_s + (_dst - _tmp) + _rplclen;
				_old = _dst;
			} else
#endif /* HAVE_REALLOC_MREMAP */
			{
				PJSTR_GROW(*_cap, *_sz + _rplclen - _ptnlen);
				_tmp = (uc *)malloc(*_cap);
				PJSTR_MALLOC_ERR(_tmp, return JSTR_REG_RET_MALLOC_ERROR);
				if (_dst != _old) {
					memcpy(_tmp, *(uc **)_s, _dst - *(uc **)_s);
					memcpy(_tmp + (_dst - *(uc **)_s),
					       _old,
					       _p - _old);
					_dst = _tmp + (_dst - *(uc **)_s);
					memcpy(_dst, _rplc, _rplclen);
					_dst += _rplclen;
					memcpy(_dst,
					       _p,
					       (*(uc **)_s + *_sz) - (_p + _ptnlen) + 1);
					_old = _dst;
				} else {
#if 0
#	ifdef __clang__
#		pragma clang diagnostic ignored "-Wunknown-warning-option"
#		pragma clang diagnostic push
#	elif defined __GNUC__
#		pragma GCC diagnostic ignored "-Wanalyzer-use-of-uninitialized-value"
#		pragma GCC diagnostic push
#	endif
					memcpy(_tmp, *_s, _p - *(uc **)_s);
#	ifdef __GNUC__
#		pragma GCC diagnostic pop
#	elif defined __clang__
#		pragma clang diagnostic pop
#	endif
#else
					memcpy(_tmp, *_s, _p - *(uc **)_s);
#endif
					memcpy(_tmp + (_p - *(uc **)_s), _rplc, _rplclen);
					memcpy(_tmp + (_p - (*(uc **)_s + _rplclen)),
					       _p + _ptnlen,
					       (*(uc **)_s + *_sz) - (_p + _ptnlen) + 1);
				}
				_p = _tmp + (_p - *(uc **)_s);
				free(*_s);
				*_s = (char *)_tmp;
			}
		}
		*_sz += _rplclen - _ptnlen;
		_p += _rplclen;
		if (jstr_unlikely(*_p == '\0'))
			break;
	}
	if (_dst != _old) {
		memmove(_dst, _old, (*(uc **)_s + *_sz) - _old + 1);
		*_sz = (_dst + (*(uc **)_s + *_sz - _old)) - *(uc **)_s;
	}
	return _ret;
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_len(char **JSTR_RST const _s,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const char *JSTR_RST const _rplc,
		 const size_t _rplclen,
		 const regex_t *JSTR_RST const _preg,
		 const int _eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len(PJSTR_FLAG_USE_NOT_N, _s, _sz, _cap, _rplc, 0, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_len(char **JSTR_RST const _s,
	       size_t *JSTR_RST const _sz,
	       size_t *JSTR_RST const _cap,
	       const char *JSTR_RST const _rplc,
	       const size_t _n,
	       const size_t _rplclen,
	       const regex_t *JSTR_RST const _preg,
	       const int _eflags) JSTR_NOEXCEPT
{
	return pjstr_reg_base_rplcall_len(PJSTR_FLAG_USE_N, _s, _sz, _cap, _rplc, _n, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcall_now_len(char **JSTR_RST const _s,
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
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rplcall_len(_s, _sz, _cap, _rplc, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplcn_now_len(char **JSTR_RST const _s,
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
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rplcn_len(_s, _sz, _cap, _rplc, _n, _rplclen, _preg, _eflags);
}

JSTR_INLINE
JSTR_FUNC
static jstr_reg_errcode_ty
jstr_reg_rplc_len(char **JSTR_RST const _s,
	      size_t *JSTR_RST const _sz,
	      size_t *JSTR_RST const _cap,
	      const char *JSTR_RST const _rplc,
	      const size_t _rplclen,
	      const regex_t *JSTR_RST const _preg,
	      const int _eflags) JSTR_NOEXCEPT
{
	regmatch_t _rm;
	const jstr_reg_errcode_ty _ret = PJSTR_REG_EXEC(_preg, *_s, *_sz, 1, &_rm, _eflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR)
	    || jstr_unlikely(_rm.rm_eo == _rm.rm_so))
		return _ret;
	if (jstr_unlikely(!pjstr_rplcat_len(_s, _sz, _cap, _rm.rm_so, _rplc, _rplclen, _rm.rm_eo - _rm.rm_so)))
		return JSTR_REG_RET_MALLOC_ERROR;
	return _ret;
}

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_reg_rplc_now_len(char **JSTR_RST const _s,
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
		return JSTR_REG_RET_NOERROR;
	const jstr_reg_errcode_ty _ret = jstr_reg_comp(_preg, _ptn, _cflags);
	if (jstr_unlikely(_ret != JSTR_REG_RET_NOERROR))
		return _ret;
	return jstr_reg_rplc_len(_s, _sz, _cap, _rplc, _rplclen, _preg, _eflags);
}

PJSTR_END_DECLS

#undef PJSTR_REG_EXEC
#undef PJSTR_REG_DEB
#undef JSTR_REG_DEBUG

#endif /* JSTR_REGEX_DEF_H */
