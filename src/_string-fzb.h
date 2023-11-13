/* See LICENSE file for copyright and license details. */

#ifndef JSTRP_SELECTOR_STRING_FZB_H
#define JSTRP_SELECTOR_STRING_FZB_H 1
#include "jstr-macros.h"
JSTRP_BEGIN_DECLS
#if JSTR_ARCH_SH
#	include "_glibc-sh-string-fzb.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzb.h"
#elif JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzb.h"
#else
#	include "_glibc-generic-string-fzb.h"
#endif
JSTRP_END_DECLS
#endif /* JSTRP_SELECTOR_STRING_FZB_H */
