#ifndef JSTR_STRING_H_DEF
#define JSTR_STRING_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_ctype.h"
#include "_jstr_macros.h"

#ifdef __cplusplus
namespace jstr {
#endif /* JSTR_NAMESPACE */

#ifdef JSTR_HAS_MEMMEM
#	define PRIVATE_JSTR_MEMMEM(hs, hlen, ne, nlen) memmem(hs, hlen, ne, nlen)
#else
#	define PRIVATE_JSTR_MEMMEM(hs, hlen, ne, nlen) strstr(hs, ne)
#endif /* JSTR_HAS_MEMMEM */

#ifdef __cplusplus
#	define JSTR_GLOBAL(func) ::func
#else
#	define JSTR_GLOBAL(func) func
#endif /* __cpluslus */

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static void *jstr_mempcpy(char *JSTR_RST const dst,
			  const char *JSTR_RST const src,
			  const size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_MEMPCPY
	return JSTR_GLOBAL(mempcpy(dst, src, n));
#else
	memcpy(dst, src, n);
	return dst + n;
#endif /* !JSTR_HAS_STPCPY */
}

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_stpcpy(char *JSTR_RST const dst,
			 const char *JSTR_RST const src) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STPCPY
	return JSTR_GLOBAL(stpcpy(dst, src));
#else
	const size_t slen = strlen(src);
	memcpy(dst, src, slen + 1);
	return dst + slen;
#endif /* !JSTR_HAS_STPCPY */
}

/*
  Return pointer to last C in S.
  Return value:
  pointer to last C;
  NULL if not found.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static void *jstr_memrchr(const void *JSTR_RST const s,
			  const int c,
			  size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_MEMRCHR
	return (void *)JSTR_GLOBAL(memrchr(s, c, n));
#else
	if (unlikely(!*(char *)s))
		return NULL;
	const unsigned char *const start = (unsigned char *)s;
	const unsigned char *end = start + n - 1;
	do
		if (*end == c)
			return (void *)end;
	while (--end <= start);
	return NULL;
#endif /* !JSTR_HAS_MEMRCHR */
}

/*
  Return value:
  Pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *jstr_stpcat(char *JSTR_RST dst,
			 const char *JSTR_RST src) JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_stpcpy(dst, src);
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_strncasecmp(const char *JSTR_RST const s1,
			    const char *JSTR_RST const s2,
			    size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRNCASECMP
	return JSTR_GLOBAL(strncasecmp(s1, s2, n));
#else
	const unsigned char *p1 = (unsigned char *)s1;
	const unsigned char *p2 = (unsigned char *)s2;
	for (char c; n--; ++p1, ++p2) {
		switch (*p1) {
		default:
			JSTR_CASE_LOWER
			c = *p1;
			break;
			JSTR_CASE_UPPER
			c = *p1 - 'A' + 'a';
			break;
		case '\0':
			return *p2;
		}
		switch (*p2) {
		default:
			JSTR_CASE_LOWER
			if (*p2 != c)
				return c - *p2;
			break;
			JSTR_CASE_UPPER
			if ((*p2 - 'A' + 'a') != c)
				return c - (*p2 - 'A' + 'a');
			break;
		case '\0':
			return 1;
		}
	}
	return 0;
#endif /* JSTR_HAS_STRNCASECMP */
}

