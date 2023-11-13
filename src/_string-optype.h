/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_OPTYPE_H
#define JSTRP_SELECTOR_STRING_OPTYPE_H 1

#include "jstr-macros.h"

#if JSTR_ARCH_X86_32
#	include "_glibc-x86_64-x32-string-optype.h"
#else
#	include "_glibc-generic-string-optype.h"
#endif

#endif /* JSTRP_SELECTOR_STRING_OPTYPE_H */
