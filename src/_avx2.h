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
	unsigned int m, zm;
	const __m256i cv = _mm256_set1_epi8(c);
	const __m256i zv = _mm256_setzero_si256();
	__m256i sv;
	if ((uintptr_t)s & (sizeof(__m256i) - 1)) {
		sv = _mm256_loadu_si256((const __m256i *)s);
		m = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
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
		m = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		zm = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, zv));
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
	const __m256i cv = _mm256_set1_epi8(c);
	__m256i sv;
	unsigned int m;
	uint32_t i;
	if ((uintptr_t)p & (sizeof(__m256i) - 1)) {
		p -= sizeof(__m256i);
		sv = _mm256_loadu_si256((const __m256i *)p);
		m = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		if (m) {
			i = 31 - _lzcnt_u32(m);
			if (p + i < (unsigned char *)s)
				return NULL;
			return (char *)p + i;
		}
		p = (const unsigned char *)JSTR_PTR_ALIGN_UP(p, sizeof(__m256i));
	}
	for (;;) {
		p -= sizeof(__m256i);
		sv = _mm256_load_si256((const __m256i *)p);
		m = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(sv, cv));
		if (m) {
			i = 31 - _lzcnt_u32(m);
			if (p + i < (unsigned char *)s)
				break;
			return (char *)p + i;
		}
	}
	return NULL;
}

/* May not be fast for long needles. */
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static void *
pjstr_memmem_avx2(const char *hs,
                  size_t hs_len,
                  const char *ne,
                  size_t ne_len)
{
	if (ne_len == 1)
		return (void *)memchr(hs, *(unsigned char *)ne, hs_len);
	if (jstr_unlikely(ne_len == 0))
		return (char *)hs;
	if (jstr_unlikely(hs_len == ne_len))
		return !memcmp(hs, ne, ne_len) ? (void *)hs : NULL;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	const __m256i nv = _mm256_set1_epi8(*(char *)ne);
	const char *const end = hs + hs_len - ne_len - sizeof(__m256i);
	unsigned int m;
	uint32_t i;
	__m256i hv;
	for (;; hs += sizeof(__m256i)) {
		hv = _mm256_loadu_si256((const __m256i *)hs);
		m = (unsigned int)_mm256_movemask_epi8(_mm256_cmpeq_epi8(hv, nv));
		while (m) {
			i = _tzcnt_u32(m);
			if (jstr_unlikely(hs + i > end))
				return NULL;
			if (!memcmp(hs + i, ne, ne_len))
				return (char *)hs + i;
			m = _blsr_u32(m);
		}
		hs += sizeof(__m256i);
		if (jstr_unlikely(hs > end))
			return NULL;
	}
	return NULL;
}

#endif /* PJSTR_STRCHRNUL_H* */
