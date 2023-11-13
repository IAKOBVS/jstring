/* See LICENSE file for copyright and license details. */

#include "jstr-macros.h"

/* The programmer is expected to check if HAYSTACKLEN >= NEEDLELEN.
   Before including this file, define:
   JSTRP_MEMMEM_FN         Unique name for the function.
   JSTRP_MEMMEM_RETTYPE    Return type.
   JSTRP_MEMMEM_HASH2      Hash function.
   JSTRP_MEMMEM_CHECK_EOL  If HAYSTACKLEN is not constant, as in the case of strstr, we are going to check for the end of the HAYSTACK.
   JSTRP_MEMMEM_CMP_FN     Comparison function used to compare HAYSTACK with NEEDLE.
   If the above macros are not defined, it will default to memmem. */

#define JSTRP_MEMMEM_FN_SHORT JSTR_CONCAT(JSTRP_MEMMEM_FN, _short_ne)
#define JSTRP_MEMMEM_FN_LONG  JSTR_CONCAT(JSTRP_MEMMEM_FN, _long_ne)

#define JSTRP_MEMMEM_SHORT_NEEDLE 1
#include "_jstr-memmem-impl.h"

JSTRP_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_INLINE
static JSTRP_MEMMEM_RETTYPE
JSTRP_MEMMEM_FN_SHORT(const unsigned char *hs,
                      const size_t hl,
                      const unsigned char *ne,
                      const size_t nl)
JSTR_NOEXCEPT
{
	return JSTRP_MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

#undef JSTRP_MEMMEM_SHORT_NEEDLE
#include "_jstr-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_NOINLINE
static JSTRP_MEMMEM_RETTYPE
JSTRP_MEMMEM_FN_LONG(const unsigned char *hs,
                     const size_t hl,
                     const unsigned char *ne,
                     const size_t nl)
JSTR_NOEXCEPT
{
	return JSTRP_MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

JSTR_FUNC_PURE
static JSTRP_MEMMEM_RETTYPE
JSTRP_MEMMEM_FN(const JSTRP_MEMMEM_RETTYPE hs,
                const size_t hl,
                const JSTRP_MEMMEM_RETTYPE ne,
                const size_t nl)
{
	if (jstr_likely(nl < 257))
		return JSTRP_MEMMEM_FN_SHORT((unsigned char *)hs, hl, (unsigned char *)ne, nl);
	return JSTRP_MEMMEM_FN_LONG((unsigned char *)hs, hl, (unsigned char *)ne, nl);
}

JSTRP_END_DECLS

#undef JSTRP_MEMMEM_FN
#undef JSTRP_MEMMEM_RETTYPE
#undef JSTRP_MEMMEM_HASH2
#undef JSTRP_MEMMEM_HASH2_ICASE
#undef JSTRP_MEMMEM_HASH2_SETUP
#undef JSTRP_MEMMEM_CHECK_EOL
#undef JSTRP_MEMMEM_CMP_FN
