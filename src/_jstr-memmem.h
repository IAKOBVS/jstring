/* See LICENSE file for copyright and license details. */

#include "jstr-macros.h"

/* The programmer is expected to check if HAYSTACKLEN >= NEEDLELEN.
   Before including this file, define:
   PJSTR_MEMMEM_FN         Unique name for the function.
   PJSTR_MEMMEM_RETTYPE    Return type.
   PJSTR_MEMMEM_HASH2      Hash function.
   PJSTR_MEMMEM_CHECK_EOL  If HAYSTACKLEN is not constant, as in the case of strstr, we are going to check for the end of the HAYSTACK.
   PJSTR_MEMMEM_CMP_FN     Comparison function used to compare HAYSTACK with NEEDLE.
   If the above macros are not defined, it will default to memmem. */

#define PJSTR_MEMMEM_FN_SHORT JSTR_CONCAT(PJSTR_MEMMEM_FN, _short_ne)
#define PJSTR_MEMMEM_FN_LONG  JSTR_CONCAT(PJSTR_MEMMEM_FN, _long_ne)

#define PJSTR_MEMMEM_SHORT_NEEDLE 1
#include "_jstr-memmem-impl.h"

PJSTR_BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_INLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FN_SHORT(const unsigned char *hs,
                      const size_t hl,
                      const unsigned char *ne,
                      const size_t nl)
JSTR_NOEXCEPT
{
	return PJSTR_MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

#undef PJSTR_MEMMEM_SHORT_NEEDLE
#include "_jstr-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_NOINLINE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FN_LONG(const unsigned char *hs,
                     const size_t hl,
                     const unsigned char *ne,
                     const size_t nl)
JSTR_NOEXCEPT
{
	return PJSTR_MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

JSTR_FUNC_PURE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FN(const PJSTR_MEMMEM_RETTYPE hs,
                const size_t hl,
                const PJSTR_MEMMEM_RETTYPE ne,
                const size_t nl)
{
	if (jstr_likely(nl < 257))
		return PJSTR_MEMMEM_FN_SHORT((unsigned char *)hs, hl, (unsigned char *)ne, nl);
	return PJSTR_MEMMEM_FN_LONG((unsigned char *)hs, hl, (unsigned char *)ne, nl);
}

PJSTR_END_DECLS

#undef PJSTR_MEMMEM_FN
#undef PJSTR_MEMMEM_RETTYPE
#undef PJSTR_MEMMEM_HASH2
#undef PJSTR_MEMMEM_HASH2_ICASE
#undef PJSTR_MEMMEM_HASH2_SETUP
#undef PJSTR_MEMMEM_CHECK_EOL
#undef PJSTR_MEMMEM_CMP_FN
