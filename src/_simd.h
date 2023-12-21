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

/* Select AVX512, AVX2, or SSE. */

#if defined __AVX512F__
#	include <immintrin.h>
typedef __m512i jstr_vec_ty;
typedef uint64_t jstr_vec_mask_ty;
#	define LOAD(x)           _mm512_load_si512(x)
#	define LOADU(x)          _mm512_loadu_si512(x)
#	define STORE(dst, src)   _mm512_store_si512(dst, src)
#	define STOREU(dst, src)  _mm512_storeu_si512(dst, src)
#	define CMPEQ8_MASK(x, y) _mm512_cmpeq_epi8_mask(x, y)
#	define SETZERO(x)        _mm512_setzero_si512(x)
#	define SETONE8(x)        _mm512_set1_epi8(x)
#	define POPCNT(x)         _mm_popcnt_u64(x)
#	define TZCNT(x)          _tzcnt_u64(x)
#	define BLSR(x)           _blsr_u64(x)
#	define LZCNT(x)          _lzcnt_u64(x)
#elif defined __AVX2__
#	include <immintrin.h>
typedef __m256i jstr_vec_ty;
typedef uint32_t jstr_vec_mask_ty;
#	define LOAD(x)           _mm256_load_si256(x)
#	define LOADU(x)          _mm256_loadu_si256(x)
#	define STORE(dst, src)   _mm256_store_si256(dst, src)
#	define STOREU(dst, src)  _mm256_storeu_si256(dst, src)
#	define CMPEQ8_MASK(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))
#	define SETZERO(x)        _mm256_setzero_si256(x)
#	define SETONE8(x)        _mm256_set1_epi8(x)
#	define POPCNT(x)         _mm_popcnt_u32(x)
#	define TZCNT(x)          _tzcnt_u32(x)
#	define BLSR(x)           _blsr_u32(x)
#	define LZCNT(x)          _lzcnt_u32(x)
#elif defined __SSE__
#	include <emmintrin.h>
typedef __m128i jstr_vec_ty;
typedef uint16_t jstr_vec_mask_ty;
#	define LOAD(x)           _mm_load_si128(x)
#	define LOADU(x)          _mm_loadu_si128(x)
#	define STORE(dst, src)   _mm_store_si128(dst, src)
#	define STOREU(dst, src)  _mm_storeu_si128(dst, src)
#	define CMPEQ8_MASK(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))
#	define SETZERO(x)        _mm_setzero_si128(x)
#	define SETONE8(x)        _mm_set1_epi8(x)
#	if JSTR_HAS_BUILTIN(__builtin_popcount)
#		define POPCNT(x) __builtin_popcount(x)
#	endif
#	if JSTR_HAS_BUILTIN(__builtin_ia32_tzcnt_u16)
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
		zm = (MASK)CMPEQ8_MASK(sv, zv);
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
		sv = LOAD((const VEC *)src);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		if (zm)
			break;
		STOREU((VEC *)dst, sv);
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
	while (JSTR_PTR_IS_NOT_ALIGNED(src, VEC_SIZE))
		if (jstr_unlikely((*dst++ = *src++) == '\0'))
			return dst - 1;
	if (JSTR_PTR_IS_ALIGNED(dst, VEC_SIZE))
		return pjstr_stpcpy_simd_aligned(dst, src);
	return pjstr_stpcpy_simd_unaligned_src(dst, src);
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
pjstr_stpcpy_simd_aligned_unroll(char *JSTR_RESTRICT dst,
                                 const char *JSTR_RESTRICT src)
{
	const VEC zv = SETZERO();
	VEC sv0, sv1, sv2, sv3;
	MASK zm0, zm1, zm2, zm3;
	for (;; src += VEC_SIZE * 4, dst += VEC_SIZE * 4) {
		sv0 = LOAD((const VEC *)src);
		sv1 = LOAD((const VEC *)src + 1);
		sv2 = LOAD((const VEC *)src + 2);
		sv3 = LOAD((const VEC *)src + 3);
		zm0 = (MASK)CMPEQ8_MASK(sv0, zv);
		zm1 = (MASK)CMPEQ8_MASK(sv1, zv);
		zm2 = (MASK)CMPEQ8_MASK(sv2, zv);
		zm3 = (MASK)CMPEQ8_MASK(sv3, zv);
		if (zm0 | zm1 | zm2 | zm3)
			break;
		STORE((VEC *)dst, sv0);
		STORE((VEC *)dst + 1, sv1);
		STORE((VEC *)dst + 2, sv2);
		STORE((VEC *)dst + 3, sv3);
	}
	if (zm0)
		goto tail;
	STORE((VEC *)dst, sv0);
	dst += VEC_SIZE, src += VEC_SIZE;
	if (zm1)
		goto tail;
	STORE((VEC *)dst, sv1);
	dst += VEC_SIZE, src += VEC_SIZE;
	if (zm2)
		goto tail;
	STORE((VEC *)dst, sv2);
	dst += VEC_SIZE, src += VEC_SIZE;
	goto tail;
tail:
	while ((*dst++ = *src++))
		;
	return dst - 1;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
pjstr_stpcpy_simd_unaligned_src_unroll(char *JSTR_RESTRICT dst,
                                       const char *JSTR_RESTRICT src)
{
	const VEC zv = SETZERO();
	VEC sv0, sv1, sv2, sv3;
	MASK zm0, zm1, zm2, zm3;
	for (;; src += VEC_SIZE * 4, dst += VEC_SIZE * 4) {
		sv0 = LOAD((const VEC *)src);
		sv1 = LOAD((const VEC *)src + 1);
		sv2 = LOAD((const VEC *)src + 2);
		sv3 = LOAD((const VEC *)src + 3);
		zm0 = (MASK)CMPEQ8_MASK(sv0, zv);
		zm1 = (MASK)CMPEQ8_MASK(sv1, zv);
		zm2 = (MASK)CMPEQ8_MASK(sv2, zv);
		zm3 = (MASK)CMPEQ8_MASK(sv3, zv);
		if (zm0 | zm1 | zm2 | zm3)
			break;
		STOREU((VEC *)dst, sv0);
		STOREU((VEC *)dst + 1, sv1);
		STOREU((VEC *)dst + 2, sv2);
		STOREU((VEC *)dst + 3, sv3);
	}
	if (zm0)
		goto tail;
	STOREU((VEC *)dst, sv0);
	dst += VEC_SIZE, src += VEC_SIZE;
	if (zm1)
		goto tail;
	STOREU((VEC *)dst, sv1);
	dst += VEC_SIZE, src += VEC_SIZE;
	if (zm2)
		goto tail;
	STOREU((VEC *)dst, sv2);
	dst += VEC_SIZE, src += VEC_SIZE;
	goto tail;
tail:
	while ((*dst++ = *src++))
		;
	return dst - 1;
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
static char *
pjstr_stpcpy_simd_unroll(char *JSTR_RESTRICT dst,
                         const char *JSTR_RESTRICT src)
{
	while (JSTR_PTR_IS_NOT_ALIGNED(src, VEC_SIZE * 4))
		if (jstr_unlikely((*dst++ = *src++) == '\0'))
			return dst - 1;
	if (JSTR_PTR_IS_ALIGNED(dst, VEC_SIZE * 4))
		return pjstr_stpcpy_simd_aligned_unroll(dst, src);
	return pjstr_stpcpy_simd_unaligned_src_unroll(dst, src);
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
	if (jstr_unlikely(n == 0))
		return NULL;
	const char *const end = s + n;
	MASK cm0, cm1, m, zm, i;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_PTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
	if (off) {
		s -= off;
		sv = LOAD((const VEC *)s);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = (cm0 | cm1 | zm) >> off;
		if (m) {
			i = off + TZCNT(m);
			goto ret_early;
		}
		s += VEC_SIZE;
	}
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm0 | cm1 | zm;
		if (m | (s >= end))
			break;
	}
	if (s >= end)
		return NULL;
	i = TZCNT(m);
ret_early:
	return s + i < end ? (char *)s + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strnchr_simd(const char *s,
                   int c,
                   size_t n)
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const char *const end = s + n;
	MASK cm, m, zm, i;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_PTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
	if (off) {
		s -= off;
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = (cm | zm) >> off;
		if (m) {
			i = off + TZCNT(m);
			goto ret_early;
		}
		s += VEC_SIZE;
	}
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm | zm;
		if (m | (s >= end))
			break;
	}
	if (s >= end)
		return NULL;
	i = TZCNT(m);
ret_early:
	return s + i < end ? (char *)s + i : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strchrnul_simd(const char *s,
                     int c)
{
	MASK cm, m, zm;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_PTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
	if (off) {
		s -= off;
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = (cm | zm) >> off;
		if (m)
			return (char *)s + off + TZCNT(m);
		s += VEC_SIZE;
	}
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm | zm;
		if (m)
			break;
	}
	return (char *)s + TZCNT(m);
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_INLINE
static char *
pjstr_strchr_simd(const char *s,
                  int c)
{
	s = pjstr_strchrnul_simd(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strcasechrnul_simd(const char *s,
                         int c)
{
	MASK m, cm0, cm1, zm;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_PTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
	if (off) {
		s -= off;
		sv = LOAD((const VEC *)s);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		m = (cm0 | cm1 | zm) >> off;
		if (m)
			return (char *)s + off + TZCNT(m);
		s += VEC_SIZE;
	}
	for (;; s += VEC_SIZE) {
		sv = LOAD((const VEC *)s);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		m = cm0 | cm1 | zm;
		if (m)
			break;
	}
	return (char *)s + TZCNT(m);
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
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *const end = p + n;
	MASK cm0, cm1, m, i;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const unsigned int off = JSTR_PTR_DIFF(p, JSTR_PTR_ALIGN_DOWN(p, VEC_SIZE));
	if (off) {
		p -= off;
		sv = LOAD((const VEC *)p);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		m = (cm0 | cm1) >> off;
		if (m) {
			i = off + TZCNT(m);
			goto ret_early;
		}
		p += VEC_SIZE;
	}
	for (; p < end; p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		m = cm0 | cm1;
		if (m)
			goto ret;
	}
	return NULL;
ret:;
	i = TZCNT(m);
ret_early:
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
	const unsigned char *p = (unsigned char *)s + n - VEC_SIZE;
	MASK m, i;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const unsigned int off = JSTR_PTR_DIFF(JSTR_PTR_ALIGN_UP(p, VEC_SIZE), p);
	if (off) {
		p += off;
		sv = LOAD((const VEC *)p);
		m = (MASK)CMPEQ8_MASK(sv, cv) << off;
		if (m) {
			i = (sizeof(MASK) * CHAR_BIT - 1) - LZCNT(m);
			return p - off + i >= (unsigned char *)s ? (char *)p - off + i : NULL;
		}
		p -= VEC_SIZE;
	}
	for (; p + VEC_SIZE >= (unsigned char *)s; p -= VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		m = (MASK)CMPEQ8_MASK(sv, cv);
		if (m)
			goto ret;
	}
	return NULL;
ret:;
	i = (sizeof(MASK) * CHAR_BIT - 1) - LZCNT(m);
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
	const VEC nv0 = SETONE8(*((char *)ne + shift));
	const VEC nv1 = SETONE8(*((char *)ne + shift + 1));
	VEC hv0, hv1, nv;
	MASK i, hm0, hm1, m;
	const unsigned char *hp;
	if (JSTR_PTR_ALIGN_UP(ne, 4096) - (uintptr_t)ne >= VEC_SIZE || JSTR_PTR_IS_ALIGNED(ne, 4096) || ne_len >= VEC_SIZE)
		nv = LOADU((VEC *)ne);
	else
		memcpy(&nv, ne, JSTR_MIN(VEC_SIZE, ne_len));
	const MASK sh = ne_len < VEC_SIZE ? VEC_SIZE - ne_len : 0;
	const MASK matchm = (MASK)-1 << sh;
	MASK cmpm;
	VEC hv;
	const unsigned int off = JSTR_PTR_DIFF(h, JSTR_PTR_ALIGN_DOWN(h, VEC_SIZE));
	h -= off;
	hv0 = LOAD((const VEC *)h);
	hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
	hm1 = (MASK)CMPEQ8_MASK(hv0, nv1) >> 1;
	m = (hm0 & hm1) >> off;
	while (m) {
		i = TZCNT(m);
		m = BLSR(m);
		hp = h + off + i - shift;
		if (jstr_unlikely(hp > end))
			return NULL;
		if (JSTR_PTR_ALIGN_UP(hp, 4096) - (uintptr_t)hp >= VEC_SIZE || JSTR_PTR_IS_ALIGNED(hp, 4096)) {
			hv = LOADU((VEC *)hp);
			cmpm = (MASK)CMPEQ8_MASK(hv, nv) << sh;
			if (cmpm == matchm)
				if (ne_len <= VEC_SIZE || !memcmp(hp + VEC_SIZE, (const char *)ne + VEC_SIZE, ne_len - VEC_SIZE))
					return (void *)hp;
		} else {
			if (!memcmp(hp, ne, ne_len))
				return (void *)hp;
		}
	}
	h += VEC_SIZE - 1;
	for (; h - shift + VEC_SIZE <= end; h += VEC_SIZE) {
		hv0 = LOADU((const VEC *)h);
		hv1 = LOAD((const VEC *)(h + 1));
		hm1 = (MASK)CMPEQ8_MASK(hv1, nv1);
		hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
		m = hm0 & hm1;
		while (m) {
match:
			i = TZCNT(m);
			m = BLSR(m);
			hp = h + i - shift;
			if (jstr_unlikely(hp > end))
				return NULL;
			if (JSTR_PTR_ALIGN_UP(hp, 4096) - (uintptr_t)hp >= VEC_SIZE || JSTR_PTR_IS_ALIGNED(hp, 4096)) {
				hv = LOADU((VEC *)hp);
				cmpm = (MASK)CMPEQ8_MASK(hv, nv) << sh;
				if (cmpm == matchm)
					if (ne_len <= VEC_SIZE || !memcmp(hp + VEC_SIZE, (const char *)ne + VEC_SIZE, ne_len - VEC_SIZE))
						return (void *)hp;
			} else {
				if (!memcmp(hp, ne, ne_len))
					return (void *)hp;
			}
		}
	}
	if (h - shift <= end) {
		hv1 = LOAD((const VEC *)(h + 1));
		m = (MASK)CMPEQ8_MASK(hv1, nv1);
		if (m)
			goto match;
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
	const VEC nv0 = SETONE8((char)c);
	const VEC nv1 = SETONE8((char)jstr_toupper(c));
	const VEC nv2 = SETONE8((char)jstr_tolower(*((unsigned char *)ne + shift + 1)));
	const VEC nv3 = SETONE8((char)jstr_toupper(*((unsigned char *)ne + shift + 1)));
	VEC hv0, hv1;
	MASK i, hm0, hm1, hm2, hm3, m;
	const unsigned int off = JSTR_PTR_DIFF(h, JSTR_PTR_ALIGN_DOWN(h, VEC_SIZE));
	h -= off;
	hv0 = LOAD((const VEC *)h);
	hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
	hm1 = (MASK)CMPEQ8_MASK(hv0, nv1);
	hm2 = (MASK)CMPEQ8_MASK(hv0, nv2) >> 1;
	hm3 = (MASK)CMPEQ8_MASK(hv0, nv3) >> 1;
	m = ((hm0 | hm1) & (hm2 | hm3)) >> off;
	while (m) {
		i = TZCNT(m);
		m = BLSR(m);
		if (jstr_unlikely(h + off + i - shift > end))
			return NULL;
		if (!jstr_strcasecmpeq_len((const char *)h + off + i - shift, (const char *)ne, ne_len))
			return (char *)h + off + i - shift;
	}
	h += VEC_SIZE - 1;
	for (; h - shift + VEC_SIZE <= end; h += VEC_SIZE) {
		hv0 = LOADU((const VEC *)h);
		hv1 = LOAD((const VEC *)(h + 1));
		hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
		hm1 = (MASK)CMPEQ8_MASK(hv0, nv1);
		hm2 = (MASK)CMPEQ8_MASK(hv1, nv2);
		hm3 = (MASK)CMPEQ8_MASK(hv1, nv3);
		m = (hm0 | hm1) & (hm2 | hm3);
		while (m) {
match:
			i = TZCNT(m);
			m = BLSR(m);
			if (jstr_unlikely(h + i - shift > end))
				return NULL;
			if (!jstr_strcasecmpeq_len((const char *)h + i - shift, (const char *)ne, ne_len))
				return (char *)h + i - shift;
		}
	}
	if (h - shift <= end) {
		hv1 = LOAD((const VEC *)(h + 1));
		hm2 = (MASK)CMPEQ8_MASK(hv1, nv2);
		hm3 = (MASK)CMPEQ8_MASK(hv1, nv3);
		m = hm2 | hm3;
		if (m)
			goto match;
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
		m = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
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
		m = (MASK)CMPEQ8_MASK(sv, cv);
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
