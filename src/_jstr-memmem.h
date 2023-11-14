/* See LICENSE file for copyright and license details. */

#include "jstr-macros.h"

/* The programmer is expected to check if HAYSTACKLEN >= NEEDLELEN.
   Before including this file, define:
   JSTRP__MEMMEM_FN         Unique name for the function.
   JSTRP__MEMMEM_RETTYPE    Return type.
   JSTRP__MEMMEM_HASH2      Hash function.
   JSTRP__MEMMEM_CHECK_EOL  If HAYSTACKLEN is not constant, as in the case of strstr, we are going to check for the end of the HAYSTACK.
   JSTRP__MEMMEM_CMP_FN     Comparison function used to compare HAYSTACK with NEEDLE.
   If the above macros are not defined, it will default to memmem. */

#define JSTRP__MEMMEM_FN_SHORT JSTR_CONCAT(JSTRP__MEMMEM_FN, _short_ne)
#define JSTRP__MEMMEM_FN_LONG  JSTR_CONCAT(JSTRP__MEMMEM_FN, _long_ne)

#define JSTRP__MEMMEM_SHORT_NEEDLE 1
#include "_jstr-memmem-impl.h"

JSTRP__BEGIN_DECLS

JSTR_FUNC_PURE
JSTR_INLINE
static JSTRP__MEMMEM_RETTYPE
JSTRP__MEMMEM_FN_SHORT(const unsigned char *hs,
                      const size_t hl,
                      const unsigned char *ne,
                      const size_t nl)
JSTR_NOEXCEPT
{
	return JSTRP__MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

#undef JSTRP__MEMMEM_SHORT_NEEDLE
#include "_jstr-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_NOINLINE
static JSTRP__MEMMEM_RETTYPE
JSTRP__MEMMEM_FN_LONG(const unsigned char *hs,
                     const size_t hl,
                     const unsigned char *ne,
                     const size_t nl)
JSTR_NOEXCEPT
{
	return JSTRP__MEMMEM_FN_IMPL(hs, hl, ne, nl);
}

JSTR_FUNC_PURE
static JSTRP__MEMMEM_RETTYPE
JSTRP__MEMMEM_FN(const JSTRP__MEMMEM_RETTYPE hs,
                const size_t hl,
                const JSTRP__MEMMEM_RETTYPE ne,
                const size_t nl)
{
	if (jstr_likely(nl < 257))
		return JSTRP__MEMMEM_FN_SHORT((unsigned char *)hs, hl, (unsigned char *)ne, nl);
	return JSTRP__MEMMEM_FN_LONG((unsigned char *)hs, hl, (unsigned char *)ne, nl);
}

JSTRP__END_DECLS

#undef JSTRP__MEMMEM_FN
#undef JSTRP__MEMMEM_RETTYPE
#undef JSTRP__MEMMEM_HASH2
#undef JSTRP__MEMMEM_HASH2_ICASE
#undef JSTRP__MEMMEM_HASH2_SETUP
#undef JSTRP__MEMMEM_CHECK_EOL
#undef JSTRP__MEMMEM_CMP_FN
