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
#include "_jstr_memmem.h"
#include "_jstr_string.h"

#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)

#define PRIVATE_JSTR_USE_MEMMEM34 1

#ifdef __cplusplus
namespace jstr {
#endif /* __cpluslus */

enum {
	PRIVATE_JSTR_FLAG_REPLACE_USE_N = 1,
	PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER = 10,
	PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER = 100,
	PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER_FORCE = 1000,
};

#if 0

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
JSTR_INLINE
static char *private_jstr_base_rm_memmem1(const int _flag,
					  char *JSTR_RST s,
					  const int c,
					  size_t n,
					  const size_t sz) JSTR_NOEXCEPT
{
	if (unlikely(_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N))
		if (unlikely(n == 0))
			return s + sz;
	s = (char *)memchr(s, c, sz);
	if (unlikely(!s))
		return s + sz;
	unsigned char *dst = (unsigned char *)s;
	const unsigned char *src = dst + 1;
	const unsigned char *const end = dst + sz;
	for (;; ++src)
		if (*src != c) {
			if (unlikely(!*src))
				break;
			*dst++ = *src;
		} else {
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N) {
				if (unlikely(!--n)) {
					memmove(dst, src, end - src + 1);
					return (char *)dst + (end - src + 1);
				}
			}
		}
	*dst = '\0';
	return (char *)dst;
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rmall_memmem1(char *JSTR_RST s,
					const int c,
					const size_t sz) JSTR_NOEXCEPT
{
	return private_jstr_base_rm_memmem1(0, s, c, 0, sz);
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rmn_memmem1(char *JSTR_RST s,
				      const int c,
				      size_t n,
				      const size_t sz) JSTR_NOEXCEPT
{
	return private_jstr_base_rm_memmem1(PRIVATE_JSTR_FLAG_REPLACE_USE_N, s, c, n, sz);
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_INLINE
static void private_jstr_base_rplc_memmem1(const int _flag,
					   char *JSTR_RST s,
					   const int _searc,
					   const int _rplc,
					   size_t n,
					   const size_t sz) JSTR_NOEXCEPT
{
	unsigned char *dst = (unsigned char *)s;
	unsigned char *old = dst;
	unsigned char *p = dst;
	unsigned char *end = dst + sz;
	while ((_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N && n--) && memchr(s, _searc, n)) {
		if (likely(dst != old))
			memmove(dst, old, p - old);
		dst += (p - old);
		old += (p - old);
		++old;
		++p;
	}
	memmove(dst, old, end - old + 1);

	if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
		if (unlikely(n == 0))
			return;
	s = (char *)memchr(s, _searc, sz);
	if (unlikely(!s))
		return;
	goto MTC;
	for (;; ++s)
		if (*s != _searc) {
			if (unlikely(!*s))
				break;
		} else {
		MTC:
			*s = _rplc;
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
				if (unlikely(!--n))
					break;
		}
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void private_jstr_rplcall_memmem1(char *JSTR_RST s,
					 const int _searc,
					 const int _rplc,
					 const size_t sz) JSTR_NOEXCEPT
{
	private_jstr_base_rplc_memmem1(0, s, _searc, _rplc, 0, sz);
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
static void private_jstr_rplcn_memmem1(char *JSTR_RST s,
				       const int _searc,
				       const int _rplc,
				       size_t n,
				       const size_t sz) JSTR_NOEXCEPT
{
	private_jstr_base_rplc_memmem1(PRIVATE_JSTR_FLAG_REPLACE_USE_N, s, _searc, _rplc, n, sz);
}

#endif

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *private_jstr_base_memmem2(const int _flag,
				       char *JSTR_RST s,
				       const char *JSTR_RST const _searc,
				       const char *JSTR_RST const _rplc,
				       size_t n,
				       const size_t sz,
				       const size_t _rplclen) JSTR_NOEXCEPT
{
	if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
		if (unlikely(n == 0))
			return s + sz;
	if (unlikely(sz < 2))
		return s + sz;
	const unsigned char *src = (unsigned char *)s + 2;
	const unsigned char *const end = (unsigned char *)s + sz;
	unsigned char *dst = (unsigned char *)s;
	const uint16_t nw = _searc[0] << 8 | _searc[1];
	uint16_t sw = s[0] << 8 | s[1];
	for (; src <= end; sw = sw << 8 | *src++)
		if (sw != nw) {
			*dst++ = *(src - 2);
		} else {
			++src;
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER) {
				if (_rplclen == 1) {
					*dst++ = *_rplc;
				} else {
					*dst = *_rplc;
					*(dst + 1) = *(_rplc + 1);
					dst += 2;
				}
				if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N) {
					if (unlikely(!--n)) {
						memmove(dst, src - 1, end - src - 1 + 1);
						return (char *)dst + (end - src - 1);
					}
				}
			}
		}
	if (sw != nw) {
		*dst++ = *(src - 2);
	}
	*dst = '\0';
	return (char *)dst;
}

#if PRIVATE_JSTR_USE_MEMMEM34

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *private_jstr_base_memmem3(const int _flag,
				       char *JSTR_RST s,
				       const char *JSTR_RST const _searc,
				       const char *JSTR_RST const _rplc,
				       size_t n,
				       const size_t sz,
				       const size_t _rplclen) JSTR_NOEXCEPT
{
	if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
		if (unlikely(n == 0))
			return s + sz;
	if (unlikely(sz < 3))
		return s + sz;
	const unsigned char *src = (unsigned char *)s + 3;
	const unsigned char *const end = (unsigned char *)s + sz;
	unsigned char *dst = (unsigned char *)s;
	const uint32_t nw = _searc[0] << 24 | _searc[1] << 16 | _searc[2] << 8;
	uint32_t sw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8;
	for (; src <= end; sw = (sw | *src++) << 8)
		if (sw != nw) {
			*dst++ = *(src - 3);
		} else {
			src += 2;
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER) {
				memcpy(dst, _rplc, _rplclen);
				dst += _rplclen;
			}
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N) {
				if (unlikely(!--n)) {
					memmove(dst, src - 2, end - src - 2 + 1);
					return (char *)dst + (end - src - 2);
				}
			}
		}
	if (sw != nw) {
		*dst = *(src - 3);
		*(dst + 1) = *(src - 2);
		dst += 2;
	}
	*dst = '\0';
	return (char *)dst;
}

JSTR_WARN_UNUSED
JSTR_INLINE
JSTR_RETURNS_NONNULL
static char *private_jstr_base_memmem4(const int _flag,
				       char *JSTR_RST s,
				       const char *JSTR_RST const _searc,
				       const char *JSTR_RST const _rplc,
				       size_t n,
				       const size_t sz,
				       const size_t _rplclen) JSTR_NOEXCEPT
{
	if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
		if (unlikely(n == 0))
			return s + sz;
	if (unlikely(sz < 4))
		return s + sz;
	const unsigned char *const end = (unsigned char *)s + sz;
	const unsigned char *src = (unsigned char *)s + 4;
	unsigned char *dst = (unsigned char *)s;
	const uint32_t nw = _searc[0] << 24 | _searc[1] << 16 | _searc[2] << 8 | _searc[3];
	uint32_t sw = dst[0] << 24 | dst[1] << 16 | dst[2] << 8 | dst[3];
	for (; src <= end; sw = sw << 8 | *src++)
		if (sw != nw) {
			*dst++ = *(src - 4);
		} else {
			src += 3;
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER) {
				memcpy(dst, _rplc, _rplclen);
				dst += _rplclen;
			}
			if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N) {
				if (unlikely(!--n)) {
					memmove(dst, src - 3, end - src - 3 + 1);
					return (char *)dst + (end - src - 3);
				}
			}
		}
	if (sw != nw) {
		*dst = *(src - 4);
		*(dst + 1) = *(src - 3);
		*(dst + 2) = *(src - 2);
		dst += 3;
	}
	*dst = '\0';
	return (char *)dst;
}

