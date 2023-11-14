/* See LICENSE file for copyright and license details. */

#ifndef PJSTR_SELECTOR_STRING_SHIFT_H
#define PJSTR_SELECTOR_STRING_SHIFT_H 1
#include "jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-shift.h"
#else
#	include "_glibc-generic-string-shift.h"
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_SHIFT_H */
