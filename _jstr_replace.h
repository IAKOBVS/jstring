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
	char *const p = (char *)jstr_memmem(s, sz, searc, searclen);
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
	char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
	if (unlikely(!dst))
		return s + sz;
	if (unlikely(dst + searclen + searclen >= s + sz)) {
		*dst = '\0';
		return dst;
	}
	switch (searclen) {
	case 0:
		return dst;
		break;
	case 1: {
		s = dst;
		goto MTC1;
		for (;; ++s)
			if (*s != *searc) {
				if (unlikely(!*s))
					break;
				*dst++ = *s;
			} else {
			MTC1:
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
		goto MTC2;
		for (; *s; sw = sw << 8 | *s)
			if (sw != nw) {
				*dst++ = *(s++ - 1);
			} else {
			MTC2:
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
		goto MTC3;
		for (; *s; sw = (sw | *s++) << 8)
			if (sw != nw) {
				*dst++ = *(s - 3);
			} else {
			MTC3:
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
		goto MTC4;
		for (; *s; sw = sw << 8 | *s++)
			if (sw != nw) {
				*dst++ = *(s - 4);
			} else {
			MTC4:
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
	if (unlikely(!dst))
		return s + sz;
	if (unlikely(dst + searclen + searclen >= s + sz)) {
		*dst = '\0';
		return dst;
	}
	switch (searclen) {
	case 0:
		return dst;
		break;
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
		goto MTC4;
		for (; *s; hw = hw << 8 | *s++)
			if (hw != nw)
				*dst++ = *(s - 4);
			else
			MTC4:
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
		char *mtc = (char *)jstr_memmem(s, sz, searc, searclen);
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
		char *mtc = (char *)jstr_memmem(*s, *sz, searc, searclen);
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
	if (unlikely(rplclen == 0))
		return jstr_rmn_mem_p(s, searc, n, sz, searclen);
	char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
	if (unlikely(!dst))
		return s + sz;
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
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const char *const end = s + sz - searclen;
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
						if (unlikely(!--n))
							break;
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
							if (unlikely(!--n))
								break;
							continue;
						}
					}
					*dst++ = *src++;
				}
			}
			memmove(dst, src, end + searclen - src + 1);
			return dst + (end + searclen - src);
			break;
		}
		}
	}
	do {
		memmove(dst + rplclen,
			dst + searclen,
			(s + sz + 1) - dst + searclen);
		memcpy(dst, rplc, rplclen);
		dst += rplclen;
		sz += (long long)(rplclen - searclen);
	} while (--n && (dst = (char *)jstr_memmem(dst, (s + sz) - dst, searc, searclen)));
	return s + sz;
}

/*
  Replace all SEARCH in S with REPLACE.
  Assumes that S have enough space for REPLACE.
*/
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
static char *jstr_replaceall_mem_p_f(char *JSTR_RST const s,
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
	char *dst = (char *)jstr_memmem(s, sz, searc, searclen);
	if (unlikely(!dst))
		return s + sz;
	if (rplclen <= searclen) {
		switch (searclen) {
		case 1: {
			jstr_replaceallc_mem(s, *searc, *rplc, sz);
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
			const uint16_t nw = searc[0] << 8 | searc[searclen - 1];
			const char *const end = s + sz - searclen;
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
			return dst + (end + searclen - src);
			break;
		}
		}
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
	char *dst = (char *)jstr_memmem(*s, *sz, searc, searclen);
	if (unlikely(!dst))
		return;
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
			uint32_t hw = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
			goto MTC4;
			for (; *src; hw = hw << 8 | *src++)
				if (hw != nw) {
					*dst++ = *(src - 4);
				} else {
				MTC4:
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
						if (unlikely(!--n))
							break;
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
							if (unlikely(!--n))
								break;
							continue;
						}
					}
					*dst++ = *src++;
				}
			}
			memmove(dst, src, end + searclen - src + 1);
			*sz = (dst + (end + searclen - src)) - *s;
			return;
			break;
		}
		}
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
		*sz += (long long)(rplclen - searclen);
	} while (n-- && (dst = (char *)jstr_memmem(dst, (*s + *sz) - dst, searc, searclen)));
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
	char *dst = (char *)jstr_memmem(*s, *sz, searc, searclen);
	if (unlikely(!dst))
		return;
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
			return;
			break;
		}
		}
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

#endif /* JSTR_H_REPLACE_DEF */
