/* Zero byte detection; basics.  ARM version.
   Copyright (C) 2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
#ifndef PJSTR_STRING_FZA_H
#define PJSTR_STRING_FZA_H 1

#include "jstr-macros.h"


#include "jstr-macros.h"

#include "string-misc.h"
#include "string-optype.h"

/* The functions return a byte mask.  */

/* This function returns at least one bit set within every byte
   of X that is zero.  */
static JSTR_INLINE pjstr_op_ty
pjstr_find_zero_all(pjstr_op_ty x)
{
	/* Use unsigned saturated subtraction from 1 in each byte.
	   That leaves 1 for every byte that was zero.  */
	pjstr_op_ty ones = pjstr_repeat_bytes(0x01);
	pjstr_op_ty ret;
	asm("uqsub8 %0,%1,%2"
	    : "=r"(ret)
	    : "r"(ones), "r"(x));
	return ret;
}

/* Identify bytes that are equal between X1 and X2.  */
static JSTR_INLINE pjstr_op_ty
pjstr_find_eq_all(pjstr_op_ty x1, pjstr_op_ty x2)
{
	return pjstr_find_zero_all(x1 ^ x2);
}

/* Identify zero bytes in X1 or equality between X1 and X2.  */
static JSTR_INLINE pjstr_op_ty
pjstr_find_zero_eq_all(pjstr_op_ty x1, pjstr_op_ty x2)
{
	return pjstr_find_zero_all(x1) | pjstr_find_zero_all(x1 ^ x2);
}

/* Identify zero bytes in X1 or inequality between X1 and X2.  */
static JSTR_INLINE pjstr_op_ty
pjstr_find_zero_ne_all(pjstr_op_ty x1, pjstr_op_ty x2)
{
	/* Make use of the fact that we'll already have ONES in a register.  */
	pjstr_op_ty ones = pjstr_repeat_bytes(0x01);
	return pjstr_find_zero_all(x1) | (pjstr_find_zero_all(x1 ^ x2) ^ ones);
}

/* Define the "inexact" versions in terms of the exact versions.  */
#define pjstr_find_zero_low pjstr_find_zero_all


#define pjstr_find_eq_low pjstr_find_eq_all


#define pjstr_find_zero_eq_low pjstr_find_zero_eq_all


#endif /* _STRING_FZA_H */
