/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE. */

#ifndef PJSTR_AVX2_H
#define PJSTR_AVX2_H

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "jstr-ptr-arith.h"
#include "jstr-rarebyte.h"
#include "jstr-stdstring.h"

#ifdef __AVX2__
#	include <immintrin.h>
typedef __m256i JSTR_VEC;
#	define JSTR_VEC_SIZE    sizeof(JSTR_VEC)
#	define MASK             uint32_t
#	define LOAD(x)          _mm256_load_si256(x)
#	define LOADU(x)         _mm256_loadu_si256(x)
#	define STORE(dst, src)  _mm256_store_si256(dst, src)
#	define STOREU(dst, src) _mm256_storeu_si256(dst, src)
#	define MOVEMASK8(x)     _mm256_movemask_epi8(x)
#	define CMPEQ8(x, y)     _mm256_cmpeq_epi8(x, y)
#	define SETZERO(x)       _mm256_setzero_si256(x)
#	define SET18(x)         _mm256_set1_epi8(x)
#	define POPCNT(x)        _mm_popcnt_u32(x)
#	define TZCNT(x)         _tzcnt_u32(x)
#	define BLSR(x)          _blsr_u32(x)
#	define LZCNT(x)         _lzcnt_u32(x)
#else
#	include <emmintrin.h>
typedef __m128i JSTR_VEC;
#	define JSTR_VEC_SIZE    sizeof(JSTR_VEC)
#	define MASK             uint16_t
#	define LOAD(x)          _mm_load_si128(x)
#	define LOADU(x)         _mm_loadu_si128(x)
#	define STORE(dst, src)  _mm_store_si128(dst, src)
#	define STOREU(dst, src) _mm_storeu_si128(dst, src)
#	define MOVEMASK8(x)     _mm_movemask_epi8(x)
#	define CMPEQ8(x, y)     _mm_cmpeq_epi8(x, y)
#	define SETZERO(x)       _mm_setzero_si128(x)
#	define SET18(x)         _mm_set1_epi8(x)
#	if JSTR_HAS_BUILTIN(__builtin_popcount)
#		define POPCNT(x) __builtin_popcount(x)
#	endif
#	if JSTR_HAS_BUILTIN(__builtin_ia32_tzcnt_u32)
#		define TZCNT(x) __builtin_ia32_tzcnt_u16(x)
#	endif
#endif

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
pjstr_stpcpy_simd_aligned(char *JSTR_RESTRICT dst,
                          const char *JSTR_RESTRICT src)
{
	JSTR_VEC sv;
	const JSTR_VEC zv = SETZERO();
	uint32_t zm;
	for (;; src += JSTR_VEC_SIZE, dst += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)src);
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		if (zm)
			break;
		STORE((JSTR_VEC *)dst, sv);
	}
	while ((*dst++ = *src++))
		;
	return dst - 1;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
