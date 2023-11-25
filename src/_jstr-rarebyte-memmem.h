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

PJSTR_BEGIN_DECLS
#include <string.h>
PJSTR_END_DECLS

#ifndef PJSTR_RAREBYTE_RETTYPE
#	define PJSTR_RAREBYTE_RETTYPE void *
#endif
#ifndef PJSTR_RAREBYTE_FUNC
#	define PJSTR_RAREBYTE_FUNC pjstr_memmem
#endif
#ifndef PJSTR_RAREBYTE_CMP_FUNC
#	define PJSTR_RAREBYTE_CMP_FUNC memcmp
#endif
#define CMP_FUNC PJSTR_RAREBYTE_CMP_FUNC

#if JSTR_ENDIAN_LITTLE
#	define SH <<
#elif JSTR_ENDIAN_BIG
#	define SH >>
#else
#	error "Can't detect endianness!"
#endif
#define TOWORD32(x) ((u64)x[3] SH 24 | (u64)x[2] SH 16 | (u64)x[1] SH 8 | (u64)x[0])
#define TOWORD64(x) ((u64)x[7] SH 56 | (u64)x[6] SH 48 | (u64)x[5] SH 40 | (u64)x[4] SH 32 | TOWORD32(x))

#if JSTR_HAVE_UNALIGNED_ACCESS
#	if JSTR_HAVE_ATTR_MAY_ALIAS
#		define EQ(hs, ne_align, ne_len) (ne_len < 8) ? ((u32 *)hs == ne_align) : ((u64 *)hs == ne_align)
#	else
#		define EQ(hs, ne_align, ne_len) !memcmp(hs, &(ne_align), (ne_len < 8) ? 4 : 8)
#	endif
#endif

JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static PJSTR_RAREBYTE_RETTYPE
PJSTR_RAREBYTE_FUNC(const unsigned char *JSTR_RESTRICT h,
                    size_t h_len,
                    const unsigned char *JSTR_RESTRICT n,
                    size_t n_len,
                    const unsigned char *rarebyte)
{
#ifndef PJSTR_RAREBYTE_CMP_FUNC
	typedef uint32_t u32 JSTR_ATTR_MAY_ALIAS;
	typedef uint64_t u64 JSTR_ATTR_MAY_ALIAS;
#endif
	typedef unsigned char u;
	if (jstr_unlikely(h_len < n_len))
		return NULL;
	int c;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, n);
	c = *(u *)rarebyte;
	h += idx;
#ifndef PJSTR_RAREBYTE_CMP_FUNC
	const int skip_bytes = (ne_len < 8) ? 4 : 8;
	u64 ne_align;
#	if JSTR_HAVE_ATTR_MAY_ALIAS
	ne_align = (ne_len < 8) ? (u32 *)ne : (u64 *)ne;
#	else
	ne_align = (ne_len < 8) ? TOWORD32(ne) : TOWORD64(ne);
#	endif
#endif
	const unsigned char *end = (u *)h + h_len - n_len + 1;
	for (; (h = (const u *)memchr(h, c, end - h)); ++h)
#ifndef PJSTR_RAREBYTE_CMP_FUNC
		if (EQ(h - idx, ne_align, n_len))
			if (!CMP_FUNC((char *)h - idx, (char *)n + skip_bytes, nlast_len - skip_bytes))
				return (PJSTR_RAREBYTE_RETTYPE)h - idx;
#else
		if (!CMP_FUNC((char *)h - idx, (char *)n, n_len))
			return (PJSTR_RAREBYTE_RETTYPE)(h - idx);
#endif
	return NULL;
}

#undef CMP_FUNC
#undef EQ
#undef TOWORD64
#undef TOWORD32
#undef SH
#undef PJSTR_RAREBYTE_FUNC
#undef PJSTR_RAREBYTE_CMP_FUNC
