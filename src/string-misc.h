#ifndef PJSTR_SELECTOR_STRING_MISC_H
#define PJSTR_SELECTOR_STRING_MISC_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS

#include "generic-string-misc.h"
#include <endian.h>

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_ctoword_rev(const char *JSTR_RESTRICT const p)
{
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (pjstr_op_ty)p[0] << 56 | (pjstr_op_ty)p[1] << 48 | (pjstr_op_ty)p[2] << 40 | (pjstr_op_ty)p[3] << 32 | (pjstr_op_ty)p[4] << 24 | (pjstr_op_ty)p[5] << 16 | (pjstr_op_ty)p[6] << 8 | (pjstr_op_ty)p[7];
	else
		return (pjstr_op_ty)p[0] >> 56 | (pjstr_op_ty)p[1] >> 48 | (pjstr_op_ty)p[2] >> 40 | (pjstr_op_ty)p[3] >> 32 | (pjstr_op_ty)p[4] >> 24 | (pjstr_op_ty)p[5] >> 16 | (pjstr_op_ty)p[6] >> 8 | (pjstr_op_ty)p[7];
}

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_uctoword_rev(const unsigned char *JSTR_RESTRICT const p)
{
	return pjstr_ctoword_rev((char *)p);
}

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_ctoword(const char *JSTR_RESTRICT const p)
{
#if JSTR_HAVE_ATTR_MAY_ALIAS
	return *(pjstr_op_ty *)p;
#else
	if (__BYTE_ORDER == __LITTLE_ENDIAN)
		return (pjstr_op_ty)p[7] << 56 | (pjstr_op_ty)p[6] << 48 | (pjstr_op_ty)p[5] << 40 | (pjstr_op_ty)p[4] << 32 | (pjstr_op_ty)p[3] << 24 | (pjstr_op_ty)p[2] << 16 | (pjstr_op_ty)p[1] << 8 | (pjstr_op_ty)p[0];
	else
		return (pjstr_op_ty)p[7] >> 56 | (pjstr_op_ty)p[6] >> 48 | (pjstr_op_ty)p[5] >> 40 | (pjstr_op_ty)p[4] >> 32 | (pjstr_op_ty)p[3] >> 24 | (pjstr_op_ty)p[2] >> 16 | (pjstr_op_ty)p[1] >> 8 | (pjstr_op_ty)p[0];
#endif
}

JSTR_INLINE
JSTR_FUNC_PURE
static pjstr_op_ty
pjstr_uctoword(const unsigned char *JSTR_RESTRICT const p)
{
	return pjstr_ctoword((char *)p);
}

PJSTR_END_DECLS

#endif /* PJSTR_STRING_MISC_H */
