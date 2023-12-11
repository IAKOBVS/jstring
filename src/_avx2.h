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

#ifndef PJSTR_STRCHRNUL_H
#define PJSTR_STRCHRNUL_H

#include <immintrin.h>

#include "jstr-macros.h"
#include "jstr-ptr-arith.h"
#include "jstr-rarebyte.h"

JSTR_FUNC_PURE
static char *
pjstr_strchrnul_avx2(const char *s,
                     int c)
{
	if (*s == (char)c || jstr_unlikely(*s == '\0'))
		return (char *)s;
	uint32_t m, zm;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8(c);
	const __m256i zv = _mm256_setzero_si256();
	if ((uintptr_t)s & (sizeof(__m256i) - 1)) {
		sv = _mm256_loadu_si256((const __m256i *)s);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		if (m) {
			m = _tzcnt_u32(m);
			if (zm) {
				zm = _tzcnt_u32(zm);
				if (jstr_unlikely(m > zm))
					return (char *)s + zm;
			}
			return (char *)s + m;
		} else if (zm) {
			zm = _tzcnt_u32(zm);
			return (char *)s + zm;
		}
		s = (const char *)JSTR_PTR_ALIGN_UP(s, sizeof(__m256i));
	}
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_load_si256((const __m256i *)s);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
		if (m) {
			m = _tzcnt_u32(m);
			if (zm) {
				zm = _tzcnt_u32(zm);
				if (jstr_unlikely(m > zm))
					break;
			}
			return (char *)s + m;
		} else if (zm) {
			zm = _tzcnt_u32(zm);
			break;
		}
	}
	return (char *)s + zm;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 3))
JSTR_FUNC_PURE
static void *
pjstr_memrchr_avx2(const void *s,
                   int c,
                   size_t n)
{
	if (jstr_unlikely(n == 0))
		return NULL;
	const unsigned char *p = (unsigned char *)s + n - 1;
	if (*p == (unsigned char)c)
		return (void *)p;
	--p;
	uint32_t i, m;
	__m256i sv;
	const __m256i cv = _mm256_set1_epi8(c);
	if ((uintptr_t)p & (sizeof(__m256i) - 1)) {
		p -= sizeof(__m256i);
		sv = _mm256_loadu_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		if (m) {
			i = 31 - _lzcnt_u32(m);
			if (p + i < (unsigned char *)s)
				return NULL;
			return (char *)p + i;
		}
		p = (const unsigned char *)JSTR_PTR_ALIGN_DOWN(p, sizeof(__m256i));
	}
	for (;;) {
		p -= sizeof(__m256i);
		sv = _mm256_load_si256((const __m256i *)p);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		if (m) {
			i = 31 - _lzcnt_u32(m);
			if (p + i < (unsigned char *)s)
				break;
			return (char *)p + i;
		}
	}
	return NULL;
}

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
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
	__m256i nv = _mm256_set1_epi8(*(char *)ne);
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *n = (const unsigned char *)ne;
	const unsigned char *const end = h + hs_len - ne_len;
	const int c1 = *(n + 1);
	n += 2, ne_len -= 2;
	__m256i hv;
	uint32_t i, m;
	if ((uintptr_t)h & (sizeof(__m256i) - 1)) {
		hv = _mm256_loadu_si256((const __m256i *)h);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv));
		for (; m; m = _blsr_u32(m)) {
			i = _tzcnt_u32(m);
			if (jstr_unlikely(h + i > end))
				return NULL;
			if (*(h + i + 1) == c1 && !memcmp(h + i + 2, n, ne_len))
				return (char *)h + i;
		}
		h += sizeof(__m256i);
		if (jstr_unlikely(h > end))
			return NULL;
		h = (const unsigned char *)JSTR_PTR_ALIGN_DOWN(h, sizeof(__m256i));
	}
	for (;;) {
		hv = _mm256_load_si256((const __m256i *)h);
		m = (uint32_t)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv));
		for (; m; m = _blsr_u32(m)) {
			i = _tzcnt_u32(m);
			if (jstr_unlikely(h + i > end))
				return NULL;
			if (*(h + i + 1) == c1 && !memcmp(h + i + 2, n, ne_len))
				return (char *)h + i;
		}
		h += sizeof(__m256i);
		if (jstr_unlikely(h > end))
			return NULL;
	}
	return NULL;
}

#endif /* PJSTR_STRCHRNUL_H* */
