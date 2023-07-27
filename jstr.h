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
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append_mem_f(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      const char *JSTR_RST const src,
			      const size_t srclen) JSTR_NOEXCEPT
{
	memcpy(*s, src, srclen + 1);
	*sz += srclen;
}

/*
   Append SRC to DST.
   Use non-f version for bounds checking.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append_f(char **JSTR_RST const s,
			  size_t *JSTR_RST const sz,
			  const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_append_mem_f(s, sz, src, strlen(src));
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
	jstr_append_mem_f(s, sz, src, srclen);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_append(char **JSTR_RST const s,
			size_t *JSTR_RST const sz,
			size_t *JSTR_RST const cap,
			const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_append_mem(s, sz, cap, src, strlen(src));
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
			  const char *JSTR_RST const ne,
			  size_t sz,
			  const size_t nelen) JSTR_NOEXCEPT
{
	int count = 0;
	for (; (s = (char *)memmem(s, sz, ne, nelen));
	     ++count, s += nelen, sz -= nelen)
		;
	return count;
}

/*
  Count occurences of NE in HS.
  Return value:
  occurences of NE in HS.
*/
JSTR_INLINE
JSTR_CONST
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_count(const char *JSTR_RST const s,
		      const char *JSTR_RST const ne,
		      const size_t sz) JSTR_NOEXCEPT
{
	return jstr_count_mem(s, ne, sz, strlen(ne));
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
		      const char *JSTR_RST const ne) JSTR_NOEXCEPT
{
	int count = 0;
	while ((s = strstr(s, ne)))
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
  Reverse S.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_rev(char *JSTR_RST const s) JSTR_NOEXCEPT
{
	jstr_rev_mem(s, strlen(s));
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
			     const size_t n) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	char *end = s + n - 1;
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
  Trim spaces in [ \t] from end of S.
  Return value:
  Pointer to '\0' in S;
  S if SLEN is 0.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_trim_p(char *JSTR_RST const s) JSTR_NOEXCEPT
{
	return jstr_trim_mem_p(s, strlen(s));
}

/*
  Remove first C in S.
  Return value:
  Pointer to '\0' in S;
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *jstr_rmc_mem(char *JSTR_RST s,
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
static char *jstr_rmc(char *JSTR_RST s,
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
static char *jstr_rmallc_mem(char *JSTR_RST s,
			     const int c,
			     const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	s = (char *)memchr(s, c, sz);
	if (!s)
		return s + sz;
	const char *src = s;
	do
		if (*src != c)
			*s++ = *src;
	while (*++src);
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
static char *jstr_rmallc(char *JSTR_RST s,
			 const int c) JSTR_NOEXCEPT
{
	if (unlikely(!*s))
		return s;
	s = strchr(s, c);
	if (!s)
		return s;
	const char *src = s;
	do
		if (*src != c)
			*s++ = *src;
	while (*++src);
	*s = '\0';
	return s;
}

/*
  Remove characters in REJECT in S.
  Return value:
  New length of S;
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
			      const int srch,
			      const int rplc,
			      const size_t ssz) JSTR_NOEXCEPT
{
	if ((s = (char *)memchr(s, srch, ssz)))
		*s = rplc;
}

/*
  Replace first SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replacec(char *JSTR_RST s,
			  const int srch,
			  const int rplc) JSTR_NOEXCEPT
{
	if ((s = strchr(s, srch)))
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
			   const char *JSTR_RST const ne,
			   const size_t sz,
			   const size_t nelen) JSTR_NOEXCEPT
{
	char *const p = (char *)jstr_memmem(s, sz, ne, nelen);
	if (p) {
		memmove(p, p + nelen, (s + sz) - p);
		return s + sz - nelen;
	}
	return s + sz;
}

/*
  Remove first HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rm_p(char *JSTR_RST const s,
		       const char *JSTR_RST const ne,
		       const size_t sz) JSTR_NOEXCEPT
{
	return jstr_rm_mem_p(s, ne, sz, strlen(ne));
}

/*
  Remove all HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rmall_mem_p(char *JSTR_RST s,
			      const char *JSTR_RST const ne,
			      size_t sz,
			      size_t nelen) JSTR_NOEXCEPT
{
	if (unlikely(nelen == 0))
		return s + sz;
	char *dst = (char *)memchr(s, *ne, sz);
	if (!dst)
		return s + sz;
	sz -= (s - dst);
	s = dst;
	switch (nelen) {
	case 0:
		return dst;
		break;
	case 1: {
		do
			if (*s != *ne)
				*dst++ = *s;
		while (*++s);
		*dst = '\0';
		return dst;
		break;
	}
	case 2: {
		const uint16_t nw = ne[0] << 8 | ne[1];
		uint16_t sw = s[0] << 8 | s[1];
		for (++s, sz -= 2; sz--; sw = sw << 8 | *s)
			if (sw != nw)
				*dst++ = *(s++ - 1);
			else
				s += 2;
		if (sw != nw)
			*dst++ = *(s - 1);
		*dst = '\0';
		return dst;
		break;
	}
	case 3: {
		const uint32_t nw = ne[0] << 24 | ne[1] << 16 | ne[2] << 8;
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8;
		for (s += 3, sz -= 3; sz--; sw = (sw | *s++) << 8)
			if (sw != nw)
				*dst++ = *(s - 3);
			else
				s += 2;
		if (sw != nw)
			*dst++ = *(s - 3);
		*dst = '\0';
		return dst;
		break;
	}
	case 4: {
		const uint32_t nw = ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		for (s += 4, sz -= 4; sz--; sw = sw << 8 | *s++)
			if (sw != nw)
				*dst++ = *(s - 4);
			else
				s += 3;
		if (sw != nw)
			*dst++ = *(s - 4);
		*dst = '\0';
		return dst;
		break;
	}
	default: {
		const uint16_t nw = ne[0] << 8 | ne[nelen - 1];
		const char *const end = s + sz - nelen;
		if (nelen < 15)
			while (s <= end)
				if (nw == (s[0] << 8 | s[nelen - 1])
				    && !memcmp(s, ne, nelen))
					s += nelen;
				else
					*dst++ = *s++;
		else
			for (const size_t off = nelen - 9; s <= end;)
				if (nw == (s[0] << 8 | s[nelen - 1])
				    && !memcmp(s + off, ne + off, 8)
				    && !memcmp(s, ne, nelen))
					s += nelen;
				else
					*dst++ = *s++;
		memcpy(dst, s, end + nelen - s + 1);
		return dst + (end + nelen - s);
		break;
	}
	}
}

/*
  Remove all HS in S.
  Return value:
  Pointer to '\0' in S.
*/
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static char *jstr_rmall_p(char *JSTR_RST const s,
			  const char *JSTR_RST const ne,
			  const size_t sz) JSTR_NOEXCEPT
{
	return jstr_rmall_mem_p(s, ne, sz, strlen(ne));
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc_mem(char *JSTR_RST s,
				 const int srch,
				 const int rplc,
				 const size_t ssz) JSTR_NOEXCEPT
{
	if ((s = (char *)memchr(s, srch, ssz)))
		do
			if (*s == srch)
				*s = rplc;
		while (*++s);
}

/*
  Replace all SEARCH in REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceallc(char *JSTR_RST s,
			     const int srch,
			     const int rplc) JSTR_NOEXCEPT
{
	if ((s = strchr(s, srch)))
		do
			if (*s == srch)
				*s = rplc;
		while (*++s);
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
static void jstr_replace_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const srch,
			     const char *JSTR_RST const rplc,
			     const size_t srchlen,
			     const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(srchlen == 0))
		return;
	switch (rplclen) {
	case 0:
		*sz = jstr_rm_mem_p(*s, srch, *sz, srchlen) - *s;
		break;
	case 1:
		if (srchlen == 1) {
			jstr_replacec(*s, *srch, *rplc);
			--*sz;
			return;
		}
		break;
	default: {
		char *mtc = (char *)jstr_memmem(*s, *sz, srch, srchlen);
		if (!mtc)
			return;
		if (rplclen <= srchlen || *cap > *sz + rplclen - srchlen + 1) {
			memmove(mtc + rplclen,
				mtc + srchlen,
				(*s + *sz + 1) - mtc + srchlen);
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
		*sz += (long long)(rplclen - srchlen);
		break;
	}
	}
}

/*
  Replace first SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replace(char **JSTR_RST const s,
			 size_t *JSTR_RST const sz,
			 size_t *JSTR_RST const cap,
			 const char *JSTR_RST const srch,
			 const char *JSTR_RST const rplc) JSTR_NOEXCEPT
{
	jstr_replace_mem(s, sz, cap, srch, rplc, strlen(srch), strlen(rplc));
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
static void jstr_replaceall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const srch,
				const char *JSTR_RST const rplc,
				const size_t srchlen,
				const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(srchlen == 0))
		return;
	switch (rplclen) {
	case 0: {
		*sz = jstr_rmall_mem_p(*s, srch, *sz, srchlen) - *s;
		break;
	}
	case 1:
		if (srchlen == 1) {
			jstr_replaceallc(*s, *srch, *rplc);
			--*sz;
			return;
		}
		break;
	default: {
		char *mtc = *s;
		if (rplclen <= srchlen)
			while ((mtc = (char *)jstr_memmem(mtc, (*s + *sz) - mtc, srch, srchlen))) {
				memmove(mtc + rplclen,
					mtc + srchlen,
					(*s + *sz + 1) - mtc + srchlen);
				memcpy(mtc, rplc, rplclen);
				mtc += rplclen;
				*sz += (long long)(rplclen - srchlen);
			}
		else
			for (char *tmp;
			     (mtc = (char *)jstr_memmem(mtc, (*s + *sz) - mtc, srch, srchlen));
			     *sz += (long long)(rplclen - srchlen))
				if (*cap > *sz + rplclen - srchlen + 1) {
					memmove(mtc + rplclen,
						mtc + srchlen,
						(*s + *sz + 1) - mtc + srchlen);
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
		break;
	}
	}
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_replaceall(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const char *JSTR_RST const srch,
			    const char *JSTR_RST const rplc) JSTR_NOEXCEPT
{
	jstr_replaceall_mem(s, sz, cap, srch, rplc, strlen(srch), strlen(rplc));
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
		JSTR_REALLOC(*s, *cap, *sz + srclen + 1, return);
		*sz += srclen;
		*(*s + *sz) = '\0';
	}
	memcpy(*s + at, src, srclen);
}

/*
  Insert SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insert(char **JSTR_RST const s,
			size_t *JSTR_RST const sz,
			size_t *JSTR_RST const cap,
			const size_t at,
			const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_insert_mem(s, sz, cap, at, src, strlen(src));
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
  Insert SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftc(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const int c,
			    const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_insertaftc_mem(s, sz, cap, c, src, strlen(src));
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
  Insert SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftallc(char **JSTR_RST const s,
			       size_t *JSTR_RST const sz,
			       size_t *JSTR_RST const cap,
			       const int c,
			       const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_insertaftallc_mem(s, sz, cap, c, src, strlen(src));
}

/*
  Insert SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft_mem(char **JSTR_RST const s,
			       size_t *JSTR_RST const sz,
			       size_t *JSTR_RST const cap,
			       const char *JSTR_RST const ne,
			       const char *JSTR_RST const src,
			       const size_t nelen,
			       const size_t srclen) JSTR_NOEXCEPT
{
	switch (nelen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftc_mem(s, sz, cap, *ne, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)jstr_memmem(*s, *sz, ne, nelen);
		if (p)
			jstr_insert_mem(s, sz, cap, p - *s + nelen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaft(char **JSTR_RST const s,
			   size_t *JSTR_RST const sz,
			   size_t *JSTR_RST const cap,
			   const char *JSTR_RST const ne,
			   const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_insertaft_mem(s, sz, cap, ne, src, strlen(ne), strlen(src));
}

/*
  Insert SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftall_mem(char **JSTR_RST const s,
				  size_t *JSTR_RST const sz,
				  size_t *JSTR_RST const cap,
				  const char *JSTR_RST const ne,
				  const char *JSTR_RST const src,
				  const size_t nelen,
				  const size_t srclen) JSTR_NOEXCEPT
{
	switch (nelen) {
	case 0:
		break;
	case 1: {
		jstr_insertaftallc_mem(s, sz, cap, *ne, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		for (char *p;
		     (p = (char *)jstr_memmem(*s + off, *sz - off, ne, nelen));
		     off += *s - p + nelen)
			jstr_insert_mem(s, sz, cap, p - *s + nelen, src, srclen);
		break;
	}
	}
}

/*
  Insert SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_insertaftall(char **JSTR_RST const s,
			      size_t *JSTR_RST const sz,
			      size_t *JSTR_RST const cap,
			      const char *JSTR_RST const ne,
			      const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_insertaftall_mem(s, sz, cap, ne, src, strlen(ne), strlen(src));
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
  Slip SRC into DST[AT].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slip(char **JSTR_RST const s,
		      size_t *JSTR_RST const sz,
		      size_t *JSTR_RST const cap,
		      const size_t at,
		      const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_slip_mem(s, sz, cap, at, src, strlen(src));
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
  Slip SRC after C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftc(char **JSTR_RST const s,
			  size_t *JSTR_RST const sz,
			  size_t *JSTR_RST const cap,
			  const int c,
			  const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_slipaftc_mem(s, sz, cap, c, src, strlen(src));
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
  Slip SRC after all C in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftallc(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const int c,
			     const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_slipaftallc_mem(s, sz, cap, c, src, strlen(src));
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaft_mem(char **JSTR_RST const s,
			     size_t *JSTR_RST const sz,
			     size_t *JSTR_RST const cap,
			     const char *JSTR_RST const ne,
			     const char *JSTR_RST const src,
			     const size_t nelen,
			     const size_t srclen) JSTR_NOEXCEPT
{
	switch (nelen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftc_mem(s, sz, cap, *ne, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		const char *const p = (char *)jstr_memmem(*s, *sz, ne, nelen);
		if (p)
			jstr_slip_mem(s, sz, cap, p - *s + nelen, src, srclen);
		break;
	}
	}
}

/*
  Slip SRC after end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaft(char **JSTR_RST const s,
			 size_t *JSTR_RST const sz,
			 size_t *JSTR_RST const cap,
			 const char *JSTR_RST const ne,
			 const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_slipaft_mem(s, sz, cap, ne, src, strlen(ne), strlen(src));
}

/*
  Slip SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftall_mem(char **JSTR_RST const s,
				size_t *JSTR_RST const sz,
				size_t *JSTR_RST const cap,
				const char *JSTR_RST const ne,
				const char *JSTR_RST const src,
				const size_t nelen,
				const size_t srclen) JSTR_NOEXCEPT
{
	switch (nelen) {
	case 0:
		break;
	case 1: {
		jstr_slipaftallc_mem(s, sz, cap, *ne, src, srclen);
		break;
	}
	default: {
		if (unlikely(srclen == 0))
			return;
		size_t off = 0;
		for (char *p;
		     (p = (char *)jstr_memmem(*s + off, *sz - off, ne, nelen));
		     off += p - *s + nelen)
			jstr_slip_mem(s, sz, cap, p - *s + nelen, src, srclen);
	}
	}
}

/*
  Slip SRC after all end of NE in DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_slipaftall(char **JSTR_RST const s,
			    size_t *JSTR_RST const sz,
			    size_t *JSTR_RST const cap,
			    const char *JSTR_RST const ne,
			    const char *JSTR_RST const src) JSTR_NOEXCEPT
{
	jstr_slipaftall_mem(s, sz, cap, ne, src, strlen(ne), strlen(src));
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
static int jstr_endswith_mem(const char *JSTR_RST const s1,
			     const char *JSTR_RST const s2,
			     const size_t l1,
			     const size_t l2) JSTR_NOEXCEPT
{
	return (l1 < l2) ? 1 : memcmp(s1 + l1 - l2, s2, l2);
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
static int jstr_endswith(const char *JSTR_RST const s1,
			 const char *JSTR_RST const s2,
			 const size_t l1) JSTR_NOEXCEPT
{
	return jstr_endswith_mem(s1, s2, l1, strlen(s2));
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
