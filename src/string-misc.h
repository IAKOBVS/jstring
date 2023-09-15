#ifndef PJSTR_SELECTOR_STRING_MISC_H
#define PJSTR_SELECTOR_STRING_MISC_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "generic-string-misc.h"
#include <endian.h>

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_ctow_rev(const char *JSTR_RESTRICT const p)
{
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (jstr_word_ty)p[0] << 56 | (jstr_word_ty)p[1] << 48 | (jstr_word_ty)p[2] << 40 | (jstr_word_ty)p[3] << 32 | (jstr_word_ty)p[4] << 24 | (jstr_word_ty)p[5] << 16 | (jstr_word_ty)p[6] << 8 | (jstr_word_ty)p[7];
	else
		return (jstr_word_ty)p[0] >> 56 | (jstr_word_ty)p[1] >> 48 | (jstr_word_ty)p[2] >> 40 | (jstr_word_ty)p[3] >> 32 | (jstr_word_ty)p[4] >> 24 | (jstr_word_ty)p[5] >> 16 | (jstr_word_ty)p[6] >> 8 | (jstr_word_ty)p[7];
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_uctow_rev(const unsigned char *JSTR_RESTRICT const p)
{
	return jstr_word_ctow_rev((char *)p);
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_ctow(const char *JSTR_RESTRICT const p)
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(jstr_word_ty *)p;
#else
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (jstr_word_ty)p[7] << 56 | (jstr_word_ty)p[6] << 48 | (jstr_word_ty)p[5] << 40 | (jstr_word_ty)p[4] << 32 | (jstr_word_ty)p[3] << 24 | (jstr_word_ty)p[2] << 16 | (jstr_word_ty)p[1] << 8 | (jstr_word_ty)p[0];
	else
		return (jstr_word_ty)p[7] >> 56 | (jstr_word_ty)p[6] >> 48 | (jstr_word_ty)p[5] >> 40 | (jstr_word_ty)p[4] >> 32 | (jstr_word_ty)p[3] >> 24 | (jstr_word_ty)p[2] >> 16 | (jstr_word_ty)p[1] >> 8 | (jstr_word_ty)p[0];
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static jstr_word_ty
jstr_word_uctow(const unsigned char *JSTR_RESTRICT const p)
{
	return jstr_word_ctow((char *)p);
}

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