/*
  Compare S1 with S2 case-insensitively.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_strcasecmp(const char *JSTR_RST s1,
			   const char *JSTR_RST s2) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASECMP
	return JSTR_GLOBAL(strcasecmp(s1, s2));
#else
	for (char c;; ++s1, ++s2) {
		switch (*s1) {
		default:
			JSTR_CASE_LOWER
			c = *s1;
			break;
			JSTR_CASE_UPPER
			c = *s1 - 'A' + 'a';
			break;
		case '\0':
			return *s2;
		}
		switch (*s2) {
		default:
			JSTR_CASE_LOWER
			if (*s2 != c)
				return c - *s2;
			break;
			JSTR_CASE_UPPER
			if ((*s2 - 'A' + 'a') != c)
				return c - (*s2 - 'A' + 'a');
			break;
		case '\0':
			return 1;
		}
	}
#endif
}

JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static void *private_jstr_memrmem(const void *JSTR_RST const hs,
				  const size_t hslen,
				  const void *JSTR_RST const ne,
				  const size_t nelen) JSTR_NOEXCEPT
{
#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % sizeof(shift))
#define PRIVATE_JSTR_MEMMEMR(shift_type, ne_iterator_type)                    \
	do {                                                                  \
		const unsigned char *h = (unsigned char *)hs + hslen - nelen; \
		const unsigned char *const n = (unsigned char *)ne;           \
		const unsigned char *const start = (unsigned char *)hs;       \
		size_t tmp;                                                   \
		size_t shift1;                                                \
		size_t mtc1 = nelen - 1;                                      \
		size_t off = 0;                                               \
		shift_type shift[256];                                        \
		memset(shift, 0, sizeof(shift));                              \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i) \
			shift[JSTR_HASH2(n + i)] = i;                         \
		shift1 = mtc1 - shift[JSTR_HASH2(n + mtc1)];                  \
		shift[JSTR_HASH2(n + mtc1)] = mtc1;                           \
		while (h >= start) {                                          \
			do {                                                  \
				h -= mtc1;                                    \
				tmp = shift[JSTR_HASH2(h)];                   \
			} while (!tmp && h >= start);                         \
			h -= tmp;                                             \
			if (tmp < mtc1)                                       \
				continue;                                     \
			if (mtc1 < 15 || !memcmp(h + off, n + off, 8)) {      \
				if (!memcmp(h, n, nelen))                     \
					return (void *)h;                     \
				off = (off >= 8 ? off : mtc1) - 8;            \
			}                                                     \
			h -= shift1;                                          \
		}                                                             \
		return NULL;                                                  \
	} while (0)
	if (unlikely(hslen > 256))
		PRIVATE_JSTR_MEMMEMR(size_t, size_t);
	PRIVATE_JSTR_MEMMEMR(uint8_t, int);
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
static void *jstr_memrmem_constexpr(const void *JSTR_RST const hs,
				    const size_t hslen,
				    const void *JSTR_RST const ne,
				    const size_t nelen) JSTR_NOEXCEPT
{
	if (unlikely(hslen < nelen))
		return NULL;
	switch (nelen) {
	case 0:
		return (void *)hs;
	case 1:
		return jstr_memrchr(hs, *(char *)ne, hslen);
	case 2: {
		const unsigned char *const start = (unsigned char *)hs;
		const unsigned char *h = start + hslen;
		const unsigned char *n = (unsigned char *)ne;
		const uint16_t nw = n[1] << 8 | n[0];
		uint16_t hw = h[-1] << 8 | h[0];
		for (h -= 2; h >= start && hw != nw; hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	default: return private_jstr_memrmem(hs, hslen, ne, nelen);
	}
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static void *jstr_memrmem(const void *JSTR_RST const hs,
			  const size_t hslen,
			  const void *JSTR_RST const ne,
			  const size_t nelen) JSTR_NOEXCEPT
{
	return jstr_memrmem_constexpr(hs, hslen, ne, nelen);
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *jstr_strrstr_constexpr(const char *JSTR_RST const hs,
				    const size_t hslen,
				    const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
	return (char *)jstr_memrmem_constexpr(hs, hslen, ne, strlen((char *)ne));
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *jstr_strrstr(const char *JSTR_RST const hs,
			  const size_t hslen,
			  const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
	return (char *)jstr_memrmem(hs, hslen, ne, strlen((char *)ne));
}

JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *private_jstr_strcasestr_mem3(const char *JSTR_RST const hs,
					  const size_t hslen,
					  const char *JSTR_RST const ne,
					  const size_t nelen) JSTR_NOEXCEPT
{
#define JSTR_HASH2_LOWER(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % sizeof(shift))
#define PRIVATE_JSTR_STRSTRCASE(shift_type, ne_iterator_type)                                      \
	do {                                                                                       \
		const unsigned char *h = (unsigned char *)hs;                                      \
		const unsigned char *const n = (unsigned char *)ne;                                \
		const unsigned char *const end = h + hslen - nelen;                                \
		size_t tmp;                                                                        \
		size_t shift1;                                                                     \
		size_t mtc1 = nelen - 1;                                                           \
		size_t off = 0;                                                                    \
		shift_type shift[256];                                                             \
		memset(shift, 0, sizeof(shift));                                                   \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i) {                    \
			shift[i] = JSTR_HASH2_LOWER(ne + i);                                       \
		}                                                                                  \
		shift1 = mtc1 - shift[JSTR_HASH2_LOWER(n + mtc1)];                                 \
		shift[JSTR_HASH2_LOWER(n + mtc1)] = mtc1;                                          \
		while (h <= end) {                                                                 \
			do {                                                                       \
				h += mtc1;                                                         \
				tmp = shift[JSTR_HASH2_LOWER(h)];                                  \
			} while (!tmp && h <= end);                                                \
			h -= tmp;                                                                  \
			if (tmp < mtc1)                                                            \
				continue;                                                          \
			if (mtc1 < 15 || !jstr_strncasecmp((char *)h + off, (char *)n + off, 8)) { \
				if (!jstr_strncasecmp((char *)h, (char *)n, nelen))                \
					return (char *)h;                                          \
				off = (off >= 8 ? off : mtc1) - 8;                                 \
			}                                                                          \
			h += shift1;                                                               \
		}                                                                                  \
		return NULL;                                                                       \
	} while (0)
	if (unlikely(hslen > 256))
		PRIVATE_JSTR_STRSTRCASE(size_t, size_t);
	PRIVATE_JSTR_STRSTRCASE(uint8_t, int);
}

/*
   Find C in S case-insensitively.
   S MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_strcasechr(const char *JSTR_RST const s,
			     const int c,
			     const size_t n) JSTR_NOEXCEPT
{
	enum { l = 0,
	       u,
	};
	unsigned char cc[2];
	switch (c) {
		JSTR_CASE_UPPER
		cc[u] = c;
		cc[l] = c - 'A' + 'a';
		break;
		JSTR_CASE_LOWER
		cc[l] = c;
		cc[u] = c - 'a' + 'A';
		break;
	default:
		return (char *)memchr(s, c, n);
	}
	unsigned char *h = (unsigned char *)s;
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

/*
   Find C in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_memcasechr(const char *JSTR_RST const s,
			     const int c,
			     size_t n) JSTR_NOEXCEPT
{
	enum { l = 0,
	       u,
	};
	unsigned char cc[2];
	switch (c) {
		JSTR_CASE_UPPER
		cc[u] = c;
		cc[l] = c - 'A' + 'a';
		break;
		JSTR_CASE_LOWER
		cc[l] = c;
		cc[u] = c - 'a' + 'A';
		break;
	default:
		return (char *)memchr(s, c, n);
	}
	unsigned char *h = (unsigned char *)s;
	for (; n--; ++h)
		switch (*h) {
			JSTR_CASE_UPPER
			if (*h == cc[u])
				return (char *)h;
			break;
			JSTR_CASE_LOWER
			if (*h == cc[l])
				return (char *)h;
			break;
		}
	return NULL;
}

/*
   Use ONLY if strcasestr is unavailable.
   Find NE in HS case-insensitively.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *jstr_strcasestr_mem_constexpr(const char *JSTR_RST const hs,
					   const size_t hslen,
					   const char *JSTR_RST const ne,
					   const size_t nelen) JSTR_NOEXCEPT
{
#ifdef jJSTR_HAS_STRCASESTR
	return (char *)JSTR_GLOBAL(strcasestr(hs, ne));
#else
	switch (nelen) {
	case 0: return (char *)hs;
	case 1: return jstr_strcasechr(hs, *ne, hslen);
	case 2:
		switch (*ne) {
			JSTR_CASE_UPPER
			JSTR_CASE_LOWER
			break;
		default:
			switch (*(ne + 1)) {
				JSTR_CASE_UPPER
				JSTR_CASE_LOWER
				break;
			default:
				return (char *)PRIVATE_JSTR_MEMMEM(hs, hslen, ne, nelen);
			}
		}
		/* FALLTHROUGH */
	default:
		return private_jstr_strcasestr_mem3(hs, hslen, ne, nelen);
	}