pjstr_stpcpy_simd_unaligned_src(char *JSTR_RESTRICT dst,
                                const char *JSTR_RESTRICT src)
{
	JSTR_VEC sv;
	const JSTR_VEC zv = SETZERO();
	uint32_t zm;
	for (;; src += JSTR_VEC_SIZE, dst += JSTR_VEC_SIZE) {
		sv = LOADU((const JSTR_VEC *)src);
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		if (zm)
			break;
		STORE((JSTR_VEC *)dst, sv);
	}
	while ((*dst++ = *src++))
		;
	return dst - 1;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
static char *
pjstr_stpcpy_simd(char *JSTR_RESTRICT dst,
                  const char *JSTR_RESTRICT src)
{
	while (JSTR_PTR_IS_NOT_ALIGNED(dst, JSTR_VEC_SIZE))
		if (jstr_unlikely((*dst++ = *src++) == '\0'))
			return dst - 1;
	if (JSTR_PTR_IS_ALIGNED(src, JSTR_VEC_SIZE))
		return pjstr_stpcpy_simd_aligned(dst, src);
	return pjstr_stpcpy_simd_unaligned_src(dst, src);
}

#ifndef TZCNT

#	define JSTR_HAVENT_STRNCASECHR_SIMD   1
#	define JSTR_HAVENT_MEMCASECHR_SIMD    1
#	define JSTR_HAVENT_STRCHRNUL_SIMD     1
#	define JSTR_HAVENT_STRNCHR_SIMD       1
#	define JSTR_HAVENT_STRCASECHR_SIMD    1
#	define JSTR_HAVENT_STRCASECHRNUL_SIMD 1
#	define JSTR_HAVENT_STRCHRUL_SIMD      1

#else

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strncasechr_simd(const char *s,
                       int c,
                       size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE); ++p) {
		if (jstr_unlikely(n-- == 0) || jstr_unlikely(*p == '\0'))
			return NULL;
		if (jstr_unlikely(jstr_tolower(*p) == c))
			return (char *)p;
	}
	const unsigned char *const end = p + n;
	uint32_t hm0, hm1, m, zm;
	JSTR_VEC sv;
	const JSTR_VEC cv0 = SET18((char)c);
	const JSTR_VEC cv1 = SET18((char)jstr_toupper(c));
	const JSTR_VEC zv = SETZERO();
	for (;; p += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)p);
		hm0 = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv0));
		hm1 = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv1));
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		m = hm0 | hm1 | zm;
		if (m | (p >= end))
			break;
	}
	if (p >= end)
		return NULL;
	const uint32_t i = TZCNT(m);
	return p + i < end ? (char *)p + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strnchr_simd(const char *s,
                   int c,
                   size_t n)
{
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, JSTR_VEC_SIZE); ++s) {
		if (jstr_unlikely(n-- == 0) || jstr_unlikely(*s == '\0'))
			return NULL;
		if (jstr_unlikely(*s == (char)c))
			return (char *)s;
	}
	const char *const end = s + n;
	uint32_t hm, m, zm;
	JSTR_VEC sv;
	const JSTR_VEC cv = SET18((char)c);
	const JSTR_VEC zv = SETZERO();
	for (;; s += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)s);
		hm = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		m = hm | zm;
		if (m | (s >= end))
			break;
	}
	if (s >= end)
		return NULL;
	const uint32_t i = TZCNT(m);
	return s + i < end ? (char *)s + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strchrnul_simd(const char *s,
                     int c)
{
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, JSTR_VEC_SIZE); ++s)
		if (jstr_unlikely(*s == '\0') || *s == (char)c)
			return (char *)s;
	uint32_t m, m1, zm;
	JSTR_VEC sv;
	const JSTR_VEC cv = SET18((char)c);
	const JSTR_VEC zv = SETZERO();
	for (;; s += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)s);
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		m1 = m | zm;
		if (m1)
			break;
	}
	return (char *)s + TZCNT(m1);
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strcasechrnul_simd(const char *s,
                         int c)
{
	if (!jstr_isalpha(c)) {
#	if JSTR_HAVE_STRCHRNUL
		return (char *)strchrnul(s, c);
#	elif !JSTR_HAVENT_STRCHRNUL_SIMD
		return pjstr_strchrnul_simd(s, c);
#	else
		char *p = strchr(s, c);
		return *p == (char)c ? p : (char *)s + strlen(s);
#	endif
	}
	const unsigned char *p = (const unsigned char *)s;
	c = jstr_tolower(c);
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE); ++p)
		if (jstr_unlikely(*p == '\0') || jstr_tolower(*p) == c)
			return (char *)p;
	uint32_t m, m1, m2, zm;
	JSTR_VEC sv;
	const JSTR_VEC cv0 = SET18((char)c);
	const JSTR_VEC cv1 = SET18((char)jstr_toupper(c));
	const JSTR_VEC zv = SETZERO();
	for (;; p += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)p);
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv0));
		m1 = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv1));
		m2 = m | m1 | zm;
		if (m2)
			break;
	}
	return (char *)p + TZCNT(m2);
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_INLINE
static char *
pjstr_strcasechr_simd(const char *s,
                      int c)
{
	s = pjstr_strcasechrnul_simd(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memcasechr_simd(const void *s,
                      int c,
                      size_t n)
{
	if (!jstr_isalpha(c))
		return (char *)memchr(s, c, n);
	if (jstr_unlikely(n == 0))
		return NULL;
	c = jstr_tolower(c);
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *const end = p + n;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE); ++p) {
		if (jstr_unlikely(p >= end))
			return NULL;
		if (jstr_tolower(*p) == c)
			return (void *)p;
	}
	uint32_t m, m1, m2;
	JSTR_VEC sv;
	const JSTR_VEC cv = SET18((char)c);
	const JSTR_VEC cv1 = SET18((char)jstr_toupper(c));
	for (; p < end; p += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)p);
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		m1 = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv1));
		m2 = m | m1;
		if (m2)
			goto ret;
	}
	return NULL;
