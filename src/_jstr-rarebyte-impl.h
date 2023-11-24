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

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
#if PJSTR_RAREBYTE_USE_LEN
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
#endif
static PJSTR_RAREBYTE_RETTYPE
PJSTR_RAREBYTE_FUNC(const unsigned char *JSTR_RESTRICT h,
                    size_t h_len,
                    const unsigned char *JSTR_RESTRICT n,
                    size_t n_len,
                    const unsigned char *rarebyte)
{
#if PJSTR_RAREBYTE_USE_LEN
	if (jstr_unlikely(h_len < n_len))
		return NULL;
#endif
	typedef unsigned char u;
	int c;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, n);
	c = *(u *)rarebyte;
	h += idx;
	const unsigned char *end = (u *)h + h_len - n_len + 1;
	for (; (h = (const u *)memchr(h, c, end - h)); ++h)
		if (!PJSTR_RAREBYTE_CMP_FUNC((char *)h - idx, (char *)n, n_len))
			return (PJSTR_RAREBYTE_RETTYPE)(h - idx);
	return NULL;
}

#undef PJSTR_RAREBYTE_FUNC
#undef PJSTR_RAREBYTE_CMP_FUNC
#undef PJSTR_RAREBYTE_USE_LEN
#undef PJSTR_RAREBYTE_HSLEN
#undef PJSTR_RAREBYTE_NELEN
