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

JSTR_FUNC_PURE
static char *
pjstr_strchrnul_avx2(const char *s,
                     int c)
{
	const __m256i cv = _mm256_set1_epi8(c);
	const __m256i zv = _mm256_setzero_si256();
	__m256i sv;
	unsigned int m, zm;
	for (;; s += sizeof(__m256i)) {
		sv = _mm256_loadu_si256((const __m256i *)s);
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

#endif /* PJSTR_STRCHRNUL_H* */
