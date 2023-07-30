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

#include "jstr_builder.h"
#include "jstr_ctype.h"
#include "jstr_macros.h"
#include "jstr_string.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && defined(__cplusplus)
#	define JSTR_IN_NAMESPACE 1
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
   Return value:
   New len of S.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_INLINE
static char *jstr_append_mem_p_f(char *JSTR_RST const s,
				 const char *JSTR_RST const src,
				 const size_t sz,
				 const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(s, src, srclen + 1);
	return s + sz + srclen;
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append_mem(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const char *JSTR_RST const src,
			    const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + srclen)
		JSTR_REALLOC(*s, *cap, *sz + srclen, return);
	*sz = jstr_append_mem_p_f(*s, src, *sz, srclen) - *s;
}

/*
  Count occurences of C in S.
  Return value:
  Occurences of C in S.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_countc(const char *JSTR_RST s,
		       const int c) JSTR_NOEXCEPT
{
	int count = 0;
	for (;; ++s)
		if (*s == c)
			++count;
		else if (unlikely(!*s))
			break;
	return count;
}

#ifdef JSTR_HAS_MEMMEM

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_count_mem(const char *JSTR_RST s,
			  const char *JSTR_RST const searc,
			  size_t sz,
			  const size_t searclen) JSTR_NOEXCEPT
{
	int count = 0;
	for (; (s = (char *)memmem(s, sz, searc, searclen));
	     ++count, s += searclen, sz -= searclen)
		;
	return count;
}

#else

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_count(const char *JSTR_RST s,
		      const char *JSTR_RST const searc) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = strstr(s, searc)))
		++count;
	return count;
}

#endif /* __JSTR_HAS_MEMMEM */

