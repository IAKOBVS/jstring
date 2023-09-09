#ifndef JSTR_STRING_DEF_H
#define JSTR_STRING_DEF_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr_builder.h"
#include "jstr_config.h"
#include "jstr_ctype.h"
#include "jstr_itoa.h"
#include "jstr_macros.h"
#include "jstr_std_string.h"

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

PJSTR_BEGIN_DECLS

#if JSTR_HAVE_MEMMEM
#	define PJSTR_MEMMEM(_hs, hlen, _ne, nlen) memmem(_hs, hlen, _ne, nlen)
#else
#	define PJSTR_MEMMEM(_hs, hlen, _ne, nlen) strstr(_hs, _ne)
#endif /* HAVE_MEMMEM */

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
#if JSTR_HAVE_STRNCASECMP
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strncasecmp(const char *JSTR_RST _s1,
		 const char *JSTR_RST _s2,
		 size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(_s1, _s2, _n);
#else
	if (jstr_unlikely(_n == 0))
		return 0;
	const unsigned char *_p1 = (unsigned char *)_s1;
	const unsigned char *_p2 = (unsigned char *)_s2;
	int ret;
	while (!(ret = jstr_tolower(*_p1) - jstr_tolower(*_p2++))
	       && *_p1++
	       && _n--)
		;
	return ret;
#endif /* HAVE_STRNCASECMP */
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
#if JSTR_HAVE_STRNCASECMP
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strcasecmp_len(const char *JSTR_RST const _s1,
		    const char *JSTR_RST const _s2,
		    size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(_s1, _s2, _n);
#else
	const unsigned char *_p1 = (unsigned char *)_s1;
	const unsigned char *_p2 = (unsigned char *)_s2;
	while (!(jstr_tolower(*_p1++) - jstr_tolower(*_p2++))
	       && _n--)
		;
	return _n ? jstr_tolower(*(_p1 - 1)) - jstr_tolower(*(_p2 - 1)) : 0;
#endif /* HAVE_STRNCASECMP */
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASECMP
JSTR_INLINE
#endif
JSTR_NOTHROW
static int
jstr_strcasecmp(const char *JSTR_RST _s1,
		const char *JSTR_RST _s2) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(_s1, _s2);
#else
	const unsigned char *_p1 = (unsigned char *)_s1;
	const unsigned char *_p2 = (unsigned char *)_s2;
	int ret;
	while (!(ret = jstr_tolower(*_p1) - jstr_tolower(*_p2++))
	       && *_p1++)
		;
	return ret;
#endif
}

JSTR_FUNC_PURE
static void *
pjstr_strrstr_len_bmh(const unsigned char *JSTR_RST _hs,
		      const size_t _hslen,
		      const unsigned char *JSTR_RST const _ne,
		      const size_t _nelen) JSTR_NOEXCEPT
{
#define H(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)
#define PJSTR_STRRSTR_BMH(table_type, ne_iterator_type)                           \
	do {                                                                      \
		table_type _shift[256];                                           \
		(sizeof(table_type) == sizeof(size_t))                            \
		? memset(_shift, 0, sizeof(_shift))                               \
		: (memset(_shift, 0, 64),                                         \
		   memset(_shift + 64, 0, 64),                                    \
		   memset(_shift + 64 + 64, 0, 64),                               \
		   memset(_shift + 64 + 64 + 64, 0, 64));                         \
		for (ne_iterator_type _i = 1; _i < (ne_iterator_type)_mtc1; ++_i) \
			_shift[H(_ne + _i)] = _i;                                 \
		const size_t shft1 = _mtc1 - _shift[H(_ne + _mtc1)];              \
		_shift[H(_ne + _mtc1)] = _mtc1;                                   \
		do {                                                              \
			do {                                                      \
				_hs -= _mtc1;                                     \
				_tmp = _shift[H(_hs)];                            \
			} while (!_tmp && _hs > _start);                          \
			_hs -= _tmp;                                              \
			if (_mtc1 < 15 || !memcmp(_hs + _off, _ne + _off, 8)) {   \
				if (!memcmp(_hs, _ne, _mtc1))                     \
					return (void *)_hs;                       \
				_off = (_off >= 8 ? _off : _mtc1) - 8;            \
			}                                                         \
			_hs -= shft1;                                             \
		} while (_hs > _start);                                           \
		return NULL;                                                      \
	} while (0)
	const unsigned char *const _start = _hs - 1;
	_hs += _hslen;
	size_t _tmp;
	const size_t _mtc1 = _nelen - 1;
	size_t _off = 0;
	if (jstr_unlikely(_nelen > 256))
		PJSTR_STRRSTR_BMH(size_t, size_t);
	PJSTR_STRRSTR_BMH(uint8_t, int);
#undef H
#undef PJSTR_STRRSTR_BMH
}

#if JSTR_HAVE_MEMRCHR
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static void *
jstr_memrchr(const void *JSTR_RST _s,
	     const int _c,
	     const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(_s, _c, _n);
#elif JSTR_HAVE_ATTR_MAY_ALIAS
	const unsigned char *_end = (unsigned char *)_s + _n;
	switch (_n % sizeof(pjstr_op_ty)) {
	case 7:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 6:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 5:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 4:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 3:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 2:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 1:
		if (*_end-- == _c)
			return (void *)(_end + 1);
		/* fallthrough */
	case 0: break;
	}
	const pjstr_op_ty *_sw = (pjstr_op_ty *)_end;
	const pjstr_op_ty _cc = pjstr_repeat_bytes(_c);
	const pjstr_op_ty *const _start = (pjstr_op_ty *)_s - 1;
	for (; _sw > _start; --_sw)
		if (pjstr_has_eq(*_sw, _cc))
			return (void *)((char *)_sw + pjstr_index_last_eq(*_sw, _cc));
	return NULL;
#else
	const unsigned char *_end = (unsigned char *)_s + _n;
	const unsigned char *_start = (unsigned char *)_s - 1;
	while (*_end != (unsigned char)_c
	       && _end-- > _start)
		;
	return (_end == _start) ? NULL : (void *)_end;
#endif
}

/*
   Find last NE in HS (ASCII).
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
static void *
jstr_strrstr_len(const void *JSTR_RST const _hs,
		 const size_t _hslen,
		 const void *JSTR_RST const _ne,
		 const size_t _nelen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_hslen < _nelen))
		return NULL;
	switch (_nelen) {
	case 0:
		return (void *)((unsigned char *)_hs + _hslen);
	case 1:
		return (void *)jstr_memrchr(_hs, *(char *)_ne, _hslen);
	case 2: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = (unsigned char *)_hs + _hslen - 1;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint16_t _nw = _n[1] << 8 | _n[0];
		uint16_t _hw = _h[0] << 8 | _h[-1];
		for (_h -= 2; (_h != _start) & (_hw != _nw); _hw = _hw << 8 | *_h--)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	case 3: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = (unsigned char *)_hs + _hslen - 1;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint32_t _nw = _n[2] << 24 | _n[1] << 16 | _n[0] << 8;
		uint32_t _hw = _h[0] << 24 | _h[-1] << 16 | _h[-2] << 8;
		for (_h -= 3; (_h != _start) & (_hw != _nw); _hw = (_hw | *_h--) << 8)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	case 4: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = (unsigned char *)_hs + _hslen - 1;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint32_t _nw = _n[3] << 24 | _n[2] << 16 | _n[1] << 8 | _n[0];
		uint32_t _hw = _h[0] << 24 | _h[-1] << 16 | _h[-2] << 8 | _h[-3];
		for (_h -= 4; (_h != _start) & (_hw != _nw); _hw = _hw << 8 | *_h--)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	default: return pjstr_strrstr_len_bmh((unsigned char *)_hs, _hslen, (unsigned char *)_ne, _nelen);
	}
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strrstr(const char *JSTR_RST const _hs,
	     const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
	return (char *)jstr_strrstr_len(_hs, strlen(_hs), _ne, strlen(_ne));
}

JSTR_FUNC_PURE
static char *
pjstr_strcasestr_len_bmh(const char *JSTR_RST const _hs,
			 const size_t _hslen,
			 const char *JSTR_RST const _ne,
			 const size_t _nelen) JSTR_NOEXCEPT
{
#define HL(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define PJSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                                 \
	do {                                                                                               \
		table_type _shift[256];                                                                    \
		(sizeof(table_type) == sizeof(size_t))                                                     \
		? memset(_shift, 0, sizeof(_shift))                                                        \
		: (memset(_shift, 0, 64),                                                                  \
		   memset(_shift + 64, 0, 64),                                                             \
		   memset(_shift + 64 + 64, 0, 64),                                                        \
		   memset(_shift + 64 + 64 + 64, 0, 64));                                                  \
		for (ne_iterator_type _i = 1; _i < (ne_iterator_type)_mtc1; ++_i)                          \
			_shift[HL(_n + _i)] = _i;                                                          \
		const size_t _shft1 = _mtc1 - _shift[HL(_n + _mtc1)];                                      \
		_shift[HL(_n + _mtc1)] = _mtc1;                                                            \
		do {                                                                                       \
			do {                                                                               \
				_h += _mtc1;                                                               \
				_tmp = _shift[HL(_h)];                                                     \
			} while ((!_tmp) & (_h < _end));                                                   \
			_h -= _tmp;                                                                        \
			if (_tmp < _mtc1)                                                                  \
				continue;                                                                  \
			if (_mtc1 < 15 || !jstr_strcasecmp_len((char *)_h + _off, (char *)_n + _off, 8)) { \
				if (!jstr_strcasecmp_len((char *)_h, (char *)_n, _mtc1))                   \
					return (char *)_h;                                                 \
				_off = (_off >= 8 ? _off : _mtc1) - 8;                                     \
			}                                                                                  \
			_h += _shft1;                                                                      \
		} while (_h < _end);                                                                       \
		return NULL;                                                                               \
	} while (0)
	const unsigned char *_h = (unsigned char *)_hs;
	const unsigned char *const _n = (unsigned char *)_ne;
	const unsigned char *const _end = _h + _hslen - _nelen + 1;
	size_t _tmp;
	const size_t _mtc1 = _nelen - 1;
	size_t _off = 0;
	if (jstr_unlikely(_nelen > 256))
		PJSTR_STRCASESTR_BMH(size_t, size_t);
	PJSTR_STRCASESTR_BMH(uint8_t, int);
#undef HL
#undef PJSTR_STRCASESTR_BMH
}

JSTR_FUNC_PURE
static char *
pjstr_strcasestr_bmh(const char *JSTR_RST const _hs,
		     const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
#define PJSTR_HASH2_LOWER(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define PJSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                                 \
	do {                                                                                               \
		table_type _shift[256];                                                                    \
		(sizeof(table_type) == sizeof(size_t))                                                     \
		? memset(_shift, 0, sizeof(_shift))                                                        \
		: (memset(_shift, 0, 64),                                                                  \
		   memset(_shift + 64, 0, 64),                                                             \
		   memset(_shift + 64 + 64, 0, 64),                                                        \
		   memset(_shift + 64 + 64 + 64, 0, 64));                                                  \
		for (ne_iterator_type _i = 1; _i < (ne_iterator_type)_mtc1; ++_i)                          \
			_shift[PJSTR_HASH2_LOWER(_n + _i)] = _i;                                           \
		const size_t _shft1 = _mtc1 - _shift[PJSTR_HASH2_LOWER(_n + _mtc1)];                       \
		_shift[PJSTR_HASH2_LOWER(_n + _mtc1)] = _mtc1;                                             \
		goto start_##table_type;                                                                   \
		for (;;) {                                                                                 \
			if (jstr_unlikely(_h > _end)) {                                                    \
				_end += jstr_strnlen((char *)_end + _mtc1, 2048);                          \
				if (_h > _end)                                                             \
					return NULL;                                                       \
			}                                                                                  \
			start_##table_type:;                                                               \
			do {                                                                               \
				_h += _mtc1;                                                               \
				_tmp = _shift[PJSTR_HASH2_LOWER(_h)];                                      \
			} while ((!_tmp) & (_h <= _end));                                                  \
			_h -= _tmp;                                                                        \
			if (_tmp < _mtc1)                                                                  \
				continue;                                                                  \
			if (_mtc1 < 15 || !jstr_strcasecmp_len((char *)_h + _off, (char *)_n + _off, 8)) { \
				if (!jstr_strcasecmp_len((char *)_h, (char *)_n, _mtc1))                   \
					return (char *)_h;                                                 \
				_off = (_off >= 8 ? _off : _mtc1) - 8;                                     \
			}                                                                                  \
			_h += _shft1;                                                                      \
		}                                                                                          \
		return NULL;                                                                               \
	} while (0)
	const size_t _nelen = strlen(_ne);
	size_t _hslen = jstr_strnlen(_hs, _nelen | 512);
	if (_hslen < _nelen)
		return NULL;
	const unsigned char *_h = (unsigned char *)_hs;
	const unsigned char *const _n = (unsigned char *)_ne;
	const unsigned char *_end = _h + _hslen - _nelen;
	size_t _tmp;
	const size_t _mtc1 = _nelen - 1;
	size_t _off = 0;
	if (jstr_unlikely(_nelen > 256))
		PJSTR_STRCASESTR_BMH(size_t, size_t);
	PJSTR_STRCASESTR_BMH(uint8_t, int);
#undef PJSTR_HASH2_LOWER
#undef PJSTR_STRCASESTR_BMH
}

#define L(c) jstr_tolower(c)

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr2(const unsigned char *JSTR_RST _h,
		  const unsigned char *JSTR_RST _n) JSTR_NOEXCEPT
{
	const uint16_t _nw = L(_n[0]) << 8 | L(_n[1]);
	uint16_t _hw = L(_h[0]) << 8 | L(_h[1]);
	for (_h++; *_h && _hw != _nw; _hw = _hw << 8 | L(*++_h))
		;
	return _hw == _nw ? (char *)(_h - 1) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr3(const unsigned char *JSTR_RST _h,
		  const unsigned char *JSTR_RST _n) JSTR_NOEXCEPT
{
	const uint32_t _nw = L(_n[0]) << 24 | L(_n[1]) << 16 | L(_n[2]) << 8;
	uint32_t _hw = L(_h[0]) << 24 | L(_h[1]) << 16 | L(_h[2]) << 8;
	for (_h += 2; *_h && _hw != _nw; _hw = (_hw | L(*++_h)) << 8)
		;
	return _hw == _nw ? (char *)(_h - 2) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr4(const unsigned char *JSTR_RST _h,
		  const unsigned char *JSTR_RST _n) JSTR_NOEXCEPT
{
	const uint32_t _nw = L(_n[0]) << 24 | L(_n[1]) << 16 | L(_n[2]) << 8 | L(_n[3]);
	uint32_t _hw = L(_h[0]) << 24 | L(_h[1]) << 16 | L(_h[2]) << 8 | L(_h[3]);
	for (_h += 3; *_h && _hw != _nw; _hw = _hw << 8 | L(*++_h))
		;
	return _hw == _nw ? (char *)(_h - 3) : NULL;
}

#undef L

/*
   Find NE in HS case-insensitively (ASCII).
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASESTR
JSTR_INLINE
#endif
static char *
jstr_strcasestr_len(const char *JSTR_RST _hs,
		    size_t _hslen,
		    const char *JSTR_RST const _ne,
		    const size_t _nelen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8)
	return (char *)strcasestr(_hs, _ne);
#else /* It seems to be much faster than glibc strcasestr which seems to use strcasestr.c */
	if (jstr_unlikely(_nelen == 0))
		return (char *)_hs;
	if (_hslen > 4) {
		if (jstr_unlikely(_hslen < _nelen))
			return NULL;
		return pjstr_strcasestr_len_bmh(_hs, _hslen, _ne, _nelen);
	}
	int _is_alpha0 = jstr_isalpha(*_ne);
	const char *const _start = _hs;
	if (_is_alpha0) {
		const char _a[] = { (char)jstr_tolower(*_ne), (char)jstr_toupper(*_ne), '\0' };
		_hs = strpbrk(_hs, _a);
	} else {
		_hs = (char *)memchr(_hs, *_ne, _hslen);
	}
	if (_hs == NULL
	    || !jstr_strncasecmp(_hs, _ne, _nelen))
		return (char *)_hs;
	_hslen -= _hs - _start;
	_is_alpha0 += jstr_isalpha(_ne[1]);
	if (jstr_unlikely(_hslen < _nelen))
		return NULL;
	switch (_nelen) {
	default: /* case 4: */
		if (_is_alpha0
		    + jstr_isalpha(_ne[2])
		    + jstr_isalpha(_ne[3]))
			return pjstr_strcasestr4((unsigned char *)_hs, (unsigned char *)_ne);
		break;
	case 3:
		if (_is_alpha0
		    + jstr_isalpha(_ne[2]))
			return pjstr_strcasestr3((unsigned char *)_hs, (unsigned char *)_ne);
		break;
	case 2:
		if (_is_alpha0)
			return pjstr_strcasestr2((unsigned char *)_hs, (unsigned char *)_ne);
		break;
	}
	return (char *)strstr(_hs, _ne);
#endif
}

