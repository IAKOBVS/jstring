#ifndef PJSTR_SELECTOR_STRING_MISC_H
#define PJSTR_SELECTOR_STRING_MISC_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "generic-string-misc.h"
#include <endian.h>

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_ctow(const char *JSTR_RESTRICT const _ch)
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(pjstr_op_ty *)_ch;
#else
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (pjstr_op_ty)_ch[7] << 56 | (pjstr_op_ty)_ch[6] << 48 | (pjstr_op_ty)_ch[5] << 40 | (pjstr_op_ty)_ch[4] << 32 | (pjstr_op_ty)_ch[3] << 24 | (pjstr_op_ty)_ch[2] << 16 | (pjstr_op_ty)_ch[1] << 8 | (pjstr_op_ty)_ch[0];
	else
		return (pjstr_op_ty)_ch[7] >> 56 | (pjstr_op_ty)_ch[6] >> 48 | (pjstr_op_ty)_ch[5] >> 40 | (pjstr_op_ty)_ch[4] >> 32 | (pjstr_op_ty)_ch[3] >> 24 | (pjstr_op_ty)_ch[2] >> 16 | (pjstr_op_ty)_ch[1] >> 8 | (pjstr_op_ty)_ch[0];
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_uctow(const unsigned char *JSTR_RESTRICT const _ch)
{
	return pjstr_ctow((char *)_ch);
}

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
