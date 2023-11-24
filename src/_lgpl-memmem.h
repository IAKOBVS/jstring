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

#include "jstr-macros.h"

/* The programmer is expected to check if HAYSTACKLEN >= NEEDLELEN.
   Before including this file, define:
   PJSTR_MEMMEM_FUNC         Unique name for the function.
   PJSTR_MEMMEM_RETTYPE    Return type.
   PJSTR_MEMMEM_HASH2      Hash function.
   PJSTR_MEMMEM_CHECK_EOL  If HAYSTACKLEN is not constant, as in the case of strstr, we are going to check for the end of the HAYSTACK.
   PJSTR_MEMMEM_CMP_FUNC     Comparison function used to compare HAYSTACK with NEEDLE.
   If the above macros are not defined, it will default to memmem. */

#define PJSTR_MEMMEM_FUNC_SHORT JSTR_CONCAT(PJSTR_MEMMEM_FUNC, _short_ne)
#define PJSTR_MEMMEM_FUNC_LONG  JSTR_CONCAT(PJSTR_MEMMEM_FUNC, _long_ne)

#define PJSTR_MEMMEM_SHORT_NEEDLE 1
#include "_lgpl-memmem-impl.h"

PJSTR_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FUNC_SHORT(const unsigned char *hs,
                      size_t hl,
                      const unsigned char *ne,
                      size_t nl)
JSTR_NOEXCEPT
{
	return PJSTR_MEMMEM_FUNC_IMPL(hs, hl, ne, nl);
}

#undef PJSTR_MEMMEM_SHORT_NEEDLE
#include "_lgpl-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_ATTR_NOINLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FUNC_LONG(const unsigned char *hs,
                     size_t hl,
                     const unsigned char *ne,
                     size_t nl)
JSTR_NOEXCEPT
{
	return PJSTR_MEMMEM_FUNC_IMPL(hs, hl, ne, nl);
}

JSTR_FUNC_PURE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FUNC(const PJSTR_MEMMEM_RETTYPE hs,
                size_t hl,
                const PJSTR_MEMMEM_RETTYPE ne,
                size_t nl)
{
	if (jstr_likely(nl < 257))
		return PJSTR_MEMMEM_FUNC_SHORT((const unsigned char *)hs, hl, (const unsigned char *)ne, nl);
	return PJSTR_MEMMEM_FUNC_LONG((const unsigned char *)hs, hl, (const unsigned char *)ne, nl);
}

PJSTR_END_DECLS

#undef PJSTR_MEMMEM_FUNC
#undef PJSTR_MEMMEM_RETTYPE
#undef PJSTR_MEMMEM_HASH2
#undef PJSTR_MEMMEM_HASH2_ICASE
#undef PJSTR_MEMMEM_HASH2_SETUP
#undef PJSTR_MEMMEM_CHECK_EOL
#undef PJSTR_MEMMEM_CMP_FUNC
