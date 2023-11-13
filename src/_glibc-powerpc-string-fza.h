/* Zero byte detection; basics.  PowerPC version.
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

#ifndef PJSTR_POWERPC_STRING_FZA_H
#define PJSTR_POWERPC_STRING_FZA_H 1

#include "jstr-macros.h"

/* PowerISA 2.05 (POWER6) provides cmpb instruction.  */
#ifdef _ARCH_PWR6
# include "_string-misc.h"
# include "_string-optype.h"

/* The functions return a byte mask.  */
typedef jstr_word_ty find_t;

/* This function returns 0xff for each byte that is
   equal between X1 and X2.  */

static JSTR_INLINE jstr_word_ty
find_eq_all (jstr_word_ty x1, jstr_word_ty x2)
{
  return __builtin_cmpb (x1, x2);
}

/* This function returns 0xff for each byte that is zero in X.  */

static JSTR_INLINE jstr_word_ty
find_zero_all (jstr_word_ty x)
{
  return find_eq_all (x, 0);
}

/* Identify zero bytes in X1 or equality between X1 and X2.  */

static JSTR_INLINE jstr_word_ty
find_zero_eq_all (jstr_word_ty x1, jstr_word_ty x2)
{
  return find_zero_all (x1) | find_eq_all (x1, x2);
}

/* Identify zero bytes in X1 or inequality between X1 and X2.  */

static JSTR_INLINE jstr_word_ty
find_zero_ne_all (jstr_word_ty x1, jstr_word_ty x2)
{
  return find_zero_all (x1) | ~find_eq_all (x1, x2);
}

/* Define the "inexact" versions in terms of the exact versions.  */
# define find_zero_low		find_zero_all
# define find_eq_low		find_eq_all
# define find_zero_eq_low	find_zero_eq_all
#else
# include "_glibc_generic-string-fza.h"
#endif /* _ARCH_PWR6  */

#endif /* _POWERPC_STRING_FZA_H  */