/*
   Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
#if JSTR_HAVE_STRCASESTR
JSTR_INLINE
#endif
static char *
jstr_strcasestr(const char *JSTR_RST _hs,
		const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8)
	return (char *)strcasestr(_hs, _ne);
#else /* It seems to be much faster than glibc strcasestr which seems to use strcasestr.c */
	if (jstr_unlikely(_ne[0] == '\0'))
		return (char *)_hs;
	int _is_alpha0 = jstr_isalpha(*_ne);
	if (_is_alpha0) {
		const char _a[] = { (char)jstr_tolower(*_ne), (char)jstr_toupper(*_ne), '\0' };
		_hs = strpbrk(_hs, _a);
	} else {
		_hs = strchr(_hs, *_ne);
	}
	if (jstr_unlikely(_hs == NULL)
	    || !jstr_strcasecmp(_hs, _ne))
		return (char *)_hs;
	_is_alpha0 += jstr_isalpha(_ne[1]);
	if (_ne[2] == '\0') {
		if (jstr_unlikely(_hs[1] == '\0'))
			return NULL;
		if (_is_alpha0)
			return pjstr_strcasestr2((unsigned char *)_hs, (unsigned char *)_ne);
		return (char *)strstr(_hs, _ne);
	} else if (_ne[3] == '\0') {
		if (jstr_unlikely(_hs[1] == '\0')
		    || jstr_unlikely(_hs[2] == '\0'))
			return NULL;
		if (_is_alpha0
		    + jstr_isalpha(_ne[2]))
			return pjstr_strcasestr3((unsigned char *)_hs, (unsigned char *)_ne);
		return (char *)strstr(_hs, _ne);
	} else if (_ne[4] == '\0') {
		if (jstr_unlikely(_hs[1] == '\0' || jstr_unlikely(_hs[2] == '\0') || jstr_unlikely(_hs[3] == '\0')))
			return NULL;
		if (_is_alpha0
		    + jstr_isalpha(_ne[2])
		    + jstr_isalpha(_ne[3]))
			return pjstr_strcasestr4((unsigned char *)_hs, (unsigned char *)_ne);
		return (char *)strstr(_hs, _ne);
	}
	return pjstr_strcasestr_bmh(_hs, _ne);
