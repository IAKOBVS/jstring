/* See LICENSE file for copyright and license details. */

#ifndef JSTRP__SELECTOR_STRING_FZI_H
#define JSTRP__SELECTOR_STRING_FZI_H 1
#include "jstr-macros.h"
JSTRP__BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "_glibc-riscv-string-fzi.h"
#else
#	include "_glibc-generic-string-fzi.h"
#endif
JSTRP__END_DECLS
#endif /* JSTRP__SELECTOR_STRING_FZI_H */