#endif

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic ignored "-Wstringop-overread" /* NOLINT */
#	pragma GCC diagnostic push
#endif /* Gnuc || Clang */

JSTR_INLINE
static void private_jstr_memmem5(const int _flag,
				 char **JSTR_RST s,
				 size_t *JSTR_RST const sz,
				 size_t *JSTR_RST const cap,
				 const char *JSTR_RST const _searc,
				 const char *JSTR_RST const _rplc,
				 size_t n,
				 const size_t _searclen,
				 const size_t _rplclen) JSTR_NOEXCEPT
{
#define PRIVATE_JSTR_MEMMEM5(shift_type, ne_iterator_type)                                                                          \
	do {                                                                                                                        \
		unsigned char *src = (unsigned char *)*s;                                                                           \
		unsigned char *dst = src;                                                                                           \
		unsigned char *old = src;                                                                                           \
		const unsigned char *end = src + *sz - _searclen;                                                                   \
		size_t off = 0;                                                                                                     \
		size_t tmp;                                                                                                         \
		size_t shift1;                                                                                                      \
		size_t mtc1 = _searclen - 1;                                                                                        \
		unsigned char *tmp_malloc;                                                                                          \
		shift_type shift[256];                                                                                              \
		memset(shift, 0, sizeof(shift));                                                                                    \
		for (ne_iterator_type i = 1; i < (ne_iterator_type)mtc1; ++i)                                                       \
			shift[JSTR_HASH2(_searc + i)] = i;                                                                          \
		shift1 = mtc1 - shift[JSTR_HASH2(_searc + mtc1)];                                                                   \
		shift[JSTR_HASH2(_searc + mtc1)] = mtc1;                                                                            \
		do {                                                                                                                \
			do {                                                                                                        \
				src += mtc1;                                                                                        \
				tmp = shift[JSTR_HASH2(src)];                                                                       \
			} while (!tmp && src <= end);                                                                               \
			src -= tmp;                                                                                                 \
			if (tmp < mtc1)                                                                                             \
				continue;                                                                                           \
			if (mtc1 < 15 || !memcmp(src + off, _searc + off, 8)) {                                                     \
				if (!memcmp(src, _searc, _searclen)) {                                                              \
					if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER) {                                    \
						if (cap) {                                                                          \
							if (_rplclen <= _searclen || *cap > *sz + _rplclen - _searclen) {           \
								memmove(src + _rplclen,                                             \
									src + _searclen,                                            \
									(end + _searclen + 1) - src + _searclen);                   \
								memcpy(src, _rplc, _rplclen);                                       \
								src += _rplclen;                                                    \
							} else {                                                                    \
								JSTR_GROW(*cap, *sz + _rplclen);                                    \
								tmp_malloc = (unsigned char *)malloc(*cap);                         \
								JSTR_MALLOC_ERR(tmp_malloc, return);                                \
								memcpy(tmp_malloc, *s, src - *(unsigned char **)s);                 \
								memcpy(tmp_malloc + (src - *(unsigned char **)s), _rplc, _rplclen); \
								memcpy(tmp_malloc + (src - *(unsigned char **)s) + _rplclen,        \
								       src + _searclen,                                             \
								       (end + _searclen + 1) - (src + _searclen));                  \
								src = tmp_malloc + (src - *(unsigned char **)s) + _rplclen;         \
								free(*s);                                                           \
								*s = (char *)tmp_malloc;                                            \
							}                                                                           \
							*sz += (_rplclen - _searclen);                                              \
							end = *(unsigned char **)s + *sz - _searclen;                               \
						}                                                                                   \
					} else if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER_FORCE) {                       \
						memmove(src + _rplclen,                                                             \
							src + _searclen,                                                            \
							(end + _searclen + 1) - src + _searclen);                                   \
						memcpy(src, _rplc, _rplclen);                                                       \
						src += _rplclen;                                                                    \
						*sz += (_rplclen - _searclen);                                                      \
						end = *(unsigned char **)s + *sz - _searclen;                                       \
					} else if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER) {                              \
						if (likely(dst != old))                                                             \
							memmove(dst, old, src - old);                                               \
						dst += (src - old);                                                                 \
						old += (src - old);                                                                 \
						memcpy(dst, _rplc, _rplclen);                                                       \
						dst += _rplclen;                                                                    \
						old += _searclen;                                                                   \
						src += _searclen;                                                                   \
					} else {                                                                                    \
						if (likely(dst != old))                                                             \
							memmove(dst, old, src - old);                                               \
						dst += (src - old);                                                                 \
						old += (src - old);                                                                 \
						old += _searclen;                                                                   \
						src += _searclen;                                                                   \
					}                                                                                           \
					if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)                                                \
						if (unlikely(!--n))                                                                 \
							break;                                                                      \
					continue;                                                                                   \
				}                                                                                                   \
				off = (off >= 8 ? off : mtc1) - 8;                                                                  \
			}                                                                                                           \
			src += shift1;                                                                                              \
		} while (src <= end);                                                                                               \
		if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER) {                                                            \
		} else if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER_FORCE) {                                               \
		} else {                                                                                                            \
			if (unlikely(dst == old))                                                                                   \
				return;                                                                                             \
			memmove(dst, old, end + _searclen - old + 1);                                                               \
			*sz = (char *)dst + (end + _searclen - old) - *s;                                                           \
		}                                                                                                                   \
	} while (0)
	if (_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N)
		if (unlikely(n == 0))
			return;
	if (unlikely(*sz < _searclen))
		return;
	if (unlikely(_searclen == 1)) {
		unsigned char *p = *(unsigned char **)s;
		const unsigned char *const end = p + *sz;
		while ((_flag & PRIVATE_JSTR_FLAG_REPLACE_USE_N && n--)
		       && (p = (unsigned char *)memchr(p, *_searc, end - p))) {
			memmove(p + 1, p, (*(unsigned char **)s + *sz) - p + 1);
			memcpy(p, _searc, _searclen);
			p += _searclen;
			*sz += (_searclen - 1);
		}
		return;
	}
	if (unlikely(_searclen > 256)) {
		PRIVATE_JSTR_MEMMEM5(size_t, size_t);
		return;
	}
	PRIVATE_JSTR_MEMMEM5(uint8_t, int);
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void private_jstr_rplcn_grow(char **JSTR_RST s,
				    size_t *JSTR_RST sz,
				    size_t *JSTR_RST cap,
				    const char *JSTR_RST const _searc,
				    const char *JSTR_RST const _rplc,
				    const size_t n,
				    const size_t _searclen,
				    const size_t _rplclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER | PRIVATE_JSTR_FLAG_REPLACE_USE_N, s, sz, cap, _searc, _rplc, n, _searclen, _rplclen);
}

JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static void private_jstr_rplcall_grow(char **JSTR_RST s,
				      size_t *JSTR_RST sz,
				      size_t *JSTR_RST cap,
				      const char *JSTR_RST const _searc,
				      const char *JSTR_RST const _rplc,
				      const size_t _searclen,
				      const size_t _rplclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER, s, sz, cap, _searc, _rplc, 0, _searclen, _rplclen);
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rplcn_f(char *s,
				  const char *JSTR_RST const _searc,
				  const char *JSTR_RST const _rplc,
				  const size_t n,
				  size_t sz,
				  const size_t _searclen,
				  const size_t _rplclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER_FORCE | PRIVATE_JSTR_FLAG_REPLACE_USE_N, &s, &sz, NULL, _searc, _rplc, n, _searclen, _rplclen);
	return s + sz;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_MAYBE_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rplcall_f(char *s,
				    const char *JSTR_RST const _searc,
				    const char *JSTR_RST const _rplc,
				    size_t sz,
				    const size_t _searclen,
				    const size_t _rplclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_HIGHER_FORCE, &s, &sz, NULL, _searc, _rplc, 0, _searclen, _rplclen);
	return s + sz;
}

#if defined(__GNUC__) || defined(__clang__)
#	pragma GCC diagnostic pop
#endif /* Gnuc || Clang */

