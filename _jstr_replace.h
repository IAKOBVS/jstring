#ifndef JSTR_H_REPLACE_DEF
#define JSTR_H_REPLACE_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_macros.h"
#include "_jstr_string.h"

#define JSTR_EXTERN_C  1
#define JSTR_NAMESPACE 0

#if JSTR_NAMESPACE && defined(__cplusplus)
namespace jstr {
#endif /* JSTR_NAMESPACE */
#if JSTR_EXTERN_C && defined(__cplusplus)
extern "C" {
#endif /* JSTR_EXTERN_C */

#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % sizeof(shift))

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
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
		return s + sz;
	memmove(s, s + 1, sz - (s - sstart) + 1);
	return s + sz - (s - sstart);
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
	s = strchr(s, c);
	if (unlikely(!s))
		return s;
	const char *src = s;
	while ((*s++ = *++src))
		;
	return s - 1;
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
	if (unlikely(!s))
		return s + sz;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
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
	if (unlikely(!s))
		return s;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
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
	if (unlikely(!s))
		return s;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		} else {
			if (unlikely(!--n)) {
				do
					*s++ = *src++;
				while (*src);
				break;
			}
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
	if (unlikely(!s))
		return s;
	const char *src = s + 1;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*s++ = *src;
		} else {
			if (unlikely(!--n)) {
				do
					*s++ = *src++;
				while (*src);
				break;
			}
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
	char *const p = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
	if (unlikely(!p))
		return s + sz;
	memmove(p, p + searclen, (s + sz) - p);
	return s + sz - searclen;
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
	if (unlikely(s + searclen + searclen >= s + sz)) {
		*s = '\0';
		return s;
	}
	char *dst = s;
	switch (searclen) {
	case 1: {
		s = dst;
		for (;; ++s)
			if (*s != *searc) {
				if (unlikely(!*s))
					break;
				*dst++ = *s;
			} else {
				if (unlikely(!--n)) {
					do
						*dst++ = *s++;
					while (*s);
					break;
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
		for (; *s; sw = sw << 8 | *s)
			if (sw != nw) {
				*dst++ = *(s++ - 1);
			} else {
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 1);
					while (*s);
					break;
				} else {
					s += 2;
				}
			}
		if (sw != nw)
			*dst++ = *(s - 1);
		goto END_UNDER_4;
		break;
	}
	case 3: {
		s = dst + 3;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8;
		for (; *s; sw = (sw | *s++) << 8)
			if (sw != nw) {
				*dst++ = *(s - 3);
			} else {
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 3);
					while (*s);
					break;
				} else {
					s += 2;
				}
			}
		if (sw != nw) {
			memmove(dst, s - 3, 3);
			dst += 3;
		}
		goto END_UNDER_4;
		break;
	}
	case 4: {
		s = dst + 4;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
		uint32_t sw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		for (; *s; sw = sw << 8 | *s++)
			if (sw != nw) {
				*dst++ = *(s - 4);
			} else {
				if (unlikely(!--n)) {
					do
						*dst++ = *(s++ - 4);
					while (*s);
					break;
				} else {
					s += 3;
				}
			}
		if (sw != nw) {
			memmove(dst, s - 4, 4);
			dst += 4;
		}
		goto END_UNDER_4;
		break;
	}
	END_UNDER_4:
		*dst = '\0';
		return dst;
		break;
	default: {
		const char *const end = s + sz - searclen;
		if (unlikely(searclen > 256)) {
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const size_t off = searclen - 9;
			while (s <= end) {
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s + 1, searc + 1, searclen - 10)) {
						s += searclen;
						if (unlikely(!--n))
							break;
						continue;
					}
				}
				*dst++ = *s++;
			}
			memmove(dst, s, end + searclen - s + 1);
			return dst + (end + searclen - s);
		}
		uint8_t shift[256];
		size_t tmp;
		size_t shift1;
		size_t m1 = searclen - 1;
		size_t off = 0;
		memset(shift, 0, sizeof(shift));
		for (int i = 1; i < m1; ++i)
			shift[JSTR_HASH2(searc + i)] = i;
		shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
		shift[JSTR_HASH2(searc + m1)] = m1;
		char *old = dst;
		while (s <= end) {
			do {
				s += m1;
				tmp = shift[JSTR_HASH2(s)];
			} while (!tmp && s <= end);
			s -= tmp;
			if (tmp < m1)
				continue;
			if (m1 < 15 || !memcmp(s + off, searc + off, 8)) {
				if (!memcmp(s, searc, searclen)) {
					memmove(dst, old, s - old);
					dst += (s - old);
					old += (s - old);
					s += searclen;
					if (unlikely(!--n))
						break;
					continue;
				}
				off = (off >= 8 ? off : m1) - 8;
			}
			s += shift1;
		}
		memmove(dst, old, end + searclen - old + 1);
		return dst + (end + searclen - old);
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
	if (unlikely(s + searclen + searclen >= s + sz)) {
		*s = '\0';
		return s;
	}
	char *dst = s;
	switch (searclen) {
	case 1: {
		s = dst + 1;
		for (;; ++s)
			if (*s != *searc) {
				if (unlikely(!*s))
					break;
				*dst++ = *s;
			}
		goto END_UNDER_4;
		break;
	}
	case 2: {
		s = dst + 1;
		const uint16_t nw = searc[0] << 8 | searc[1];
		uint16_t hw = s[0] << 8 | s[1];
		for (; *s; hw = hw << 8 | *s)
			if (hw != nw)
				*dst++ = *(s++ - 1);
			else
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
		for (; *s; hw = (hw | *s++) << 8)
			if (hw != nw)
				*dst++ = *(s - 3);
			else
				s += 2;
		if (hw != nw) {
			memmove(dst, s - 3, 3);
			dst += 3;
		}
		goto END_UNDER_4;
		break;
	}
	case 4: {
		s = dst + 4;
		const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
		uint32_t hw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
		for (; *s; hw = hw << 8 | *s++)
			if (hw != nw)
				*dst++ = *(s - 4);
			else
				s += 3;
		if (hw != nw) {
			memmove(dst, s - 4, 4);
			dst += 4;
		}
		goto END_UNDER_4;
		break;
	}
	END_UNDER_4:
		*dst = '\0';
		return dst;
		break;
	default: {
		const char *const end = s + sz - searclen;
		if (unlikely(searclen > 256)) {
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const size_t off = searclen - 9;
			while (s <= end) {
				if (nw == (s[0] << 8 | s[searclen - 1])
				    && !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s + 1, searc + 1, searclen - 10)) {
						s += searclen;
						continue;
					}
				}
				*dst++ = *s++;
			}
			memmove(dst, s, end + searclen - s + 1);
			return dst + (end + searclen - s);
		}
		uint8_t shift[256];
		size_t tmp;
		size_t shift1;
		size_t m1 = searclen - 1;
		size_t off = 0;
		memset(shift, 0, sizeof(shift));
		for (int i = 1; i < m1; ++i)
			shift[JSTR_HASH2(searc + i)] = i;
		shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
		shift[JSTR_HASH2(searc + m1)] = m1;
		char *old = dst;
		while (s <= end) {
			do {
				s += m1;
				tmp = shift[JSTR_HASH2(s)];
			} while (!tmp && s <= end);
			s -= tmp;
			if (tmp < m1)
				continue;
			if (m1 < 15 || !memcmp(s + off, searc + off, 8)) {
				if (!memcmp(s, searc, searclen)) {
					memmove(dst, old, s - old);
					dst += (s - old);
					old += (s - old);
					s += searclen;
					continue;
				}
				off = (off >= 8 ? off : m1) - 8;
			}
			s += shift1;
		}
		memmove(dst, old, end + searclen - old + 1);
		return dst + (end + searclen - old);
		break;
	}
	}
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
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
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
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	*s = rplc;
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
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc)
		MTC:
			*s = rplc;
		else if (unlikely(!*s))
			break;
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
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc)
		MTC:
			*s = rplc;
		else if (unlikely(!*s))
			break;
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
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s == searc) {
		MTC:
			*s = rplc;
			if (unlikely(!--n))
				break;
		} else if (unlikely(!*s)) {
			break;
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
	s = strchr(s, searc);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;;) {
		if (*s == searc) {
		MTC:
			*s = rplc;
			if (unlikely(!--n))
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
			jstr_replacec_mem(s, *searc, *rplc, sz);
			return s + sz;
		}
		/* FALLTHROUGH */
	default: {
		char *mtc = (char *)JSTR_MEMMEM(s, sz, searc, searclen);
		if (unlikely(!mtc))
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
JSTR_MAYBE_UNUSED
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
			jstr_replacec_mem(*s, *searc, *rplc, *sz);
			return;
		}
		break;
	default: {
		char *mtc = (char *)JSTR_MEMMEM(*s, *sz, searc, searclen);
		if (unlikely(!mtc))
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
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_replacen_mem_p_f(char *JSTR_RST s,
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
	if (unlikely(rplclen == 0))
		return jstr_rmn_mem_p(s, searc, n, sz, searclen);
	char *dst = s;
	if (unlikely(dst + searclen + searclen >= s + sz))
		goto RPLC_GROW;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replacenc_mem(s, *searc, *rplc, n, sz);
			return s + sz;
			break;
		}
		case 2: {
			const char *src = dst + 1;
			const uint16_t nw = searc[0] << 8 | searc[1];
			uint16_t hw = dst[0] << 8 | dst[1];
			for (; *src; hw = hw << 8 | *src)
				if (hw != nw) {
					*dst++ = *(src++ - 1);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
					if (unlikely(!--n)) {
						memmove(dst, src - 1, s + sz - src - 1 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw)
				*dst++ = *(src - 1);
			goto END_UNDER_4;
			break;
		}
		case 3: {
			const char *src = dst + 3;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8;
			for (; *src; hw = (hw | *src++) << 8)
				if (hw != nw) {
					*dst++ = *(src - 3);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
					if (unlikely(!--n)) {
						memmove(dst, src - 3, s + sz - src - 3 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw) {
				memmove(dst, src - 3, 3);
				dst += 3;
			}
			goto END_UNDER_4;
			break;
		}
		case 4: {
			const char *src = dst + 4;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8 | dst[3];
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw)
					*dst++ = *(src - 4);
				else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 3;
					if (unlikely(!--n)) {
						memmove(dst, src - 4, s + sz - src - 4 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw) {
				memmove(dst, src - 4, 4);
				dst += 4;
			}
			goto END_UNDER_4;
			break;
		}
		END_UNDER_4:
			*dst = '\0';
			return dst;
			break;
		default: {
			const char *const end = s + sz - searclen;
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			if (unlikely(searclen > 256)) {
				const size_t off = searclen - 9;
				while (s <= end) {
					if (nw == (s[0] << 8 | s[searclen - 1])
					    && !memcmp(s + off, searc + off, 8)) {
						if (!memcmp(s + 1, searc + 1, searclen - 10)) {
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							s += searclen;
							if (unlikely(!--n))
								break;
							continue;
						}
					}
					*dst++ = *s++;
				}
				memmove(dst, s, end + searclen - s + 1);
				return dst + (end + searclen - s);
				break;
			}
			uint8_t shift[256];
			size_t tmp;
			size_t shift1;
			size_t m1 = searclen - 1;
			size_t off = 0;
			memset(shift, 0, sizeof(shift));
			for (int i = 1; i < m1; ++i)
				shift[JSTR_HASH2(searc + i)] = i;
			shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
			shift[JSTR_HASH2(searc + m1)] = m1;
			char *old = dst;
			while (s <= end) {
				do {
					s += m1;
					tmp = shift[JSTR_HASH2(s)];
				} while (!tmp && s <= end);
				s -= tmp;
				if (tmp < m1)
					continue;
				if (m1 < 15 || !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s, searc, searclen)) {
						memmove(dst, old, s - old);
						dst += (s - old);
						old += (s - old);
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						old += searclen;
						s += searclen;
						if (unlikely(!--n))
							break;
						continue;
					}
					off = (off >= 8 ? off : m1) - 8;
				}
				s += shift1;
			}
			memmove(dst, old, end + searclen - old + 1);
			return dst + (end + searclen - old);
			break;
		}
		}
	}
RPLC_GROW:
	while (n-- && (dst = (char *)JSTR_MEMMEM(dst, (s + sz) - dst, searc, searclen))) {
		memmove(dst + rplclen,
			dst + searclen,
			(s + sz + 1) - dst + searclen);
		memcpy(dst, rplc, rplclen);
		dst += rplclen;
		sz += (long long)(rplclen - searclen);
	}
	return s + sz;
}

/*
  Replace all SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
/* JSTR_NONNULL_ALL */
/* JSTR_WARN_UNUSED */
/* JSTR_MAYBE_UNUSED */
static char *jstr_replaceall_mem_p_f(char *JSTR_RST s,
				     const char *JSTR_RST const searc,
				     const char *JSTR_RST const rplc,
				     size_t sz,
				     const size_t searclen,
				     const size_t rplclen) JSTR_NOEXCEPT
{
	if (unlikely(searclen == 0))
		return s + sz;
	if (unlikely(rplclen == 0))
		return jstr_rmall_mem_p(s, searc, sz, searclen);
	char *dst = s;
	if (unlikely(dst + searclen + searclen >= s + sz))
		goto RPLC;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replaceallc_mem(s, *searc, *rplc, sz);
			break;
		}
		case 2: {
			const char *src = dst + 1;
			const uint16_t nw = searc[0] << 8 | searc[1];
			uint16_t hw = dst[0] << 8 | dst[1];
			for (; *src; hw = hw << 8 | *src)
				if (hw != nw) {
					*dst++ = *(src++ - 1);
				} else {
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
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8;
			for (; *src; hw = (hw | *src++) << 8)
				if (hw != nw) {
					*dst++ = *(src - 3);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
				}
			if (hw != nw) {
				memmove(dst, src - 3, 3);
				dst += 3;
			}
			goto END_UNDER_4;
			break;
		}
		case 4: {
			const char *src = dst + 4;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8 | dst[3];
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw)
					*dst++ = *(src - 4);
				else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 3;
				}
			if (hw != nw) {
				memmove(dst, src - 4, 3);
				dst += 4;
			}
			goto END_UNDER_4;
			break;
		}
		END_UNDER_4:
			*dst = '\0';
			return dst;
			break;
		default: {
			const char *const end = s + sz - searclen;
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			if (unlikely(searclen > 256)) {
				const size_t off = searclen - 9;
				while (s <= end) {
					if (nw == (s[0] << 8 | s[searclen - 1])
					    && !memcmp(s + off, searc + off, 8)) {
						if (!memcmp(s + 1, searc + 1, searclen - 10)) {
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							s += searclen;
							continue;
						}
					}
					*dst++ = *s++;
				}
				memmove(dst, s, end + searclen - s + 1);
				return dst + (end + searclen - s);
			}
			uint8_t shift[256];
			size_t tmp;
			size_t shift1;
			size_t m1 = searclen - 1;
			size_t off = 0;
			memset(shift, 0, sizeof(shift));
			for (int i = 1; i < m1; ++i)
				shift[JSTR_HASH2(searc + i)] = i;
			shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
			shift[JSTR_HASH2(searc + m1)] = m1;
			char *old = dst;
			while (s <= end) {
				do {
					s += m1;
					tmp = shift[JSTR_HASH2(s)];
				} while (!tmp && s <= end);
				s -= tmp;
				if (tmp < m1)
					continue;
				if (m1 < 15 || !memcmp(s + off, searc + off, 8)) {
					if (!memcmp(s, searc, searclen)) {
						memmove(dst, old, s - old);
						dst += (s - old);
						old += (s - old);
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						old += searclen;
						s += searclen;
						continue;
					}
					off = (off >= 8 ? off : m1) - 8;
				}
				s += shift1;
			}
			memmove(dst, old, end + searclen - old + 1);
			return dst + (end + searclen - old);
			break;
		}
		}
	}
