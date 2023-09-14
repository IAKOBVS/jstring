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
#include "jstr-ctype.h"
#include "jstr-itoa.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

#include "string-fza.h"
#include "string-fzb.h"
#include "string-fzc.h"
#include "string-fzi.h"
#include "string-misc.h"
#include "string-opthr.h"
#include "string-optype.h"
#include "string-shift.h"

#define R JSTR_RESTRICT

#if JSTR_HAVE_MEMMEM
#	define PJSTR_MEMMEM(hs, hslen, ne, nelen) memmem(hs, hslen, ne, nelen)
#else
#	define PJSTR_MEMMEM(hs, hslen, ne, nelen) strstr(hs, ne)
#endif /* HAVE_MEMMEM */

PJSTR_BEGIN_DECLS

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
pjstr_strrstr_len_bmh(const unsigned char *R hs,
		      const size_t hl,
		      const unsigned char *R const ne,
		      const size_t nl) JSTR_NOEXCEPT
{
#define BZERO_SHIFT(shift) ((sizeof((shift)) / sizeof(*(shift)) == sizeof(size_t)) \
			    ? memset(shift, 0, sizeof(shift))                      \
			    : (memset(shift, 0, 64),                               \
			       memset(shift + 64, 0, 64),                          \
			       memset(shift + 64 + 64, 0, 64),                     \
			       memset(shift + 64 + 64 + 64, 0, 64)))
#define H(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)
#define PJSTR_STRRSTR_BMH(table_type, ne_iterator_type)                       \
	do {                                                                  \
		table_type shift[256];                                        \
		BZERO_SHIFT(shift);                                           \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i) \
			shift[H(ne + i)] = i;                                 \
		const size_t shft1 = mtc1 - shift[H(ne + mtc1)];              \
		shift[H(ne + mtc1)] = mtc1;                                   \
		do {                                                          \
			do {                                                  \
				hs -= mtc1;                                   \
				tmp = shift[H(hs)];                           \
			} while (!tmp & (hs > start));                        \
			hs -= tmp;                                            \
			if (mtc1 < 15 || !memcmp(hs + off, ne + off, 8)) {    \
				if (!memcmp(hs, ne, nl))                      \
					return (void *)hs;                    \
				off = (off >= 8 ? off : mtc1) - 8;            \
			}                                                     \
			hs -= shft1;                                          \
		} while (hs > start);                                         \
		return NULL;                                                  \
	} while (0)
	const unsigned char *const start = hs + nl - 1;
	hs += hl - 1;
	size_t tmp;
	const size_t mtc1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
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
#	if JSTR_HAVE_ATTR_MAY_ALIAS
	const jstr_word_ty *w = (jstr_word_ty *)PJSTR_PTR_ALIGN_DOWN(end, sizeof(jstr_word_ty));
	const jstr_word_ty *const start = (jstr_word_ty *)PJSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	if ((u *)w != end) {
		if (jstr_word_has_eq(*w, cc)) {
			const unsigned char *const ret = (u *)w + jstr_word_index_last_eq(*w, cc);
			if ((uintptr_t)(ret - (u *)s) <= end - (u *)s)
				return (void *)ret;
		}
	}
	for (--w; w > start; --w)
		if (jstr_word_has_eq(*w, cc))
			return (void *)((u *)w + jstr_word_index_last_eq(*w, cc));
	if (jstr_word_has_eq(*start, cc)) {
		w = (jstr_word_ty *)((u *)start + jstr_word_index_last_eq(*start, cc));
		if ((uintptr_t)((u *)w - (u *)s) <= end - (u *)s)
			return (void *)w;
	}
