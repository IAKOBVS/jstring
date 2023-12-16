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

/* If AVX2 is not available, use SSE. */

#ifdef __AVX2__
#	include <immintrin.h>
typedef __m256i jstr_vec_ty;
typedef uint32_t jstr_vec_mask_ty;
#	define MASK             uint32_t
#	define LOAD(x)          _mm256_load_si256(x)
#	define LOADU(x)         _mm256_loadu_si256(x)
#	define STORE(dst, src)  _mm256_store_si256(dst, src)
#	define STOREU(dst, src) _mm256_storeu_si256(dst, src)
#	define MOVEMASK8(x)     _mm256_movemask_epi8(x)
#	define CMPEQ8(x, y)     _mm256_cmpeq_epi8(x, y)
#	define SETZERO(x)       _mm256_setzero_si256(x)
#	define SETONE8(x)       _mm256_set1_epi8(x)
#	define POPCNT(x)        _mm_popcnt_u32(x)
#	define TZCNT(x)         _tzcnt_u32(x)
#	define BLSR(x)          _blsr_u32(x)
#	define LZCNT(x)         _lzcnt_u32(x)
#else
#	include <emmintrin.h>
typedef __m128i jstr_vec_ty;
typedef uint16_t jstr_vec_mask_ty;
#	define LOAD(x)          _mm_load_si128(x)
#	define LOADU(x)         _mm_loadu_si128(x)
#	define STORE(dst, src)  _mm_store_si128(dst, src)
#	define STOREU(dst, src) _mm_storeu_si128(dst, src)
#	define MOVEMASK8(x)     _mm_movemask_epi8(x)
#	define CMPEQ8(x, y)     _mm_cmpeq_epi8(x, y)
#	define SETZERO(x)       _mm_setzero_si128(x)
#	define SETONE8(x)       _mm_set1_epi8(x)
#	if JSTR_HAS_BUILTIN(__builtin_popcount)
#		define POPCNT(x) __builtin_popcount(x)
#	endif
#	if JSTR_HAS_BUILTIN(__builtin_ia32_tzcnt_u32)
#		define TZCNT(x) __builtin_ia32_tzcnt_u16(x)
#	endif
#endif
#define VEC       jstr_vec_ty
#define VEC_SIZE  sizeof(VEC)
#define MASK      jstr_vec_mask_ty
#define MASK_SIZE sizeof(MASK)

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
pjstr_stpcpy_simd_aligned(char *JSTR_RESTRICT dst,
                          const char *JSTR_RESTRICT src)
{
	VEC sv;
	const VEC zv = SETZERO();
	MASK zm;
	for (;; src += VEC_SIZE, dst += VEC_SIZE) {
		sv = LOAD((const VEC *)src);
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		if (zm)
			break;
		STORE((VEC *)dst, sv);
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
	VEC sv;
	const VEC zv = SETZERO();
	MASK zm;
	for (;; src += VEC_SIZE, dst += VEC_SIZE) {
		sv = LOADU((const VEC *)src);
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		if (zm)
			break;
		STORE((VEC *)dst, sv);
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
	while (JSTR_PTR_IS_NOT_ALIGNED(dst, VEC_SIZE))
		if (jstr_unlikely((*dst++ = *src++) == '\0'))
			return dst - 1;
	if (JSTR_PTR_IS_ALIGNED(src, VEC_SIZE))
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE); ++p) {
		if (jstr_unlikely(n-- == 0) || jstr_unlikely(*p == '\0'))
			return NULL;
		if (jstr_unlikely(jstr_tolower(*p) == c))
			return (char *)p;
	}
	const unsigned char *const end = p + n;
	MASK hm0, hm1, m, zm;
	VEC sv;
	const VEC cv0 = SETONE8((char)c);
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	for (;; p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		hm0 = (MASK)MOVEMASK8(CMPEQ8(sv, cv0));
		hm1 = (MASK)MOVEMASK8(CMPEQ8(sv, cv1));
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		m = hm0 | hm1 | zm;
		if (m | (p >= end))
			break;
	}
	if (p >= end)
		return NULL;
	const MASK i = TZCNT(m);
	return p + i < end ? (char *)p + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strnchr_simd(const char *s,
                   int c,
                   size_t n)
{
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, VEC_SIZE); ++s) {
		if (jstr_unlikely(n-- == 0) || jstr_unlikely(*s == '\0'))
			return NULL;
		if (jstr_unlikely(*s == (char)c))
			return (char *)s;
	}
	const char *const end = s + n;
	MASK hm, m, zm;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		hm = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		m = hm | zm;
		if (m | (s >= end))
			break;
	}
	if (s >= end)
		return NULL;
	const MASK i = TZCNT(m);
	return s + i < end ? (char *)s + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strchrnul_simd(const char *s,
                     int c)
{
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, VEC_SIZE); ++s)
		if (jstr_unlikely(*s == '\0') || *s == (char)c)
			return (char *)s;
	MASK cm, m, zm;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		cm = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		m = cm | zm;
		if (m)
			break;
	}
	return (char *)s + TZCNT(m);
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE); ++p)
		if (jstr_unlikely(*p == '\0') || jstr_tolower(*p) == c)
			return (char *)p;
	MASK m, m1, m2, zm;
	VEC sv;
	const VEC cv0 = SETONE8((char)c);
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	for (;; p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
		m = (MASK)MOVEMASK8(CMPEQ8(sv, cv0));
		m1 = (MASK)MOVEMASK8(CMPEQ8(sv, cv1));
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE); ++p) {
		if (jstr_unlikely(p >= end))
			return NULL;
		if (jstr_tolower(*p) == c)
			return (void *)p;
	}
	MASK m, m1, m2;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	for (; p < end; p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		m = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		m1 = (MASK)MOVEMASK8(CMPEQ8(sv, cv1));
		m2 = m | m1;
		if (m2)
			goto ret;
	}
	return NULL;
