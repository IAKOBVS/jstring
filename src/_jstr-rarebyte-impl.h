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