#	else
	const unsigned char *p = (u *)PJSTR_PTR_ALIGN_DOWN(end, sizeof(jstr_word_ty));
	const unsigned char *const start = (u *)PJSTR_PTR_ALIGN_DOWN(s, sizeof(jstr_word_ty));
	jstr_word_ty w;
	if (p != end) {
		w = pjstr_uctoword(p);
		if (jstr_word_has_eq(w, cc)) {
			const unsigned char *const ret = p + jstr_word_index_last_eq(w, cc);
			if ((uintptr_t)(ret - (u *)s) <= end - (u *)s)
				return (void *)ret;
		}
	}
	for (p -= sizeof(jstr_word_ty); p > start; p -= sizeof(jstr_word_ty)) {
		w = pjstr_uctoword(p);
		if (jstr_word_has_eq(w, cc))
			return (void *)(p + jstr_word_index_last_eq(w, cc));
	}
	w = pjstr_uctoword(start);
	if (jstr_word_has_eq(w, cc)) {
		p = start + jstr_word_index_last_eq(w, cc);
		if ((uintptr_t)(p - (u *)s) <= end - (u *)s)
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
		return (void *)jstr_memrchr(hs, *(char *)ne, hslen);
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
	default: return pjstr_strrstr_len_bmh((unsigned char *)hs, hslen, (unsigned char *)ne, nelen);
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
pjstr_strcasestr_len_bmh(const unsigned char *R h,
			 const size_t hl,
			 const unsigned char *R n,
			 const size_t nl) JSTR_NOEXCEPT
{
#define HL(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define PJSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                            \
	do {                                                                                          \
		table_type shift[256];                                                                \
		BZERO_SHIFT(shift);                                                                   \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i)                         \
			shift[HL(n + i)] = i;                                                         \
		const size_t shft1 = mtc1 - shift[HL(n + mtc1)];                                      \
		shift[HL(n + mtc1)] = mtc1;                                                           \
		do {                                                                                  \
			do {                                                                          \
				h += mtc1;                                                            \
				tmp = shift[HL(h)];                                                   \
			} while ((!tmp) & (h < end));                                                 \
			h -= tmp;                                                                     \
			if (tmp < mtc1)                                                               \
				continue;                                                             \
			if (mtc1 < 15 || !jstr_strcasecmp_len((char *)h + off, (char *)n + off, 8)) { \
				if (!jstr_strcasecmp_len((char *)h, (char *)n, mtc1))                 \
					return (char *)h;                                             \
				off = (off >= 8 ? off : mtc1) - 8;                                    \
			}                                                                             \
			h += shft1;                                                                   \
		} while (h < end);                                                                    \
		return NULL;                                                                          \
	} while (0)
	const unsigned char *const end = h + hl - nl + 1;
	size_t tmp;
	const size_t mtc1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
		PJSTR_STRCASESTR_BMH(size_t, size_t);
	PJSTR_STRCASESTR_BMH(uint8_t, int);
#undef HL
#undef PJSTR_STRCASESTR_BMH
}

JSTR_FUNC_PURE
static char *
pjstr_strcasestr_bmh(const unsigned char *R h,
		     const unsigned char *R n) JSTR_NOEXCEPT
{
#define HL(p) (((size_t)(jstr_tolower((p)[0])) - ((size_t)jstr_tolower((p)[-1]) << 3)) % 256)
#define PJSTR_STRCASESTR_BMH(table_type, ne_iterator_type)                                            \
	do {                                                                                          \
		table_type shift[256];                                                                \
		BZERO_SHIFT(shift);                                                                   \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i)                         \
			shift[HL(n + i)] = i;                                                         \
		const size_t shft1 = mtc1 - shift[HL(n + mtc1)];                                      \
		shift[HL(n + mtc1)] = mtc1;                                                           \
		goto start_##table_type;                                                              \
		for (;;) {                                                                            \
			if (jstr_unlikely(h >= end)) {                                                \
				end += jstr_strnlen((char *)end + mtc1, 2048);                        \
				if (h > end)                                                          \
					return NULL;                                                  \
			}                                                                             \
			start_##table_type:;                                                          \
			do {                                                                          \
				h += mtc1;                                                            \
				tmp = shift[HL(h)];                                                   \
			} while ((!tmp) & (h < end));                                                 \
			h -= tmp;                                                                     \
			if (tmp < mtc1)                                                               \
				continue;                                                             \
			if (mtc1 < 15 || !jstr_strcasecmp_len((char *)h + off, (char *)n + off, 8)) { \
				if (!jstr_strcasecmp_len((char *)h, (char *)n, mtc1))                 \
					return (char *)h;                                             \
				off = (off >= 8 ? off : mtc1) - 8;                                    \
			}                                                                             \
			h += shft1;                                                                   \
		}                                                                                     \
		return NULL;                                                                          \
	} while (0)
	const size_t nl = strlen((char *)n);
	size_t hl = jstr_strnlen((char *)h, nl | 512);
	if (hl < nl)
		return NULL;
	const unsigned char *end = h + hl - nl + 1;
	size_t tmp;
	const size_t mtc1 = nl - 1;
	size_t off = 0;
	if (jstr_unlikely(nl > 256))
		PJSTR_STRCASESTR_BMH(size_t, size_t);
	PJSTR_STRCASESTR_BMH(uint8_t, int);
#undef HL
#undef PJSTR_STRCASESTR_BMH
}

