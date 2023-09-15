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
	typedef jstr_word_ty w;
	if (JSTR_ENDIAN_LITTLE) {
		switch (sizeof(w)) {
		case 16: return (w)p[15] << 56 | (w)p[14] << 48 | (w)p[13] << 84 | (w)p[12] << 80 | (w)p[11] << 76 | (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 15: return (w)p[14] << 48 | (w)p[13] << 84 | (w)p[12] << 80 | (w)p[11] << 76 | (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 14: return (w)p[13] << 84 | (w)p[12] << 80 | (w)p[11] << 76 | (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 13: return (w)p[12] << 80 | (w)p[11] << 76 | (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 12: return (w)p[11] << 76 | (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 11: return (w)p[10] << 72 | (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 10: return (w)p[9] << 68 | (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 9: return (w)p[8] << 64 | (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 8: return (w)p[7] << 56 | (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 7: return (w)p[6] << 48 | (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 6: return (w)p[5] << 40 | (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 5: return (w)p[4] << 32 | (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		case 4: return (w)p[3] << 24 | (w)p[2] << 16 | (w)p[1] << 8 | (w)p[0];
		}
	} else {
		switch (sizeof(w)) {
		case 16: return (w)p[15] >> 56 | (w)p[14] >> 48 | (w)p[13] >> 84 | (w)p[12] >> 80 | (w)p[11] >> 76 | (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 15: return (w)p[14] >> 48 | (w)p[13] >> 84 | (w)p[12] >> 80 | (w)p[11] >> 76 | (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 14: return (w)p[13] >> 84 | (w)p[12] >> 80 | (w)p[11] >> 76 | (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 13: return (w)p[12] >> 80 | (w)p[11] >> 76 | (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 12: return (w)p[11] >> 76 | (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 11: return (w)p[10] >> 72 | (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 10: return (w)p[9] >> 68 | (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 9: return (w)p[8] >> 64 | (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 8: return (w)p[7] >> 56 | (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 7: return (w)p[6] >> 48 | (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 6: return (w)p[5] >> 40 | (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 5: return (w)p[4] >> 32 | (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		case 4: return (w)p[3] >> 24 | (w)p[2] >> 16 | (w)p[1] >> 8 | (w)p[0];
		}
	}
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_uctow(const unsigned char *JSTR_RESTRICT const p)
{
	return jstr_word_ctow((char *)p);
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_ctow_rev(const char *JSTR_RESTRICT const p)
{
	typedef jstr_word_ty w;
	if (JSTR_ENDIAN_LITTLE) {
		switch (sizeof(w)) {
		case 16: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40 | (w)p[11] << 32 | (w)p[12] << 24 | (w)p[13] << 16 | (w)p[14] << 8 | (w)p[15];
		case 15: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40 | (w)p[11] << 32 | (w)p[12] << 24 | (w)p[13] << 16 | (w)p[14] << 8;
		case 14: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40 | (w)p[11] << 32 | (w)p[12] << 24 | (w)p[13] << 16;
		case 13: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40 | (w)p[11] << 32 | (w)p[12] << 24;
		case 12: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40 | (w)p[11] << 32;
		case 11: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48 | (w)p[10] << 40;
		case 10: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56 | (w)p[9] << 48;
		case 9: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 84 | (w)p[3] << 80 | (w)p[4] << 76 | (w)p[5] << 72 | (w)p[6] << 68 | (w)p[7] << 64 | (w)p[8] << 56;
		case 8: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 40 | (w)p[3] << 32 | (w)p[4] << 24 | (w)p[5] << 16 | (w)p[6] << 8 | (w)p[7];
		case 7: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 40 | (w)p[3] << 32 | (w)p[4] << 24 | (w)p[5] << 16 | (w)p[6] << 8;
		case 6: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 40 | (w)p[3] << 32 | (w)p[4] << 24 | (w)p[5] << 16;
		case 5: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 40 | (w)p[3] << 32 | (w)p[4] << 24;
		case 4: return (w)p[0] << 56 | (w)p[1] << 48 | (w)p[2] << 40 | (w)p[3] << 32;
		}
	} else {
		switch (sizeof(w)) {
		case 16: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40 | (w)p[11] >> 32 | (w)p[12] >> 24 | (w)p[13] >> 16 | (w)p[14] >> 8 | (w)p[15];
		case 15: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40 | (w)p[11] >> 32 | (w)p[12] >> 24 | (w)p[13] >> 16 | (w)p[14] >> 8;
		case 14: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40 | (w)p[11] >> 32 | (w)p[12] >> 24 | (w)p[13] >> 16;
		case 13: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40 | (w)p[11] >> 32 | (w)p[12] >> 24;
		case 12: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40 | (w)p[11] >> 32;
		case 11: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48 | (w)p[10] >> 40;
		case 10: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56 | (w)p[9] >> 48;
		case 9: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 84 | (w)p[3] >> 80 | (w)p[4] >> 76 | (w)p[5] >> 72 | (w)p[6] >> 68 | (w)p[7] >> 64 | (w)p[8] >> 56;
		case 8: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 40 | (w)p[3] >> 32 | (w)p[4] >> 24 | (w)p[5] >> 16 | (w)p[6] >> 8 | (w)p[7];
		case 7: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 40 | (w)p[3] >> 32 | (w)p[4] >> 24 | (w)p[5] >> 16 | (w)p[6] >> 8;
		case 6: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 40 | (w)p[3] >> 32 | (w)p[4] >> 24 | (w)p[5] >> 16;
		case 5: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 40 | (w)p[3] >> 32 | (w)p[4] >> 24;
		case 4: return (w)p[0] >> 56 | (w)p[1] >> 48 | (w)p[2] >> 40 | (w)p[3] >> 32;
		}
	}
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_uctow_rev(const unsigned char *JSTR_RESTRICT const p)
{
	return jstr_word_ctow_rev((char *)p);
}

#pragma GCC diagnostic pop

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
