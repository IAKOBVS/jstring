#ifndef JSTR_STRING_H
#define JSTR_STRING_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-std-string.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

JSTR_FUNC
JSTR_INLINE
static char *
jstr_cpy_p(char *R dst,
	   const jstr_ty *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(dst, src->data, src->size);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_dup(jstr_ty *R dst,
	 const jstr_ty *R src)
JSTR_NOEXCEPT
{
	dst->data = (char *)malloc(src->capacity);
	PJSTR_MALLOC_ERR(dst->data, return 0);
	dst->size = jstr_cpy_p(dst->data, dst) - dst->data;
	dst->size = src->size;
	dst->capacity = src->capacity;
	return 1;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeat_len_p_unsafe(char *R s,
			 const size_t sz,
			 size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n < 2))
		return s + sz;
	--n;
	if (jstr_likely(sz > 1))
		while (n--)
			s = (char *)jstr_mempmove(s + sz, s, sz);
	else if (sz == 1)
		s = (char *)memset(s, *s, n) + n;
	*s = '\0';
	return s;
}

/*
   Return value:
   0 on error;
   1 otherwise.
*/
JSTR_FUNC
static int
jstr_repeat_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n < 2))
		return 1;
	PJSTR_RESERVE(s, sz, cap, *sz * n, return 0);
	*sz = jstr_repeat_len_p_unsafe(*s, *sz, n) - *s;
	return 1;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeatcpy_len_p(char *R dst,
		     const char *R src,
		     const size_t srcsz,
		     size_t n)
JSTR_NOEXCEPT
{
	if (jstr_likely(srcsz > 1))
		while (n--)
			dst = (char *)jstr_mempcpy(dst, src, srcsz);
	else if (srcsz == 1)
		dst = (char *)memset(dst, *src, n) + n;
	*dst = '\0';
	return dst;
}

/* Return ptr to '\0' in DST. */
JSTR_FUNC
static char *
jstr_repeatcpy_p(char *R dst,
		 const char *R src,
		 const size_t n)
JSTR_NOEXCEPT
{
	return jstr_repeatcpy_len_p(dst, src, strlen(src), n);
}

/* basename() for non nul-terminated strings. */
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_basename_len(const char *R fname,
		  const size_t sz)
JSTR_NOEXCEPT
{
	const char *const p = (char *)memchr(fname, '/', sz);
	return p ? (char *)p + 1 : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_basename(const char *R fname)
JSTR_NOEXCEPT
{
	const char *const p = strchr(fname, '/');
	return p ? (char *)p + 1 : NULL;
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
jstr_strncasecmp(const char *R s1,
		 const char *R s2,
		 size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	const unsigned char *R p1 = (unsigned char *)s1;
	const unsigned char *R p2 = (unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++))
	       && *p1++
	       && n--)
		;
	return ret;
#endif
}

/*
  Compare S1 with S2 case-insensitively.
  N must be lower than the length of S1 or S2.
  Return value:
  0 if strings match;
  non-zero otherwise.
*/
#if JSTR_HAVE_STRNCASECMP
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static int
jstr_strcasecmp_len(const char *R s1,
		    const char *R s2,
		    size_t n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	if (jstr_unlikely(n == 0))
		return 0;
	const unsigned char *R p1 = (unsigned char *)s1;
	const unsigned char *R p2 = (unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1++) - jstr_tolower(*p2++))
	       && n--)
		;
	return ret;
#endif
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
static int
jstr_strcasecmp(const char *R s1,
		const char *R s2)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASECMP
	return strcasecmp(s1, s2);
#else
	const unsigned char *R p1 = (unsigned char *)s1;
	const unsigned char *R p2 = (unsigned char *)s2;
	int ret;
	while (!(ret = jstr_tolower(*p1) - jstr_tolower(*p2++))
	       && *p1++)
		;
	return ret;
#endif
}

JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_cmp(const void *R s1,
	 const size_t s1len,
	 const void *R s2,
	 const size_t s2len)
{
	return (s1len == s2len) ? memcmp(s1, s2, s1len) : *(unsigned char *)s1 - *(unsigned char *)s2;
}

JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_casecmp(const char *R s1,
	     const size_t s1len,
	     const char *R s2,
	     const size_t s2len)
{
	return (s1len == s2len) ? jstr_strcasecmp(s1, s2) : *s1 - *s2;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strnstr2(const unsigned char *h,
	       const unsigned char *R n,
	       const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint16_t nw = n[0] << 8 | n[1];
	uint16_t hw = h[0] << 8 | h[1];
	for (++h; *h && h <= end && hw != nw; hw = hw << 8 | *++h)
		;
	return hw == nw ? (char *)(h - 1) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strnstr3(const unsigned char *h,
	       const unsigned char *R n,
	       const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8;
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8;
	for (h += 2; *h && h <= end && hw != nw; hw = (hw | *++h) << 8)
		;
	return hw == nw ? (char *)(h - 2) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strnstr4(const unsigned char *h,
	       const unsigned char *R n,
	       const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
	for (h += 3; *h && h <= end && hw != nw; hw = hw << 8 | *++h)
		;
	return hw == nw ? (char *)(h - 3) : NULL;
}

#if !JSTR_HAVE_MEMMEM

JSTR_FUNC_PURE
JSTR_INLINE
static void *
pjstr_memmem2(const unsigned char *h,
	      const unsigned char *R n,
	      const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint16_t nw = n[0] << 8 | n[1];
	uint16_t hw = h[0] << 8 | h[1];
	for (++h; h <= end && hw != nw; hw = hw << 8 | *++h)
		;
	return hw == nw ? (char *)(h - 1) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static void *
pjstr_memmem3(const unsigned char *h,
	      const unsigned char *R n,
	      const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8;
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8;
	for (h += 2; h <= end && hw != nw; hw = (hw | *++h) << 8)
		;
	return hw == nw ? (char *)(h - 2) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static void *
pjstr_memmem4(const unsigned char *h,
	      const unsigned char *R n,
	      const unsigned char *const end)
JSTR_NOEXCEPT
{
	const uint32_t nw = n[0] << 24 | n[1] << 16 | n[2] << 8 | n[3];
	uint32_t hw = h[0] << 24 | h[1] << 16 | h[2] << 8 | h[3];
	for (h += 3; h <= end && hw != nw; hw = hw << 8 | *++h)
		;
	return hw == nw ? (char *)(h - 3) : NULL;
}

#endif

#if !JSTR_HAVE_MEMMEM
#	define PJSTR_MEMMEM_FN	     pjstr_memmem
#	define PJSTR_MEMMEM_RETTYPE void *
#	include "_jstr-memmem.h"
#endif

JSTR_FUNC_PURE
static void *
jstr_memmem(const void *R hs,
	    const size_t hslen,
	    const void *R ne,
	    const size_t nelen)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMMEM
	return memmem(hs, hslen, ne, nelen);
#else
	typedef unsigned char u;
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	const unsigned char *const h = (u *)hs;
	const unsigned char *const n = (u *)ne;
	switch (nelen) {
	case 0: return (void *)hs;
	case 1: return (void *)memchr(hs, *n, nelen);
	case 2: return pjstr_memmem2(h, n, h + hslen - nelen);
	case 3: return pjstr_memmem3(h, n, h + hslen - nelen);
	case 4: return pjstr_memmem4(h, n, h + hslen - nelen);
	}
	return pjstr_memmem(h, hslen, n, nelen);
#endif
}

#define PJSTR_MEMMEM_FN	     pjstr_strnstr
#define PJSTR_MEMMEM_RETTYPE char *
#include "_jstr-memmem.h"

JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memnmem(const void *R hs,
	     const size_t hslen,
	     const void *R ne,
	     const size_t nelen,
	     const size_t n)
JSTR_NOEXCEPT
{
	return jstr_memmem(hs, JSTR_MIN(hslen, n), ne, nelen);
}

JSTR_FUNC_PURE
static char *
jstr_strnstr(const char *R hs,
	     const char *R ne,
	     size_t n)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(n == 0)
	    || jstr_unlikely(*hs == '\0'))
		return NULL;
	if (jstr_unlikely(*ne == '\0'))
		return (char *)hs;
	const char *start = hs;
	hs = jstr_strnchr(hs, *ne, n);
	n = hs - start;
	if (hs == NULL || ne[1] == '\0')
		return (char *)hs;
	if (jstr_unlikely(hs[1] == '\0'))
		return NULL;
	if (ne[2] == '\0')
		return pjstr_strnstr2((u *)hs, (u *)ne, (u *)hs + n);
	if (jstr_unlikely(hs[2] == '\0'))
		return NULL;
	if (ne[3] == '\0')
		return pjstr_strnstr3((u *)hs, (u *)ne, (u *)hs + n);
	if (jstr_unlikely(hs[3] == '\0'))
		return NULL;
	if (ne[4] == '\0')
		return pjstr_strnstr4((u *)hs, (u *)ne, (u *)hs + n);
	const size_t nl = strlen(ne);
	if (jstr_unlikely(n < nl))
		return NULL;
	size_t hl = jstr_strnlen(hs, nl);
	if (jstr_unlikely(hl < nl))
		return NULL;
	if (!memcmp(hs, ne, nl))
		return (char *)hs;
	if (hl == nl)
		return NULL;
	++hs;
	--hl;
	hl += jstr_strnlen(hs + hl, n - hl);
	return pjstr_strnstr(hs, hl, ne, nl);
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
static char *
jstr_strrstr_len(const void *R hs,
		 const size_t hslen,
		 const void *R ne,
		 const size_t nelen)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	if (jstr_unlikely(nelen == 0))
		return (char *)hs + hslen;
	const unsigned char *h = (u *)jstr_memrchr(hs, *((char *)ne + nelen - 1), hslen);
	if (h == NULL || (uintptr_t)((u *)h - (u *)hs + 1) < nelen)
		return NULL;
	const unsigned char *const start = (u *)hs;
	const unsigned char *const n = (u *)ne;
	switch (nelen) {
	case 1:
		return (char *)h;
	case 2: {
		const uint16_t nw = n[1] << 8 | n[0];
		uint16_t hw = h[0] << 8 | h[-1];
		for (--h; h >= start && hw != nw; hw = hw << 8 | *--h)
			;
		return hw == nw ? (char *)h : NULL;
	}
	case 3: {
		const uint32_t nw = n[2] << 24 | n[1] << 16 | n[0] << 8;
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8;
		for (h -= 2; h >= start && hw != nw; hw = (hw | *--h) << 8)
			;
		return hw == nw ? (char *)h : NULL;
	}
	default: {
		const uint32_t nw = n[3] << 24 | n[2] << 16 | n[1] << 8 | n[0];
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8 | h[-3];
		for (h -= 3; h >= start; hw = hw << 8 | *--h) {
			if (hw == nw)
				if (nelen == 4 || !memcmp(h, n, nelen))
					return (char *)h;
		}
		return NULL;
	}
	}
}

/*
   Find last NE in HS.
   Return value:
   Pointer to NE;
   NULL if not found.
*/
JSTR_FUNC_PURE
static char *
jstr_memrmem(const void *R hs,
	     const size_t hslen,
	     const void *R ne,
	     const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_strrstr_len(hs, hslen, ne, nelen);
}

#if !JSTR_HAVE_STRCASESTR_OPTIMIZED
#	define PJSTR_MEMMEM_FN	       pjstr_strcasestr
#	define PJSTR_MEMMEM_RETTYPE   char *
#	define PJSTR_MEMMEM_CMP_FN    jstr_strcasecmp_len
#	define PJSTR_MEMMEM_HASH2(p)  (((size_t)(jstr_tolower((p)[0])) - ((size_t)(jstr_tolower((p)[-1])) << 3)) % 256)
#	define PJSTR_MEMMEM_CHECK_EOL (1)
#	include "_jstr-memmem.h"

JSTR_FUNC_PURE
static char *
pjstr_strcasestr_bmh(const char *R h,
		     const char *R n)
JSTR_NOEXCEPT
{
	const size_t nl = strlen(n);
	const size_t hl = jstr_strnlen(h, nl | 512);
	if (hl < nl)
		return NULL;
	if (!jstr_strcasecmp_len(h, n, nl))
		return (char *)h;
	if (jstr_unlikely(hl == nl))
		return NULL;
	return pjstr_strcasestr(h + 1, hl - 1, n, nl);
}

#	define L(c) jstr_tolower(c)

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strcasestr2(const unsigned char *R h,
		  const unsigned char *R n)
JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (++h; *h && hw != nw; hw = hw << 8 | L(*++h))
		;
	return hw == nw ? (char *)(h - 1) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strcasestr3(const unsigned char *R h,
		  const unsigned char *R n)
JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8;
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8;
	for (h += 2; *h && hw != nw; hw = (hw | L(*++h)) << 8)
		;
	return hw == nw ? (char *)(h - 2) : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strcasestr4(const unsigned char *R h,
		  const unsigned char *R n)
JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8 | L(n[3]);
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8 | L(h[3]);
	for (h += 3; *h && hw != nw; hw = hw << 8 | L(*++h))
		;
	return hw == nw ? (char *)(h - 3) : NULL;
}

#	undef L

#endif /* HAVE_STRCASESTR_OPTIMIZED */

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strcasechr_generic(const char *R s,
			 int c)
JSTR_NOEXCEPT
{
	c = (char)jstr_tolower(c);
	for (; *s && jstr_tolower(*s) != c; ++s)
		;
	return *s ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_memcasechr(const char *R s,
		int c,
		size_t n)
JSTR_NOEXCEPT
{
	c = (char)jstr_tolower(c);
	for (; n-- && jstr_tolower(*s) != c; ++s)
		;
	return n ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
pjstr_strcasechr_len(const char *R s,
		     int c,
		     const int n)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCSPN_OPTIMIZED
	if (n > JSTR_STRCASECHR_THRES * 2) {
		c = (char)jstr_tolower(c);
		if (jstr_tolower(*s) == c)
			return (char *)s;
		const char a[] = { (char)c, (char)(c - 'a' + 'A'), '\0' };
		s += strcspn(s, a);
		return *s ? (char *)s : NULL;
	}
#endif
	return pjstr_strcasechr_generic(s, c);
	(void)n;
}

/*
   Return value:
   ptr to first C in S ignoring case;
   NULL if not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strcasechr_len(const char *R s,
		    const int c,
		    const size_t n)
JSTR_NOEXCEPT
{
	return (jstr_isalpha(c)) ? pjstr_strcasechr_len(s, c, n) : (char *)memchr(s, c, n);
}

/*
   Return value:
   ptr to first C in S ignoring case;
   NULL if not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strcasechr(const char *R s,
		const int c)
JSTR_NOEXCEPT
{
	return jstr_isalpha(c) ? pjstr_strcasechr_generic(s, c) : (char *)strchr(s, c);
}

#if !JSTR_HAVE_STRCASESTR_OPTIMIZED
#	define PJSTR_MEMMEM_FN	      pjstr_strcasestr_len
#	define PJSTR_MEMMEM_RETTYPE  char *
#	define PJSTR_MEMMEM_CMP_FN   jstr_strcasecmp_len
#	define PJSTR_MEMMEM_HASH2(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)(jstr_tolower((p)[-1])) << 3)) % 256)
#	include "_jstr-memmem.h"
#endif

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
jstr_strcasestr_len(const char *R hs,
		    size_t hslen,
		    const char *R ne,
		    const size_t nelen)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
	(void)hslen;
	(void)nelen;
#else
	typedef unsigned char u;
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	if (nelen > 4)
		return pjstr_strcasestr_len(hs, hslen, ne, nelen);
	if (jstr_unlikely(nelen == 0))
		return (char *)hs;
	int is_alpha = jstr_isalpha(*ne);
	const char *const start = hs;
	hs = is_alpha ? pjstr_strcasechr_len(hs, *ne, nelen) : (char *)memchr(hs, *ne, hslen);
	if (hs == NULL || (hslen -= hs - start) < nelen)
		return NULL;
	switch (nelen) {
	case 1: return (char *)hs;
	case 2:
		if (is_alpha
		    | jstr_isalpha(ne[1]))
			return pjstr_strcasestr2((u *)hs, (u *)ne);
		break;
	case 3:
		if (is_alpha
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((u *)hs, (u *)ne);
		break;
	default: /* case 4: */
		if (is_alpha
		    | jstr_isalpha(ne[1])
		    | jstr_isalpha(ne[2])
		    | jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((u *)hs, (u *)ne);
		break;
	}
	if (!memcmp(hs, ne, nelen))
		return (char *)hs;
	if (jstr_unlikely(hslen == nelen))
		return NULL;
	return jstr_strstr_len(hs + 1, hslen - 1, ne, nelen);
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
jstr_strcasestr(const char *R hs,
		const char *R ne)
JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
#else
	if (jstr_unlikely(ne[0] == '\0'))
		return (char *)hs;
	int is_isalpha0 = jstr_isalpha(*ne);
	hs = is_isalpha0 ? pjstr_strcasechr_generic(hs, *ne) : strchr(hs, *ne);
	if (hs == NULL || ne[1] == '\0')
		return (char *)hs;
	if (hs == NULL)
		assert(0);
	if (jstr_unlikely(hs[1] == '\0'))
		return NULL;
	is_isalpha0 |= jstr_isalpha(ne[1]);
	typedef unsigned char u;
	size_t nelen;
	if (ne[2] == '\0') {
		if (is_isalpha0)
			return pjstr_strcasestr2((u *)hs, (u *)ne);
		nelen = 2;
	} else if (ne[3] == '\0') {
		if (jstr_unlikely(hs[2] == '\0'))
			return NULL;
		if (is_isalpha0
		    | jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((u *)hs, (u *)ne);
		nelen = 3;
	} else if (ne[4] == '\0') {
		if (jstr_unlikely(hs[2] == '\0') || jstr_unlikely(hs[3] == '\0'))
			return NULL;
		if (is_isalpha0
		    | jstr_isalpha(ne[2])
		    | jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((u *)hs, (u *)ne);
		nelen = 4;
	} else {
		return pjstr_strcasestr_bmh(hs, ne);
	}
	if (!memcmp(hs, ne, nelen))
		return (char *)hs;
	if (jstr_unlikely(hs[nelen] == '\0'))
		return NULL;
	return (char *)strstr(hs + 1, ne);
#endif
}

/*
   Reverse of STRCSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *R s,
		  const char *R reject,
		  const size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(reject[0] == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0')) {
		const char *const p = (char *)jstr_memrchr(s, *reject, sz);
		return p ? (size_t)(p - s) : sz;
	}
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	for (int i = 0, n = sz % 4;; --i) {
		if (t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while (((p < (u *)s) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = ((u *)s + sz) - p;
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

/*
   Reverse of STRCSPN.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrcspn(const char *R s,
	      const char *R reject)
JSTR_NOEXCEPT
{
	return jstr_strrcspn_len(s, reject, strlen(s));
}

/*
   Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrspn_len(const char *R s,
		 const char *R accept,
		 const size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*accept == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s + sz - 1;
		while ((p != s) && (*p == *accept))
			--p;
		return p - s;
	}
	const unsigned char *p = (u *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s + sz - 1;
	for (int i = 0, n = sz % 4;; --i) {
		if (!t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
	} while ((p >= (u *)s) & (c0 & c1 & c2 & c3));
	size_t cnt = ((u *)s + sz) - p;
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

/*
   Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrspn(const char *R s,
	     const char *R accept)
JSTR_NOEXCEPT
{
	return jstr_strrspn_len(s, accept, strlen(s));
}

/*
   Reverse of STRPBRK.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strrpbrk_len(const char *R s,
		  const char *R accept,
		  const size_t sz)
JSTR_NOEXCEPT
{
	const size_t len = jstr_strrcspn_len(s, accept, sz);
	return (len != sz) ? (char *)(s + sz - len) : NULL;
}

/*
   Reverse of STRPBRK.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strrpbrk(const char *R s,
	      const char *R accept)
JSTR_NOEXCEPT
{
	return jstr_strrpbrk_len(s, accept, strlen(s));
}

JSTR_FUNC_PURE
static size_t
jstr_memspn(const char *R s,
	    const char *R accept,
	    size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*accept == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s;
		for (; sz-- && (*p == *accept); ++p)
			;
		return p - s;
	}
	const unsigned char *p = (u *)accept;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (u *)s;
	for (int i = 0, n = sz % 4;; ++i) {
		if (!t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_DOWN(p, 4);
	const unsigned char *const end = (u *)s + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p += 4;
	} while ((p < end) & (c0 & c1 & c2 & c3));
	size_t cnt = p - (u *)p;
	cnt = (sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2));
	return (cnt < sz) ? cnt : sz;
}

JSTR_FUNC_PURE
static size_t
jstr_memcspn(const char *R s,
	     const char *R reject,
	     const size_t sz)
JSTR_NOEXCEPT
{
	typedef unsigned char u;
	if (jstr_unlikely(*reject == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0'))
		return (char *)jstr_memchrnul(s, *reject, sz) - s;
	unsigned char t[256];
	JSTR_BZERO_ARRAY(t);
	const unsigned char *p = (u *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (u *)s + sz - 1;
	for (int i = 0, n = sz % 4;; ++i) {
		if (t[p[i]])
			return (p + i) - (u *)s;
		if (--n == 0) {
			if (sz < 4)
				return sz;
			break;
		}
	}
	p = (u *)JSTR_PTR_ALIGN_UP(p, 4);
	const unsigned char *const end = p + sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[1]];
		c2 = t[p[2]];
		c3 = t[p[3]];
		p -= 4;
	} while (((p >= end) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = p - (u *)s;
	cnt = sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
	return (cnt < sz) ? cnt : sz;
}

JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_mempbrk(const char *R s,
	     const char *R accept,
	     const size_t sz)
JSTR_NOEXCEPT
{
	s += jstr_memcspn(s, accept, sz);
	return *s ? (char *)s : NULL;
}

/*
   Return value:
   ptr to first non-C in S;
   ptr to '\0' if C is not found;
   S if C is '\0';
*/
JSTR_FUNC_PURE
static char *
jstr_strchrnul_inv(const char *R s,
		   int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(c == '\0'))
		return (char *)s;
	c = (char)c;
	while (*s++ == c)
		;
	return (char *)s - 1;
}

/*
   Return value:
   ptr to first non-C in S;
   NULL if non-C is not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_strchr_inv(const char *R s,
		const int c)
JSTR_NOEXCEPT
{
	s = jstr_strchrnul_inv(s, c);
	return *s ? (char *)s : NULL;
}

/*
   Return value:
   ptr to first non-C in S.
   S + N if C is not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_memchrnul_inv(const void *R s,
		   int c,
		   size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (unsigned char *)s;
	const unsigned char *end = p + n;
	c = (unsigned char)c;
	while (p != end
	       && *p == c)
		++p;
	return (char *)p;
}

/*
   Return value:
   ptr to first non-C in S.
   NULL if C is not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memchr_inv(const void *R s,
		const int c,
		const size_t n)
JSTR_NOEXCEPT
{
	const void *const end = (unsigned char *)s + n;
	s = (void *)jstr_memchrnul_inv(s, c, n);
	return (s != end) ? (void *)s : NULL;
}

/*
   Return value:
   ptr to first non-C in S from S + N - 1
   NULL if C is not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_memrchr_inv(const void *R s,
		 int c,
		 const size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *end = (unsigned char *)s + n - 1;
	const unsigned char *const start = (unsigned char *)s;
	c = (unsigned char)c;
	while (end >= start
	       && *end == c)
		--end;
	return (end >= start) ? (void *)end : NULL;
}

/*
   Return value:
   ptr to first non-C in S from S + strlen(S) - 1
   NULL if C is not found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static void *
jstr_strrchr_inv(const void *R s,
		 int c,
		 const size_t n)
JSTR_NOEXCEPT
{
	return jstr_memrchr_inv((char *)s, c, n);
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_caseends_len(const char *R hs,
		  const size_t hslen,
		  const char *R ne,
		  const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) ? !jstr_strcasecmp_len(hs + hslen - nelen, ne, nelen) : 0;
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_caseends(const char *R hs,
	      const char *R ne)
JSTR_NOEXCEPT
{
	return jstr_caseends_len(hs, strlen(hs), ne, strlen(ne));
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends_len(const char *R hs,
	      const size_t hslen,
	      const char *R ne,
	      const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) ? !memcmp(hs + hslen - nelen, ne, nelen) : 0;
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_ends(const char *R hs,
	  const char *R ne)
JSTR_NOEXCEPT
{
	return jstr_ends_len(hs, strlen(hs), ne, strlen(ne));
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_casestarts_len(const char *R hs,
		    const size_t hslen,
		    const char *R ne,
		    const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) ? !jstr_strcasecmp_len(hs, ne, nelen) : 0;
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_casestarts(const char *R hs,
		const char *R ne)
JSTR_NOEXCEPT
{
	return (jstr_tolower(*hs) == jstr_tolower(*ne)) ? !jstr_strcasecmp_len(hs, ne, strlen(ne)) : (*ne == '\0');
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_casestartsnul_len(const char *R hs,
		       const size_t hslen,
		       const char *R ne,
		       const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) ? !jstr_strcasecmp_len(hs, ne, nelen) : nelen;
}

/*
  Check if S1 starts with S2 case-insensitively.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_casestartsnul(const char *R hs,
		   const char *R ne)
JSTR_NOEXCEPT
{
	if (jstr_tolower(*hs) == jstr_tolower(*ne)) {
		const size_t nelen = strlen(ne);
		if (!jstr_strncasecmp(hs, ne, nelen))
			return nelen;
	}
	return 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_startsnul_len(const char *R hs,
		   const size_t hslen,
		   const char *R ne,
		   const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) && !memcmp(hs, ne, nelen) ? nelen : 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  NELEN if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static size_t
jstr_startsnul(const char *R hs,
	       const char *R ne)
JSTR_NOEXCEPT
{
	if (*hs == *ne) {
		const size_t nelen = strlen(ne);
		if (!strncmp(hs, ne, nelen))
			return nelen;
	}
	return 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_starts_len(const char *R hs,
		const size_t hslen,
		const char *R ne,
		const size_t nelen)
JSTR_NOEXCEPT
{
	return jstr_likely(hslen >= nelen) ? !memcmp(hs, ne, nelen) : 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_starts(const char *R hs,
	    const char *R ne)
JSTR_NOEXCEPT
{
	return (*hs == *ne) ? !strncmp(hs, ne, strlen(ne)) : (*ne == '\0');
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_FUNC_PURE
static size_t
jstr_countchr(const char *R s,
	      const int c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return 0;
	size_t cnt = 0;
	while ((s = strchr(s, c)))
		++s, ++cnt;
	return cnt;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_FUNC_PURE
static size_t
jstr_countchr_len(const char *R s,
		  const int c,
		  const size_t sz)
JSTR_NOEXCEPT
{
	size_t cnt = 0;
	const char *const end = s + sz;
	while (*s && (s = (char *)memchr(s, c, end - s)))
		++s, ++cnt;
	return cnt;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
static size_t
jstr_count_len(const char *R s,
	       const char *R find,
	       const size_t sz,
	       const size_t findlen)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return jstr_countchr_len(s, *find, sz);
	if (jstr_unlikely(findlen == 0))
		return 0;
	size_t cnt = 0;
#if JSTR_HAVE_MEMMEM
	const char *const end = s + sz;
	while (*s && (s = jstr_strstr_len(s, end - s, find, findlen)))
#else
	while (*s && (s = strstr(s, find)))
#endif
		++cnt, s += findlen;
	return cnt;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_FUNC_PURE
static size_t
jstr_count(const char *R s,
	   const char *R find)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(find[0] == '\0'))
		return 0;
	if (jstr_unlikely(find[1] == '\0'))
		return jstr_countchr(s, *find);
	if (jstr_unlikely(*s == '\0'))
		return 0;
	const size_t findlen = strlen(find);
	size_t cnt = 0;
	while ((s = strstr(s, find)))
		++cnt, s += findlen;
	return cnt;
}

/*
   Return value:
   ptr to startning of line;
   BEGIN if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_start(const char *const start,
		const char *end)
JSTR_NOEXCEPT
{
	end = (char *)jstr_memrchr(start, '\n', end - start);
	return (char *)(end ? end + 1 : start);
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next_len(const char *start,
		   const char *const end)
JSTR_NOEXCEPT
{
	start = (char *)memchr(start, '\n', end - start);
	return (start && *(start + 1)) ? (char *)start + 1 : NULL;
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next(const char *R s)
JSTR_NOEXCEPT
{
	s = strchr(s, '\n');
	return (s && *(s + 1)) ? (char *)s + 1 : NULL;
}

/*
   Return value:
   ptr to next line or '\0'.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next_len_nul(const char *start,
		       const char *const end)
JSTR_NOEXCEPT
{
	start = jstr_line_next_len(start, end);
	return (char *)(start ? start : end);
}

/*
   Return value:
   ptr to next line or '\0'.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next_nul(const char *R s)
JSTR_NOEXCEPT
{
	s = jstr_strchrnul(s, '\n');
	return (char *)((*s && *(s + 1)) ? s + 1 : s);
}

/*
   Return the number of newlines + 1.
   Return 0 if string is empty.
*/
JSTR_FUNC_PURE
static size_t
jstr_line_number(const char *start,
		 const char *const end)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(start == end))
		return 0;
	size_t cnt = 1;
	while ((start = (char *)memchr(start, '\n', end - start)))
		++start, ++cnt;
	return cnt;
}