#define L(c) jstr_tolower(c)

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr2(const unsigned char *R h,
		  const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint16_t nw = L(n[0]) << 8 | L(n[1]);
	uint16_t hw = L(h[0]) << 8 | L(h[1]);
	for (h++; *h && hw != nw; hw = hw << 8 | L(*++h))
		;
	return hw == nw ? (char *)(h - 1) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr3(const unsigned char *R h,
		  const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8;
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8;
	for (h += 2; *h && hw != nw; hw = (hw | L(*++h)) << 8)
		;
	return hw == nw ? (char *)(h - 2) : NULL;
}

JSTR_INLINE
JSTR_FUNC_PURE
static char *
pjstr_strcasestr4(const unsigned char *R h,
		  const unsigned char *R n) JSTR_NOEXCEPT
{
	const uint32_t nw = L(n[0]) << 24 | L(n[1]) << 16 | L(n[2]) << 8 | L(n[3]);
	uint32_t hw = L(h[0]) << 24 | L(h[1]) << 16 | L(h[2]) << 8 | L(h[3]);
	for (h += 3; *h && hw != nw; hw = hw << 8 | L(*++h))
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
#if JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8)
	return (char *)strcasestr(hs, ne);
#else /* It seems to be much faster than glibc strcasestr which seems to use strcasestr.c */
	if (jstr_unlikely(nelen == 0))
		return (char *)hs;
	if (hslen > 4) {
		if (jstr_unlikely(hslen < nelen))
			return NULL;
		return pjstr_strcasestr_len_bmh((unsigned char *)hs, hslen, (unsigned char *)ne, nelen);
	}
	int is_alpha0 = jstr_isalpha(*ne);
	const char *const start = hs;
	if (is_alpha0) {
		const char a[] = { (char)jstr_tolower(*ne), (char)jstr_toupper(*ne), '\0' };
		hs = strpbrk(hs, a);
	} else {
		hs = (char *)memchr(hs, *ne, hslen);
	}
	if (jstr_unlikely(hs == NULL)
	    || !jstr_strncasecmp(hs, ne, nelen))
		return (char *)hs;
	hslen -= hs - start;
	if (jstr_unlikely(hslen < nelen))
		return NULL;
	is_alpha0 += jstr_isalpha(ne[1]);
	switch (nelen) {
	default: /* case 4: */
		if (is_alpha0
		    + jstr_isalpha(ne[2])
		    + jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((unsigned char *)hs, (unsigned char *)ne);
		break;
	case 3:
		if (is_alpha0
		    + jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((unsigned char *)hs, (unsigned char *)ne);
		break;
	case 2:
		if (is_alpha0)
			return pjstr_strcasestr2((unsigned char *)hs, (unsigned char *)ne);
		break;
	}
	return (char *)PJSTR_MEMMEM(hs, hslen, ne, nelen);
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
#if JSTR_HAVE_STRCASESTR && (JSTR_ARCH_POWERPC64 || JSTR_ARCH_POWERPC8)
	return (char *)strcasestr(hs, ne);
#else /* It seems to be much faster than glibc strcasestr which seems to use strcasestr.c */
	if (jstr_unlikely(ne[0] == '\0'))
		return (char *)hs;
	int is_alpha0 = jstr_isalpha(*ne);
	if (is_alpha0) {
		const char a[] = { (char)jstr_tolower(*ne), (char)jstr_toupper(*ne), '\0' };
		hs = strpbrk(hs, a);
	} else {
		hs = strchr(hs, *ne);
	}
	if (jstr_unlikely(hs == NULL)
	    || !jstr_strcasecmp(hs, ne))
		return (char *)hs;
	is_alpha0 += jstr_isalpha(ne[1]);
	if (ne[2] == '\0') {
		if (jstr_unlikely(hs[1] == '\0'))
			return NULL;
		if (is_alpha0)
			return pjstr_strcasestr2((unsigned char *)hs, (unsigned char *)ne);
		return (char *)strstr(hs, ne);
	} else if (ne[3] == '\0') {
		if (jstr_unlikely(hs[1] == '\0')
		    || jstr_unlikely(hs[2] == '\0'))
			return NULL;
		if (is_alpha0
		    + jstr_isalpha(ne[2]))
			return pjstr_strcasestr3((unsigned char *)hs, (unsigned char *)ne);
		return (char *)strstr(hs, ne);
	} else if (ne[4] == '\0') {
		if (jstr_unlikely(hs[1] == '\0' || jstr_unlikely(hs[2] == '\0') || jstr_unlikely(hs[3] == '\0')))
			return NULL;
		if (is_alpha0
		    + jstr_isalpha(ne[2])
		    + jstr_isalpha(ne[3]))
			return pjstr_strcasestr4((unsigned char *)hs, (unsigned char *)ne);
		return (char *)strstr(hs, ne);
	}
	return pjstr_strcasestr_bmh((unsigned char *)hs, (unsigned char *)ne);
#endif
}

/*
   Reverse of STRCSPN.
*/
JSTR_FUNC_PURE
static size_t
jstr_strrcspn_len(const char *R const s,
		  const char *R const reject,
		  size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(reject[0] == '\0'))
		return 0;
	if (jstr_unlikely(reject[1] == '\0')) {
		const char *const p = (char *)jstr_memrchr(s, *reject, sz);
		return p ? (size_t)((s + sz - 1) - p) : sz;
	}
	if (jstr_unlikely(sz == 0))
		return 0;
	unsigned char t[256];
	memset(t, 0, 64);
	memset(t + 64, 0, 64);
	memset(t + 128, 0, 64);
	memset(t + 192, 0, 64);
	const unsigned char *p = (unsigned char *)reject;
	do
		t[*p] = 1;
	while (*p++);
	p = (unsigned char *)s + sz - 1;
	for (int i = sz % 4; i-- > 0;)
		if (t[*p--])
			return ((unsigned char *)s + sz - 1) - (p + 1);
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
	size_t cnt = ((unsigned char *)s + sz - 1) - p;
	return ((c0 | c1) != 0)
	       ? cnt - c0 + 1
	       : cnt - c2 + 3;
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
*/
JSTR_FUNC_PURE
static size_t
jstr_strrspn_len(const char *R const s,
		 const char *R const accept,
		 size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*accept == '\0'))
		return 0;
	if (jstr_unlikely(sz == 0))
		return 0;
	if (jstr_unlikely(accept[1] == '\0')) {
		const char *p = s + sz - 1;
		while (*p-- == *accept)
			;
		return (s + sz - 1) - (p + 1);
	}
	const unsigned char *p = (unsigned char *)accept;
	unsigned char t[256];
	memset(t, 0, 64);
	memset(t + 64, 0, 64);
	memset(t + 128, 0, 64);
	memset(t + 192, 0, 64);
	do
		t[*p++] = 1;
	while (*p);
	p = (unsigned char *)s + sz - 1;
	for (int i = sz % 4; i-- > 0;)
		if (!t[*p--])
			return ((unsigned char *)s + sz - 1) - (p + 1);
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
	size_t cnt = ((unsigned char *)s + sz - 1) - p;
	return ((c0 & c1) == 0)
	       ? cnt + c0
	       : cnt + c2 + 2;
}

/*
   Reverse of STRSPN.
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
	s = s + sz - jstr_strrcspn_len(s, accept, sz);
	return *s ? (char *)s : NULL;
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
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static int
jstr_endswith_len(const char *R const hs,
		  const char *R const ne,
		  const size_t hsz,
		  const size_t nelen) JSTR_NOEXCEPT
{
	return (hsz < nelen) ? 1 : memcmp(hs + hsz - nelen, ne, nelen);
}

/*
  Check if S2 is in end of S1.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_endswith(const char *R const hs,
	      const char *R const ne,
	      const size_t hsz) JSTR_NOEXCEPT
{
	return jstr_endswith_len(hs, ne, hsz, strlen(ne));
}

/*
  Check if S1 starts with S2.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_startswith_len(const char *R const hs,
		    const char *R const ne,
		    const size_t hsz,
		    const size_t nelen) JSTR_NOEXCEPT
{
	return (hsz < nelen) ? 1 : memcmp(hs, ne, nelen);
}

/*
  Check if S1 starts wtih S2.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_MAYBE_UNUSED
static int
jstr_startswith(const char *R const hs,
		const char *R const ne) JSTR_NOEXCEPT
{
	return jstr_startswith_len(hs, ne, strlen(hs), strlen(ne));
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_FUNC_PURE
static size_t
jstr_countc(const char *R s,
	    const int c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*s == '\0'))
		return 0;
	size_t cnt = 0;
	while ((s = strchr(s, c)))
		++cnt;
	return cnt;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
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
		++cnt;
	return cnt;
}

#if JSTR_HAVE_MEMMEM

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
	if (jstr_unlikely(findlen == 0))
		return 0;
	size_t cnt = 0;
	const char *const end = s + sz;
	while ((s = (char *)memmem(s, end - s, find, findlen)))
		++cnt, s += findlen;
	return cnt;
}

#endif

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
	const size_t findlen = strlen(find);
	size_t cnt = 0;
	while ((s = strstr(s, find)))
		++cnt, s += findlen;
	return cnt;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_STRING_H */
