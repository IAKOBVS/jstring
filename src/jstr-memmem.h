#include "jstr-macros.h"

/* Before including this file, define:
   PJSTR_MEMMEM_FN         Unique name for the function.
   PJSTR_MEMMEM_RETTYPE    Return type.
   PJSTR_MEMMEM_HASH2      Hash function.
   PJSTR_MEMMEM_CHECK_EOL  If HAYSTACKLEN is not constant, as in the case of strstr, we are going to check for the end of the HAYSTACK.
   PJSTR_MEMMEM_CMP_FN     Comparison function used to compare HAYSTACK with NEEDLE.
   If the above macros are not defined, it will default to memmem. */

#define PJSTR_MEMMEM_SHORT_NEEDLE 1
#include "jstr-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_INLINE
static PJSTR_MEMMEM_RETTYPE
JSTR_CONCAT(PJSTR_MEMMEM_FN, _short)(const unsigned char *JSTR_RESTRICT hs,
				     const size_t hl,
				     const unsigned char *JSTR_RESTRICT ne,
				     const size_t nl)
JSTR_NOEXCEPT
{
	return JSTR_CONCAT(PJSTR_MEMMEM_FN, _short_impl)(hs, hl, ne, nl);
}

#undef PJSTR_MEMMEM_SHORT_NEEDLE
#include "jstr-memmem-impl.h"

JSTR_FUNC_PURE
JSTR_NOINLINE
static PJSTR_MEMMEM_RETTYPE
JSTR_CONCAT(PJSTR_MEMMEM_FN, _long)(const unsigned char *JSTR_RESTRICT hs,
				    const size_t hl,
				    const unsigned char *JSTR_RESTRICT ne,
				    const size_t nl)
JSTR_NOEXCEPT
{
	return JSTR_CONCAT(PJSTR_MEMMEM_FN, _long_impl)(hs, hl, ne, nl);
}

JSTR_FUNC_PURE
static PJSTR_MEMMEM_RETTYPE
PJSTR_MEMMEM_FN(const PJSTR_MEMMEM_RETTYPE JSTR_RESTRICT hs,
		const size_t hl,
		const PJSTR_MEMMEM_RETTYPE JSTR_RESTRICT ne,
		const size_t nl)
{
	if (jstr_likely(nl < 257))
		return JSTR_CONCAT(PJSTR_MEMMEM_FN, _short)((unsigned char *)hs, hl, (unsigned char *)ne, nl);
	return JSTR_CONCAT(PJSTR_MEMMEM_FN, _long)((unsigned char *)hs, hl, (unsigned char *)ne, nl);
}

#undef PJSTR_MEMMEM_FN
#undef PJSTR_MEMMEM_RETTYPE
#undef PJSTR_MEMMEM_HASH2
#undef PJSTR_MEMMEM_CHECK_EOL
#undef PJSTR_MEMMEM_CMP_FN
#undef PJSTR_MEMMEM_REVERSE
