#ifndef PJSTR_SELECTOR_STRING_MISC_H
#define PJSTR_SELECTOR_STRING_MISC_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "generic-string-misc.h"
#include <endian.h>

#pragma GCC diagnostic ignored "-Wshift-count-overflow"
#pragma GCC diagnostic push

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_ctow(const char *JSTR_RESTRICT const p)
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(jstr_word_ty *)p;
#else
#	define PJSTR_WORD_CTOW(sh)                                                                                                                                                                                                                                                                      \
		do {                                                                                                                                                                                                                                                                                     \
			switch (sizeof(w)) {                                                                                                                                                                                                                                                             \
			case 16: return (w)p[15] sh 120 | (w)p[14] sh 112 | (w)p[13] sh 104 | (w)p[12] sh 96 | (w)p[11] sh 88 | (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0]; \
			case 15: return (w)p[14] sh 112 | (w)p[13] sh 104 | (w)p[12] sh 96 | (w)p[11] sh 88 | (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                   \
			case 14: return (w)p[13] sh 104 | (w)p[12] sh 96 | (w)p[11] sh 88 | (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                     \
			case 13: return (w)p[12] sh 96 | (w)p[11] sh 88 | (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                       \
			case 12: return (w)p[11] sh 88 | (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                        \
			case 11: return (w)p[10] sh 80 | (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                         \
			case 10: return (w)p[9] sh 72 | (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                          \
			case 9: return (w)p[8] sh 64 | (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                           \
			case 8: return (w)p[7] sh 56 | (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                           \
			case 7: return (w)p[6] sh 48 | (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                                           \
			case 6: return (w)p[5] sh 40 | (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                                                           \
			case 5: return (w)p[4] sh 32 | (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                                                                           \
			case 4: return (w)p[3] sh 24 | (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                                                                                           \
			case 3: return (w)p[2] sh 16 | (w)p[1] sh 8 | (w)p[0];                                                                                                                                                                                                                           \
			case 2: return (w)p[1] sh 8 | (w)p[0];                                                                                                                                                                                                                                           \
			}                                                                                                                                                                                                                                                                                \
		} while (0)
	typedef jstr_word_ty w;
	if (JSTR_ENDIAN_LITTLE)
		PJSTR_WORD_CTOW(<<);
	else
		PJSTR_WORD_CTOW(>>);
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_uctow(const unsigned char *JSTR_RESTRICT const p)
{
	return jstr_word_ctow((char *)p);
}

#pragma GCC diagnostic pop

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
