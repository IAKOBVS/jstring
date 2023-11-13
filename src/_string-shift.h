/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_SHIFT_H
#define JSTRP_SELECTOR_STRING_SHIFT_H 1
#include "jstr-macros.h"
JSTRP_BEGIN_DECLS
#if JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-shift.h"
#else
#	include "_glibc-generic-string-shift.h"
#endif
JSTRP_END_DECLS
#endif /* JSTRP_SELECTOR_STRING_SHIFT_H */
