#ifndef JSTR_STD_H_DEF
#define JSTR_STD_H_DEF

#include <stddef.h>
#include <string.h>

#include "jstr_macros.h"

#define JSTR_RST  JSTR_RESTRICT
#define JSTR_NOEX JSTR_NOEXCEPT

/*
  Return value:
  pointer to '\0' in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_stpcpy(char *JSTR_RST dst,
			 const char *JSTR_RST src) JSTR_NOEX
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
static char *jstr_memrchr(char *JSTR_RST s,
			  const int c,
			  size_t n) JSTR_NOEX
#ifndef JSTR_HAS_MEMRCHR
{
	if (unlikely(!n))
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
	return JSTR_CAST(char *) memrchr(s, c, n);
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
			 const char *JSTR_RST src) JSTR_NOEX
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
			const char *JSTR_RST s2) JSTR_NOEX
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

#undef JSTR_RST
#undef JSTR_NOEX

#endif /* JSTR_STD_H_DEF */