ret:;
	const MASK i = TZCNT(m2);
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE);) {
		--p;
		if (jstr_unlikely(p < (unsigned char *)s))
			return NULL;
		if (*p == (unsigned char)c)
			return (void *)p;
	}
	MASK m;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	while (p >= (unsigned char *)s) {
		p -= VEC_SIZE;
		sv = LOAD((const VEC *)p);
		m = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		if (m)
			goto ret;
	}
	return NULL;
ret:;
	const MASK i = 31 - _lzcnt_u32(m);
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(h, VEC_SIZE); ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (*h == *((unsigned char *)ne + shift) && !memcmp(h - shift, ne, ne_len))
			return (void *)(h - shift);
	}
	const VEC nv = SETONE8(*((char *)ne + shift));
	const VEC nv1 = SETONE8(*((char *)ne + shift + 1));
	VEC hv0, hv1;
	MASK i, hm0, hm1, m;
	for (; h - shift <= end; h += VEC_SIZE) {
		hv0 = LOAD((const VEC *)h);
		hv1 = LOADU((const VEC *)(h + 1));
		hm0 = (MASK)MOVEMASK8(CMPEQ8(hv0, nv));
		hm1 = (MASK)MOVEMASK8(CMPEQ8(hv1, nv1));
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
	const int c = jstr_tolower(*(ne + shift));
	for (; JSTR_PTR_IS_NOT_ALIGNED(h, VEC_SIZE); ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (jstr_tolower(*h) == c && !jstr_strcasecmpeq_len((const char *)h - shift, (const char *)ne, ne_len))
			return (char *)(h - shift);
	}
	const VEC nv = SETONE8((char)c);
	const VEC nv1 = SETONE8((char)jstr_toupper(c));
	const VEC nv2 = SETONE8((char)jstr_tolower(*((unsigned char *)ne + shift + 1)));
	const VEC nv3 = SETONE8((char)jstr_toupper(*((unsigned char *)ne + shift + 1)));
	VEC hv0, hv1;
	MASK i, hm0, hm1, hm2, hm3, m;
	for (; h - shift <= end; h += VEC_SIZE) {
		hv0 = LOAD((const VEC *)h);
		hv1 = LOADU((const VEC *)(h + 1));
		hm0 = (MASK)MOVEMASK8(CMPEQ8(hv0, nv));
		hm1 = (MASK)MOVEMASK8(CMPEQ8(hv0, nv1));
		hm2 = (MASK)MOVEMASK8(CMPEQ8(hv1, nv2));
		hm3 = (MASK)MOVEMASK8(CMPEQ8(hv1, nv3));
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE); cnt += *p++ == (unsigned char)c)
		if (jstr_unlikely(*p == '\0'))
			return cnt;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	VEC sv;
	MASK m, zm;
	unsigned int cnt0;
	for (;; p += VEC_SIZE, cnt += cnt0) {
		sv = LOAD((const VEC *)p);
		m = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		zm = (MASK)MOVEMASK8(CMPEQ8(sv, zv));
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, VEC_SIZE); cnt += *p++ == (unsigned char)c)
		if (jstr_unlikely(n-- == 0))
			return cnt;
	const VEC cv = SETONE8((char)c);
	VEC sv;
	MASK m;
	for (; n >= VEC_SIZE; n -= VEC_SIZE, p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		m = (MASK)MOVEMASK8(CMPEQ8(sv, cv));
		cnt += m ? (unsigned int)POPCNT(m) : 0;
	}
	for (; n--; cnt += *p++ == (unsigned char)c) {}
	return cnt;
}

#endif

#undef VEC
#undef VEC_SIZE
#undef MASK
#undef MASK_SIZE
#undef LOAD
#undef LOADU
#undef STORE
#undef STOREU
#undef MOVEMASK8
#undef CMPEQ8
#undef SETZERO
#undef SETONE8
#undef POPCNT
#undef TZCNT
#undef BLSR
#undef LZCNT

PJSTR_END_DECLS

#endif /* PJSTR_AVX2_H* */