#endif
}

/*
   Reverse of STRCSPN.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *JSTR_RST const _s,
		  const char *JSTR_RST const _reject,
		  size_t _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_reject[0] == '\0'))
		return 0;
	if (jstr_unlikely(_reject[1] == '\0')) {
		const char *const _p = (char *)jstr_memrchr(_s, *_reject, _sz);
		return _p ? (size_t)((_s + _sz - 1) - _p) : _sz;
	}
	if (jstr_unlikely(_sz == 0))
		return 0;
	unsigned char _t[256];
	memset(_t, 0, 64);
	memset(_t + 64, 0, 64);
	memset(_t + 128, 0, 64);
	memset(_t + 192, 0, 64);
	const unsigned char *_p = (unsigned char *)_reject;
	do
		_t[*_p] = 1;
	while (*_p++);
	_p = (unsigned char *)_s + _sz - 1;
	switch (_sz % 4) {
	case 3:
		if (!_t[*_p--])
			return 0;
		/* fallthrough */
	case 2:
		if (!_t[*_p--])
			return ((unsigned char *)_s + _sz - 1) - (_p + 1);
		/* fallthrough */
	case 1:
		if (!_t[*_p--])
			return ((unsigned char *)_s + _sz - 1) - (_p + 1);
	case 0: break;
	}
	if (jstr_unlikely(_sz < 4))
		return _sz;
	unsigned int _c0, _c1, _c2, _c3;
	do {
		_c0 = _t[_p[0]];
		_c1 = _t[_p[-1]];
		_c2 = _t[_p[-2]];
		_c3 = _t[_p[-3]];
		_p -= 4;
		_sz -= 4;
	} while ((_sz != 0) & (_c0 | _c1 | _c2 | _c3));
	size_t _cnt = ((unsigned char *)_s + _sz - 1) - _p;
	return ((_c0 | _c1) != 0)
	       ? _cnt - _c0 + 1
	       : _cnt - _c2 + 3;
}