#endif
}

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#	pragma GCC diagnostic push
#endif /* defined(__GNUC__) || defined(__clang__) */

/*
   Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_memcasemem_constexpr(const char *JSTR_RST const hs,
				       const size_t hslen,
				       const char *JSTR_RST const ne,
				       const size_t nelen) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASESTR
	return (char *)JSTR_GLOBAL(strcasestr(hs, ne));
#else
	switch (nelen) {
	case 0: return (char *)hs;
	case 1: return jstr_memcasechr(hs, *ne, hslen);
	case 2:
		switch (*ne) {
			JSTR_CASE_UPPER
			JSTR_CASE_LOWER
			break;
		default:
			switch (*(ne + 1)) {
				JSTR_CASE_UPPER
				JSTR_CASE_LOWER
				break;
			default:
				return (char *)PRIVATE_JSTR_MEMMEM(hs, hslen, ne, nelen);
			}
		}
		/* FALLTHROUGH */
	default:
		return private_jstr_strcasestr_mem3(hs, hslen, ne, nelen);
	}
#endif
}

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic pop
#endif /* defined(__GNUC__) || defined(__clang__) */

/*
   Find NE in HS case-insensitively.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_memcasemem(const char *JSTR_RST const hs,
			     const size_t hslen,
			     const char *JSTR_RST const ne,
			     const size_t nelen) JSTR_NOEXCEPT
{
	return jstr_memcasemem_constexpr(hs, hslen, ne, nelen);
}

#if defined(__GNUC__) || defined(__clang__)
/* #	pragma GCC diagnostic ignored "-Wunused-parameter" */
/* #	pragma GCC diagnostic push */
#endif /* defined(__GNUC__) || defined(__clang__) */