ret:;
	const uint32_t i = TZCNT(m2);
	return p + i < end ? (char *)p + i : NULL;
}

#endif

#ifndef LZCNT

#	define JSTR_HAVENT_MEMRCHR_SIMD 1

#else

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memrchr_simd(const void *s,
                   int c,
                   size_t n)
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (unsigned char *)s + n;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE);) {
		--p;
		if (jstr_unlikely(p < (unsigned char *)s))
			return NULL;
		if (*p == (unsigned char)c)
			return (void *)p;
	}
	uint32_t m;
	JSTR_VEC sv;
	const JSTR_VEC cv = SET18((char)c);
	while (p >= (unsigned char *)s) {
		p -= JSTR_VEC_SIZE;
		sv = LOAD((const JSTR_VEC *)p);
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		if (m)
			goto ret;
	}
	return NULL;
ret:;
	const uint32_t i = 31 - _lzcnt_u32(m);
	return p + i >= (unsigned char *)s ? (char *)p + i : NULL;
}

#endif

#if !(defined TZCNT && defined BLSR)

#	define JSTR_HAVENT_MEMMEM_SIMD         1
#	define JSTR_HAVENT_STRCASESTR_LEN_SIMD 1

#else

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memmem_simd(const void *hs,
                  size_t hs_len,
                  const void *ne,
                  size_t ne_len)
{
	if (ne_len == 1)
		return (void *)memchr(hs, *(unsigned char *)ne, hs_len);
	if (jstr_unlikely(ne_len == 0))
		return (void *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *const end = h + hs_len - ne_len;
	size_t shift = JSTR_PTR_DIFF(jstr_rarebytefind_len(ne, ne_len), ne);
	if (shift == ne_len - 1)
		--shift;
	h += shift;
	for (; JSTR_PTR_IS_NOT_ALIGNED(h, JSTR_VEC_SIZE); ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (*h == *((unsigned char *)ne + shift) && !memcmp(h - shift, ne, ne_len))
			return (void *)(h - shift);
	}
	const JSTR_VEC nv = SET18(*((char *)ne + shift));
	const JSTR_VEC nv1 = SET18(*((char *)ne + shift + 1));
	JSTR_VEC hv, hv1;
	uint32_t i, hm0, hm1, m;
	for (; h - shift <= end; h += JSTR_VEC_SIZE) {
		hv = LOAD((const JSTR_VEC *)h);
		hv1 = LOADU((const JSTR_VEC *)(h + 1));
		hm0 = (uint32_t)MOVEMASK8(CMPEQ8(hv, nv));
		hm1 = (uint32_t)MOVEMASK8(CMPEQ8(hv1, nv1));
		m = hm0 & hm1;
		while (m) {
			i = TZCNT(m);
			m = _blsr_u32(m);
			if (jstr_unlikely(h + i - shift > end))
				return NULL;
			if (!memcmp(h + i - shift, ne, ne_len))
				return (char *)h + i - shift;
		}
	}
	return NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strcasestr_len_simd(const char *hs,
                          size_t hs_len,
                          const char *ne,
                          size_t ne_len)
{
	if (ne_len == 1)
		return (char *)pjstr_memcasechr_simd(hs, *(unsigned char *)ne, hs_len);
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *const end = h + hs_len - ne_len;
	size_t shift = JSTR_PTR_DIFF(jstr_rarebytefind_len(ne, ne_len), ne);
	if (shift == ne_len - 1)
		--shift;
	h += shift;
	for (const int c = jstr_tolower(*((unsigned char *)ne + shift));
	     JSTR_PTR_IS_NOT_ALIGNED(h, JSTR_VEC_SIZE);
	     ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (jstr_tolower(*h) == c && !jstr_strcasecmpeq_len((const char *)h - shift, (const char *)ne, ne_len))
			return (char *)(h - shift);
	}
	const JSTR_VEC nv = SET18((char)jstr_tolower(*((unsigned char *)ne + shift)));
	const JSTR_VEC nv1 = SET18((char)jstr_toupper(*((unsigned char *)ne + shift)));
	const JSTR_VEC nv2 = SET18((char)jstr_tolower(*((unsigned char *)ne + shift + 1)));
	const JSTR_VEC nv3 = SET18((char)jstr_toupper(*((unsigned char *)ne + shift + 1)));
	JSTR_VEC hv, hv1;
	uint32_t i, hm0, hm1, hm2, hm3, m;
	for (; h - shift <= end; h += JSTR_VEC_SIZE) {
		hv = LOAD((const JSTR_VEC *)h);
		hv1 = LOADU((const JSTR_VEC *)(h + 1));
		hm0 = (uint32_t)MOVEMASK8(CMPEQ8(hv, nv));
		hm1 = (uint32_t)MOVEMASK8(CMPEQ8(hv, nv1));
		hm2 = (uint32_t)MOVEMASK8(CMPEQ8(hv1, nv2));
		hm3 = (uint32_t)MOVEMASK8(CMPEQ8(hv1, nv3));
		m = (hm0 | hm1) & (hm2 | hm3);
		while (m) {
			i = TZCNT(m);
			m = _blsr_u32(m);
			if (jstr_unlikely(h + i - shift > end))
				return NULL;
			if (!jstr_strcasecmpeq_len((const char *)h + i - shift, (const char *)ne, ne_len))
				return (char *)h + i - shift;
		}
	}
	return NULL;
}

#endif

#ifndef POPCNT

#	define JSTR_HAVENT_COUNTCHR_SIMD     1
#	define JSTR_HAVENT_COUNTCHR_LEN_SIMD 1

#else

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static size_t
pjstr_countchr_simd(const void *s,
                    int c)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE); cnt += *p++ == (unsigned char)c)
		if (jstr_unlikely(*p == '\0'))
			return cnt;
	const JSTR_VEC cv = SET18((char)c);
	const JSTR_VEC zv = SETZERO();
	JSTR_VEC sv;
	uint32_t m, zm;
	unsigned int cnt0;
	for (;; p += JSTR_VEC_SIZE, cnt += cnt0) {
		sv = LOAD((const JSTR_VEC *)p);
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (uint32_t)MOVEMASK8(CMPEQ8(sv, zv));
		cnt0 = m ? (unsigned int)POPCNT(m) : 0;
		if (zm)
			break;
	}
	for (; *p; cnt += *p++ == (unsigned char)c) {}
	return cnt;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static size_t
