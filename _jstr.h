#ifndef JSTR_H_DEF
#define JSTR_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#ifndef __cplusplus
#	include "_jstr_pp_va_args_macros.h"
#endif /* __cplusplus */

#include "_jregex.h"
#include "_jstr_builder.h"
#include "_jstr_config.h"
#include "_jstr_ctype.h"
#include "_jstr_io.h"
#include "_jstr_macros.h"
#include "_jstr_memmem.h"
#include "_jstr_replace.h"
#include "_jstr_string.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
   Return value:
   New _len of S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *
jstr_append_mem_p_f(char *JSTR_RST const s,
		    const char *JSTR_RST const _src,
		    const size_t sz,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	memcpy(s, _src, _srclen + 1);
	return s + sz + _srclen;
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_append_mem(char **JSTR_RST const s,
		size_t *JSTR_RST const sz,
		size_t *JSTR_RST const cap,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + _srclen)
		JSTR_REALLOC(*s, *cap, *sz + _srclen, return);
	*sz = jstr_append_mem_p_f(*s, _src, *sz, _srclen) - *s;
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
jstr_countc(const char *JSTR_RST s,
	    const int c) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = strchr(s, c)))
		++count;
	return count;
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
jstr_countc_mem(const char *JSTR_RST s,
		const int c,
		const int n) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = (char *)memchr(s, c, n)))
		++count;
	return count;
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
jstr_count_mem(const char *JSTR_RST s,
	       const char *JSTR_RST const _searc,
	       size_t sz,
	       const size_t _searclen) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = (char *)memmem(s, sz, _searc, _searclen))) {
		++count;
		s += _searclen;
		sz -= _searclen;
	}
	return count;
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
jstr_count(const char *JSTR_RST s,
	   const char *JSTR_RST const _searc) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = strstr(s, _searc)))
		++count;
	return count;
}

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_rev_mem(char *JSTR_RST s,
	     const size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return;
	unsigned char *end = (unsigned char *)s + sz - 1;
	unsigned char *p = (unsigned char *)s;
	unsigned char tmp;
	do {
		tmp = *p;
		*p = *end;
		*end = tmp;
	} while (jstr_likely(++p < --end));
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_trim_mem_p(char *JSTR_RST const s,
		const size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return s;
	unsigned char *end = (unsigned char *)s + sz - 1;
	const unsigned char *const start = (unsigned char *)s;
	do {
		switch (*end) {
		case '\t':
		case ' ':
			continue;
		default:
			*++end = '\0';
			break;
		}
		break;
	} while (jstr_likely(--end >= start));
	return (char *)end;
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *
jstr_trim_p(char *JSTR_RST const s) JSTR_NOEXCEPT
{
	return jstr_trim_mem_p(s, strlen(s));
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_trim_j(Jstring *JSTR_RST const j) JSTR_NOEXCEPT
{
	j->size = jstr_trim_mem_p(j->data, j->size) - j->data;
}

/*
  Insert SRC into DST[AT].
  Assumes that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insert_mem_f(char *JSTR_RST const s,
		  const size_t at,
		  const char *JSTR_RST const _src,
		  const size_t _srclen) JSTR_NOEXCEPT
{
	memcpy(s + at, _src, _srclen);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insert_mem(char **JSTR_RST const s,
		size_t *JSTR_RST const sz,
		size_t *JSTR_RST const cap,
		const size_t at,
		const char *JSTR_RST const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (at + _srclen > *sz) {
		JSTR_REALLOC(*s, *cap, at + _srclen + 1, return);
		*sz = at + _srclen;
		*(*s + *sz) = '\0';
	}
	jstr_insert_mem_f(*s, at, _src, _srclen);
}

/*
  Insert SRC after C in DST.
  Assumes that S have enough space for S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insertaftc_mem_f(char *JSTR_RST const s,
		      const int c,
		      const char *JSTR_RST const _src,
		      const size_t sz,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		jstr_insert_mem_f(s, p - s + 1, _src, _srclen);
}

/*
  Insert SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insertaftc_mem(char **JSTR_RST const s,
		    size_t *JSTR_RST const sz,
		    size_t *JSTR_RST const cap,
		    const int c,
		    const char *JSTR_RST const _src,
		    const size_t _srclen) JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_insert_mem(s, sz, cap, p - *s + 1, _src, _srclen);
}

/*
  Insert SRC after end of NE in DST.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insertaft_mem_f(char *JSTR_RST const s,
		     const char *JSTR_RST const _searc,
		     const char *JSTR_RST const _src,
		     const size_t sz,
		     const size_t _searclen,
		     const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return;
	case 1:
		jstr_insertaftc_mem_f(s, *_searc, _src, sz, _srclen);
		return;
	default: {
		const char *const p = (char *)PRIVATE_JSTR_MEMMEM(s, sz, _searc, _searclen);
		if (p)
			jstr_insert_mem_f(s, p - s + _searclen, _src, _srclen);
		return;
	}
	}
}

/*
  Insert SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_insertaft_mem(char **JSTR_RST const s,
		   size_t *JSTR_RST const sz,
		   size_t *JSTR_RST const cap,
		   const char *JSTR_RST const _searc,
		   const char *JSTR_RST const _src,
		   const size_t _searclen,
		   const size_t _srclen) JSTR_NOEXCEPT
{
	switch (_searclen) {
	case 0: return;
	case 1:
		jstr_insertaftc_mem(s, sz, cap, *_searc, _src, _srclen);
		return;
	default: {
		const char *const p = (char *)PRIVATE_JSTR_MEMMEM(*s, *sz, _searc, _searclen);
		if (p)
			jstr_insert_mem(s, sz, cap, p - *s + _searclen, _src, _srclen);
		return;
	}
	}
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
	  const unsigned char _base)
{
#define PRIVATE_JSTR_NUMTOSTR(_max_digits)                                       \
	do {                                                                     \
		unsigned char *d = (unsigned char *)_dst;                        \
		unsigned char sbuf[_max_digits];                                 \
		unsigned char *JSTR_RST s = (unsigned char *)sbuf;               \
		unsigned char neg = (_num < 0) ? (_num = -_num, 1) : 0;          \
		unsigned char *const end = (unsigned char *)s + _max_digits - 1; \
		s = end;                                                         \
		do                                                               \
			*s-- = _num % _base + '0';                               \
		while (_num /= 10);                                              \
		if (neg)                                                         \
			*s = '-';                                                \
		else                                                             \
			++s;                                                     \
		while (s <= end)                                                 \
			*d++ = *s++;                                             \
		*d = '\0';                                                       \
		return (char *)d;                                                \
	} while (0)
	PRIVATE_JSTR_NUMTOSTR(JSTR_MAX_INT_DIGITS);
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
	  const unsigned char _base)
{
	PRIVATE_JSTR_NUMTOSTR(JSTR_MAX_LONG_DIGITS);
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
	   const unsigned char _base)
{
	PRIVATE_JSTR_NUMTOSTR(JSTR_MAX_LONG_DIGITS);
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
	  const unsigned char _base)
{
#define PRIVATE_JSTR_UNUMTOSTR(_max_digits)                     \
	do {                                                    \
		unsigned char *d = (unsigned char *)_dst;       \
		unsigned char sbuf[_max_digits];                \
		unsigned char *JSTR_RST s = sbuf;               \
		unsigned char *const end = s + _max_digits - 1; \
		s = end;                                        \
		do                                              \
			*s-- = _num % _base + '0';              \
		while (_num /= 10);                             \
		++s;                                            \
		while (s <= end)                                \
			*d++ = *s++;                            \
		*d = '\0';                                      \
		return (char *)d;                               \
	} while (0)
	PRIVATE_JSTR_UNUMTOSTR(JSTR_MAX_UINT_DIGITS);
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
	   const unsigned char _base)
{
	PRIVATE_JSTR_UNUMTOSTR(JSTR_MAX_ULONG_DIGITS);
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
	    const unsigned char _base)
{
	PRIVATE_JSTR_UNUMTOSTR(JSTR_MAX_ULONG_LONG_DIGITS);
}

#ifdef __cplusplus
} /* extern C */
#endif /* __cpluslus */

#undef JSTR_ALLOC_MULTIPLIER
#undef JSTR_ASCII_SIZE
#undef JSTR_EXIT_ON_MALLOC_ERROR
#undef JSTR_FREE_ON_DESTRUCTOR_CPP
#undef JSTR_GROW
#undef JSTR_GROWTH_MULTIPLIER
#undef JSTR_HASH2
#undef JSTR_HASH2_LOWER
#undef JSTR_H_REPLACE_DEF
#undef JSTR_IS_CHAR
#undef JSTR_IS_MMAP
#undef JSTR_MAX_INT_DIGITS
#undef JSTR_MAX_LONG_DIGITS
#undef JSTR_MAX_LONG_LONG_DIGITS
#undef JSTR_MAX_UINT_DIGITS
#undef JSTR_MAX_ULONG_DIGITS
#undef JSTR_MAX_ULONG_LONG_DIGITS
#undef JSTR_MIN_MMAP
#undef JSTR_NEXT_POW2
#undef JSTR_NULLIFY_PTR_ON_DELETE
#undef JSTR_NULLIFY_PTR_ON_DELETE
#undef JSTR_NULLIFY_PTR_ON_DESTRUCTOR_CPP
#undef JSTR_PRINT_ERR_MSG_ON_MALLOC_ERROR
#undef JSTR_PRINT_ERR_MSG_ON_REGEX_ERROR
#undef JSTR_REALLOC
#undef JSTR_RST
#undef JSTR_SAME_TYPE
#undef PRIVATE_JSTR_ALLOC_ONLY
#undef PRIVATE_JSTR_MEMMEM
#undef PRIVATE_JSTR_MEMMEMR
#undef PRIVATE_JSTR_MEMMEM_EXEC
#undef PRIVATE_JSTR_MIN_ALLOC
#undef PRIVATE_JSTR_STRSTRCASE

#endif /* JSTR_H_DEF */
