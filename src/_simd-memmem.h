/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#include "jstr-macros.h"
#include "jstr-rarebyte.h"

PJSTR_BEGIN_DECLS
#include <immintrin.h>
PJSTR_END_DECLS

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

#ifndef PJSTR_SIMD_MEMMEM_FUNC_NAME
#	define PJSTR_SIMD_MEMMEM_FUNC_NAME pjstr_simd_memmem
#endif
#ifndef PJSTR_SIMD_MEMMEM_CMP_FUNC
#	define PJSTR_SIMD_MEMMEM_CMP_FUNC memcmp
#endif
#ifndef PJSTR_SIMD_MEMMEM_MEMCASECHR
#	define PJSTR_SIMD_MEMMEM_MEMCASECHR pjstr_memcasechr_simd
#endif

#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
#	define OR_UPPER_MASK(x) | (x)
#else
#	define OR_UPPER_MASK(x)
#endif

PJSTR_BEGIN_DECLS

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_NO_SANITIZE_ADDRESS
static void *
PJSTR_SIMD_MEMMEM_FUNC_NAME(const void *hs,
                            size_t hs_len,
                            const void *ne,
                            size_t ne_len)
JSTR_NOEXCEPT
{
	if (ne_len == 1)
#ifndef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
		return (void *)memchr(hs, *(unsigned char *)ne, hs_len);
#else
		return (void *)PJSTR_SIMD_MEMMEM_MEMCASECHR(hs, *(unsigned char *)ne, hs_len);
#endif
	if (jstr_unlikely(ne_len == 0))
		return (void *)hs;
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
	VEC hv0, hv1;
	MASK i, hm0, hm1, m;
#ifndef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
	VEC hv, nv;
	MASK cmpm;
#else
	MASK hm0u, hm1u;
#endif
	const unsigned char *h = (const unsigned char *)hs;
	const unsigned char *const end = h + hs_len - ne_len;
	const unsigned char *hp;
	size_t shift = JSTR_PTR_DIFF(jstr_rarebytefind_len(ne, ne_len), ne);
	if (shift == ne_len - 1)
		--shift;
	h += shift;
#ifndef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
	const unsigned int matchsh = ne_len < VEC_SIZE ? VEC_SIZE - ne_len : 0;
	const MASK matchm = (MASK)-1 << matchsh;
	const VEC nv0 = SETONE8(*((char *)ne + shift));
	const VEC nv1 = SETONE8(*((char *)ne + shift + 1));
	if (JSTR_PTR_ALIGN_UP(ne, JSTR_PAGE_SIZE) - (uintptr_t)ne >= VEC_SIZE || JSTR_PTR_IS_ALIGNED(ne, VEC_SIZE) || ne_len >= VEC_SIZE)
		nv = LOADU((VEC *)ne);
	else
		memcpy(&nv, ne, JSTR_MIN(VEC_SIZE, ne_len));
#else
	const VEC nv0 = SETONE8((char)jstr_tolower(*((unsigned char *)ne + shift)));
	const VEC nv1 = SETONE8((char)jstr_tolower(*((unsigned char *)ne + shift + 1)));
	const VEC nv0u = SETONE8((char)jstr_toupper(*((unsigned char *)ne + shift)));
	const VEC nv1u = SETONE8((char)jstr_toupper(*((unsigned char *)ne + shift + 1)));
#endif
	const unsigned int off = JSTR_PTR_DIFF(h, JSTR_PTR_ALIGN_DOWN(h, VEC_SIZE));
	unsigned int off2 = (JSTR_PTR_DIFF(end, (h - shift)) < VEC_SIZE)
	                    ? VEC_SIZE - (unsigned int)(end - (h - shift)) - 1
	                    : 0;
	h -= off;
	hv0 = LOAD((const VEC *)h);
	hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
	hm1 = (MASK)CMPEQ8_MASK(hv0, nv1) >> 1;
#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
	hm0u = (MASK)CMPEQ8_MASK(hv0, nv0u);
	hm1u = (MASK)CMPEQ8_MASK(hv0, nv1u) >> 1;
#endif
	/* Clear matched bits that are out of bounds. */
	m = ((((hm0 OR_UPPER_MASK(hm0u)) & (hm1 OR_UPPER_MASK(hm1u))) >> off) << off2) >> off2;
	while (m) {
		i = TZCNT(m);
		m = BLSR(m);
		hp = h + off + i - shift;
#ifndef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
		if (JSTR_PTR_ALIGN_UP(hp, JSTR_PAGE_SIZE) - (uintptr_t)hp >= VEC_SIZE) {
			hv = LOADU((VEC *)hp);
			cmpm = (MASK)CMPEQ8_MASK(hv, nv) << matchsh;
			if (cmpm == matchm)
				if (ne_len <= VEC_SIZE || !PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp + VEC_SIZE, (const char *)ne + VEC_SIZE, ne_len - VEC_SIZE))
					return (void *)hp;
		} else {
			if (!PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp, (const char *)ne, ne_len))
				return (void *)hp;
		}
