#ifndef PJSTR_SELECTOR_STRING_MISC_H
#define PJSTR_SELECTOR_STRING_MISC_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "generic-string-misc.h"
#include <endian.h>

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_ctoword(const char *JSTR_RESTRICT const ch)
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(pjstr_op_ty *)ch;
#else
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (pjstr_op_ty)ch[7] << 56 | (pjstr_op_ty)ch[6] << 48 | (pjstr_op_ty)ch[5] << 40 | (pjstr_op_ty)ch[4] << 32 | (pjstr_op_ty)ch[3] << 24 | (pjstr_op_ty)ch[2] << 16 | (pjstr_op_ty)ch[1] << 8 | (pjstr_op_ty)ch[0];
	else
		return (pjstr_op_ty)ch[7] >> 56 | (pjstr_op_ty)ch[6] >> 48 | (pjstr_op_ty)ch[5] >> 40 | (pjstr_op_ty)ch[4] >> 32 | (pjstr_op_ty)ch[3] >> 24 | (pjstr_op_ty)ch[2] >> 16 | (pjstr_op_ty)ch[1] >> 8 | (pjstr_op_ty)ch[0];
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_uctoword(const unsigned char *JSTR_RESTRICT const ch)
{
	return pjstr_ctoword((char *)ch);
}

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
