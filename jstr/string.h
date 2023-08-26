#ifndef JSTR_STRING_H_DEF
#define JSTR_STRING_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "config.h"
#include "ctype.h"
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#if JSTR_HAVE_MEMMEM
#	define PRIV_JSTR_MEMMEM(_hs, hlen, _ne, nlen) memmem(_hs, hlen, _ne, nlen)
#else
#	define PRIV_JSTR_MEMMEM(_hs, hlen, _ne, nlen) strstr(_hs, _ne)
#endif /* JSTR_HAVE_MEMMEM */

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static void *
jstr_mempcpy(char *JSTR_RST const _dst,
	     const char *JSTR_RST const _src,
	     const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMPCPY
	return mempcpy(_dst, _src, _n);
#else
	return (char *)memcpy(_dst, _src, _n) + _n;
#endif /* !JSTR_HAVE_STPCPY */
}

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_stpcpy(char *JSTR_RST const _dst,
	    const char *JSTR_RST const _src) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STPCPY
	return stpcpy(_dst, _src);
#else
	const size_t slen = strlen(_src);
	return memcpy(_dst, _src, slen + 1) + slen;
#endif /* !JSTR_HAVE_STPCPY */
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *
jstr_strchrnul(const char *JSTR_RST const _s,
	       const int _c)
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strchrnul(_s, _c);
#else
	const size_t _n = strlen(_s);
	const void *const p = memchr(_s, _c, _n);
	return (p != NULL) ? (char *)p : (char *)_s + _n;
#endif /* JSTR_HAVE_STRCHRNUL */
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *
jstr_strdup(const char *JSTR_RST const _s)
{
#if JSTR_HAVE_STRCHRNUL
	return (char *)strdup(_s);
#else
	const size_t len = strlen(_s) + 1;
	void *p = malloc(len);
	if (jstr_unlikely(p == NULL))
		return NULL;
	return (char *)memcpy(p, _s, len);
#endif /* JSTR_HAVE_STRCHRNUL */
}

/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
   Return the position in DEST one byte past where C was copied, or
   NULL if C was not found in the first N bytes of SRC.  */
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static void *
jstr_memccpy(void *JSTR_RST _dst,
	     const void *JSTR_RST _src,
	     int _c,
	     const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(_dst, _src, _c, _n);
#else
	void *p = memchr(_src, _c, _n);
	if (p != NULL)
		return jstr_mempcpy(_dst, _src, p - _src + 1);
	memcpy(_dst, _src, _n);
	return NULL;
#endif /* JSTR_HAVE_MEMCPY */
}

/*
  Return value:
  Pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_stpcat(char *JSTR_RST _dst,
	    const char *JSTR_RST _src) JSTR_NOEXCEPT
{
	_dst += strlen(_dst);
	return jstr_stpcpy(_dst, _src);
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_strncasecmp(const char *JSTR_RST const _s1,
		 const char *JSTR_RST const _s2,
		 const size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(_s1, _s2, _n);
#else
	const unsigned char *_p1 = (unsigned char *)_s1;
	const unsigned char *_p2 = (unsigned char *)_s2;
	const unsigned char *const _end = _p1 + _n;
	for (char _c; _p1 < _end; ++_p1, ++_p2) {
		switch (*_p1) {
		default:
			JSTR_CASE_LOWER
			_c = *_p1;
			break;
			JSTR_CASE_UPPER
			_c = *_p1 - 'A' + 'a';
			break;
		case '\0':
			return *_p2;
		}
		switch (*_p2) {
		default:
			JSTR_CASE_LOWER
			if (*_p2 != _c)
				return _c - *_p2;
			break;
			JSTR_CASE_UPPER
			if ((*_p2 - 'A' + 'a') != _c)
				return _c - (*_p2 - 'A' + 'a');
			break;
		case '\0':
			return 1;
		}
	}
	return 0;
#endif /* JSTR_HAVE_STRNCASECMP */
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_strcasecmp(const char *JSTR_RST _s1,
		const char *JSTR_RST _s2) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(_s1, _s2);