#else
		if (!PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp, (const char *)ne, ne_len))
			return (void *)hp;
#endif
	}
	h += VEC_SIZE - 1;
	for (; h - shift + VEC_SIZE <= end; h += VEC_SIZE) {
		hv0 = LOADU((const VEC *)h);
		hv1 = LOAD((const VEC *)(h + 1));
		hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
		hm1 = (MASK)CMPEQ8_MASK(hv1, nv1);
#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
		hm0u = (MASK)CMPEQ8_MASK(hv0, nv0u);
		hm1u = (MASK)CMPEQ8_MASK(hv1, nv1u);
#endif
		m = (hm0 OR_UPPER_MASK(hm0u)) & (hm1 OR_UPPER_MASK(hm1u));
		while (m) {
match:
			i = TZCNT(m);
			m = BLSR(m);
			hp = h + i - shift;
#ifndef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
			if (JSTR_PTR_ALIGN_UP(hp, JSTR_PAGE_SIZE) - (uintptr_t)hp >= VEC_SIZE) {
				hv = LOADU((VEC *)hp);
				cmpm = (MASK)CMPEQ8_MASK(hv, nv) << matchsh;
				if (cmpm == matchm)
					if (ne_len <= VEC_SIZE || !PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp + VEC_SIZE, (const char *)ne + VEC_SIZE, ne_len - VEC_SIZE))
						return (void *)hp;
			} else {
				if (!PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp, (const char *)ne, ne_len))
					return (void *)hp;
			}
#else
			if (!PJSTR_SIMD_MEMMEM_CMP_FUNC((const char *)hp, (const char *)ne, ne_len))
				return (void *)hp;
#endif
		}
	}
	if (h - shift <= end) {
		off2 = VEC_SIZE - (unsigned int)(end - (h - shift)) - 1;
		hv1 = LOAD((const VEC *)(h + 1));
		hm1 = (MASK)CMPEQ8_MASK(hv1, nv1);
#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
		hm1u = (MASK)CMPEQ8_MASK(hv1, nv1u);
#endif
		if (JSTR_PTR_ALIGN_UP(h, JSTR_PAGE_SIZE) - (uintptr_t)h >= VEC_SIZE) {
			hv0 = LOADU((const VEC *)h);
			hm0 = (MASK)CMPEQ8_MASK(hv0, nv0);
#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
			hm0u = (MASK)CMPEQ8_MASK(hv0, nv0u);
#endif
		} else {
			hm0 = 1 | (MASK)CMPEQ8_MASK(hv1, nv0) << 1;
#ifdef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
			hm0u = 1 | (MASK)CMPEQ8_MASK(hv1, nv0u) << 1;
#endif
		}
		m = (((hm0 OR_UPPER_MASK(hm0u)) & (hm1 OR_UPPER_MASK(hm1u))) << off2) >> off2;
		if (m)
			goto match;
	}
	return NULL;
}

PJSTR_END_DECLS

/* So that these macros will be namespaced by the build script.
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

#undef PJSTR_SIMD_MEMMEM_FUNC_NAME
#undef PJSTR_SIMD_MEMMEM_CMP_FUNC
#undef PJSTR_SIMD_MEMMEM_MEMCASECHR
#undef PJSTR_SIMD_MEMMEM_USE_AS_ICASE
#undef OR_UPPER_MASK
