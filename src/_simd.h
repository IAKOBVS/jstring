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
	do {
		s += VEC_SIZE;
		sv = LOAD((const VEC *)s);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm0 | cm1 | zm;
	} while (!(m | (s >= end)));
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
	s -= off;
	sv = LOAD((const VEC *)s);
	cm = (MASK)CMPEQ8_MASK(sv, cv);
	zm = (MASK)CMPEQ8_MASK(sv, zv);
	m = (cm | zm) >> off;
	if (m) {
		i = off + TZCNT(m);
		goto ret_early;
	}
	do {
		s += VEC_SIZE;
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm | zm;
	} while (!(m | (s >= end)));
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
	s -= off;
	sv = LOAD((const VEC *)s);
	cm = (MASK)CMPEQ8_MASK(sv, cv);
	zm = (MASK)CMPEQ8_MASK(sv, zv);
	m = (cm | zm) >> off;
	if (m)
		return (char *)s + off + TZCNT(m);
	do {
		s += VEC_SIZE;
		sv = LOAD((const VEC *)s);
		cm = (MASK)CMPEQ8_MASK(sv, cv);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		m = cm | zm;
	} while (!m);
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
	s -= off;
	sv = LOAD((const VEC *)s);
	zm = (MASK)CMPEQ8_MASK(sv, zv);
	cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
	cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
	m = (cm0 | cm1 | zm) >> off;
	if (m)
		return (char *)s + off + TZCNT(m);
	do {
		s += VEC_SIZE;
		sv = LOAD((const VEC *)s);
		zm = (MASK)CMPEQ8_MASK(sv, zv);
		cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
		cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
		m = cm0 | cm1 | zm;
	} while (!m);
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
	p -= off;
	sv = LOAD((const VEC *)p);
	cm0 = (MASK)CMPEQ8_MASK(sv, cv0);
	cm1 = (MASK)CMPEQ8_MASK(sv, cv1);
	m = (cm0 | cm1) >> off;
	if (m) {
		i = off + TZCNT(m);
		goto ret_early;
	}
	while ((p += VEC_SIZE) < end) {
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
	p += off;
	sv = LOAD((const VEC *)p);
	m = (MASK)CMPEQ8_MASK(sv, cv) << off;
	if (m) {
		i = (sizeof(MASK) * CHAR_BIT - 1) - LZCNT(m);
		return p - off + i >= (unsigned char *)s ? (char *)p - off + i : NULL;
	}
	p -= VEC_SIZE;
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

#if !(defined TZCNT && defined BLSR && (defined __AVX2__ || defined __AVX512F__))

#	define JSTR_HAVENT_MEMMEM_SIMD         1
#	define JSTR_HAVENT_STRCASESTR_LEN_SIMD 1

#else

#define PJSTR_SIMD_USE_AS_MEMCASEMEM 1
#define PJSTR_SIMD_CMP_FUNC jstr_strcasecmpeq_len
#define PJSTR_SIMD_FUNC_NAME pjstr_strcasestr_len_simd
#include "_simd-memmem.h"

#define PJSTR_SIMD_FUNC_NAME pjstr_memmem_simd
#include "_simd-memmem.h"

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