#else
	for (char _c;; ++_s1, ++_s2) {
		switch (*_s1) {
		default:
			JSTR_CASE_LOWER
			_c = *_s1;
			break;
			JSTR_CASE_UPPER
			_c = *_s1 - 'A' + 'a';
			break;
		case '\0':
			return *_s2;
		}
		switch (*_s2) {
		default:
			JSTR_CASE_LOWER
			if (*_s2 != _c)
				return _c - *_s2;
			break;
			JSTR_CASE_UPPER
			if ((*_s2 - 'A' + 'a') != _c)
				return _c - (*_s2 - 'A' + 'a');
			break;
		case '\0':
			return 1;
		}
	}
#endif
}

JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static void *
priv_jstr_memrmem(const void *JSTR_RST const _hs,
		  const size_t _hslen,
		  const void *JSTR_RST const _ne,
		  const size_t _nelen) JSTR_NOEXCEPT
{
#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)
#define PRIV_JSTR_MEMMEMR(shift_type, ne_iterator_type)                                     \
	do {                                                                                \
		const unsigned char *_h = (unsigned char *)_hs + _hslen + _nelen;           \
		const unsigned char *const _n = (unsigned char *)_ne;                       \
		const unsigned char *const _start = (unsigned char *)_hs - 1;               \
		size_t _tmp;                                                                \
		const size_t _mtc1 = _nelen - 1;                                            \
		size_t _off = 0;                                                            \
		shift_type _shift[256];                                                     \
		memset(_shift, 0, sizeof(_shift));                                          \
		for (ne_iterator_type i = 1; jstr_likely(i < (ne_iterator_type)_mtc1); ++i) \
			_shift[JSTR_HASH2(_n + i)] = i;                                     \
		const size_t shft1 = _mtc1 - _shift[JSTR_HASH2(_n + _mtc1)];                \
		_shift[JSTR_HASH2(_n + _mtc1)] = _mtc1;                                     \
		do {                                                                        \
			do {                                                                \
				_h -= _mtc1;                                                \
				_tmp = _shift[JSTR_HASH2(_h)];                              \
			} while (!_tmp && _h > _start);                                     \
			_h -= _tmp;                                                         \
			if (_mtc1 < 15 || !memcmp(_h + _off, _n + _off, 8)) {               \
				if (!memcmp(_h, _n, _nelen))                                \
					return (void *)_h;                                  \
				_off = (_off >= 8 ? _off : _mtc1) - 8;                      \
			}                                                                   \
			_h -= shft1;                                                        \
		} while (_h > _start);                                                      \
		return NULL;                                                                \
	} while (0)
	if (jstr_unlikely(_hslen > 256))
		PRIV_JSTR_MEMMEMR(size_t, size_t);
	PRIV_JSTR_MEMMEMR(uint8_t, int);
}