/*
   Reverse of STRCSPN.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrcspn(const char *JSTR_RST const _s,
	      const char *JSTR_RST const _reject) JSTR_NOEXCEPT
{
	return jstr_strrcspn_len(_s, _reject, strlen(_s));
}

/*
   Reverse of STRSPN.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrspn_len(const char *JSTR_RST const _s,
		 const char *JSTR_RST const _accept,
		 size_t _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_accept == '\0'))
		return 0;
	if (jstr_unlikely(_sz == 0))
		return 0;
	if (jstr_unlikely(_accept[1] == '\0')) {
		const char *_p = _s + _sz - 1;
		while (*_p == *_accept)
			--_p;
		return (_s + _sz - 1) - _p;
	}
	const unsigned char *_p = (unsigned char *)_accept;
	unsigned char _t[256];
	memset(_t, 0, 64);
	memset(_t + 64, 0, 64);
	memset(_t + 128, 0, 64);
	memset(_t + 192, 0, 64);
	do
		_t[*_p++] = 1;
	while (*_p);
	_p = (unsigned char *)_s + _sz - 1;
	switch (_sz % 4) {
	case 3:
		if (!_t[*_p--])
			return 0;
		/* fallthrough */
	case 2:
		if (!_t[*_p--])
			return ((unsigned char *)_s + _sz - 1) - (_p + 1);
		/* fallthrough */
	case 1:
		if (!_t[*_p--])
			return ((unsigned char *)_s + _sz - 1) - (_p + 1);
	case 0: break;
	}
	if (jstr_unlikely(_sz < 4))
		return _sz;
	unsigned int _c0, _c1, _c2, _c3;
	do {
		_c0 = _t[_p[0]];
		_c1 = _t[_p[-1]];
		_c2 = _t[_p[-2]];
		_c3 = _t[_p[-3]];
		_p -= 4;
		_sz -= 4;
	} while ((_sz != 0) & (_c0 & _c1 & _c2 & _c3));
	size_t _cnt = ((unsigned char *)_s + _sz - 1) - _p;
	return ((_c0 & _c1) == 0)
	       ? _cnt + _c0
	       : _cnt + _c2 + 2;
}

