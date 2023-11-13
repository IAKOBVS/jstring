/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_FZI_H
#define JSTRP_SELECTOR_STRING_FZI_H 1
#include "jstr-macros.h"
JSTRP_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "_glibc-riscv-string-fzi.h"
#else
#	include "_glibc-generic-string-fzi.h"
#endif
JSTRP_END_DECLS
#endif /* JSTRP_SELECTOR_STRING_FZI_H */
