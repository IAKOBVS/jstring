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
   SOFTWARE.

   MIT License (Expat) */

#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-stdstring.h"

PJSTR_BEGIN_DECLS
#include <stdint.h>
#include <string.h>
PJSTR_END_DECLS

#ifndef PJSTR_RAREBYTE_RETTYPE
#	define PJSTR_RAREBYTE_RETTYPE void *
#endif
#ifndef PJSTR_RAREBYTE_FUNC
#	define PJSTR_RAREBYTE_FUNC pjstr_memmem
#endif

#if !defined PJSTR_RAREBYTE_CMP_FUNC && JSTR_HAVE_UNALIGNED_ACCESS
#	define PJSTR_RAREBYTE_CMP_FUNC memcmp
#	define USE_UNALIGNED           1
#	if JSTR_ENDIAN_LITTLE
#		define SH <<
#	elif JSTR_ENDIAN_BIG
#		define SH >>
#	else
#		error "Can't detect endianness!"
#	endif
#	define TOWORD32(x) ((uint32_t)(x)[3] SH 24 | (uint32_t)(x)[2] SH 16 | (uint32_t)(x)[1] SH 8 | (uint32_t)(x)[0])
#	define TOWORD64(x) (uint64_t)((uint64_t)(x)[7] SH 56 | (uint64_t)(x)[6] SH 48 | (uint64_t)(x)[5] SH 40 | (uint64_t)(x)[4] SH 32 | (uint64_t)TOWORD32((x)))
#	if JSTR_HAVE_ATTR_MAY_ALIAS
#		define EQ64(hs, ne_align) (*(u64 *)(hs) == ne_align)
#		define EQ32(hs, ne_align) (*(u32 *)(hs) == (uint32_t)ne_align)
#	else
#		define EQ64(hs, ne_align, ne_len) !memcmp(hs, &(ne_align), 8)
#		define EQ32(hs, ne_align, ne_len) !memcmp(hs, &(ne_align), 4)
#	endif
#endif
#define CMP_FUNC PJSTR_RAREBYTE_CMP_FUNC

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static PJSTR_RAREBYTE_RETTYPE
PJSTR_RAREBYTE_FUNC(const unsigned char *hs,
                    size_t hs_len,
                    const unsigned char *ne,
                    size_t ne_len,
                    const unsigned char *const rarebyte)
{
#if USE_UNALIGNED
	typedef uint32_t u32 JSTR_ATTR_MAY_ALIAS;
	typedef uint64_t u64 JSTR_ATTR_MAY_ALIAS;
#endif
	typedef unsigned char u;
	int c;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, ne);
	c = *(u *)rarebyte;
	hs += idx;
#if USE_UNALIGNED
	u64 ne_align;
	const unsigned char *nelast;
	size_t nelast_len;
	if (ne_len < 8) {
		nelast = ne + 4;
		nelast_len = ne_len - 4;
		if (JSTR_HAVE_ATTR_MAY_ALIAS)
			ne_align = (u64) * (u32 *)ne;
		else
			ne_align = (u64)TOWORD32(ne);
	} else {
		nelast = ne + 8;
		nelast_len = ne_len - 8;
		if (JSTR_HAVE_ATTR_MAY_ALIAS)
			ne_align = *(u64 *)ne;
		else
			ne_align = TOWORD64(ne);
	}
#endif
	const unsigned char *end = (u *)hs + hs_len - ne_len + 1;
	for (; (hs = (const u *)memchr(hs, c, end - hs)); ++hs) {
#if USE_UNALIGNED
		/* If CMP_FUNC is memcmp(), quickly compare first 4/8 bytes before calling memcmp(). */
		if (ne_len < 8) {
			if (EQ32(hs - idx, ne_align) && !jstr_memcmpeq_loop(hs - idx + 4, nelast, nelast_len))
				return (char *)hs - idx;
		} else {
			if (EQ64(hs - idx, ne_align) && !memcmp(hs - idx + 8, nelast, nelast_len))
				return (char *)hs - idx;
		}
#else
		if (!CMP_FUNC((char *)hs - idx, (char *)ne, ne_len))
			return (PJSTR_RAREBYTE_RETTYPE)(hs - idx);
#endif
	}
	return NULL;
}

#undef USE_MEMCMP
#undef USE_UNALIGNED
#undef CMP_FUNC
#undef EQ32
#undef EQ64
#undef TOWORD64
#undef TOWORD32
#undef SH
#undef PJSTR_RAREBYTE_FUNC
#undef PJSTR_RAREBYTE_CMP_FUNC
#undef PJSTR_RAREBYTE_RETTYPE
