#ifndef JSTR_STRING_H
#define JSTR_STRING_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-ctype.h"
#include "jstr-std-string.h"

#include "jstr-string-fza.h"
#include "jstr-string-fzb.h"
#include "jstr-string-fzc.h"
#include "jstr-string-fzi.h"
#include "jstr-string-misc.h"
#include "jstr-string-opthr.h"
#include "jstr-string-optype.h"
#include "jstr-string-shift.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

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
jstr_strncasecmp(const char *R const s1,
		 const char *R const s2,
		 size_t n) JSTR_NOEXCEPT
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
jstr_strcasecmp_len(const char *R const s1,
		    const char *R const s2,
		    size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRNCASECMP
	return strncasecmp(s1, s2, n);
#else
	const unsigned char *R p1 = (unsigned char *)s1;
	const unsigned char *R p2 = (unsigned char *)s2;
	while (!(jstr_tolower(*p1++) - jstr_tolower(*p2++))
	       && n--)
		;
	return n ? jstr_tolower(*(p1 - 1)) - jstr_tolower(*(p2 - 1)) : 0;
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
jstr_strcasecmp(const char *R s1,
		const char *R s2) JSTR_NOEXCEPT
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
static void *
p_jstr_strrstr_len_bmh(const unsigned char *R hs,
		       const size_t hl,
		       const unsigned char *R const ne,
		       const size_t nl) JSTR_NOEXCEPT
{
#define BZERO(shift) ((sizeof(shift) == 256)                  \
		      ? (memset(shift, 0, 64),                \
			 memset(shift + 64, 0, 64),           \
			 memset(shift + 64 + 64, 0, 64),      \
			 memset(shift + 64 + 64 + 64, 0, 64)) \
		      : memset(shift, 0, sizeof(shift)))
#define H(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)
#define P_JSTR_STRRSTR_BMH(table_type, ne_iterator_type)                    \
	do {                                                                \
		table_type shift[256];                                      \
		BZERO(shift);                                               \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)m1; ++i) \
			shift[H(ne + i)] = i;                               \
		const size_t shift1 = m1 - shift[H(ne + m1)];               \
		shift[H(ne + m1)] = m1;                                     \
		do {                                                        \
			do {                                                \
				hs -= m1;                                   \
				tmp = shift[H(hs)];                         \
			} while (!tmp & (hs > start));                      \
			hs -= tmp;                                          \
			if (m1 < 15 || !memcmp(hs + off, ne + off, 8)) {    \
				if (!memcmp(hs, ne, nl))                    \
					return (void *)hs;                  \
				off = (off >= 8 ? off : m1) - 8;            \
			}                                                   \
			hs -= shift1;                                       \
		} while (hs > start);                                       \
		return NULL;                                                \
	} while (0)
	const unsigned char *const start = hs + nl - 1;
	hs += hl - 1;
	size_t tmp;
	const size_t m1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
		P_JSTR_STRRSTR_BMH(size_t, size_t);
	P_JSTR_STRRSTR_BMH(uint8_t, int);
#undef H
#undef P_JSTR_STRRSTR_BMH
}

