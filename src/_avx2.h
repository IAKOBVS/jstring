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

#include <immintrin.h>

#include "jstr-ptr-arith.h"
#include "jstr-rarebyte.h"
#include "jstr-stdstring.h"

/* Unrolling doesn't seem to be worth it. */
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strchrnul_avx2(const char *s,
                     int c)
{
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, sizeof(__m256i)); ++s)
		if (jstr_unlikely(*s == '\0') || *s == (char)c)
			return (char *)s;
	uint32_t m, m1, zm;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8((char)c);
	const __m256i zv = _mm256_setzero_si256();
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)s);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		m1 = m | zm;
		if (m1)
			break;
	}
	return (char *)s + _tzcnt_u32(m1);
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strcasechrnul_avx2(const char *s,
                         int c)
{
	if (!jstr_isalpha(c))
#if JSTR_HAVE_STRCHRNUL
		return (char *)strchrnul(s, c);
#else
		return pjstr_strchrnul_avx2(s, c);
#endif
	c = jstr_tolower(c);
	for (; JSTR_PTR_IS_NOT_ALIGNED(s, sizeof(__m256i)); ++s)
		if (jstr_unlikely(*s == '\0') || jstr_tolower(*s) == c)
			return (char *)s;
	uint32_t m, m1, m2, zm;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8((char)c);
	const __m256i cv1 = _mm256_set1_epi8((char)jstr_toupper(c));
	const __m256i zv = _mm256_setzero_si256();
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)s);
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		m2 = m | m1 | zm;
		if (m2)
			break;
	}
	return (char *)s + _tzcnt_u32(m2);
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_INLINE
static char *
pjstr_strcasechr_avx2(const char *s,
                      int c)
{
	s = pjstr_strcasechrnul_avx2(s, c);
	return *s == (char)c ? (char *)s : NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memcasechr_avx2(const void *s,
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, sizeof(__m256i)); ++p) {
		if (jstr_unlikely(p >= end))
			return NULL;
		if (jstr_tolower(*p) == c)
			return (void *)p;
	}
	uint32_t m, m1, m2;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8((char)c);
	const __m256i cv1 = _mm256_set1_epi8((char)jstr_toupper(c));
	for (; p < end; p += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		m2 = m | m1;
		if (m2)
			goto ret;
	}
	return NULL;
ret:;
	const uint32_t i = _tzcnt_u32(m2);
	return p + i < end ? (char *)p + i : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memrchr_avx2(const void *s,
                   int c,
                   size_t n)
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (unsigned char *)s + n;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, sizeof(__m256i));) {
		--p;
		if (jstr_unlikely(p < (unsigned char *)s))
			return NULL;
		if (*p == (unsigned char)c)
			return (void *)p;
	}
	uint32_t m;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8((char)c);
	while (p >= (unsigned char *)s) {
		p -= sizeof(__m256i);
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		if (m)
			goto ret;
	}
	return NULL;
ret:;
	const uint32_t i = 31 - _lzcnt_u32(m);
	return p + i >= (unsigned char *)s ? (char *)p + i : NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
pjstr_memmem_avx2(const void *hs,
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
	for (; JSTR_PTR_IS_NOT_ALIGNED(h, sizeof(__m256i)); ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (*h == *((unsigned char *)ne + shift) && !memcmp(h - shift, ne, ne_len))
			return (void *)(h - shift);
	}
	const __m256i nv = _mm256_set1_epi8(*((char *)ne + shift));
	const __m256i nv1 = _mm256_set1_epi8(*((char *)ne + shift + 1));
	__m256i hv, hv1;
	uint32_t i, m, m1, m2;
	for (; h - shift <= end; h += sizeof(__m256i)) {
		hv = _mm256_load_si256((const __m256i *)h);
		hv1 = _mm256_loadu_si256((const __m256i *)(h + 1));
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv1, nv1));
		m2 = m & m1;
		while (m2) {
			i = _tzcnt_u32(m2);
			m2 = _blsr_u32(m2);
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
static void *
pjstr_strcasestr_len_avx2(const void *hs,
                          size_t hs_len,
                          const void *ne,
                          size_t ne_len)
{
	if (ne_len == 1)
		return (void *)pjstr_memcasechr_avx2(hs, *(unsigned char *)ne, hs_len);
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
	for (const int c = jstr_tolower(*((unsigned char *)ne + shift));
	     JSTR_PTR_IS_NOT_ALIGNED(h, sizeof(__m256i));
	     ++h) {
		if (jstr_unlikely(h - shift > end))
			return NULL;
		if (jstr_tolower(*h) == c && !jstr_strcasecmpeq_len((char *)h - shift, (char *)ne, ne_len))
			return (void *)(h - shift);
	}
	const __m256i nv = _mm256_set1_epi8((char)jstr_tolower(*((unsigned char *)ne + shift)));
	const __m256i nv1 = _mm256_set1_epi8((char)jstr_toupper(*((unsigned char *)ne + shift)));
	const __m256i nv2 = _mm256_set1_epi8((char)jstr_tolower(*((unsigned char *)ne + shift + 1)));
	const __m256i nv3 = _mm256_set1_epi8((char)jstr_toupper(*((unsigned char *)ne + shift + 1)));
	__m256i hv, hv1;
	uint32_t i, m, m1, m2, m3, m4;
	for (; h - shift <= end; h += sizeof(__m256i)) {
		hv = _mm256_load_si256((const __m256i *)h);
		hv1 = _mm256_loadu_si256((const __m256i *)(h + 1));
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv1));
		m2 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv1, nv2));
		m3 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv1, nv3));
		m4 = (m | m1) & (m2 | m3);
		while (m4) {
			i = _tzcnt_u32(m4);
			m4 = _blsr_u32(m4);
			if (jstr_unlikely(h + i - shift > end))
				return NULL;
			if (!jstr_strcasecmpeq_len((char *)h + i - shift, (char *)ne, ne_len))
				return (char *)h + i - shift;
		}
	}
	return NULL;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static size_t
pjstr_countchr_avx2(const void *s,
                    int c)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, sizeof(__m256i));) {
		if (jstr_unlikely(*p == '\0'))
			return cnt;
		cnt += *p++ == (unsigned char)c;
	}
	const __m256i cv = _mm256_set1_epi8((char)c);
	const __m256i zv = _mm256_setzero_si256();
	__m256i sv;
	uint32_t m, zm;
	unsigned int cnt0;
	for (;; p += sizeof(__m256i), cnt += cnt0) {
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		cnt0 = m ? (unsigned int)_mm_popcnt_u32(m) : 0;
		if (zm)
			break;
	}
	while (*p)
		cnt += *p++ == (unsigned char)c;
	return cnt;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static size_t
pjstr_countchr_len_avx2(const void *s,
                        int c,
                        size_t n)
{
	const unsigned char *p = (const unsigned char *)s;
	size_t cnt = 0;
	for (; JSTR_PTR_IS_NOT_ALIGNED(p, sizeof(__m256i));) {
		if (jstr_unlikely(n-- == 0))
			return cnt;
		cnt += *p++ == (unsigned char)c;
	}
	const __m256i cv = _mm256_set1_epi8((char)c);
	__m256i sv;
	uint32_t m;
	for (; n >= sizeof(__m256i); n -= sizeof(__m256i), p += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		cnt += m ? (unsigned int)_mm_popcnt_u32(m) : 0;
	}
	while (n--)
		cnt += *p++ == (unsigned char)c;
	return cnt;
}

PJSTR_END_DECLS

#endif /* PJSTR_AVX2_H* */
