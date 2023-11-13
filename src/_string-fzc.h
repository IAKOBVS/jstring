/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_FZC_H
#define JSTRP_SELECTOR_STRING_FZC_H 1
#include "jstr-macros.h"
JSTRP_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzc.h"
#else
#	include "_glibc-generic-string-fzc.h"
#endif
JSTRP_END_DECLS
#endif /* JSTRP_SELECTOR_STRING_FZC_H */