RPLC:
	while ((dst = (char *)JSTR_MEMMEM(dst, (s + sz) - dst, searc, searclen))) {
		memmove(dst + rplclen,
			dst + searclen,
			(s + sz + 1) - dst + searclen);
		memcpy(dst, rplc, rplclen);
		dst += rplclen;
		sz += (long long)(rplclen - searclen);
	}
	return s + sz;
}

/*
  Replace N SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
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
	if (unlikely(rplclen == 0)) {
		*sz = jstr_rmn_mem_p(*s, searc, n, *sz, searclen) - *s;
		return;
	}
	char *dst = *s;
	if (unlikely(dst + searclen + searclen >= *s + *sz))
		goto RPLC_GROW;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replacenc_mem(*s, *searc, *rplc, n, *sz);
			return;
			break;
		}
		case 2: {
			const char *src = dst + 1;
			const uint16_t nw = searc[0] << 8 | searc[1];
			uint16_t hw = dst[0] << 8 | dst[1];
			for (; *src; hw = hw << 8 | *src)
				if (hw != nw) {
					*dst++ = *(src++ - 1);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
					if (unlikely(!--n)) {
						memmove(dst, src - 1, *s + *sz - src - 1 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw)
				*dst++ = *(src - 1);
			goto END_UNDER_4;
			break;
		}
		case 3: {
			const char *src = dst + 3;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8;
			for (; *src; hw = (hw | *src++) << 8)
				if (hw != nw) {
					*dst++ = *(src - 3);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
					if (unlikely(!--n)) {
						memmove(dst, src - 3, *s + *sz - src - 3 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw) {
				memmove(dst, src - 3, 3);
				dst += 3;
			}
			goto END_UNDER_4;
			break;
		}
		case 4: {
			const char *src = dst + 4;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8 | dst[3];
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw) {
					*dst++ = *(src - 4);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 3;
					if (unlikely(!--n)) {
						memmove(dst, src - 4, *s + *sz - src - 4 + 1);
						goto END_UNDER_4;
						break;
					}
				}
			if (hw != nw) {
				memmove(dst, src - 4, 4);
				dst += 4;
			}
			goto END_UNDER_4;
			break;
		}
		END_UNDER_4:
			*dst = '\0';
			*sz = dst - *s;
			return;
			break;
		default: {
			char *src = *s;
			const char *const end = *s + *sz - searclen;
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			if (unlikely(searclen > 256)) {
				const size_t off = searclen - 9;
				while (src <= end) {
					if (nw == (dst[0] << 8 | dst[searclen - 1])
					    && !memcmp(src + off, searc + off, 8)) {
						if (!memcmp(src + 1, searc + 1, searclen - 10)) {
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							src += searclen;
							if (unlikely(!--n))
								break;
							continue;
						}
					}
					*dst++ = *src++;
				}
				memmove(dst, src, end + searclen - src + 1);
				*sz = dst + (end + searclen - src) - *s;
				return;
			}
			uint8_t shift[256];
			size_t tmp;
			size_t shift1;
			size_t m1 = searclen - 1;
			size_t off = 0;
			memset(shift, 0, sizeof(shift));
			for (int i = 1; i < m1; ++i)
				shift[JSTR_HASH2(searc + i)] = i;
			shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
			shift[JSTR_HASH2(searc + m1)] = m1;
			char *old = dst;
			while (src <= end) {
				do {
					src += m1;
					tmp = shift[JSTR_HASH2(src)];
				} while (!tmp && src <= end);
				src -= tmp;
				if (tmp < m1)
					continue;
				if (m1 < 15 || !memcmp(src + off, searc + off, 8)) {
					if (!memcmp(src, searc, searclen)) {
						memmove(dst, old, src - old);
						dst += (src - old);
						old += (src - old);
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						old += searclen;
						src += searclen;
						if (unlikely(!--n))
							break;
						continue;
					}
					off = (off >= 8 ? off : m1) - 8;
				}
				src += shift1;
			}
			memmove(dst, old, end + searclen - old + 1);
			*sz = dst + (end + searclen - old) - *s;
			return;
			break;
		}
		}
	}
RPLC_GROW:;
	char *tmp;
	while (n-- && (dst = (char *)JSTR_MEMMEM(dst, (*s + *sz) - dst, searc, searclen))) {
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
		*sz += (long long)(rplclen - searclen);
	}
}

/*
  Replace all SEARCH in S with REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
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
	char *dst = *s;
	if (unlikely(dst + searclen + searclen >= *s + *sz))
		goto RPLC_GROW;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replaceallc_mem(*s, *searc, *rplc, *sz);
			return;
			break;
		}
		case 2: {
			const char *src = dst + 1;
			const uint16_t nw = searc[0] << 8 | searc[1];
			uint16_t hw = dst[0] << 8 | dst[1];
			for (; *src; hw = hw << 8 | *src)
				if (hw != nw) {
					*dst++ = *(src++ - 1);
				} else {
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
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8;
			for (; *src; hw = (hw | *src++) << 8)
				if (hw != nw) {
					*dst++ = *(src - 3);
				} else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 2;
				}
			if (hw != nw) {
				memmove(dst, src - 3, 3);
				dst += 3;
			}
			goto END_UNDER_4;
			break;
		}
		case 4: {
			const char *src = dst + 4;
			const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
			uint32_t hw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8 | dst[3];
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw)
					*dst++ = *(src - 4);
				else {
					memcpy(dst, rplc, rplclen);
					dst += rplclen;
					src += 3;
				}
			if (hw != nw) {
				memmove(dst, src - 4, 4);
				dst += 4;
			}
			goto END_UNDER_4;
			break;
		}
		END_UNDER_4:
			*dst = '\0';
			*sz = dst - *s;
			return;
			break;
		default: {
			const char *const end = *s + *sz - searclen;
			char *src = dst;
			if (unlikely(searclen > 256)) {
				const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
				const size_t off = searclen - 9;
				while (src <= end) {
					if (nw == (dst[0] << 8 | dst[searclen - 1])
					    && !memcmp(src + off, searc + off, 8)) {
						if (!memcmp(src + 1, searc + 1, searclen - 10)) {
							memcpy(dst, rplc, rplclen);
							dst += rplclen;
							src += searclen;
							continue;
						}
					}
					*dst++ = *src++;
				}
				memmove(dst, src, end + searclen - src + 1);
				*sz = (dst + (end + searclen - src)) - *s;
				return;
			}
			uint8_t shift[256];
			size_t tmp;
			size_t shift1;
			size_t m1 = searclen - 1;
			size_t off = 0;
			memset(shift, 0, sizeof(shift));
			for (int i = 1; i < m1; ++i)
				shift[JSTR_HASH2(searc + i)] = i;
			shift1 = m1 - shift[JSTR_HASH2(searc + m1)];
			shift[JSTR_HASH2(searc + m1)] = m1;
			char *old = dst;
			while (src <= end) {
				do {
					src += m1;
					tmp = shift[JSTR_HASH2(src)];
				} while (!tmp && src <= end);
				src -= tmp;
				if (tmp < m1)
					continue;
				if (m1 < 15 || !memcmp(src + off, searc + off, 8)) {
					if (!memcmp(src, searc, searclen)) {
						memmove(dst, old, src - old);
						dst += (src - old);
						old += (src - old);
						memcpy(dst, rplc, rplclen);
						dst += rplclen;
						old += searclen;
						src += searclen;
						continue;
					}
					off = (off >= 8 ? off : m1) - 8;
				}
				src += shift1;
			}
			memmove(dst, old, end + searclen - old + 1);
			*sz = dst + (end + searclen - old) - *s;
			return;
			break;
		}
		}
	}
RPLC_GROW:;
	char *tmp;
	while ((dst = (char *)JSTR_MEMMEM(dst, (*s + *sz) - dst, searc, searclen))) {
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
	}
}

#if JSTR_EXTERN_C && defined(__cplusplus)
}
#endif /* JSTR_EXTERN_C */
#if JSTR_NAMESPACE && !JSTR_IN_NAMESPACE && defined(__cplusplus)
}
#endif /* JSTR_NAMESPACE */

#undef JSTR_HASH2

#endif /* JSTR_H_REPLACE_DEF */