#if JSTR_HAVE_MEMRCHR
JSTR_INLINE
#endif
JSTR_FUNC_PURE
static void *
jstr_memrchr(const void *R s,
	     const int c,
	     const size_t n) JSTR_NOEXCEPT
{
#if JSTR_HAVE_MEMRCHR
	return (void *)memrchr(s, c, n);
#else
	if (jstr_unlikely(n == 0))
		return NULL;
	typedef unsigned char u;
	const jstr_word_ty cc = jstr_word_repeat_bytes(c);
	const unsigned char *const end = (unsigned char *)s + n;
	enum { WORDSIZ = sizeof(jstr_word_ty) };
#	if JSTR_HAVE_ATTR_MAY_ALIAS
	const jstr_word_ty *w = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(end, WORDSIZ);
	const jstr_word_ty *const start = (jstr_word_ty *)JSTR_PTR_ALIGN_DOWN(s, WORDSIZ);
	if ((u *)w != end) {
		if (jstr_word_has_eq(*w, cc)) {
			const unsigned char *const ret = (u *)w + jstr_word_index_last_eq(*w, cc);
			if ((uintptr_t)(ret - (u *)s) <= (size_t)(end - (u *)s))
				return (void *)ret;
		}
	}
	while (--w > start)
		if (jstr_word_has_eq(*w, cc))
			return (void *)((u *)w + jstr_word_index_last_eq(*w, cc));
	if (jstr_word_has_eq(*start, cc)) {
		w = (jstr_word_ty *)((u *)start + jstr_word_index_last_eq(*start, cc));
		if ((uintptr_t)((u *)w - (u *)s) <= (size_t)(end - (u *)s))
			return (void *)w;
	}
#	else
	const unsigned char *p = (u *)JSTR_PTR_ALIGN_DOWN(end, WORDSIZ);
	const unsigned char *const start = (u *)JSTR_PTR_ALIGN_DOWN(s, WORDSIZ);
	jstr_word_ty w;
	if (p != end) {
		w = jstr_word_uctow(p);
		if (jstr_word_has_eq(w, cc)) {
			const unsigned char *const ret = p + jstr_word_index_last_eq(w, cc);
			if ((uintptr_t)(ret - (u *)s) <= (size_t)(end - (u *)s))
				return (void *)ret;
		}
	}
	while ((p -= WORDSIZ) > start) {
		w = jstr_word_uctow(p);
		if (jstr_word_has_eq(w, cc))
			return (void *)(p + jstr_word_index_last_eq(w, cc));
	}
	w = jstr_word_uctow(start);
	if (jstr_word_has_eq(w, cc)) {
		p = start + jstr_word_index_last_eq(w, cc);
		if ((uintptr_t)(p - (u *)s) <= (size_t)(end - (u *)s))
			return (void *)p;
	}
#	endif
	return NULL;
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
jstr_strrstr_len(const void *R const hs,
		 const size_t hslen,
		 const void *R const ne,
		 const size_t nelen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	switch (nelen) {
	case 0:
		return (void *)((unsigned char *)hs + hslen);
	case 1:
		return (void *)jstr_memrchr(hs, *(unsigned char *)ne, hslen);
	case 2: {
		const unsigned char *const start = (unsigned char *)hs - 1;
		const unsigned char *h = (unsigned char *)hs + hslen - 1;
		const unsigned char *n = (unsigned char *)ne;
		const uint16_t nw = n[1] << 8 | n[0];
		uint16_t hw = h[0] << 8 | h[-1];
		for (h -= 2; (h != start) & (hw != nw); hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	case 3: {
		const unsigned char *const start = (unsigned char *)hs - 1;
		const unsigned char *h = (unsigned char *)hs + hslen - 1;
		const unsigned char *n = (unsigned char *)ne;
		const uint32_t nw = n[2] << 24 | n[1] << 16 | n[0] << 8;
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8;
		for (h -= 3; (h != start) & (hw != nw); hw = (hw | *h--) << 8)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	case 4: {
		const unsigned char *const start = (unsigned char *)hs - 1;
		const unsigned char *h = (unsigned char *)hs + hslen - 1;
		const unsigned char *n = (unsigned char *)ne;
		const uint32_t nw = n[3] << 24 | n[2] << 16 | n[1] << 8 | n[0];
		uint32_t hw = h[0] << 24 | h[-1] << 16 | h[-2] << 8 | h[-3];
		for (h -= 4; (h != start) & (hw != nw); hw = hw << 8 | *h--)
			;
		return hw == nw ? (void *)(h + 1) : NULL;
	}
	default: return p_jstr_strrstr_len_bmh((unsigned char *)hs, hslen, (unsigned char *)ne, nelen);
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
jstr_strrstr(const char *R const hs,
	     const char *R const ne) JSTR_NOEXCEPT
{
	return (char *)jstr_strrstr_len(hs, strlen(hs), ne, strlen(ne));
}

JSTR_FUNC_PURE
static char *
p_jstr_strcasestr_len_bmh(const unsigned char *R h,
			  const size_t hl,
			  const unsigned char *R n,
			  const size_t nl) JSTR_NOEXCEPT
{
#define HL(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define P_JSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                             \
	do {                                                                                            \
		table_type shift[256];                                                                  \
		BZERO(shift);                                                                           \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)m1; ++i) {                           \
			shift[HL(n + i)] = i;                                                           \
		}                                                                                       \
		const size_t shift1 = m1 - shift[HL(n + m1)];                                           \
		shift[HL(n + m1)] = m1;                                                                 \
		do {                                                                                    \
			do {                                                                            \
				h += m1;                                                                \
				tmp = shift[HL(h)];                                                     \
			} while (!tmp & (h < end));                                                     \
			h -= tmp;                                                                       \
			if (tmp < m1)                                                                   \
				continue;                                                               \
			if (m1 < 15 || !jstr_strcasecmp_len((char *)(h + off), (char *)(n + off), 8)) { \
				if (!jstr_strcasecmp_len((char *)h, (char *)n, m1))                     \
					return (char *)h;                                               \
				off = (off >= 8 ? off : m1) - 8;                                        \
			}                                                                               \
			h += shift1;                                                                    \
		} while (h < end);                                                                      \
		return NULL;                                                                            \
	} while (0)
	const unsigned char *const end = h + hl - nl + 1;
	size_t tmp;
	const size_t m1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
		P_JSTR_STRCASESTR_BMH(size_t, size_t);
	P_JSTR_STRCASESTR_BMH(uint8_t, int);
#undef HL
#undef P_JSTR_STRCASESTR_BMH
}

JSTR_FUNC_PURE
static char *
p_jstr_strcasestr_bmh(const unsigned char *R h,
		      const unsigned char *R n) JSTR_NOEXCEPT
{
#define HL(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define P_JSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                             \
	do {                                                                                            \
		table_type shift[256];                                                                  \
		BZERO(shift);                                                                           \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)m1; ++i)                             \
			shift[HL(n + i)] = i;                                                           \
		const size_t shift1 = m1 - shift[HL(n + m1)];                                           \
		shift[HL(n + m1)] = m1;                                                                 \
		goto start_##table_type;                                                                \
		for (;;) {                                                                              \
			if (jstr_unlikely(h >= end)) {                                                  \
				end += jstr_strnlen((char *)(end + m1), 2048);                          \
				if (h > end)                                                            \
					return NULL;                                                    \
			}                                                                               \
			start_##table_type:;                                                            \
			do {                                                                            \
				h += m1;                                                                \
				tmp = shift[HL(h)];                                                     \
			} while ((!tmp) & (h < end));                                                   \
			h -= tmp;                                                                       \
			if (tmp < m1)                                                                   \
				continue;                                                               \
			if (m1 < 15 || !jstr_strcasecmp_len((char *)(h + off), (char *)(n + off), 8)) { \
				if (!jstr_strcasecmp_len((char *)h, (char *)n, m1))                     \
					return (char *)h;                                               \
				off = (off >= 8 ? off : m1) - 8;                                        \
			}                                                                               \
			h += shift1;                                                                    \
		}                                                                                       \
		return NULL;                                                                            \
	} while (0)
	const size_t nl = strlen((char *)n);
	const size_t hl = jstr_strnlen((char *)h, nl | 512);
	if (hl < nl)
		return NULL;
	if (!jstr_strncasecmp((char *)h, (char *)n, nl))
		return (char *)h;
	const unsigned char *end = h + hl - nl + 1;
	size_t tmp;
	const size_t m1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
		P_JSTR_STRCASESTR_BMH(size_t, size_t);
	P_JSTR_STRCASESTR_BMH(uint8_t, int);
#undef HL
#undef P_JSTR_STRCASESTR_BMH
}

#define L(c) jstr_tolower(c)

JSTR_INLINE
JSTR_FUNC_PURE
static char *
p_jstr_strcasestr2(const unsigned char *R h,
		   const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (h++; (*h != '\0') & (hw != nw); hw = hw << 8 | L(*++h))
		;
	return hw == nw ? (char *)(h - 1) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
p_jstr_strcasestr3(const unsigned char *R h,
		   const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8;
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8;
	for (h += 2; (*h != '\0') & (hw != nw); hw = (hw | L(*++h)) << 8)
		;
	return hw == nw ? (char *)(h - 2) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
p_jstr_strcasestr4(const unsigned char *R h,
		   const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8 | L(n[3]);
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8 | L(h[3]);
	for (h += 3; (*h != '\0') & (hw != nw); hw = hw << 8 | L(*++h))
		;
	return hw == nw ? (char *)(h - 3) : NULL;
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
jstr_strcasestr_len(const char *R hs,
		    size_t hslen,
		    const char *R const ne,
		    const size_t nelen) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
#else
	if (jstr_unlikely(nelen == 0))
		return (char *)hs;
	typedef unsigned char u;
	if (nelen > 4) {
		if (jstr_unlikely(hslen < nelen))
			return NULL;
		return p_jstr_strcasestr_len_bmh((u *)hs, hslen, (u *)ne, nelen);
	}
	int is_alpha0 = jstr_isalpha(*ne);
	const char *const start = hs;
	if (is_alpha0) {
		const char a[] = { (char)jstr_tolower(*ne), (char)jstr_toupper(*ne), '\0' };
		hs = strpbrk(hs, a);
	} else {
		hs = (char *)memchr(hs, *ne, hslen);
	}
	if (jstr_unlikely(hs == NULL))
		return (char *)hs;
	hslen -= hs - start;
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	is_alpha0 += jstr_isalpha(ne[1]);
	switch (nelen) {
	case 2:
		if (is_alpha0)
			return p_jstr_strcasestr2((u *)hs, (u *)ne);
		break;
	case 3:
		if (is_alpha0
		    + jstr_isalpha(ne[2]))
			return p_jstr_strcasestr3((u *)hs, (u *)ne);
		break;
	default: /* case 4: */
		if (is_alpha0
		    + jstr_isalpha(ne[2])
		    + jstr_isalpha(ne[3]))
			return p_jstr_strcasestr4((u *)hs, (u *)ne);
		break;
	}
	return JSTR_STRSTR_LEN(hs, hslen, ne, nelen);
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
		const char *R const ne) JSTR_NOEXCEPT
{
#if JSTR_HAVE_STRCASESTR_OPTIMIZED
	return (char *)strcasestr(hs, ne);
#else
	if (jstr_unlikely(ne[0] == '\0'))
		return (char *)hs;
	int is_alpha0 = jstr_isalpha(*ne);
	if (is_alpha0) {
		const char a[] = { (char)jstr_tolower(*ne), (char)jstr_toupper(*ne), '\0' };
		hs = strpbrk(hs, a);
	} else {
		hs = strchr(hs, *ne);
	}
	if (jstr_unlikely(hs == NULL))
		return (char *)hs;
	is_alpha0 += jstr_isalpha(ne[1]);
	typedef unsigned char u;
	if (ne[2] == '\0') {
		if (jstr_unlikely(hs[1] == '\0'))
			return NULL;
		if (is_alpha0)
			return p_jstr_strcasestr2((u *)hs, (u *)ne);
	} else if (ne[3] == '\0') {
		if (jstr_unlikely(hs[1] == '\0')
		    || jstr_unlikely(hs[2] == '\0'))
			return NULL;
		if (is_alpha0
		    + jstr_isalpha(ne[2]))
			return p_jstr_strcasestr3((u *)hs, (u *)ne);
	} else if (ne[4] == '\0') {
		if (jstr_unlikely(hs[1] == '\0' || jstr_unlikely(hs[2] == '\0') || jstr_unlikely(hs[3] == '\0')))
			return NULL;
		if (is_alpha0
		    + jstr_isalpha(ne[2])
		    + jstr_isalpha(ne[3]))
			return p_jstr_strcasestr4((u *)hs, (u *)ne);
	} else {
		return p_jstr_strcasestr_bmh((u *)hs, (u *)ne);
	}
	return (char *)strstr(hs, ne);
#endif
}

/*
   Reverse of STRCSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *R const s,
		  const char *R const reject,
		  size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(reject[0] == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(reject[1] == '\0')) {
		const char *const p = (char *)jstr_memrchr(s, *reject, sz);
		return p ? (size_t)(p - s) : sz;
	}
	unsigned char t[256];
	BZERO(t);
	const unsigned char *p = (unsigned char *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (unsigned char *)s + sz - 1;
	for (int i = sz % 4; i-- > 0;)
		if (t[*p--])
			return (p + 1) - (unsigned char *)s;
	if (jstr_unlikely(sz < 4))
		return sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
		sz -= 4;
	} while (((sz == 0) | c0 | c1 | c2 | c3) == 0);
	size_t cnt = ((unsigned char *)s + sz) - p;
	return sz - (((c0 | c1) != 0) ? cnt - c0 + 1 : cnt - c2 + 3);
}

/*
   Reverse of STRCSPN.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrcspn(const char *R const s,
	      const char *R const reject) JSTR_NOEXCEPT
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
jstr_strrspn_len(const char *R const s,
		 const char *R const accept,
		 size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*accept == '\0')
	    || jstr_unlikely(sz == 0))
		return sz;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s + sz - 1;
		while ((p != s) && (*p == *accept))
			--p;
		return p - s;
	}
	const unsigned char *p = (unsigned char *)accept;
	unsigned char t[256];
	BZERO(t);
	do
		t[*p++] = 1;
	while (*p);
	p = (unsigned char *)s + sz - 1;
	for (int i = sz % 4; i-- > 0;)
		if (!t[*p--])
			return (p + 1) - (unsigned char *)s;
	if (jstr_unlikely(sz < 4))
		return sz;
	unsigned int c0, c1, c2, c3;
	do {
		c0 = t[p[0]];
		c1 = t[p[-1]];
		c2 = t[p[-2]];
		c3 = t[p[-3]];
		p -= 4;
		sz -= 4;
	} while ((sz != 0) & (c0 & c1 & c2 & c3));
	size_t cnt = ((unsigned char *)s + sz) - p;
	return sz - (((c0 & c1) == 0) ? cnt + c0 : cnt + c2 + 2);
}

/*
   Reverse of STRSPN.
   Return the offset from S if found;
   otherwise, return S + SZ.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_strrspn(const char *R const s,
	     const char *R const accept) JSTR_NOEXCEPT
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
		  const char *R const accept,
		  const size_t sz) JSTR_NOEXCEPT
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
jstr_strrpbrk(const char *R const s,
	      const char *R const accept) JSTR_NOEXCEPT
{
	return jstr_strrpbrk_len(s, accept, strlen(s));
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
jstr_endswith_len(const char *R const hs,
		  const size_t hslen,
		  const char *R const ne,
		  const size_t nelen) JSTR_NOEXCEPT
{
	return (hslen >= nelen) ? !memcmp(hs + hslen - nelen, ne, nelen) : 0;
}

/*
  Check if S2 is in end of S1.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith(const char *R const hs,
	      const char *R const ne) JSTR_NOEXCEPT
{
	const size_t nelen = strlen(ne);
	const size_t hslen = jstr_strnlen(ne, nelen);
	return (hslen >= nelen) ? jstr_endswith_len(hs, hslen, ne, nelen) : 0;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_startswith_len(const char *R const hs,
		    const size_t hslen,
		    const char *R const ne,
		    const size_t nelen) JSTR_NOEXCEPT
{
	return (hslen >= nelen) ? memcmp(hs, ne, nelen) : 1;
}

/*
  Check if S1 starts with S2.
  Return value:
  1 if true;
  0 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_startswith(const char *R const hs,
		const char *R const ne) JSTR_NOEXCEPT
{
	return !strncmp(hs, ne, strlen(ne));
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_FUNC_PURE
static size_t
jstr_countc(const char *R s,
	    const int c) JSTR_NOEXCEPT
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
jstr_countc_len(const char *R s,
		const int c,
		const size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	size_t cnt = 0;
	const char *const end = s + sz;
	while ((s = (char *)memchr(s, c, end - s)))
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
	       const char *R const find,
	       const size_t sz,
	       const size_t findlen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(findlen == 1))
		return jstr_countc_len(s, *find, sz);
	if (jstr_unlikely(findlen == 0)
	    || jstr_unlikely(sz == 0))
		return 0;
	size_t cnt = 0;
#if JSTR_HAVE_MEMMEM
	const char *const end = s + sz;
	while ((s = JSTR_STRSTR_LEN(s, end - s, find, findlen)))
#else
	while ((s = strstr(s, find)))
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
	   const char *R const find) JSTR_NOEXCEPT
{
	if (jstr_unlikely(find[0] == '\0'))
		return 0;
	if (jstr_unlikely(find[1] == '\0'))
		return jstr_countc(s, *find);
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
   ptr to beginning of line;
   BEGIN if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_begin(const char *const begin,
		const char *end) JSTR_NOEXCEPT
{
	end = (char *)jstr_memrchr(begin, '\n', end - begin);
	return (char *)(end ? end + 1 : begin);
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next_len(const char *const begin,
		   const char *end) JSTR_NOEXCEPT
{
	end = (char *)memchr(begin, '\n', end - begin);
	return end
	       ? ((*(end + 1) != '\0')
		  ? (char *)end + 1
		  : NULL)
	       : NULL;
}

/*
   Return value:
   ptr to next line;
   NULL if no newline was found.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static char *
jstr_line_next(const char *s) JSTR_NOEXCEPT
{
	s = strchr(s, '\n');
	return s
	       ? ((*(s + 1) != '\0')
		  ? (char *)s + 1
		  : NULL)
	       : NULL;
}

/*
   Return the number of newlines + 1.
*/
JSTR_FUNC_PURE
static size_t
jstr_line_number(const char *begin,
		 const char *const end) JSTR_NOEXCEPT
{
	if (jstr_unlikely(begin == end))
		return 0;
	size_t cnt = 1;
	while ((begin = (char *)memchr(begin, '\n', end - begin)))
		++begin, ++cnt;
	return cnt;
}

P_JSTR_END_DECLS

#undef BZERO
#undef R

#endif /* JSTR_STRING_H */