#define DEFINE_JSTR_MEMMEMN_FUNCS(N)                                                                                                                               \
	JSTR_WARN_UNUSED                                                                                                                                           \
	JSTR_MAYBE_UNUSED                                                                                                                                          \
	JSTR_NONNULL_ALL                                                                                                                                           \
	JSTR_RETURNS_NONNULL                                                                                                                                       \
	static char *private_jstr_rplcall_memmem##N(char *JSTR_RST s,                                                                                              \
						    const char *JSTR_RST const _searc,                                                                             \
						    const char *JSTR_RST const _rplc,                                                                              \
						    const size_t sz,                                                                                               \
						    const size_t _rplclen)                                                                                         \
	{                                                                                                                                                          \
		return private_jstr_base_memmem##N(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER, s, _searc, _rplc, 0, sz, _rplclen);                                   \
	}                                                                                                                                                          \
                                                                                                                                                                   \
	JSTR_WARN_UNUSED                                                                                                                                           \
	JSTR_MAYBE_UNUSED                                                                                                                                          \
	JSTR_NONNULL_ALL                                                                                                                                           \
	JSTR_RETURNS_NONNULL                                                                                                                                       \
	static char *private_jstr_rplcn_memmem##N(char *JSTR_RST s,                                                                                                \
						  const char *JSTR_RST const _searc,                                                                               \
						  const char *JSTR_RST const _rplc,                                                                                \
						  const size_t n,                                                                                                  \
						  const size_t sz,                                                                                                 \
						  const size_t _rplclen)                                                                                           \
	{                                                                                                                                                          \
		return private_jstr_base_memmem##N(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER | PRIVATE_JSTR_FLAG_REPLACE_USE_N, s, _searc, _rplc, n, sz, _rplclen); \
	}                                                                                                                                                          \
                                                                                                                                                                   \
	JSTR_WARN_UNUSED                                                                                                                                           \
	JSTR_MAYBE_UNUSED                                                                                                                                          \
	JSTR_NONNULL_ALL                                                                                                                                           \
	JSTR_RETURNS_NONNULL                                                                                                                                       \
	static char *private_jstr_rmall_memmem##N(char *JSTR_RST s,                                                                                                \
						  const char *JSTR_RST const _searc,                                                                               \
						  const size_t sz)                                                                                                 \
	{                                                                                                                                                          \
		return private_jstr_base_memmem##N(0, s, _searc, NULL, 0, sz, 0);                                                                                  \
	}                                                                                                                                                          \
                                                                                                                                                                   \
	JSTR_WARN_UNUSED                                                                                                                                           \
	JSTR_MAYBE_UNUSED                                                                                                                                          \
	JSTR_NONNULL_ALL                                                                                                                                           \
	JSTR_RETURNS_NONNULL                                                                                                                                       \
	static char *private_jstr_rmn_memmem##N(char *JSTR_RST s,                                                                                                  \
						const char *JSTR_RST const _searc,                                                                                 \
						const size_t n,                                                                                                    \
						const size_t sz)                                                                                                   \
	{                                                                                                                                                          \
		return private_jstr_base_memmem##N(PRIVATE_JSTR_FLAG_REPLACE_USE_N, s, _searc, NULL, n, sz, 0);                                                    \
	}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rplcall_memmem5(char *s,
					  const char *JSTR_RST const _searc,
					  const char *JSTR_RST const _rplc,
					  size_t sz,
					  const size_t _searclen,
					  const size_t _rplclen) JSTR_NOEXCEPT
{
	jstr_memmem_table t;
	jstr_memmem_comp_mem(&t, _searc, _searclen);
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER, &s, &sz, NULL, _searc, _rplc, 0, _searclen, _rplclen);
	return s + sz;
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rplcn_memmem5(char *s,
					const char *JSTR_RST const _searc,
					const char *JSTR_RST const _rplc,
					const size_t n,
					size_t sz,
					const size_t _searclen,
					const size_t _rplclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_RPLC_LOWER | PRIVATE_JSTR_FLAG_REPLACE_USE_N, &s, &sz, NULL, _searc, _rplc, n, _searclen, _rplclen);
	return s + sz;
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rmall_memmem5(char *s,
					const char *JSTR_RST const _searc,
					size_t sz,
					const size_t _searclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(0, &s, &sz, NULL, _searc, NULL, 0, _searclen, 0);
	return s + sz;
}

JSTR_MAYBE_UNUSED
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_RETURNS_NONNULL
static char *private_jstr_rmn_memmem5(char *s,
				      const char *JSTR_RST const _searc,
				      const size_t n,
				      size_t sz,
				      const size_t _searclen) JSTR_NOEXCEPT
{
	private_jstr_memmem5(PRIVATE_JSTR_FLAG_REPLACE_USE_N, &s, &sz, NULL, _searc, NULL, n, _searclen, 0);
	return s + sz;
}

DEFINE_JSTR_MEMMEMN_FUNCS(2);
#if PRIVATE_JSTR_USE_MEMMEM34
DEFINE_JSTR_MEMMEMN_FUNCS(3);
DEFINE_JSTR_MEMMEMN_FUNCS(4);
#endif

#ifdef __cplusplus
}
#endif /* __cpluslus */

#undef JSTR_HASH2
#undef PRIVATE_JSTR_MEMMEM5

#endif /* _JSTR_MEMMEM_DEF_H */