pjstr_countchr_len_simd(const void *s,
                        int c,
                        size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, JSTR_VEC_SIZE); cnt += *p++ == (unsigned char)c)
		if (jstr_unlikely(n-- == 0))
			return cnt;
	const JSTR_VEC cv = SET18((char)c);
	JSTR_VEC sv;
	uint32_t m;
	for (; n >= JSTR_VEC_SIZE; n -= JSTR_VEC_SIZE, p += JSTR_VEC_SIZE) {
		sv = LOAD((const JSTR_VEC *)p);
		m = (uint32_t)MOVEMASK8(CMPEQ8(sv, cv));
		cnt += m ? (unsigned int)POPCNT(m) : 0;
	}
#	ifdef __AVX2__
	if (n >= sizeof(__m128i)) {
		const __m128i v = _mm_load_si128((const __m128i *)p);
		m = _mm_movemask_epi8(_mm_cmpeq_epi8(v, _mm_setzero_si128()));
		cnt += m ? POPCNT(m) : 0;
		p += sizeof(__m128i);
		n -= sizeof(__m128i);
	}
#	endif
	for (; n--; cnt += *p++ == (unsigned char)c) {}
	return cnt;
}

#endif

#undef MASK
#undef LOAD
#undef LOADU
#undef STORE
#undef STOREU
#undef MOVEMASK8
#undef CMPEQ8
#undef SETZERO
#undef SET18
#undef POPCNT
#undef TZCNT
#undef BLSR
#undef LZCNT

PJSTR_END_DECLS

#endif /* PJSTR_AVX2_H* */