/*
   Find last NE in HS.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static void *
jstr_memrmem(const void *JSTR_RST const _hs,
	     const size_t _hslen,
	     const void *JSTR_RST const _ne,
	     const size_t _nelen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_hslen < _nelen))
		return NULL;
	switch (_nelen) {
	case 0:
		return (void *)_hs;
	case 1:
#if JSTR_HAVE_MEMRCHR
		return (void *)memrchr(_hs, *(char *)_ne, _hslen);
#else
		return strrchr(_hs, *(char *)_ne);
#endif
	case 2: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = _start + _hslen;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint16_t _nw = _n[1] << 8 | _n[0];
		uint16_t _hw = _h[0] << 8 | _h[-1];
		for (_h -= 2; jstr_likely(_h > _start && _hw != _nw); _hw = _hw << 8 | *_h--)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	case 3: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = _start + _hslen;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint32_t _nw = _n[2] << 24 | _n[1] << 16 | _n[0] << 8;
		uint32_t _hw = _h[0] << 24 | _h[-1] << 16 | _h[-2] << 8;
		for (_h -= 3; jstr_likely(_h > _start && _hw != _nw); _hw = (_hw | *_h--) << 8)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	case 4: {
		const unsigned char *const _start = (unsigned char *)_hs - 1;
		const unsigned char *_h = _start + _hslen;
		const unsigned char *_n = (unsigned char *)_ne;
		const uint32_t _nw = _n[3] << 24 | _n[2] << 16 | _n[1] << 8 | _n[0];
		uint32_t _hw = _h[0] << 24 | _h[-1] << 16 | _h[-2] << 8 | _h[-3];
		for (_h -= 4; jstr_likely(_h > _start && _hw != _nw); _hw = _hw << 8 | *_h--)
			;
		return _hw == _nw ? (void *)(_h + 1) : NULL;
	}
	default: return priv_jstr_memrmem(_hs, _hslen, _ne, _nelen);
	}
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *
jstr_strrstr(const char *JSTR_RST const _hs,
	     const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_ne == '\0'))
		return (char *)_hs;
	if (jstr_unlikely(*(_ne + 1) == '\0'))
		return (char *)strrchr(_hs, *_ne);
	return (char *)jstr_memrmem(_hs, strlen(_hs), _ne, strlen((char *)_ne));
}

JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *
priv_jstr_memcasemem3(const char *JSTR_RST const _hs,
		      const size_t _hslen,
		      const char *JSTR_RST const _ne,
		      const size_t _nelen) JSTR_NOEXCEPT
{
#define JSTR_HASH2_LOWER(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define PRIV_JSTR_STRSTRCASE(shift_type, ne_iterator_type)                                              \
	do {                                                                                            \
		const unsigned char *_h = (unsigned char *)_hs;                                         \
		const unsigned char *const _n = (unsigned char *)_ne;                                   \
		const unsigned char *const _end = _h + _hslen - _nelen + 1;                             \
		size_t _tmp;                                                                            \
		const size_t _mtc1 = _nelen - 1;                                                        \
		size_t _off = 0;                                                                        \
		shift_type _shift[256];                                                                 \
		memset(_shift, 0, sizeof(_shift));                                                      \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)_mtc1; ++i)                          \
			_shift[JSTR_HASH2_LOWER(_n + i)] = i;                                           \
		const size_t _shft1 = _mtc1 - _shift[JSTR_HASH2_LOWER(_n + _mtc1)];                     \
		_shift[JSTR_HASH2_LOWER(_n + _mtc1)] = _mtc1;                                           \
		do {                                                                                    \
			do {                                                                            \
				_h += _mtc1;                                                            \
				_tmp = _shift[JSTR_HASH2_LOWER(_h)];                                    \
			} while (!_tmp && _h < _end);                                                   \
			_h -= _tmp;                                                                     \
			if (_tmp < _mtc1)                                                               \
				continue;                                                               \
			if (_mtc1 < 15 || !jstr_strncasecmp((char *)_h + _off, (char *)_n + _off, 8)) { \
				if (!jstr_strncasecmp((char *)_h, (char *)_n, _nelen))                  \
					return (char *)_h;                                              \
				_off = (_off >= 8 ? _off : _mtc1) - 8;                                  \
			}                                                                               \
			_h += _shft1;                                                                   \
		} while (_h < _end);                                                                    \
		return NULL;                                                                            \
	} while (0)
	if (jstr_unlikely(_nelen > 256))
		PRIV_JSTR_STRSTRCASE(size_t, size_t);
	PRIV_JSTR_STRSTRCASE(uint8_t, int);
}

/*
   Find C in S case-insensitively.
   S MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *
priv_strcasechr(const char *JSTR_RST _s,
		const int _c,
		const size_t _n) JSTR_NOEXCEPT
{
	enum { l = 0,
	       u,
	};
	char _acc[3];
	_acc[2] = '\0';
	switch (_c) {
		JSTR_CASE_UPPER
		_acc[0] = _c;
		_acc[1] = _c - 'A' + 'a';
		break;
		JSTR_CASE_LOWER
		_acc[0] = _c;
		_acc[1] = _c - 'a' + 'A';
		break;
	default:
		return (char *)memchr(_s, _c, _n);
	}
	return (char *)strpbrk(_s, _acc);
}

#if !JSTR_HAVE_STRCASESTR

/*
   Find NE in HS case-insensitively.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *
jstr_memcasemem(const char *JSTR_RST const _hs,
		const size_t _hslen,
		const char *JSTR_RST const _ne,
		const size_t _nelen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(_hslen < _nelen))
		return NULL;
	switch (_nelen) {
	case 0: return (char *)_hs;
	case 1: return priv_strcasechr(_hs, *_ne, _hslen);
	case 2:
do2:
		if (jstr_islower(*_ne) && jstr_islower(*(_ne + 1)))
			return (char *)PRIV_JSTR_MEMMEM(_hs, _hslen, _ne, _nelen);
		break;
	case 3:
do3:
		if (jstr_islower(*(_ne + 2)))
			goto do2;
		break;
	case 4:
		if (jstr_islower(*(_ne + 3)))
			goto do3;
		break;
	}
	return priv_jstr_memcasemem3(_hs, _hslen, _ne, _nelen);
}

#endif /* !HAVE_STRCASESTR */

