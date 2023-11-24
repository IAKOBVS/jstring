#include "jstr-ctype.h"
#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <string.h>
PJSTR_END_DECLS

#ifndef PJSTR_RAREBYTE_RETTYPE
#	define PJSTR_RAREBYTE_RETTYPE void *
#endif
#ifndef PJSTR_RAREBYTE_FN
#	define PJSTR_RAREBYTE_FN pjstr_rarebyte_memmem
#endif
#ifndef PJSTR_RAREBYTE_CMP_FUNC
#	define PJSTR_RAREBYTE_CMP_FUNC memcmp
#endif
#if !defined PJSTR_RAREBYTE_USE_LEN || PJSTR_RAREBYTE_FN
#	define PJSTR_RAREBYTE_USE_LEN 1
#	define PJSTR_RAREBYTE_HSLEN   size_t hs_len,
#	define PJSTR_RAREBYTE_NELEN   size_t ne_len,
#else
#	define PJSTR_RAREBYTE_HSLEN
#	define PJSTR_RAREBYTE_NELEN
#endif

#include "_jstr-rarebyte.h"

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
#if PJSTR_RAREBYTE_USE_LEN
JSTR_ATTR_ACCESS((__read_only__, 1, 2))
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
#endif
static PJSTR_RAREBYTE_RETTYPE
PJSTR_RAREBYTE_FN(const unsigned char *JSTR_RESTRICT h, PJSTR_RAREBYTE_HSLEN const unsigned char *JSTR_RESTRICT n, PJSTR_RAREBYTE_NELEN const unsigned char *rarebyte)
{
#if PJSTR_RAREBYTE_USE_LEN
	if (jstr_unlikely(hs_len < ne_len))
		return NULL;
#endif
	typedef unsigned char u;
	int c;
	const size_t idx = JSTR_PTR_DIFF(rarebyte, n);
	c = *(u *)rarebyte;
	h += idx;
#if PJSTR_RAREBYTE_USE_LEN
	const unsigned char *end = (u *)h + hs_len - ne_len + 1;
#endif
	for (;
#if PJSTR_RAREBYTE_USE_LEN
	     (h = (const u *)memchr(h, c, end - h));
#else
	     (h = (const u *)strchr((char *)h, c));
#endif
	     ++h)
#if PJSTR_RAREBYTE_USE_LEN
		if (!PJSTR_RAREBYTE_CMP_FUNC((char *)h - idx, (char *)n, ne_len))
#else
		if (!PJSTR_RAREBYTE_CMP_FUNC((char *)h - idx, (char *)n))
#endif
			return (PJSTR_RAREBYTE_RETTYPE)(h - idx);
	return NULL;
}

#undef PJSTR_RAREBYTE_FN
#undef PJSTR_RAREBYTE_CMP_FUNC
#undef PJSTR_RAREBYTE_USE_LEN
#undef PJSTR_RAREBYTE_HSLEN
#undef PJSTR_RAREBYTE_NELEN
