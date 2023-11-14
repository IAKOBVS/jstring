/* See LICENSE file for copyright and license details. */

#ifndef JSTRP__SELECTOR_STRING_SHIFT_H
#define JSTRP__SELECTOR_STRING_SHIFT_H 1
#include "jstr-macros.h"
JSTRP__BEGIN_DECLS
#if JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-shift.h"
#else
#	include "_glibc-generic-string-shift.h"
#endif
JSTRP__END_DECLS
#endif /* JSTRP__SELECTOR_STRING_SHIFT_H */