/*
   Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *
jstr_strcasestr(const char *JSTR_RST const _hs,
		const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR
	return (char *)strcasestr(_hs, _ne);
#else
	return jstr_memcasemem(_hs, strlen(_hs), _ne, strlen(_ne));
#endif /* JSTR_HAVE_STRCASESTR */
}

/*
  Checks if S2 is in _end of S1.
  Return value:
  0 if true;
  1 if false.
  Assumes that HS is longer than NE.
  Let memcmp do the bounds check.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_endswith_mem_f(const char *JSTR_RST const _hs,
		    const char *JSTR_RST const _ne,
		    const size_t _hsz,
		    const size_t _nelen) JSTR_NOEXCEPT
{
	return memcmp(_hs + _hsz - _nelen, _ne, _nelen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith_mem_f_j(const jstr_ty *JSTR_RST const j,
		    const char *JSTR_RST const _ne,
		    const size_t _nelen) JSTR_NOEXCEPT
{
	return jstr_endswith_mem_f(j->data, _ne, j->size, _nelen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith_f(const char *JSTR_RST const _hs,
		    const char *JSTR_RST const _ne,
		    const size_t _hsz) JSTR_NOEXCEPT
{
	return jstr_endswith_mem_f(_hs, _ne, _hsz, strlen(_ne));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith_f_j(const jstr_ty *JSTR_RST const j,
		    const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
	return jstr_endswith_f(j->data, _ne, j->size);
}

/*
  Checks if S2 is in _end of S1.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_endswith_mem(const char *JSTR_RST const _hs,
		  const char *JSTR_RST const _ne,
		  const size_t _hsz,
		  const size_t _nelen) JSTR_NOEXCEPT
{
	return (_hsz < _nelen) ? 1 : memcmp(_hs + _hsz - _nelen, _ne, _nelen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith_mem_j(const jstr_ty *JSTR_RST const j,
		  const char *JSTR_RST const _ne,
		  const size_t _nelen) JSTR_NOEXCEPT
{
	return jstr_endswith_mem(j->data, _ne, j->size, _nelen);
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith(const char *JSTR_RST const _hs,
		  const char *JSTR_RST const _ne,
		  const size_t _hsz) JSTR_NOEXCEPT
{
	return jstr_endswith_mem(_hs, _ne, _hsz, strlen(_ne));
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith_j(const jstr_ty *JSTR_RST const j,
		  const char *JSTR_RST const _ne) JSTR_NOEXCEPT
{
	return jstr_endswith(j->data, _ne, j->size);
}

/*
  Converts int to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_itoa(char *JSTR_RST const _dst,
	  int _num,
	  const unsigned int _base)
{
#define PRIV_JSTR_NUMTOSTR(_max_digits)                                        \
	do {                                                                   \
		unsigned char *_d = (unsigned char *)_dst;                     \
		unsigned char _sbuf[_max_digits];                              \
		unsigned char *JSTR_RST _s = (unsigned char *)_sbuf;           \
		unsigned int _neg = (_num < 0) ? (_num = -_num, 1) : 0;        \
		unsigned char *const _end = (unsigned char *)_s + _max_digits; \
		_s = _end - 1;                                                 \
		do                                                             \
			*_s-- = _num % _base + '0';                            \
		while (_num /= 10);                                            \
		if (_neg)                                                      \
			*_s = '-';                                             \
		else                                                           \
			++_s;                                                  \
		while (_s < _end)                                              \
			*_d++ = *_s++;                                         \
		*_d = '\0';                                                    \
		return (char *)_d;                                             \
	} while (0)
	PRIV_JSTR_NUMTOSTR(JSTR_MAX_INT_DIGITS);
}

/*
  Converts long to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_ltoa(char *JSTR_RST const _dst,
	  long _num,
	  const unsigned int _base)
{
	PRIV_JSTR_NUMTOSTR(JSTR_MAX_LONG_DIGITS);
}

/*
  Converts long long to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_lltoa(char *JSTR_RST const _dst,
	   long long _num,
	   const unsigned int _base)
{
	PRIV_JSTR_NUMTOSTR(JSTR_MAX_LONG_DIGITS);
}

/*
  Converts unsigned int to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_utoa(char *JSTR_RST const _dst,
	  unsigned int _num,
	  const unsigned int _base)
{
#define PRIV_JSTR_UNUMTOSTR(_max_digits)                      \
	do {                                                  \
		unsigned char *_d = (unsigned char *)_dst;    \
		unsigned char _sbuf[_max_digits];             \
		unsigned char *JSTR_RST _s = _sbuf;           \
		unsigned char *const _end = _s + _max_digits; \
		_s = _end - 1;                                \
		do                                            \
			*_s-- = _num % _base + '0';           \
		while (_num /= 10);                           \
		++_s;                                         \
		while (_s < _end)                             \
			*_d++ = *_s++;                        \
		*_d = '\0';                                   \
		return (char *)_d;                            \
	} while (0)
	PRIV_JSTR_UNUMTOSTR(JSTR_MAX_UINT_DIGITS);
}

/*
  Converts unsigned long to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_ultoa(char *JSTR_RST const _dst,
	   unsigned long _num,
	   const unsigned int _base)
{
	PRIV_JSTR_UNUMTOSTR(JSTR_MAX_ULONG_DIGITS);
}

/*
  Converts unsigned long long to string.
  Return value:
  new pointer to '\0' in DST.
*/
JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_ulltoa(char *JSTR_RST const _dst,
	    unsigned long long _num,
	    const unsigned int _base)
{
	PRIV_JSTR_UNUMTOSTR(JSTR_MAX_ULONG_LONG_DIGITS);
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_countc(const char *JSTR_RST _s,
	    const int _c) JSTR_NOEXCEPT
{
	int cnt = 0;
	while ((_s = strchr(_s, _c)))
		++cnt;
	return cnt;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_countc_mem(const char *JSTR_RST _s,
		const int _c,
		const size_t _n) JSTR_NOEXCEPT
{
	int cnt = 0;
	const char *const _end = _s + _n;
	while ((_s = (char *)memchr(_s, _c, _end - _s)))
		++cnt;
	return cnt;
}

#if JSTR_HAVE_MEMMEM

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_count_mem(const char *JSTR_RST _s,
	       const char *JSTR_RST const _find,
	       size_t _sz,
	       const size_t _findlen) JSTR_NOEXCEPT
{
	switch (_findlen) {
	case 0: return 0;
	case 1: return jstr_countc_mem(_s, *_find, _sz);
	default: {
		int cnt = 0;
		const char *const _end = _s + _sz;
		while ((_s = (char *)memmem(_s, _end - _s, _find, _findlen)))
			++cnt, _s += _findlen;
		return cnt;
	}
	}
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_count_mem_j(const jstr_ty *JSTR_RST const j,
	       const char *JSTR_RST const _find,
	       const size_t _findlen) JSTR_NOEXCEPT
{
	return jstr_count_mem(j->data, _find, j->size, _findlen);
}

#endif

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int
jstr_count(const char *JSTR_RST _s,
	   const char *JSTR_RST const _find) JSTR_NOEXCEPT
{
	const size_t _findlen = strlen(_find);
	switch (_findlen) {
	case 0: return 0;
	case 1: return jstr_countc(_s, *_find);
	default: {
		int cnt = 0;
		while ((_s = (char *)strstr(_s, _find)))
			++cnt, _s += _findlen;
		return cnt;
	}
	}
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#endif /* JSTR_STRING_H_DEF */
