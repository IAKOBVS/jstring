/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_FZA_H
#define JSTRP_SELECTOR_STRING_FZA_H 1
#include "jstr-macros.h"
JSTRP_BEGIN_DECLS
#if JSTR_ARCH_POWERPC6
#	include "_glibc-powerpc-string-fza.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fza.h"
#elif JSTR_ARCH_RISCV
#	include "_glibc-riscv-string-fza.h"
#elif JSTR_ARCH_ARM6T2
#	include "_glibc-arm-armv6t2-string-fza.h"
#else
#	include "_glibc-generic-string-fza.h"
#endif
JSTRP_END_DECLS
#endif /* JSTRP_SELECTOR_STRING_FZA_H */
