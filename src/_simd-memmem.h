/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2025 James Tirta Halim <tirtajames45 at gmail dot com>
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

#include "jstr-macros.h"

JSTR__BEGIN_DECLS
#include <immintrin.h>
#include <string.h>
JSTR__END_DECLS

#ifndef VEC
#	define VEC __m256i
#endif
#ifndef VEC_SIZE
#	define VEC_SIZE sizeof(VEC)
#endif
#ifndef MASK
#	define MASK uint32_t
#endif
#ifndef LOAD
#	define LOAD(x) _mm256_load_si256(x)
#endif
#ifndef LOADU
#	define LOADU(x) _mm256_loadu_si256(x)
#endif
#ifndef CMPEQ8_MASK
#	define CMPEQ8_MASK(x, y) _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y))
#endif
#ifndef SETONE8
#	define SETONE8(x) _mm256_set1_epi8(x)
#endif
#ifndef TZCNT
#	define TZCNT(x) _tzcnt_u32(x)
#endif
#ifndef BLSR
#	define BLSR(x) _blsr_u32(x)
#endif
#ifndef JSTR__SIMD_RETTYPE
#	define JSTR__SIMD_RETTYPE void *
#endif
#define ONES ((MASK) - 1)

#ifndef JSTR__SIMD_MEMMEM_FUNC_NAME
#	define JSTR__SIMD_MEMMEM_FUNC_NAME jstr__simd_memmem
#endif
#ifndef JSTR__SIMD_MEMMEM_CMP_FUNC
#	define JSTR__SIMD_MEMMEM_CMP_FUNC memcmp
#endif

#define FIND_MATCH()                                                                                \
	do {                                                                                        \
		if (JSTR_PTR_NOT_CROSSING_PAGE(p, VEC_SIZE, JSTR_PAGE_SIZE)) {                      \
			hv = LOADU((const VEC *)p);                                                 \
			cmpm = (MASK)CMPEQ8_MASK(hv, nv) << matchsh;                                \
			if (cmpm == matchm)                                                         \
				return (ret_ty)p;                                                   \
		} else {                                                                            \
			if (!JSTR__SIMD_MEMMEM_CMP_FUNC((const char *)p, (const char *)ne, ne_len)) \
				return (ret_ty)p;                                                   \
		}                                                                                   \
	} while (0)

JSTR__BEGIN_DECLS

/* ne_len must be <= VEC_SIZE.
 * Worst case: O(n).
 * Best case: O(n / VEC_SIZE). */
JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
static JSTR__SIMD_RETTYPE
JSTR__SIMD_MEMMEM_FUNC_NAME(const void *hs, size_t hs_len, const void *ne, size_t ne_len)
JSTR_NOEXCEPT
{
	typedef JSTR__SIMD_RETTYPE ret_ty;
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
	const unsigned char *const end = h + hs_len - ne_len;
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
	if (JSTR_PTR_NOT_CROSSING_PAGE(ne, VEC_SIZE, JSTR_PAGE_SIZE) || ne_len >= VEC_SIZE)
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

JSTR__END_DECLS

/* So that these macros will be namespaced by the build script.
#undef ONES
#undef VEC
#undef VEC_SIZE
#undef MASK
#undef LOAD
#undef LOADU
#undef CMPEQ8_MASK
#undef SETONE8
#undef TZCNT
#undef BLSR
*/

#undef JSTR__SIMD_MEMMEM_FUNC_NAME
#undef JSTR__SIMD_MEMMEM_CMP_FUNC
#undef JSTR__SIMD_RETTYPE
#undef FIND_MATCH