/*
   Reverse of STRSPN.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrspn(const char *JSTR_RST const _s,
	     const char *JSTR_RST const _accept) JSTR_NOEXCEPT
{
	return jstr_strrspn_len(_s, _accept, strlen(_s));
}

/*
   Reverse of STRPBRK.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strrpbrk_len(const char *JSTR_RST _s,
		  const char *JSTR_RST const _accept,
		  const size_t _sz) JSTR_NOEXCEPT
{
	_s = _s + _sz - jstr_strrcspn_len(_s, _accept, _sz);
	return *_s ? (char *)_s : NULL;
}

/*
   Reverse of STRPBRK.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strrpbrk(const char *JSTR_RST const _s,
	      const char *JSTR_RST const _accept) JSTR_NOEXCEPT
{
	return jstr_strrpbrk_len(_s, _accept, strlen(_s));
}

/*
  Checks if S2 is in _end of S1.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endswith_len(const char *JSTR_RST const _hs,
		  const char *JSTR_RST const _ne,
		  const size_t _hsz,
		  const size_t _nelen) JSTR_NOEXCEPT
{
	return (_hsz < _nelen) ? 1 : memcmp(_hs + _hsz - _nelen, _ne, _nelen);
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_countc(const char *JSTR_RST _s,
	    const int _c) JSTR_NOEXCEPT
{
	size_t _cnt = 0;
	while ((_s = strchr(_s, _c)))
		++_cnt;
	return _cnt;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_countc_len(const char *JSTR_RST _s,
		const int _c,
		const size_t _n) JSTR_NOEXCEPT
{
	size_t _cnt = 0;
	const char *const _end = _s + _n;
	while ((_s = (char *)memchr(_s, _c, _end - _s)))
		++_cnt;
	return _cnt;
}

#if JSTR_HAVE_MEMMEM

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
static size_t
jstr_count_len(const char *JSTR_RST _s,
	       const char *JSTR_RST const _find,
	       const size_t _sz,
	       const size_t _findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_findlen == 1))
		return jstr_countc_len(_s, *_find, _sz);
	if (jstr_unlikely(_findlen == 0))
		return 0;
	size_t _cnt = 0;
	const char *const _end = _s + _sz;
	while ((_s = (char *)memmem(_s, _end - _s, _find, _findlen)))
		++_cnt, _s += _findlen;
	return _cnt;
}

#endif

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
static size_t
jstr_count(const char *JSTR_RST _s,
	   const char *JSTR_RST const _find) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_find[0] == '\0'))
		return 0;
	if (jstr_unlikely(_find[1] == '\0'))
		return jstr_countc(_s, *_find);
	const size_t _findlen = strlen(_find + 1) + 1;
	size_t _cnt = 0;
	while ((_s = (char *)strstr(_s, _find)))
		++_cnt, _s += _findlen;
	return _cnt;
}

PJSTR_END_DECLS

#endif /* JSTR_STRING_DEF_H */
