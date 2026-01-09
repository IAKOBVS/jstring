/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_SIMD_H
#define JSTR_SIMD_H

#include "jstr-macros.h"

#include "jstr-pointer-arith.h"
#include "jstr-stdstring.h"

JSTR__BEGIN_DECLS

#define R JSTR_RESTRICT

/* Select AVX512, AVX2, or SSE2. */

#if JSTR_ARCH_X86_64
#	include <x86intrin.h>
#endif
#ifdef __AVX512BW__
#	include <immintrin.h>
typedef __m512i jstr_vvec_ty;
typedef uint64_t jstr_vmask_ty;
#	define LOAD(x)           _mm512_load_si512(x)
#	define LOADU(x)          _mm512_loadu_si512(x)
#	define STORE(dst, src)   _mm512_store_si512(dst, src)
#	define STOREU(dst, src)  _mm512_storeu_si512(dst, src)
#	define CMPEQ8_MASK(x, y) _mm512_cmpeq_epi8_mask(x, y) /* AVX512BW */
#	define SETZERO(x)        _mm512_setzero_si512(x)
#	define SETONE8(x)        _mm512_set1_epi8(x)
#	define AND(x, y)         _mm512_and_si512(x, y)
#	define ADD8(x, y)        _mm512_add_epi8(x, y) /* AVX512BW */
#	define SUB8(x, y)        _mm512_sub_epi8(x, y) /* AVX512BW */
#	ifdef __BMI__
#		define BLSR(x)  _blsr_u64(x)
#		define TZCNT(x) _tzcnt_u64(x)
#	endif
#	ifdef __LZCNT__
#		define LZCNT(x) _lzcnt_u64(x)
#	endif
#	ifdef __POPCNT__
#		define POPCNT(x) _mm_popcnt_u64(x)
#	endif
#elif defined __AVX2__
#	include <immintrin.h>
typedef __m256i jstr_vvec_ty;
typedef uint32_t jstr_vmask_ty;
#	define LOAD(x)           _mm256_load_si256(x)
#	define LOADU(x)          _mm256_loadu_si256(x)
#	define STORE(dst, src)   _mm256_store_si256(dst, src)
#	define STOREU(dst, src)  _mm256_storeu_si256(dst, src)
#	define CMPEQ8_MASK(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))
#	define SETZERO(x)        _mm256_setzero_si256(x)
#	define SETONE8(x)        _mm256_set1_epi8(x)
#	define AND(x, y)         _mm256_and_si256(x, y)
#	define ADD8(x, y)        _mm256_add_epi8(x, y)
#	define SUB8(x, y)        _mm256_sub_epi8(x, y)
#	define CMPGT8(x, y)      _mm256_cmpgt_epi8(x, y)
#	define CMPLT8(x, y)      CMPGT8(y, x)
#	ifdef __BMI__
#		define BLSR(x)  _blsr_u32(x)
#		define TZCNT(x) _tzcnt_u32(x)
#	endif
#	ifdef __LZCNT__
#		define LZCNT(x) _lzcnt_u32(x)
#	endif
#	ifdef __POPCNT__
#		define POPCNT(x) _mm_popcnt_u32(x)
#	endif
#elif defined __SSE2__
#	include <emmintrin.h>
typedef __m128i jstr_vvec_ty;
typedef uint16_t jstr_vmask_ty;
#	define LOAD(x)           _mm_load_si128(x)
#	define LOADU(x)          _mm_loadu_si128(x)
#	define STORE(dst, src)   _mm_store_si128(dst, src)
#	define STOREU(dst, src)  _mm_storeu_si128(dst, src)
#	define CMPEQ8_MASK(x, y) _mm_movemask_epi8(_mm_cmpeq_epi8(x, y))
#	define SETZERO(x)        _mm_setzero_si128(x)
#	define SETONE8(x)        _mm_set1_epi8(x)
#	define AND(x, y)         _mm_and_si128(x, y)
#	define ADD8(x, y)        _mm_add_epi8(x, y)
#	define SUB8(x, y)        _mm_sub_epi8(x, y)
#	define CMPGT8(x, y)      _mm_cmpgt_epi8(x, y)
#	define CMPLT8(x, y)      _mm_cmplt_epi8(x, y)
#	ifdef __BMI__
#		define TZCNT(x) _tzcnt_u16(x)
#	endif
#	ifdef __LZCNT__
#		ifdef __lzcnt16
#			define LZCNT(x) __lzcnt16(x)
#		endif
#	endif
#	ifdef __POPCNT__
#		define POPCNT(x) _mm_popcnt_u32(x)
#	endif
#endif
#define VEC       jstr_vvec_ty
#define VEC_SIZE  sizeof(VEC)
#define MASK      jstr_vmask_ty
#define MASK_SIZE sizeof(MASK)
#define ONES      ((MASK) - 1)
#if JSTR_ARCH_X86_64
#	ifndef LZCNT
#		define LZCNT(x) (MASK)((x) ? ((MASK)MASK_SIZE * CHAR_BIT - 1) - _bit_scan_reverse(x) : (MASK)MASK_SIZE * CHAR_BIT)
#	endif
#	ifndef TZCNT
#		define TZCNT(x) (MASK)((x) ? _bit_scan_forward(x) : (MASK)MASK_SIZE * CHAR_BIT)
#	endif
#endif
#ifndef BLSR
#	define BLSR(x) ((x) & ((x) - 1))
#endif

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
static char *
jstr__simd_stpcpy(char *R dst, const char *R src)
JSTR_NOEXCEPT
{
	unsigned int i = JSTR_DIFF(JSTR_PTR_ALIGN_UP(src, VEC_SIZE), src);
	while (i--)
		if ((*dst++ = *src++) == '\0')
			return dst - 1;
	VEC sv;
	const VEC zv = SETZERO();
	if (JSTR_PTR_IS_ALIGNED(dst, VEC_SIZE))
		/* Aligned store to DST. */
		for (; CMPEQ8_MASK(sv = LOAD((const VEC *)src), zv);
		     src += VEC_SIZE, dst += VEC_SIZE)
			STORE((VEC *)dst, sv);
	else
		/* Unaligned store to DST. */
		for (; CMPEQ8_MASK(sv = LOAD((const VEC *)src), zv);
		     src += VEC_SIZE, dst += VEC_SIZE)
			STOREU((VEC *)dst, sv);
	while ((*dst++ = *src++)) {}
	return dst - 1;
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
jstr__simd_strncasechr(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const char *end = s + n;
	MASK cm0, cm1, m, zm, i;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
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
jstr__simd_strnchr(const char *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const char *end = s + n;
	MASK cm, m, zm, i;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
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
jstr__simd_strchrnul(const char *s, int c)
JSTR_NOEXCEPT
{
	MASK cm, m, zm;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
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
jstr__simd_strchr(const char *s, int c)
JSTR_NOEXCEPT
{
	s = jstr__simd_strchrnul(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
jstr__simd_strcasechrnul(const char *s, int c)
JSTR_NOEXCEPT
{
	MASK m, cm0, cm1, zm;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const VEC zv = SETZERO();
	const unsigned int off = JSTR_DIFF(s, JSTR_PTR_ALIGN_DOWN(s, VEC_SIZE));
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
jstr__simd_strcasechr(const char *s, int c)
JSTR_NOEXCEPT
{
	s = jstr__simd_strcasechrnul(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
jstr__simd_memcasechr(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (const unsigned char *)s;
	const unsigned char *end = p + n;
	MASK cm0, cm1, m, i;
	VEC sv;
	const VEC cv0 = SETONE8((char)jstr_tolower(c));
	const VEC cv1 = SETONE8((char)jstr_toupper(c));
	const unsigned int off = JSTR_DIFF(p, JSTR_PTR_ALIGN_DOWN(p, VEC_SIZE));
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
jstr__simd_memrchr(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (unsigned char *)s + n - VEC_SIZE;
	MASK m, i;
	VEC sv;
	const VEC cv = SETONE8((char)c);
	const unsigned int off = JSTR_DIFF(JSTR_PTR_ALIGN_UP(p, VEC_SIZE), p);
	p += off;
	sv = LOAD((const VEC *)p);
	m = (MASK)CMPEQ8_MASK(sv, cv) << off;
	if (m) {
		i = (VEC_SIZE - 1) - LZCNT(m);
		return p - off + i >= (unsigned char *)s ? (char *)p - off + i
		                                         : NULL;
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
	i = (VEC_SIZE - 1) - LZCNT(m);
	return p + i >= (unsigned char *)s ? (char *)p + i : NULL;
}

#endif

#if !defined POPCNT

#	define JSTR_HAVENT_COUNTCHR_SIMD     1
#	define JSTR_HAVENT_COUNTCHR_LEN_SIMD 1

#else

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static size_t
jstr__simd_countchr(const char *s, int c)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	unsigned int i = JSTR_DIFF(JSTR_PTR_ALIGN_UP(p, VEC_SIZE), p);
	for (; i--; cnt += *p++ == (unsigned char)c)
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
jstr__simd_countchr_len(const void *s, int c, size_t n)
JSTR_NOEXCEPT
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	unsigned int i = JSTR_DIFF(JSTR_PTR_ALIGN_UP(p, VEC_SIZE), p);
	for (; i--; --n, cnt += *p++ == (unsigned char)c)
		if (jstr_unlikely(n == 0))
			return cnt;
	const VEC cv = SETONE8((char)c);
	VEC sv;
	MASK m;
	for (; n >= VEC_SIZE; n -= VEC_SIZE, p += VEC_SIZE) {
		sv = LOAD((const VEC *)p);
		m = (MASK)CMPEQ8_MASK(sv, cv);
		cnt += (MASK)POPCNT(m);
	}
	for (; n--; cnt += *p++ == (unsigned char)c) {}
	return cnt;
}

#endif

#if defined CMPGT8 && defined CMPLT8 && defined AND && defined ADD8

JSTR_ATTR_CONST
JSTR_ATTR_INLINE
static VEC
jstr__simd_tolower_vec(const VEC v)
JSTR_NOEXCEPT
{
	const VEC gt_a = CMPGT8(v, SETONE8('A' - 1));
	const VEC lt_z = CMPLT8(v, SETONE8('Z' + 1));
	const VEC is_lower = AND(gt_a, lt_z);
	const VEC cvt = AND(is_lower, SETONE8('a' - 'A'));
	return ADD8(v, cvt);
}

JSTR_FUNC_VOID
static void
jstr__simd_tolowerstr_len(char *s, size_t n)
JSTR_NOEXCEPT
{
	int off = JSTR_PTR_ALIGN_UP(s, VEC_SIZE);
	for (; off--; ++s) {
		if (n-- == 0)
			return;
		*s = (char)jstr_tolower(*s);
	}
	for (VEC sv; n >= VEC_SIZE; n -= VEC_SIZE, s += VEC_SIZE) {
		sv = LOAD((VEC *)s);
		STORE((VEC *)s, jstr__simd_tolower_vec(sv));
	}
	for (; n--; ++s)
		*s = (char)jstr_tolower(*s);
}

JSTR_FUNC_VOID
static char *
jstr__simd_tolowerstr_p(char *s)
JSTR_NOEXCEPT
{
	int off = JSTR_PTR_ALIGN_UP(s, VEC_SIZE);
	for (; off--; ++s)
		if ((*s = (char)jstr_tolower(*s)) == '\0')
			return s;
	const VEC zv = SETZERO();
	for (VEC sv; CMPEQ8_MASK(sv = LOAD((VEC *)s), zv); s += VEC_SIZE)
		STORE((VEC *)s, jstr__simd_tolower_vec(sv));
	for (; (*s = (char)jstr_tolower(*s)); ++s) {}
	return s;
}

#endif

#if defined CMPGT8 && defined CMPLT8 && defined AND && defined SUB8

JSTR_ATTR_CONST
JSTR_ATTR_INLINE
static VEC
jstr__simd_toupper_vec(const VEC v)
JSTR_NOEXCEPT
{
	const VEC gt_a = CMPGT8(v, SETONE8('a' - 1));
	const VEC lt_z = CMPLT8(v, SETONE8('z' + 1));
	const VEC is_upper = AND(gt_a, lt_z);
	const VEC cvt = AND(is_upper, SETONE8('a' - 'A'));
	return SUB8(v, cvt);
}

#endif

#if (!defined(TZCNT) || !defined(BLSR))
#	define JSTR_HAVENT_MEMMEM_SIMD 1
#endif

#ifndef JSTR_HAVENT_MEMMEM_SIMD
#	define FIND_MATCH()                                                            \
		do {                                                                    \
			if (JSTR_PTR_NOT_CROSSING_PAGE(p, VEC_SIZE, JSTR_PAGE_SIZE)) {  \
				hv = LOADU((const VEC *)p);                             \
				cmpm = (MASK)CMPEQ8_MASK(hv, nv) << matchsh;            \
				if (cmpm == matchm)                                     \
					return (ret_ty)p;                               \
			} else {                                                        \
				if (!memcmp((const char *)p, (const char *)ne, ne_len)) \
					return (ret_ty)p;                               \
			}                                                               \
		} while (0)

/* ne_len must be <= VEC_SIZE.
 * Worst case: O(n).
 * Best case: O(n / VEC_SIZE). */
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static void *
jstr__simd_memmem(const void *hs, size_t hs_len, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	typedef void *ret_ty;
	if (ne_len == 1)
		return (ret_ty)memchr(hs, *(unsigned char *)ne, hs_len);
	if (jstr_unlikely(ne_len == 0))
		return (ret_ty)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	VEC hv0, hv1;
	MASK i, hm0, hm1, m;
	VEC hv, nv;
	MASK cmpm;
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *end = h + hs_len - ne_len;
	/* Find a unique character pair in NE. For example, "ab" in
	 * "aaaaaaaaaaaaaaaabbbbcccc". */
	const unsigned char *p = (const unsigned char *)ne + 1;
	size_t n = ne_len - 2;
	for (const int c = *(unsigned char *)ne; n-- && *p == c; ++p) {}
	/* N must be the "ab", not the "bb". */
	n = JSTR_DIFF(p, ne) - 1;
	h += n;
	const unsigned int matchsh = ne_len < VEC_SIZE ? VEC_SIZE - ne_len : 0;
	const MASK matchm = ONES << matchsh;
	const VEC nv0 = SETONE8(*((char *)ne + n));
	const VEC nv1 = SETONE8(*((char *)ne + n + 1));
	if (JSTR_PTR_NOT_CROSSING_PAGE(ne, VEC_SIZE, JSTR_PAGE_SIZE) || ne_len == VEC_SIZE)
		nv = LOADU((const VEC *)ne);
	else
		memcpy(&nv, ne, ne_len);
	const unsigned int off_s = JSTR_DIFF(h, JSTR_PTR_ALIGN_DOWN(h, VEC_SIZE));
	unsigned int off_e = (JSTR_DIFF(end, (h - n)) < VEC_SIZE) ? VEC_SIZE - (unsigned int)(end - (h - n)) - 1 : 0;
	h -= off_s;
	hv0 = LOAD((const VEC *)h);
	hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
	hm1 = (MASK)CMPEQ8_MASK(hv0, nv1) >> 1;
	/* Clear matched bits that are out of bounds. */
	m = ((hm0 & hm1) >> off_s) & (ONES >> off_e);
	while (m) {
		i = TZCNT(m);
		m = BLSR(m);
		p = h + off_s + i - n;
		FIND_MATCH();
	}
	h += VEC_SIZE - 1;
	for (; h - n + VEC_SIZE <= end; h += VEC_SIZE) {
		hv0 = LOADU((const VEC *)h);
		hv1 = LOAD((const VEC *)(h + 1));
		hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
		hm1 = (MASK)CMPEQ8_MASK(hv1, nv1);
		m = hm0 & hm1;
		while (m) {
match:
			i = TZCNT(m);
			m = BLSR(m);
			p = h + i - n;
			FIND_MATCH();
		}
	}
	if (h - n <= end) {
		off_e = VEC_SIZE - (unsigned int)(end - (h - n)) - 1;
		hv0 = LOADU((const VEC *)h);
		hv1 = LOAD((const VEC *)(h + 1));
		hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
		hm1 = (MASK)CMPEQ8_MASK(hv1, nv1);
		m = hm0 & hm1 & (ONES >> off_e);
		if (m)
			goto match;
	}
	return NULL;
}
#endif

#undef FIND_MATCH
#undef ONES
#undef LOAD
#undef LOADU
#undef STORE
#undef STOREU
#undef CMPEQ8_MASK
#undef SETZERO
#undef SETONE8
#undef AND
#undef ADD8
#undef SUB8
#undef BLSR
#undef TZCNT
#undef LZCNT
#undef POPCNT

#undef R

JSTR__END_DECLS

#endif /* JSTR_SIMD_H */
