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

#include <immintrin.h>
#include "jstr-macros.h"
#include "jstr-ptr-arith.h"
#include "jstr-rarebyte.h"

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strchrnul_avx2(const char *s,
                     int c)
{
	if (*s == (char)c || jstr_unlikely(*s == '\0'))
		return (char *)s;
	uint32_t m, zm, i, iz;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8(c);
	const __m256i zv = _mm256_setzero_si256();
	if ((uintptr_t)s & (sizeof(__m256i) - 1)) {
		sv = _mm256_loadu_si256((const __m256i *)s);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		i = _tzcnt_u32(m);
		iz = _tzcnt_u32(zm);
		if (m)
			return (char *)s + (zm ? JSTR_MIN(i, iz) : i);
		else if (zm)
			return (char *)s + iz;
		s = (const char *)JSTR_PTR_ALIGN_UP(s, sizeof(__m256i));
	}
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)s);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		i = _tzcnt_u32(m);
		iz = _tzcnt_u32(zm);
		if (m)
			return (char *)s + (zm ? JSTR_MIN(i, iz) : i);
		else if (zm)
			return (char *)s + iz;
	}
	return (char *)s + iz;
}

JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static char *
pjstr_strcasechrnul_avx2(const char *s,
                         int c)
{
	if (!jstr_isalpha(c))
		return
#if JSTR_HAVE_STRCHRNUL
		(char *)strchrnul(s, c);
#else
		pjstr_strchrnul_avx2(s, c);
#endif
	c = jstr_tolower(c);
	if (jstr_unlikely(*s == '\0') || jstr_tolower(*s) == c)
		return (char *)s;
	uint32_t m, m1, m2, zm, i, iz;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8((char)c);
	const __m256i cv1 = _mm256_set1_epi8((char)jstr_toupper(c));
	const __m256i zv = _mm256_setzero_si256();
	if ((uintptr_t)s & (sizeof(__m256i) - 1)) {
		sv = _mm256_loadu_si256((const __m256i *)s);
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		iz = _tzcnt_u32(zm);
		m2 = m | m1;
		i = _tzcnt_u32(m2);
		if (m)
			return (char *)s + (zm ? JSTR_MIN(i, iz) : i);
		if (zm)
			return (char *)s + iz;
		s = (const char *)JSTR_PTR_ALIGN_UP(s, sizeof(__m256i));
	}
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)s);
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		iz = _tzcnt_u32(zm);
		m2 = m | m1;
		i = _tzcnt_u32(m2);
		if (m)
			return (char *)s + (zm ? JSTR_MIN(i, iz) : i);
		if (zm)
			return (char *)s + iz;
	}
	return (char *)s + iz;
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
	if (jstr_tolower(*p) == c)
		return (char *)p;
	const unsigned char *const end = p + n;
	uint32_t m, m1, m2, i;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8(c);
	const __m256i cv1 = _mm256_set1_epi8((char)jstr_toupper(c));
	if ((uintptr_t)p & (sizeof(__m256i) - 1)) {
		sv = _mm256_loadu_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		m2 = m | m1;
		i = _tzcnt_u32(m2);
		if (m2)
			return p + i < end ? (char *)p + i : NULL;
		p = (const unsigned char *)JSTR_PTR_ALIGN_UP(p, sizeof(__m256i));
	}
	for (; p < end; p += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		m1 = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv1));
		m2 = m | m1;
		i = _tzcnt_u32(m2);
		if (m2)
			return p + i < end ? (char *)p + i : NULL;
	}
	return NULL;
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
	if (*--p == (unsigned char)c)
		return (void *)p;
	uint32_t i, m;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8(c);
	if ((uintptr_t)p & (sizeof(__m256i) - 1)) {
		p -= sizeof(__m256i);
		sv = _mm256_loadu_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		i = 31 - _lzcnt_u32(m);
		if (m)
			return p + i >= (unsigned char *)s ? (char *)p + i : NULL;
		p = (const unsigned char *)JSTR_PTR_ALIGN_DOWN(p, sizeof(__m256i));
	}
	while (p >= (unsigned char *)s) {
		p -= sizeof(__m256i);
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		i = 31 - _lzcnt_u32(m);
		if (m)
			return p + i >= (unsigned char *)s ? (char *)p + i : NULL;
	}
	return NULL;
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
	if (jstr_unlikely(hs_len == ne_len))
		return !memcmp(hs, ne, ne_len) ? (void *)hs : NULL;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *const end = h + hs_len - ne_len;
	size_t shift = JSTR_PTR_DIFF(jstr_rarebytefind_len(ne, ne_len), ne);
	if (shift == ne_len - 1)
		--shift;
	const __m256i nv = _mm256_set1_epi8(*((char *)ne + shift));
	const __m256i nv1 = _mm256_set1_epi8(*((char *)ne + shift + 1));
	__m256i hv, hv1;
	uint32_t i, m, m1, m2;
	h += shift;
	if ((uintptr_t)h & (sizeof(__m256i) - 1)) {
		hv = _mm256_loadu_si256((const __m256i *)h);
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
		h = (const unsigned char *)JSTR_PTR_ALIGN_UP(h, sizeof(__m256i));
	}
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

#endif /* PJSTR_AVX2_H* */
