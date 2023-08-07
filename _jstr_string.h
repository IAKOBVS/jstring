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

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static void *jstr_memcpyp(char *JSTR_RST const dst,
			  const char *JSTR_RST const src,
			  const size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_MEMPCPY
	return mempcpy(dst, src, n);
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
static char *jstr_strcpyp(char *JSTR_RST const dst,
			  const char *JSTR_RST const src) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STPCPY
	return stpcpy(dst, src);
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
static void *jstr_memchrr(const void *JSTR_RST const s,
			  const int c,
			  size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_MEMRCHR
	return (void *)memrchr(s, c, n);
#else
	if (unlikely(!*(char *)s))
		return NULL;
	const unsigned char *const start = (unsigned char *)s;
	const unsigned char *end = (unsigned char *)s + n - 1;
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
static char *jstr_strcatp(char *JSTR_RST dst,
			  const char *JSTR_RST src) JSTR_NOEXCEPT
{
	dst += strlen(dst);
	return jstr_strcpyp(dst, src);
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
static int jstr_strncmpcase(const char *JSTR_RST s1,
			    const char *JSTR_RST s2,
			    size_t n) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	for (char c; n--; ++s1, ++s2) {
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
static int jstr_strcmpcase(const char *JSTR_RST s1,
			   const char *JSTR_RST s2) JSTR_NOEXCEPT
{
#ifdef JSTR_HAS_STRCASECMP
	return strcasecmp(s1, s2);
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
static void *private_jstr_memmemr(const void *JSTR_RST const hs,
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
static void *jstr_memmemr_constexpr(const void *JSTR_RST const hs,
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
		return jstr_memchrr(hs, *(char *)ne, hslen);
	case 2: {
		const unsigned char *const start = (unsigned char *)hs;
		const unsigned char *h = (unsigned char *)hs + hslen;
		const unsigned char *n = (unsigned char *)ne;
		const uint16_t nw = n[1] << 8 | n[0];
		uint16_t hw = h[-1] << 8 | h[0];
		for (h -= 2; h >= start && hw != nw; hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	default: return private_jstr_memmemr(hs, hslen, ne, nelen);
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
static void *jstr_memmemr(const void *JSTR_RST const hs,
			  const size_t hslen,
			  const void *JSTR_RST const ne,
			  const size_t nelen) JSTR_NOEXCEPT
{
	return jstr_memmemr_constexpr(hs, hslen, ne, nelen);
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
static char *jstr_strstrr_constexpr(const char *JSTR_RST const hs,
				    const size_t hslen,
				    const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
	return (char *)jstr_memmemr_constexpr(hs, hslen, ne, strlen((char *)ne));
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
static char *jstr_strstrr(const char *JSTR_RST const hs,
			  const size_t hslen,
			  const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
	return (char *)jstr_memmemr(hs, hslen, ne, strlen((char *)ne));
}

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
JSTR_INLINE
static char *jstr_memmemcase(const char *JSTR_RST const hs,
			     const size_t hslen,
			     const char *JSTR_RST const ne,
			     const size_t nelen) JSTR_NOEXCEPT
{
/* #ifndef JSTR_HAS_STRCASESTR */
/* 	return (char *)strcasestr(hs, ne); */
/* #else */
#define JSTR_HASH2_LOWER(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % sizeof(shift))
#define PRIVATE_JSTR_STRSTRCASE(shift_type, ne_iterator_type)                         \
	do {                                                                          \
		const unsigned char *h = (unsigned char *)hs;                         \
		const unsigned char *const n = (unsigned char *)ne;                   \
		const unsigned char *const end = (unsigned char *)hs + hslen - nelen; \
		size_t tmp;                                                           \
		size_t shift1;                                                        \
		size_t mtc1 = nelen - 1;                                              \
		size_t off = 0;                                                       \
		shift_type shift[256];                                                \
		memset(shift, 0, sizeof(shift));                                      \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i)         \
			shift[JSTR_HASH2_LOWER(n + i)] = i;                           \
		shift1 = mtc1 - shift[JSTR_HASH2_LOWER(n + mtc1)];                    \
		shift[JSTR_HASH2_LOWER(n + mtc1)] = mtc1;                             \
		while (h <= end) {                                                    \
			do {                                                          \
				h += mtc1;                                            \
				tmp = shift[JSTR_HASH2_LOWER(h)];                     \
			} while (!tmp && h <= end);                                   \
			h -= tmp;                                                     \
			if (tmp < mtc1)                                               \
				continue;                                             \
			if (mtc1 < 15 || !jstr_strcmpcase(h + off, n + off, 8)) {     \
				if (!memcmp(h, n, nelen))                             \
					return (char *)h;                             \
				off = (off >= 8 ? off : mtc1) - 8;                    \
			}                                                             \
			h -= shift1;                                                  \
		}                                                                     \
		return NULL;                                                          \
	} while (0)
	if (unlikely(hslen > 256))
		PRIVATE_JSTR_STRSTRCASE(size_t, size_t);
	PRIVATE_JSTR_STRSTRCASE(uint8_t, int);
	/* #endif */
}

#ifdef JSTR_HAS_MEMMEM
#	define PRIVATE_JSTR_MEMMEM(hs, hlen, ne, nlen) memmem(hs, hlen, ne, nlen)
#else
#	define PRIVATE_JSTR_MEMMEM(hs, hlen, ne, nlen) strstr(hs, ne)
#endif /* JSTR_HAS_MEMMEM */

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_HASH2
#undef JSTR_HASH2_LOWER
#undef JSTR_EXTERN_C
#undef JSTR_NAMESPACE

#undef PRIVATE_JSTR_MEMMEMR

#endif /* JSTR_STRING_H_DEF */
