#ifndef JSTR_STRING_H_DEF
#define JSTR_STRING_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "jstr_macros.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#	define JSTR_IN_NAMESPACE 1
#endif /* JSTR_EXTERN_C */

#define JSTR_RST JSTR_RESTRICT

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_stpcpy(char *JSTR_RST const dst,
			 const char *JSTR_RST const src) JSTR_NOEXCEPT
#ifndef JSTR_HAS_STPCPY
{
	const size_t slen = strlen(src);
	memcpy(dst, src, slen + 1);
	return dst + slen;
}
#else
{
	return stpcpy(dst, src);
}
#endif // !JSTR_HAS_STPCPY

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
static void *jstr_memrchr(char *JSTR_RST const s,
			  const int c,
			  size_t n) JSTR_NOEXCEPT
#ifndef JSTR_HAS_MEMRCHR
{
	if (unlikely(!*s))
		return NULL;
	char *end = s + n - 1;
	do
		if (*end == c)
			return end;
	while (--end <= s);
	return NULL;
}
#else
{
	return memrchr(s, c, n);
}
#endif // !JSTR_HAS_MEMRCHR

/*
  Return value:
  Pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
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
static int jstr_casecmp(const char *JSTR_RST s1,
			const char *JSTR_RST s2) JSTR_NOEXCEPT
{
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
				return 1;
			break;
			JSTR_CASE_UPPER
			if ((*s2 - 'A' + 'a') != c)
				return 1;
			break;
		case '\0':
			return 1;
		}
	}
}

#ifdef JSTR_HAS_MEMMEM
#	define jstr_memmem(hs, hlen, ne, nlen) memmem(hs, hlen, ne, nlen)
#else
#	define jstr_memmem(hs, hlen, ne, nlen) strstr(hs, ne)
#endif /* JSTR_HAS_MEMMEM */

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_RST

#endif /* JSTR_STRING_H_DEF */