/*
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_rev_mem(char *JSTR_RST s,
			 const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return;
	char *end = s + sz - 1;
	char tmp;
	do {
		tmp = *s;
		*s = *end;
		*end = tmp;
	} while (++s < --end);
}

/*
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trim_mem_p(char *JSTR_RST const s,
			     const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	char *end = s + sz - 1;
	do {
		switch (*end--) {
		case '\t':
		case ' ':
			continue;
		default:
			end += 2;
			*end = '\0';
		}
		break;
	} while (end >= s);
	return end;
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmc_mem_p(char *JSTR_RST s,
			    const int c,
			    const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	const char *const sstart = s;
	if ((s = (char *)memchr(s, c, sz))) {
		memmove(s, s + 1, sz - (s - sstart) + 1);
		return s + sz - (s - sstart);
	}
	return s + sz;
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
  S if no C in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmc_p(char *JSTR_RST s,
			const int c) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	if ((s = strchr(s, c))) {
		for (const char *src = s; (*s++ = *++src);)
			;
		*s = '\0';
	}
	return s;
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmallc_mem_p(char *JSTR_RST s,
			       const int c,
			       const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	s = (char *)memchr(s, c, sz);
	if (!s)
		return s + sz;
	const char *src = s + 1;
	*s++ = *src++;
	for (;;)
		if (*src != c) {
			*s++ = *src;
			if (unlikely(!*++src))
				break;
		} else {
			++src;
		}
	*s = '\0';
	return s;
}

/*
  Remove all C in S.
  Return value:
  Pointer to '\0' in S;
  S if no C in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmallc_p(char *JSTR_RST s,
			   const int c) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	s = strchr(s, c);
	if (!s)
		return s;
	const char *src = s + 1;
	*s++ = *src++;
	for (;;)
		if (*src != c) {
			*s++ = *src;
			if (unlikely(!*++src))
				break;
		} else {
			++src;
		}
	*s = '\0';
	return s;
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmnc_mem_p(char *JSTR_RST s,
			     const int c,
			     size_t n,
			     const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	if (unlikely(n == 0))
		return s + sz;
	s = (char *)memchr(s, c, sz);
	if (!s)
		return s;
	const char *src = s + 1;
	*s++ = *src++;
	for (;;)
		if (*src != c) {
			*s++ = *src;
			if (unlikely(!*++src))
				break;
		} else if (unlikely(!--n)) {
			do
				*s++ = *src;
			while (*++src);
			break;
		} else {
			++src;
		}
	*s = '\0';
	return s;
}

/*
  Remove N C in S.
  Return value:
  Pointer to '\0' in S;
  S if no C in S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmnc_p(char *JSTR_RST s,
			 const int c,
			 size_t n) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	if (unlikely(n == 0))
		return s;
	s = (char *)strchr(s, c);
	if (!s)
		return s;
	const char *src = s + 1;
	*s++ = *src++;
	for (;;)
		if (*src != c) {
			*s++ = *src;
			if (unlikely(!*++src))
				break;
		} else if (unlikely(!--n)) {
			do
				*s++ = *src;
			while (*++src);
			break;
		} else {
			++src;
		}
	*s = '\0';
	return s;
}

/*
  Remove characters in REJECT in S.
  Return value:
  Pointer to '\0' in S;
  S if no REJECT in S, and REJECT is one character.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_stripspn_p(char *JSTR_RST s,
			     const char *JSTR_RST reject) JSTR_NOEXCEPT
{
	enum {
		ACCEPT = 0,
		REJECT,
		NUL,
		ASCII_SIZE = 256,
	};
	if (unlikely(!reject[0]))
		return s;
	if (unlikely(!reject[1]))
		return s;
	unsigned char tbl[ASCII_SIZE];
	memset(tbl, ACCEPT, 64);
	memset(&tbl[64], ACCEPT, 64);
	memset(&tbl[128], ACCEPT, 64);
	memset(&tbl[192], ACCEPT, 64);
	tbl[0] = NUL;
	do
		tbl[(unsigned char)*reject++] = REJECT;
	while (*reject);
	for (const unsigned char *src = (unsigned char *)s;; ++src) {
		switch (tbl[*src]) {
		case ACCEPT:
			*s++ = *src;
		case REJECT:
			continue;
		case NUL:;
		}
		break;
	}
	*s = '\0';
	return s;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec_mem(char *JSTR_RST s,
			      const int searc,
			      const int rplc,
			      const size_t sz) JSTR_NOEXCEPT
{
	if ((s = (char *)memchr(s, searc, sz)))
		*s = rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec(char *JSTR_RST s,
			  const int searc,
			  const int rplc) JSTR_NOEXCEPT
{
	if ((s = strchr(s, searc)))
		*s = rplc;
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rm_mem_p(char *JSTR_RST const s,
			   const char *JSTR_RST const searc,
			   const size_t sz,
			   const size_t searclen) JSTR_NOEXCEPT
{
	char *const p = (char *)jstr_memmem(s, sz, searc, searclen);
	if (p) {
		memmove(p, p + searclen, (s + sz) - p);
		return s + sz - searclen;
	}
	return s + sz;
}

/*
  Remove N HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rmn_mem_p(char *JSTR_RST s,
			    const char *JSTR_RST const searc,
			    size_t n,
			    size_t sz,
			    const size_t searclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	if (unlikely(n == 0))
		return s + sz;
	char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
	if (!dst)
		return s + sz;
	switch (searclen) {
	case 0:
		return dst;
		break;
	case 1: {
		s = dst;
		goto MTC1;
		for (;;)
			if (*s != *searc) {
				*dst++ = *s;
				if (unlikely(!*++s))
					break;
			} else {
			MTC1:
				if (unlikely(!--n)) {
					do
						*dst++ = *s;
					while (*++s);
					break;
				} else {
					++s;
				}
			}
		*dst = '\0';
		return dst;
		break;
	}
	case 2: {
		s = dst + 1;
		const uint16_t nw = searc[0] << 8 | searc[1];
		uint16_t sw = s[0] << 8 | s[1];
		goto MTC2;
		for (; *s; sw = sw << 8 | *s)
			if (sw != nw) {
				*dst++ = *(s++ - 1);
			} else {
			MTC2:
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 1);
					while (--sz);
					break;
				} else {
					s += 2;
				}
			}
		if (sw != nw)
			*dst++ = *(s - 1);
		*dst = '\0';
		return dst;
		break;
	}
	case 3: {
		s = dst + 3;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8;
		goto MTC3;
		for (; *s; sw = (sw | *s++) << 8)
			if (sw != nw) {
				*dst++ = *(s - 3);
			} else {
			MTC3:
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 3);
					while (--sz);
					break;
				} else {
					s += 2;
				}
			}
		if (sw != nw)
			*dst++ = *(s - 3);
		*dst = '\0';
		return dst;
		break;
	}
	case 4: {
		s = dst + 4;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		goto MTC4;
		for (; *s; sw = sw << 8 | *s++)
			if (sw != nw) {
				*dst++ = *(s - 4);
			} else {
			MTC4:
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 4);
					while (--sz);
					break;
				} else {
					s += 3;
				}
			}
		if (sw != nw)
			*dst++ = *(s - 4);
		*dst = '\0';
		return dst;
		break;
	}
	default: {
		const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
		const char *const end = s + sz - searclen;
		s = dst;
		if (searclen < 15) {
			goto MTC5;
			while (s <= end) {
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s, searc, searclen)) {
				MTC5:
					s += searclen;
					if (unlikely(!--n))
						break;
					continue;
				}
				*dst++ = *s++;
			}
		} else {
			const size_t off = searclen - 9;
			goto MTC6;
			while (s <= end) {
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s + 1, searc + 1, searclen - 10)) {
					MTC6:
						s += searclen;
						if (unlikely(!--n))
							break;
						continue;
					}
				}
				*dst++ = *s++;
			}
		}
		memmove(dst, s, end + searclen - s + 1);
		return dst + (end + searclen - s);
		break;
	}
	}
}

/*
  Remove all HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rmall_mem_p(char *JSTR_RST s,
			      const char *JSTR_RST const searc,
			      size_t sz,
			      const size_t searclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
	if (!dst)
		return s + sz;
	switch (searclen) {
	case 0:
		return dst;
		break;
	case 1: {
		goto MTC1;
		for (;;)
			if (*s != *searc) {
				*dst++ = *s;
				if (unlikely(!*++s))
					break;
			} else {
			MTC1:
				++s;
			}
		*dst = '\0';
		return dst;
		break;
	}
	case 2: {
		s = dst + 1;
		const uint16_t nw = searc[0] << 8 | searc[1];
		uint16_t hw = s[0] << 8 | s[1];
		goto MTC2;
		for (; *s; hw = hw << 8 | *s)
			if (hw != nw)
				*dst++ = *(s++ - 1);
			else
			MTC2:
				s += 2;
		if (hw != nw)
			*dst++ = *(s - 1);
		goto END_UNDER_4;
		break;
	}
	case 3: {
		s = dst + 3;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
		uint32_t hw = s[0] << 24 | s[1] << 16 | s[2] << 8;
		goto MTC3;
		for (; *s; hw = (hw | *s++) << 8)
			if (hw != nw)
				*dst++ = *(s - 3);
			else
			MTC3:
				s += 2;
		if (hw != nw)
			*dst++ = *(s - 3);
		goto END_UNDER_4;
		break;
	}
	case 4: {
		s = dst + 4;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
		uint32_t hw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		goto MTC4;
		for (; *s; hw = hw << 8 | *s++)
			if (hw != nw)
				*dst++ = *(s - 4);
			else
			MTC4:
				s += 3;
		if (hw != nw)
			*dst++ = *(s - 4);
		goto END_UNDER_4;
		break;
	}
	END_UNDER_4:
		*dst = '\0';
		return dst;
		break;
	default: {
		const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
		const char *const end = s + sz - searclen;
		s = dst;
		if (searclen < 15) {
			goto MTC5;
			while (s <= end)
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s, searc, searclen))
				MTC5:
					s += searclen;
				else
					*dst++ = *s++;
		} else {
			const size_t off = searclen - 9;
			goto MTC6;
			while (s <= end) {
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s + 1, searc + 1, searclen - 10)) {
					MTC6:
						s += searclen;
						continue;
					}
				}
				*dst++ = *s++;
			}
		}
		memmove(dst, s, end + searclen - s + 1);
		return dst + (end + searclen - s);
		break;
	}
	}
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc_mem(char *JSTR_RST s,
				 const int searc,
				 const int rplc,
				 const size_t sz) JSTR_NOEXCEPT
{
	if ((s = (char *)memchr(s, searc, sz)))
		do
			if (*s == searc)
				*s = rplc;
		while (*++s);
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc(char *JSTR_RST s,
			     const int searc,
			     const int rplc) JSTR_NOEXCEPT
{
	if ((s = strchr(s, searc)))
		do
			if (*s == searc)
				*s = rplc;
		while (*++s);
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacenc_mem(char *JSTR_RST s,
			       const int searc,
			       const int rplc,
			       size_t n,
			       const size_t sz) JSTR_NOEXCEPT
{
	if ((s = (char *)memchr(s, searc, sz)))
		for (;;) {
			if (*s == searc) {
				*s++ = rplc;
				if (n-- == 1)
					break;
			} else if (unlikely(!*++s)) {
				break;
			}
		}
}

/*
  Replace N SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacenc(char *JSTR_RST s,
			   const int searc,
			   const int rplc,
			   size_t n) JSTR_NOEXCEPT
{
	if ((s = strchr(s, searc)))
		for (;;) {
			if (*s == searc) {
				*s = rplc;
				if (n-- == 1)
					break;
			} else if (unlikely(!*++s)) {
				break;
			}
		}
}

/*
  Replace first SEARCH in S with REPLACE.
  Assumes that SEARCHLEN > REPLACELEN.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static char *jstr_replace_mem_p_f(char *JSTR_RST const s,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const rplc,
				  const size_t sz,
				  const size_t searclen,
				  const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	switch (rplclen) {
	case 0:
		return jstr_rm_mem_p(s, searc, sz, searclen);
		break;
	case 1:
		if (searclen == 1) {
			jstr_replacec(s, *searc, *rplc);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		char *mtc = (char *)jstr_memmem(s, sz, searc, searclen);
		if (!mtc)
			return s + sz;
		memmove(mtc + rplclen,
			mtc + searclen,
			(s + sz + 1) - mtc + searclen);
		memcpy(mtc, rplc, rplclen);
		return s + sz - (long long)(rplclen - searclen);
		break;
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
static void jstr_replace_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const searc,
			     const char *JSTR_RST const rplc,
			     const size_t searclen,
			     const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return;
	switch (rplclen) {
	case 0:
		*sz = jstr_rm_mem_p(*s, searc, *sz, searclen) - *s;
		break;
	case 1:
		if (searclen == 1) {
			jstr_replacec(*s, *searc, *rplc);
			--*sz;
			return;
		}
		break;
	default: {
		char *mtc = (char *)jstr_memmem(*s, *sz, searc, searclen);
		if (!mtc)
			return;
		if (rplclen <= searclen || *cap > *sz + rplclen - searclen + 1) {
			memmove(mtc + rplclen,
				mtc + searclen,
				(*s + *sz + 1) - mtc + searclen);
			memcpy(mtc, rplc, rplclen);
		} else {
			JSTR_GROW(*cap, *sz + rplclen + 1);
			char *const tmp = (char *)malloc(*cap);
			JSTR_MALLOC_ERR(tmp, return);
			memcpy(tmp, *s, mtc - *s);
			memcpy(tmp + (mtc - *s), rplc, rplclen);
			memcpy(tmp + (mtc - *s) + rplclen,
			       mtc + rplclen,
			       (*s + *sz + 1) - (mtc + rplclen));
			free(*s);
			*s = tmp;
		}
		*sz += (long long)(rplclen - searclen);
		break;
	}
	}
}

/*
  Replace N SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_WARN_UNUSED
static char *jstr_replacen_mem_p_f(char *JSTR_RST const s,
				   const char *JSTR_RST const searc,
				   const char *JSTR_RST const rplc,
				   size_t n,
				   size_t sz,
				   const size_t searclen,
				   const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	if (unlikely(n == 0))
		return s + sz;
	switch (rplclen) {
	case 0: {
		return jstr_rmn_mem_p(s, searc, n, sz, searclen);
		break;
	}
	case 1:
		if (searclen == 1) {
			jstr_replacenc_mem(s, *searc, *rplc, n, rplclen);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		char *mtc = s;
		while (n-- && (mtc = (char *)jstr_memmem(mtc, (s + sz) - mtc, searc, searclen))) {
			memmove(mtc + rplclen,
				mtc + searclen,
				(s + sz + 1) - mtc + searclen);
			memcpy(mtc, rplc, rplclen);
			mtc += rplclen;
			sz += (long long)(rplclen - searclen);
		}
		return s + sz;
		break;
	}
	}
}

/*
  Replace all SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_replaceall_mem_p_f(char *JSTR_RST const s,
				     const char *JSTR_RST const searc,
				     const char *JSTR_RST const rplc,
				     size_t sz,
				     const size_t searclen,
				     const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	switch (rplclen) {
	case 0: {
		return jstr_rmall_mem_p(s, searc, sz, searclen);
		break;
	}
	case 1:
		if (searclen == 1) {
			jstr_replaceallc(s, *searc, *rplc);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
		if (!dst)
			return s + sz;
		if (searclen >= rplclen) {
			const char *src = dst;
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const char *const end = s + sz - searclen;
			if (searclen < 15) {
				goto MTC1;
				while (src <= end) {
					if (nw == (src[0] << 8 | src[searclen - 1])
					    && !memcmp(src, searc, searclen)) {
					MTC1:
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						src += searclen;
						continue;
					}
					*dst++ = *src++;
				}
			} else {
				const size_t off = searclen - 9;
				goto MTC2;
				while (src <= end) {
					if (nw == (src[0] << 8 | src[searclen - 1])
					    && !memcmp(src + off, searc + off, 8)) {
						if (!memcmp(src + 1, searc + 1, searclen - 10)) {
						MTC2:
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							src += searclen;
							continue;
						}
					}
					*dst++ = *src++;
				}
			}
			memmove(dst, s, end + searclen - s + 1);
			return s + (dst + searclen - s);
		}
		do {
			memmove(dst + rplclen,
				dst + searclen,
				(s + sz + 1) - dst + searclen);
			memcpy(dst, rplc, rplclen);
			dst += rplclen;
			sz += (long long)(rplclen - searclen);
		} while ((dst = (char *)jstr_memmem(dst, (s + sz) - dst, searc, searclen)));
		return s + sz;
		break;
	}
	}
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
static void jstr_replacen_mem(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const searc,
			      const char *JSTR_RST const rplc,
			      size_t n,
			      const size_t searclen,
			      const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return;
	if (unlikely(n == 0))
		return;
	switch (rplclen) {
	case 0: {
		*sz = jstr_rmn_mem_p(*s, searc, n, *sz, searclen) - *s;
		break;
	}
	case 1:
		if (searclen == 1) {
			jstr_replacenc(*s, *searc, *rplc, n);
			--*sz;
			return;
		}
		break;
	default: {
		char *mtc = *s;
		if (rplclen <= searclen) {
			while (n-- && (mtc = (char *)jstr_memmem(mtc, (*s + *sz) - mtc, searc, searclen))) {
				memmove(mtc + rplclen,
					mtc + searclen,
					(*s + *sz + 1) - mtc + searclen);
				memcpy(mtc, rplc, rplclen);
				mtc += rplclen;
				*sz += (long long)(rplclen - searclen);
			}
		} else {
			char *tmp;
			while (n-- && (mtc = (char *)jstr_memmem(mtc, (*s + *sz) - mtc, searc, searclen))) {
				if (*cap > *sz + rplclen - searclen + 1) {
					memmove(mtc + rplclen,
						mtc + searclen,
						(*s + *sz + 1) - mtc + searclen);
					memcpy(mtc, rplc, rplclen);
					mtc += rplclen;
				} else {
					JSTR_GROW(*cap, *sz + rplclen + 1);
					tmp = (char *)malloc(*cap);
					JSTR_MALLOC_ERR(tmp, return);
					memcpy(tmp, *s, mtc - *s);
					memcpy(tmp + (mtc - *s), rplc, rplclen);
					memcpy(tmp + (mtc - *s) + rplclen,
					       mtc + rplclen,
					       (*s + *sz + 1) - (mtc + rplclen));
					mtc = tmp + (mtc - *s) + rplclen;
					free(*s);
					*s = tmp;
				}
				*sz += (long long)(rplclen - searclen);
			}
		}
		break;
	}
	}
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
static void jstr_replaceall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const rplc,
				const size_t searclen,
				const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return;
	if (unlikely(rplclen == 0)) {
		*sz = jstr_rmall_mem_p(*s, searc, *sz, searclen) - *s;
		return;
	}
	char *dst = (char *)jstr_memmem(*s, *sz, searc, searclen);
	if (unlikely(!dst))
		return;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replaceallc(*s, *searc, *rplc);
			break;
		}
		case 2: {
			const char *src = dst + 1;
			const uint16_t nw = searc[0] << 8 | searc[1];
			uint16_t hw = src[0] << 8 | src[1];
			goto MTC2;
			for (; *src; hw = hw << 8 | *src)
				if (hw != nw) {
					*dst++ = *(src++ - 1);
				} else {
				MTC2:
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
				}
			if (hw != nw)
				*dst++ = *(src - 1);
			goto END_UNDER_4;
			break;
		}
		case 3: {
			const char *src = dst + 3;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
			uint32_t hw = src[0] << 24 | src[1] << 16 | src[2] << 8;
			goto MTC3;
			for (; *src; hw = (hw | *src++) << 8)
				if (hw != nw) {
					*dst++ = *(src - 3);
				} else {
				MTC3:
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
				}
			if (hw != nw)
				*dst++ = *(src - 3);
			goto END_UNDER_4;
			break;
		}
		case 4: {
			const char *src = dst + 4;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
			uint32_t hw = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
			goto MTC4;
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw)
					*dst++ = *(src - 4);
				else {
				MTC4:
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 3;
				}
			if (hw != nw)
				*dst++ = *(src - 4);
			goto END_UNDER_4;
			break;
		}
		END_UNDER_4:
			*dst = '\0';
			*sz = dst - *s;
			break;
		default: {
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const char *const end = *s + *sz - searclen;
			const char *src = dst;
			if (searclen < 15) {
				goto MTC5;
				while (src <= end) {
					if (nw == (src[0] << 8 | src[searclen - 1])
					    && !memcmp(src, searc, searclen)) {
					MTC5:
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						src += searclen;
						continue;
					}
					*dst++ = *src++;
				}
			} else {
				const size_t off = searclen - 9;
				goto MTC6;
				while (src <= end) {
					if (nw == (src[0] << 8 | src[searclen - 1])
					    && !memcmp(src + off, searc + off, 8)) {
						if (!memcmp(src + 1, searc + 1, searclen - 10)) {
						MTC6:
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							src += searclen;
							continue;
						}
					}
					*dst++ = *src++;
				}
			}
			memmove(dst, src, end + searclen - src + 1);
			*sz = dst + (end + searclen - src) - *s;
			break;
		}
		}
		return;
	}
	char *tmp;
	do {
		if (*cap > *sz + rplclen - searclen + 1) {
			memmove(dst + rplclen,
				dst + searclen,
				(*s + *sz + 1) - dst + searclen);
			memcpy(dst, rplc, rplclen);
			dst += rplclen;
		} else {
			JSTR_GROW(*cap, *sz + rplclen + 1);
			tmp = (char *)malloc(*cap);
			JSTR_MALLOC_ERR(tmp, return);
			memcpy(tmp, *s, dst - *s);
			memcpy(tmp + (dst - *s), rplc, rplclen);
			memcpy(tmp + (dst - *s) + rplclen,
			       dst + rplclen,
			       (*s + *sz + 1) - (dst + rplclen));
			dst = tmp + (dst - *s) + rplclen;
			free(*s);
			*s = tmp;
		}
		*sz += rplclen - searclen;
	} while ((dst = (char *)jstr_memmem(dst, (*s + *sz) - dst, searc, searclen)));
}

/*
  Insert SRC into DST[AT].
  Assumes that S have enough space for SRC.
  Return value:
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert_mem_f(char *JSTR_RST const s,
			      const size_t at,
			      const char *JSTR_RST const src,
			      const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(s + at, src, srclen);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert_mem(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const size_t at,
			    const char *JSTR_RST const src,
			    const size_t srclen) JSTR_NOEXCEPT
{
	if (at + srclen > *sz) {
		JSTR_REALLOC(*s, *cap, at + srclen + 1, return);
		*sz = at + srclen;
		*(*s + *sz) = '\0';
	}
	jstr_insert_mem_f(*s, at, src, srclen);
}

/*
  Insert SRC after C in DST.
  Assumes that S have enough space for S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftc_mem_f(char *JSTR_RST const s,
				  const int c,
				  const char *JSTR_RST const src,
				  const size_t sz,
				  const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		jstr_insert_mem_f(s, p - s + 1, src, srclen);
}

/*
  Insert SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftc_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const int c,
				const char *JSTR_RST const src,
				const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_insert_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Insert SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftallc_mem(char **JSTR_RST const s,
				   size_t *JSTR_RST const sz,
				   size_t *JSTR_RST const cap,
				   const int c,
				   const char *JSTR_RST const src,
				   const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	size_t off = 0;
	for (char *p;
	     (p = (char *)memchr(*s + off, c, *sz - off));
	     off += *s - p + 1)
		jstr_insert_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Insert SRC after end of NE in DST.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft_mem_f(char *JSTR_RST const s,
				 const char *JSTR_RST const searc,
				 const char *JSTR_RST const src,
				 const size_t sz,
				 const size_t searclen,
				 const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftc_mem_f(s, *searc, src, sz, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)jstr_memmem(s, sz, searc, searclen);
		if (p)
			jstr_insert_mem_f(s, p - s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft_mem(char **JSTR_RST const s,
			       size_t *JSTR_RST const sz,
			       size_t *JSTR_RST const cap,
			       const char *JSTR_RST const searc,
			       const char *JSTR_RST const src,
			       const size_t searclen,
			       const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)jstr_memmem(*s, *sz, searc, searclen);
		if (p)
			jstr_insert_mem(s, sz, cap, p - *s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftall_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const src,
				  const size_t searclen,
				  const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftallc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		for (char *p;
		     (p = (char *)jstr_memmem(*s + off, *sz - off, searc, searclen));
		     off += *s - p + searclen)
			jstr_insert_mem(s, sz, cap, p - *s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Slip SRC into DST[AT].
  Return value:
  Pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slip_mem_p_f(char *JSTR_RST const s,
			       const size_t at,
			       const char *JSTR_RST const src,
			       const size_t sz,
			       const size_t srclen) JSTR_NOEXCEPT
{
	memmove(s + at + srclen,
		s + at,
		sz - at + 1);
	memcpy(s + at, src, srclen);
	return s + sz;
}

/*
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slip_mem(char **JSTR_RST const s,
			  size_t *JSTR_RST const sz,
			  size_t *JSTR_RST const cap,
			  const size_t at,
			  const char *JSTR_RST const src,
			  const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap > *sz + srclen + 1) {
		memmove(*s + at + srclen,
			*s + at,
			*sz - at + 1);
		memcpy(*s + at, src, srclen);
	} else {
		JSTR_GROW(*cap, *sz + srclen + 1);
		char *const tmp = (char *)malloc(*cap);
		JSTR_MALLOC_ERR(tmp, return);
		memcpy(tmp, *s, at);
		memcpy(tmp + at, src, srclen);
		memcpy(tmp + at + srclen,
		       *s + at,
		       *sz - at + 1);
		free(*s);
		*s = tmp;
	}
	*sz += srclen;
}

/*
  Slip SRC after C in DST.
  Return value:
  Pointer to '\0' in S.
  Assumes that S have enough space for SRC.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slipaftc_mem_p_f(char *JSTR_RST const s,
				   const int c,
				   const char *JSTR_RST const src,
				   const size_t sz,
				   const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return s + sz;
	const char *const p = (char *)memchr(s, c, sz);
	if (p)
		return jstr_slip_mem_p_f(s, p - s, src, sz - (p - s), srclen);
	return s + sz;
}

/*
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftc_mem(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const int c,
			      const char *JSTR_RST const src,
			      const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	const char *const p = (char *)memchr(*s, c, *sz);
	if (p)
		jstr_slip_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftallc_mem(char **JSTR_RST const s,
				 size_t *JSTR_RST const sz,
				 size_t *JSTR_RST const cap,
				 const int c,
				 const char *JSTR_RST const src,
				 const size_t srclen) JSTR_NOEXCEPT
{
	if (unlikely(srclen == 0))
		return;
	size_t off = 0;
	for (char *p;
	     (p = (char *)memchr(*s + off, c, *sz - off));
	     off += p - *s + 1)
		jstr_slip_mem(s, sz, cap, p - *s + 1, src, srclen);
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_slipaft_mem_f(char *JSTR_RST const s,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const src,
				const size_t sz,
				const size_t searclen,
				const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		return s + sz;
		break;
	case 1: {
		return jstr_slipaftc_mem_p_f(s, *searc, src, sz, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return s + sz;
		const char *const p = (char *)jstr_memmem(s, sz, searc, searclen);
		if (p)
			return jstr_slip_mem_p_f(s, p - s, src, sz, srclen);
		return s + sz;
	}
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaft_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const searc,
			     const char *JSTR_RST const src,
			     const size_t searclen,
			     const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)jstr_memmem(*s, *sz, searc, searclen);
		if (p)
			jstr_slip_mem(s, sz, cap, p - *s + searclen, src, srclen);
		break;
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const searc,
				const char *JSTR_RST const src,
				const size_t searclen,
				const size_t srclen) JSTR_NOEXCEPT
{
	switch (searclen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftallc_mem(s, sz, cap, *searc, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		for (char *p;
		     (p = (char *)jstr_memmem(*s + off, *sz - off, searc, searclen));
		     off += p - *s + searclen)
			jstr_slip_mem(s, sz, cap, p - *s + searclen, src, srclen);
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
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_endswith_mem_f(const char *JSTR_RST const hs,
			       const char *JSTR_RST const ne,
			       const size_t hsz,
			       const size_t nelen) JSTR_NOEXCEPT
{
	return memcmp(hs + hsz - nelen, ne, nelen);
}

/*
  Checks if S2 is in end of S1.
  Return value:
  0 if true;
  1 if false.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_endswith_mem(const char *JSTR_RST const hs,
			     const char *JSTR_RST const ne,
			     const size_t hsz,
			     const size_t nelen) JSTR_NOEXCEPT
{
	return (hsz < nelen) ? 1 : memcmp(hs + hsz - nelen, ne, nelen);
}

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_RST
#undef JSTR_REPLACE
#undef JSTR_MALLOC_ERR
#undef JSTR_GROW
#undef JSTR_REALLOC

#endif /* JSTR_H_DEF */