/*
   Add thousand separator to NPTR.
   Return value:
   ptr to '\0' in NPTR.
   For example: 1234 becomes 1,234.
*/
JSTR_FUNC
static char *
jstr_thousep_len_p(char *R nptr,
		   const int separator,
		   size_t sz)
JSTR_NOEXCEPT
{
	char *end = nptr + sz;
	if (*nptr == '-') {
		++nptr;
		--sz;
	} else if (jstr_unlikely(sz == 0))
		return nptr;
	if (sz < 4)
		return end;
	int dif = (sz - 1) / 3;
	end += dif;
	const char *const start = nptr;
	nptr += (sz - 1);
	for (int n = 0; nptr >= start;) {
		*(nptr + dif) = *nptr;
		--nptr;
		if (++n == 3) {
			*(nptr + dif) = separator;
			if (jstr_unlikely(--dif == 0))
				break;
			n = 0;
		}
	}
	*end = '\0';
	return (char *)end;
}

/*
   Add thousand separator to NPTR.
   Return value:
   ptr to '\0' in NPTR.
   For example: 1234 becomes 1,234.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_thousep_p(char *R nptr,
	       const int separator)
JSTR_NOEXCEPT
{
	return jstr_thousep_len_p(nptr, separator, strlen(nptr));
}

/*
   Copy SRC to DST, adding thousand separator.
   Return value:
   ptr to '\0' in DST.
*/
JSTR_FUNC
static char *
jstr_thousepcpy_len_p(char *R dst,
		      const char *R src,
		      const int separator,
		      size_t srclen)
JSTR_NOEXCEPT
{
	if (*src == '-') {
		*dst++ = '-';
		++src;
		--srclen;
	}
	if (srclen < 4) {
		while ((*dst++ = *src++))
			;
		return dst - 1;
	}
	int i = srclen % 3;
	for (int j = i; j--; *dst++ = *src++)
		;
	if (i) {
		*dst++ = separator;
		i = 0;
	}
	for (; *src; ++i) {
		if (i == 3) {
			*dst = separator;
			*(dst + 1) = *src++;
			dst += 2;
			i = 0;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
	return dst;
}

/*
   Copy SRC to DST, adding thousand separator.
   Return value:
   ptr to '\0' in DST.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_thousepcpy_p(char *R dst,
		  const char *R src,
		  const int separator)
JSTR_NOEXCEPT
{
	return jstr_thousepcpy_len_p(dst, src, separator, strlen(src));
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STRING_H */