/*
   Use ONLY if strcasestr is unavailable.
   Find NE in HS case-insensitively.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
#ifdef JSTR_HAS_STRCASESTR
JSTR_DEPRECATED("strcasestr is available! hslen and nelen are wasted.", strcasestr)
#endif /* JSTR_HAS_STRCASESTR */
static char *jstr_strcasestr_mem(const char *JSTR_RST const hs,
				 const size_t hslen,
				 const char *JSTR_RST const ne,
				 const size_t nelen) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASESTR
	return (char *)JSTR_GLOBAL(strcasestr(hs, ne));
#else
	return jstr_strcasestr_mem_constexpr(hs, hslen, ne, nelen);
#endif /* JSTR_HAS_STRCASESTR */
}

#if defined(__GNUC__) || defined(__clang__)
/* #	pragma GCC diagnostic pop */
#endif /* defined(__GNUC__) || defined(__clang__) */

/*
   Find NE in HS case-insensitively.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *jstr_strcasestr_constexpr(const char *JSTR_RST const hs,
				       const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASESTR
	return (char *)JSTR_GLOBAL(strcasestr(hs, ne));
#else
	return jstr_strcasestr_mem_constexpr(hs, strlen(hs), ne, strlen(ne));
#endif /* JSTR_HAS_STRCASESTR */
}

/*
   Find NE in HS case-insensitively.
   HS MUST be nul terminated.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *jstr_strcasestr(const char *JSTR_RST const hs,
			     const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASESTR
	return (char *)JSTR_GLOBAL(strcasestr(hs, ne));
#else
	return jstr_strcasestr_mem(hs, strlen(hs), ne, strlen(ne));
#endif /* JSTR_HAS_STRCASESTR */
}

/* Copy no more than N bytes of SRC to DEST, stopping when C is found.
   Return the position in DEST one byte past where C was copied, or
   NULL if C was not found in the first N bytes of SRC.  */
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static void *jstr_memccpy(void *JSTR_RST dst,
			  const void *JSTR_RST src,
			  int c,
			  size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_MEMCCPY
	return memccpy(dst, src, c, n);
#else
	void *p = memchr(src, c, n);
	if (p)
		return jstr_mempcpy(dst, src, p - src + 1);
	memcpy(dst, src, n);
	return NULL;
#endif /* JSTR_HAS_MEMCPY */
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static void *jstr_strchrnul(const char *JSTR_RST const s,
			    const int c)
{
#ifdef JSTR_HAS_STRCHRNUL
	return (char *)JSTR_GLOBAL(strchrnul(s, c));
#else
	const size_t n = strlen(s);
	void *p = (void *)memchr(s, c, n);
	if (p)
		return p;
	return (void *)(s + n);
#endif /* JSTR_HAS_STRCHRNUL */
}

#ifdef __cplusplus
} /* namespace jstr */
#endif /* JSTR_NAMESPACE */

#undef JSTR_HASH2
#undef JSTR_HASH2_LOWER
#undef JSTR_GLOBAL

#undef PRIVATE_JSTR_MEMRMEM
#undef PRIVATE_JSTR_STRSTRCASE
#undef JSTR_GLOBAL

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC poison private_jstr_memrmem
#	pragma GCC poison private_jstr_strcasestr_mem3
#endif /* defined(__GNUC__) || defined(__clang__) */

#endif /* JSTR_STRING_H_DEF */
