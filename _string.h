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

#include "_ctype.h"
#include "_macros.h"
#include "_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* JSTR_NAMESPACE */

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

/*
  Return pointer to last C in S.
  Return value:
  pointer to last C;
  NULL if not found.
*/
JSTR_INLINE
JSTR_PURE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static void *
jstr_memrchr(const void *JSTR_RST const _s,
	     const int _c,
	     size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(_s, _c, _n);
#else
	if (jstr_unlikely(*(char *)_s == '\0'))
		return NULL;
	const unsigned char *const start = (unsigned char *)_s;
	const unsigned char *end = start + _n - 1;
	do
		if (*end == _c)
			return (void *)end;
	while (jstr_likely(--end <= start));
	return NULL;
#endif /* !JSTR_HAVE_MEMRCHR */
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
	const size_t n = strlen(_s);
	void *p = (void *)memchr(_s, _c, n);
	if (p)
		return p;
	return (void *)(_s + n);
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
	     size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMCCPY
	return memccpy(_dst, _src, _c, _n);
#else
	void *p = memchr(_src, _c, _n);
	if (p)
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
jstr_strncasecmp(const char *JSTR_RST const s1,
		 const char *JSTR_RST const s2,
		 size_t _n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, _n);
#else
	const unsigned char *p1 = (unsigned char *)s1;
	const unsigned char *p2 = (unsigned char *)s2;
	for (char _c; _n--; ++p1, ++p2) {
		switch (*p1) {
		default:
			JSTR_CASE_LOWER
			_c = *p1;
			break;
			JSTR_CASE_UPPER
			_c = *p1 - 'A' + 'a';
			break;
		case '\0':
			return *p2;
		}
		switch (*p2) {
		default:
			JSTR_CASE_LOWER
			if (*p2 != _c)
				return _c - *p2;
			break;
			JSTR_CASE_UPPER
			if ((*p2 - 'A' + 'a') != _c)
				return _c - (*p2 - 'A' + 'a');
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
jstr_strcasecmp(const char *JSTR_RST s1,
		const char *JSTR_RST s2) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(s1, s2);
#else
	for (char _c;; ++s1, ++s2) {
		switch (*s1) {
		default:
			JSTR_CASE_LOWER
			_c = *s1;
			break;
			JSTR_CASE_UPPER
			_c = *s1 - 'A' + 'a';
			break;
		case '\0':
			return *s2;
		}
		switch (*s2) {
		default:
			JSTR_CASE_LOWER
			if (*s2 != _c)
				return _c - *s2;
			break;
			JSTR_CASE_UPPER
			if ((*s2 - 'A' + 'a') != _c)
				return _c - (*s2 - 'A' + 'a');
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
#define PRIV_JSTR_MEMMEMR(shift_type, ne_iterator_type)                                    \
	do {                                                                               \
		const unsigned char *h = (unsigned char *)_hs + _hslen + _nelen;           \
		const unsigned char *const n = (unsigned char *)_ne;                       \
		const unsigned char *const start = (unsigned char *)_hs;                   \
		size_t _tmp;                                                               \
		size_t shift1;                                                             \
		size_t mtc1 = _nelen - 1;                                                  \
		size_t off = 0;                                                            \
		shift_type shift[256];                                                     \
		memset(shift, 0, sizeof(shift));                                           \
		for (ne_iterator_type i = 1; jstr_likely(i < (ne_iterator_type)mtc1); ++i) \
			shift[JSTR_HASH2(n + i)] = i;                                      \
		shift1 = mtc1 - shift[JSTR_HASH2(n + mtc1)];                               \
		shift[JSTR_HASH2(n + mtc1)] = mtc1;                                        \
		do {                                                                       \
			do {                                                               \
				h -= mtc1;                                                 \
				_tmp = shift[JSTR_HASH2(h)];                               \
			} while (!_tmp && h >= start);                                     \
			h -= _tmp;                                                         \
			if (mtc1 < 15 || !memcmp(h + off, n + off, 8)) {                   \
				if (!memcmp(h, n, _nelen))                                 \
					return (void *)h;                                  \
				off = (off >= 8 ? off : mtc1) - 8;                         \
			}                                                                  \
			h -= shift1;                                                       \
		} while (h >= start);                                                      \
		return NULL;                                                               \
	} while (0)
	if (jstr_unlikely(_hslen > 256))
		PRIV_JSTR_MEMMEMR(size_t, size_t);
	PRIV_JSTR_MEMMEMR(uint8_t, int);
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
		return jstr_memrchr(_hs, *(char *)_ne, _hslen);
	case 2: {
		const unsigned char *const start = (unsigned char *)_hs;
		const unsigned char *h = start + _hslen - 1;
		const unsigned char *n = (unsigned char *)_ne;
		const uint16_t nw = n[1] << 8 | n[0];
		uint16_t hw = h[0] << 8 | h[-1];
		for (h -= 2; jstr_likely(h >= start) && hw != nw; hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	case 3: {
		const unsigned char *const start = (unsigned char *)_hs;
		const unsigned char *h = start + _hslen - 1;
		const unsigned char *n = (unsigned char *)_ne;
		const uint32_t nw = n[2] << 24 | n[1] << 16 | n[0] << 8;
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8;
		for (h -= 3; jstr_likely(h >= start) && hw != nw; hw = (hw | *h--) << 8)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	case 4: {
		const unsigned char *const start = (unsigned char *)_hs;
		const unsigned char *h = start + _hslen - 1;
		const unsigned char *n = (unsigned char *)_ne;
		const uint32_t nw = n[3] << 24 | n[2] << 16 | n[1] << 8 | n[0];
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8 | h[-3];
		for (h -= 4; jstr_likely(h >= start) && hw != nw; hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
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
#define PRIV_JSTR_STRSTRCASE(shift_type, ne_iterator_type)                                         \
	do {                                                                                       \
		const unsigned char *h = (unsigned char *)_hs;                                     \
		const unsigned char *const n = (unsigned char *)_ne;                               \
		const unsigned char *const end = h + _hslen - _nelen;                              \
		size_t _tmp;                                                                       \
		size_t shift1;                                                                     \
		size_t mtc1 = _nelen - 1;                                                          \
		size_t off = 0;                                                                    \
		shift_type shift[256];                                                             \
		memset(shift, 0, sizeof(shift));                                                   \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i)                      \
			shift[JSTR_HASH2_LOWER(n + i)] = i;                                        \
		shift1 = mtc1 - shift[JSTR_HASH2_LOWER(n + mtc1)];                                 \
		shift[JSTR_HASH2_LOWER(n + mtc1)] = mtc1;                                          \
		do {                                                                               \
			do {                                                                       \
				h += mtc1;                                                         \
				_tmp = shift[JSTR_HASH2_LOWER(h)];                                 \
			} while (!_tmp && h <= end);                                               \
			h -= _tmp;                                                                 \
			if (_tmp < mtc1)                                                           \
				continue;                                                          \
			if (mtc1 < 15 || !jstr_strncasecmp((char *)h + off, (char *)n + off, 8)) { \
				if (!jstr_strncasecmp((char *)h, (char *)n, _nelen))               \
					return (char *)h;                                          \
				off = (off >= 8 ? off : mtc1) - 8;                                 \
			}                                                                          \
			h += shift1;                                                               \
		} while (h <= end);                                                                \
		return NULL;                                                                       \
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
priv_strcasechr(const char *JSTR_RST const _s,
		     const int _c,
		     const size_t _n) JSTR_NOEXCEPT
{
	enum { l = 0,
	       u,
	};
	unsigned char cc[2];
	switch (_c) {
		JSTR_CASE_UPPER
		cc[u] = _c;
		cc[l] = _c - 'A' + 'a';
		break;
		JSTR_CASE_LOWER
		cc[l] = _c;
		cc[u] = _c - 'a' + 'A';
		break;
	default:
		return (char *)memchr(_s, _c, _n);
	}
	unsigned char *h = (unsigned char *)_s;
	for (;; ++h)
		switch (*h) {
			JSTR_CASE_UPPER
			if (*h == cc[u])
				return (char *)h;
			break;
			JSTR_CASE_LOWER
			if (*h == cc[l])
				return (char *)h;
			break;
		case '\0':
			return NULL;
		}
}

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#	pragma GCC diagnostic push
#endif /* defined(__GNUC__) || defined(__clang__) */

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
#if JSTR_HAVE_STRCASESTR
JSTR_INLINE
JSTR_DEPRECATED("strcasestr is available! _hslen and _nelen are wasted.", strcasestr)
#endif /* JSTR_HAVE_STRCASESTR */
static char *
jstr_memcasemem(const char *JSTR_RST const _hs,
			  const size_t _hslen,
			  const char *JSTR_RST const _ne,
			  const size_t _nelen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR
	return (char *)strcasestr(_hs, _ne);
#else
	if (jstr_unlikely(_hslen < _nelen))
		return NULL;
	switch (_nelen) {
	case 0: return (char *)_hs;
	case 1: return priv_strcasechr(_hs, *_ne, _hslen);
	case 4:
		if (jstr_islower(*(_ne + 3)))
			goto do3;
		break;
do3:
	case 3:
		if (jstr_islower(*(_ne + 2)))
			goto do2;
		break;
do2:
	case 2:
		if (jstr_islower(*_ne) && jstr_islower(*(_ne + 1)))
			return (char *)PRIV_JSTR_MEMMEM(_hs, _hslen, _ne, _nelen);
		break;
	}
	return priv_jstr_memcasemem3(_hs, _hslen, _ne, _nelen);
#endif
}

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
  Checks if S2 is in end of S1.
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

/*
  Checks if S2 is in end of S1.
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
	do {                                                                      \
		unsigned char *d = (unsigned char *)_dst;                         \
		unsigned char sbuf[_max_digits];                                  \
		unsigned char *JSTR_RST _s = (unsigned char *)sbuf;               \
		unsigned int neg = (_num < 0) ? (_num = -_num, 1) : 0;            \
		unsigned char *const end = (unsigned char *)_s + _max_digits - 1; \
		_s = end;                                                         \
		do                                                                \
			*_s-- = _num % _base + '0';                               \
		while (_num /= 10);                                               \
		if (neg)                                                          \
			*_s = '-';                                                \
		else                                                              \
			++_s;                                                     \
		while (_s <= end)                                                 \
			*d++ = *_s++;                                             \
		*d = '\0';                                                        \
		return (char *)d;                                                 \
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
	do {                                                     \
		unsigned char *d = (unsigned char *)_dst;        \
		unsigned char sbuf[_max_digits];                 \
		unsigned char *JSTR_RST _s = sbuf;               \
		unsigned char *const end = _s + _max_digits - 1; \
		_s = end;                                        \
		do                                               \
			*_s-- = _num % _base + '0';              \
		while (_num /= 10);                              \
		++_s;                                            \
		while (_s <= end)                                \
			*d++ = *_s++;                            \
		*d = '\0';                                       \
		return (char *)d;                                \
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
		const int n) JSTR_NOEXCEPT
{
	int cnt = 0;
	while ((_s = (char *)memchr(_s, _c, n)))
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
	       const char *JSTR_RST const _searc,
	       size_t _sz,
	       const size_t _searclen) JSTR_NOEXCEPT
{
	int cnt = 0;
	while ((_s = (char *)memmem(_s, _sz, _searc, _searclen))) {
		++cnt;
		_s += _searclen;
		_sz -= _searclen;
	}
	return cnt;
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
	   const char *JSTR_RST const _searc) JSTR_NOEXCEPT
{
	int cnt = 0;
	while ((_s = strstr(_s, _searc)))
		++cnt;
	return cnt;
}

#ifdef __cplusplus
} /* extern C */
#endif /* JSTR_NAMESPACE */

#endif /* JSTR_STRING_H_DEF */
