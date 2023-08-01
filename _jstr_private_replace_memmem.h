#ifndef _JSTR_MEMMEM_DEF_H
#define _JSTR_MEMMEM_DEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "_jstr_builder.h"
#include "_jstr_macros.h"
#include "_jstr_string.h"

#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % sizeof(shift))

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static void private_jstr_replacen_grow(char **JSTR_RST s,
				       size_t *JSTR_RST sz,
				       size_t *JSTR_RST cap,
				       const char *JSTR_RST const searc,
				       const char *JSTR_RST const rplc,
				       size_t n,
				       const size_t searclen,
				       const size_t rplclen) JSTR_NOEXCEPT
{
	char *tmp;
	char *dst = *s;
	while (n-- && (dst = (char *)JSTR_MEMMEM(dst, (dst + *sz) - dst, searc, searclen))) {
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
			memcpy(tmp, dst, dst - *s);
			memcpy(tmp + (dst - *s), rplc, rplclen);
			memcpy(tmp + (dst - *s) + rplclen,
			       dst + rplclen,
			       (*s + *sz + 1) - (dst + rplclen));
			dst = tmp + (dst - dst) + rplclen;
			free(*s);
			*s = tmp;
		}
		*sz += (rplclen - searclen);
	}
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static void private_jstr_replaceall_grow(char **JSTR_RST s,
					 size_t *JSTR_RST sz,
					 size_t *JSTR_RST cap,
					 const char *JSTR_RST const searc,
					 const char *JSTR_RST const rplc,
					 const size_t searclen,
					 const size_t rplclen) JSTR_NOEXCEPT
{
	char *tmp;
	char *dst = *s;
	while ((dst = (char *)JSTR_MEMMEM(dst, (dst + *sz) - dst, searc, searclen))) {
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
			memcpy(tmp, dst, dst - *s);
			memcpy(tmp + (dst - *s), rplc, rplclen);
			memcpy(tmp + (dst - *s) + rplclen,
			       dst + rplclen,
			       (*s + *sz + 1) - (dst + rplclen));
			dst = tmp + (dst - dst) + rplclen;
			free(*s);
			*s = tmp;
		}
		*sz += (rplclen - searclen);
	}
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *private_jstr_replacen_move(char *JSTR_RST s,
					const char *JSTR_RST const searc,
					const char *JSTR_RST const rplc,
					size_t n,
					size_t sz,
					const size_t searclen,
					const size_t rplclen) JSTR_NOEXCEPT
{
	while (n-- && (s = (char *)JSTR_MEMMEM(s, (s + sz) - s, searc, searclen))) {
		memmove(s + rplclen,
			s + searclen,
			(s + sz + 1) - s + searclen);
		memcpy(s, rplc, rplclen);
		s += rplclen;
		sz += (rplclen - searclen);
	}
	return s + sz;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_INLINE
static char *private_jstr_replaceall_move(char *JSTR_RST s,
					  const char *JSTR_RST const searc,
					  const char *JSTR_RST const rplc,
					  size_t sz,
					  const size_t searclen,
					  const size_t rplclen) JSTR_NOEXCEPT
{
	while ((s = (char *)JSTR_MEMMEM(s, (s + sz) - s, searc, searclen))) {
		memmove(s + rplclen,
			s + searclen,
			(s + sz + 1) - s + searclen);
		memcpy(s, rplc, rplclen);
		s += rplclen;
		sz += (rplclen - searclen);
	}
	return s + sz;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_rmall_memmem1(char *JSTR_RST s,
					const int c,
					const size_t sz) JSTR_NOEXCEPT
{
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

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_rmn_memmem1(char *JSTR_RST s,
				      const int c,
				      size_t n,
				      const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
		return s + sz;
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
				while (*s);
				break;
			}
		}
	*s = '\0';
	return s;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_replaceall_memmem1(char *JSTR_RST s,
					     const int searc,
					     const int rplc,
					     const size_t sz) JSTR_NOEXCEPT
{
	s = (char *)memchr(s, searc, sz);
	if (unlikely(!s))
		return s + sz;
	goto MTC;
	for (;; ++s)
		if (*s != searc) {
			if (unlikely(!*s))
				break;
		} else {
		MTC:
			*s = rplc;
		}
	return s;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void private_jstr_replacenc_memmem1(char *JSTR_RST s,
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
		if (*s != searc) {
			if (unlikely(!*s))
				break;
		} else {
		MTC:
			*s = rplc;
			if (unlikely(!--n))
				break;
		}
}

JSTR_WARN_UNUSED
JSTR_INLINE
static char *private_jstr_memmem2(const int use_remove,
				  const int use_replace,
				  const int use_n,
				  char *JSTR_RST s,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const rplc,
				  size_t n,
				  size_t sz,
				  const size_t rplclen) JSTR_NOEXCEPT
{
	const char *const end = s + sz;
	const char *src = s + 2;
	const uint16_t nw = searc[0] << 8 | searc[1];
	uint16_t hw = s[0] << 8 | s[1];
	for (sz -= 2; sz--; hw = hw << 8 | *src++)
		if (hw != nw) {
			*s++ = *(src - 2);
		} else {
			++src;
			if (use_replace) {
				memcpy(s, rplc, rplclen);
				s += rplclen;
			} else if (use_remove) {
			}
			if (use_n) {
				if (unlikely(!--n)) {
					memmove(s, src - 2, end - src - 2 + 1);
					memmove(s, src - 2, sz + 1);
					return s + sz + 1;
				}
			}
		}
	*s = '\0';
	return s;
}

JSTR_WARN_UNUSED
JSTR_INLINE
static char *private_jstr_memmem3(const int use_remove,
				  const int use_replace,
				  const int use_n,
				  char *JSTR_RST s,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const rplc,
				  size_t n,
				  const size_t sz,
				  const size_t rplclen) JSTR_NOEXCEPT
{
	const char *const end = s + sz;
	const char *src = s + 3;
	const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8;
	uint32_t hw = s[0] << 24 | s[1] << 16 | s[2] << 8;
	for (; *s; hw = (hw | *s++) << 8)
		if (hw != nw) {
			*s++ = *(src - 3);
		} else {
			src += 2;
			if (use_replace) {
				memcpy(s, rplc, rplclen);
				s += rplclen;
				if (use_n) {
					if (unlikely(!--n)) {
						memmove(s, src - 3, end - src - 3 + 1);
						return s + (end - src - 3);
					}
				}
			} else if (use_remove) {
				if (unlikely(!--n)) {
					memmove(s, src - 3, end - src - 3 + 1);
					return s + (end - src - 3);
				}
			}
		}
	if (hw != nw) {
		memmove(s, src - 3, 3);
		s += 3;
	}
	*s = '\0';
	return s;
}

JSTR_WARN_UNUSED
JSTR_INLINE
static char *private_jstr_memmem4(const int use_remove,
				  const int use_replace,
				  const int use_n,
				  char *JSTR_RST s,
				  const char *JSTR_RST const searc,
				  const char *JSTR_RST const rplc,
				  size_t n,
				  const size_t sz,
				  const size_t rplclen) JSTR_NOEXCEPT
{
	const char *const end = s + sz;
	const char *src = s + 4;
	const uint32_t nw = searc[0] << 24 | searc[1] << 16 | searc[2] << 8 | searc[3];
	uint32_t hw = s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3];
	for (; *s; hw = hw << 8 | *s++)
		if (hw != nw) {
			*s++ = *(src - 4);
		} else {
			src += 4;
			if (use_replace) {
				memcpy(s, rplc, rplclen);
				s += rplclen;
				if (use_n) {
					if (unlikely(!--n)) {
						memmove(s, src - 4, end - src - 4 + 1);
						return s + (end - src - 4);
					}
				}
			} else if (use_remove) {
				if (use_n) {
					if (unlikely(!--n)) {
						memmove(s, src - 4, end - src - 4 + 1);
						return s + (end - src - 4);
					}
				}
			}
		}
	if (hw != nw) {
		memmove(s, src - 4, 4);
		s += 4;
	}
	*s = '\0';
	return s;
}

JSTR_INLINE
JSTR_WARN_UNUSED
static char *private_jstr_memmem5(const int use_remove,
				  const int use_replace,
				  const int use_n,
				  char *JSTR_RST s,
				  const char *JSTR_RST searc,
				  const char *JSTR_RST rplc,
				  size_t n,
				  const size_t sz,
				  size_t searclen,
				  size_t rplclen) JSTR_NOEXCEPT
{
#define JSTR_PRIVATE_JSTR_MEMMEM5_SHIFTS                                    \
	do {                                                                \
		memset(shift, 0, sizeof(shift));                            \
		for (size_t i = 1; i < m1; ++i)                             \
			shift[JSTR_HASH2(searc + i)] = i;                   \
		shift1 = m1 - shift[JSTR_HASH2(searc + m1)];                \
		shift[JSTR_HASH2(searc + m1)] = m1;                         \
		char *old = dst;                                            \
		while (s <= end) {                                          \
			do {                                                \
				s += m1;                                    \
				tmp = shift[JSTR_HASH2(s)];                 \
			} while (!tmp && s <= end);                         \
			s -= tmp;                                           \
			if (tmp < m1)                                       \
				continue;                                   \
			if (m1 < 15 || !memcmp(s + off, searc + off, 8)) {  \
				if (!memcmp(s, searc, searclen)) {          \
					if (use_replace) {                  \
						memmove(dst, old, s - old); \
						dst += (s - old);           \
						old += (s - old);           \
						memcpy(dst, rplc, rplclen); \
						dst += rplclen;             \
					} else if (use_remove) {            \
						memmove(dst, old, s - old); \
						dst += (s - old);           \
						old += (s - old);           \
					}                                   \
					old += searclen;                    \
					s += searclen;                      \
					if (use_n) {                        \
						if (unlikely(!--n))         \
							break;              \
					}                                   \
					continue;                           \
				}                                           \
				off = (off >= 8 ? off : m1) - 8;            \
			}                                                   \
			s += shift1;                                        \
		}                                                           \
		memmove(dst, old, end + searclen - old + 1);                \
		return dst + (end + searclen - old);                        \
	} while (0)
	char *dst = s;
	const char *const end = s + sz - searclen;
	size_t off = 0;
	size_t tmp;
	size_t shift1;
	size_t m1 = searclen - 1;
	if (unlikely(searclen > 256)) {
		size_t shift[256];
		JSTR_PRIVATE_JSTR_MEMMEM5_SHIFTS;
	}
	uint8_t shift[256];
	JSTR_PRIVATE_JSTR_MEMMEM5_SHIFTS;
}

/* use_remove, use_replace, use_n */
#define DEFINE_JSTR_MEMMEMN_FUNCS(N)                                                     \
	JSTR_WARN_UNUSED                                                                 \
	JSTR_INLINE                                                                      \
	JSTR_NONNULL_ALL                                                                 \
	static char *private_jstr_replaceall_memmem##N(char *JSTR_RST s,                 \
						       const char *JSTR_RST const searc, \
						       const char *JSTR_RST const rplc,  \
						       const size_t sz,                  \
						       const size_t rplclen)             \
	{                                                                                \
		return private_jstr_memmem##N(0, 1, 0, s, searc, rplc, 0, sz, rplclen);  \
	}                                                                                \
                                                                                         \
	JSTR_WARN_UNUSED                                                                 \
	JSTR_INLINE                                                                      \
	JSTR_NONNULL_ALL                                                                 \
	static char *private_jstr_replacen_memmem##N(char *JSTR_RST s,                   \
						     const char *JSTR_RST const searc,   \
						     const char *JSTR_RST const rplc,    \
						     const size_t n,                     \
						     const size_t sz,                    \
						     const size_t rplclen)               \
	{                                                                                \
		return private_jstr_memmem##N(0, 1, 1, s, searc, rplc, n, sz, rplclen);  \
	}                                                                                \
                                                                                         \
	JSTR_WARN_UNUSED                                                                 \
	JSTR_INLINE                                                                      \
	JSTR_NONNULL_ALL                                                                 \
	static char *private_jstr_rmall_memmem##N(char *JSTR_RST s,                      \
						  const char *JSTR_RST const searc,      \
						  const size_t sz)                       \
	{                                                                                \
		return private_jstr_memmem##N(1, 0, 0, s, searc, NULL, 0, sz, 0);        \
	}                                                                                \
                                                                                         \
	JSTR_WARN_UNUSED                                                                 \
	JSTR_INLINE                                                                      \
	JSTR_NONNULL_ALL                                                                 \
	static char *private_jstr_rmn_memmem##N(char *JSTR_RST s,                        \
						const char *JSTR_RST const searc,        \
						const size_t n,                          \
						const size_t sz)                         \
	{                                                                                \
		return private_jstr_memmem##N(1, 0, 1, s, searc, NULL, n, sz, 0);        \
	}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_replaceall_memmem5(char *JSTR_RST s,
					     const char *JSTR_RST searc,
					     const char *JSTR_RST rplc,
					     const size_t sz,
					     const size_t searclen,
					     const size_t rplclen) JSTR_NOEXCEPT
{
	return private_jstr_memmem5(0, 1, 0, s, searc, rplc, 0, sz, searclen, rplclen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_replacen_memmem5(char *JSTR_RST s,
					   const char *JSTR_RST searc,
					   const char *JSTR_RST rplc,
					   const size_t n,
					   const size_t sz,
					   const size_t searclen,
					   const size_t rplclen) JSTR_NOEXCEPT
{
	return private_jstr_memmem5(0, 1, 1, s, searc, rplc, n, sz, searclen, rplclen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_rmall_memmem5(char *JSTR_RST s,
					const char *JSTR_RST searc,
					const size_t sz,
					const size_t searclen) JSTR_NOEXCEPT
{
	return private_jstr_memmem5(1, 0, 0, s, searc, NULL, 0, sz, searclen, 0);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static char *private_jstr_rmn_memmem5(char *JSTR_RST s,
				      const char *JSTR_RST searc,
				      const size_t n,
				      const size_t sz,
				      const size_t searclen) JSTR_NOEXCEPT
{
	return private_jstr_memmem5(1, 0, 1, s, searc, NULL, n, sz, searclen, 0);
}

DEFINE_JSTR_MEMMEMN_FUNCS(2);
DEFINE_JSTR_MEMMEMN_FUNCS(3);
DEFINE_JSTR_MEMMEMN_FUNCS(4);

#undef JSTR_HASH2
#undef JSTR_MEMMEM_XSTR
#undef JSTR_MEMMEM_STR

#endif /* _JSTR_MEMMEM_DEF_H */
