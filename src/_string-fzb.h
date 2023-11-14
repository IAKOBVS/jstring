/* See LICENSE file for copyright and license details. */

#ifndef JSTRP__SELECTOR_STRING_FZB_H
#define JSTRP__SELECTOR_STRING_FZB_H 1
#include "jstr-macros.h"
JSTRP__BEGIN_DECLS
#if JSTR_ARCH_SH
#	include "_glibc-sh-string-fzb.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzb.h"
#elif JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzb.h"
#else
#	include "_glibc-generic-string-fzb.h"
#endif
JSTRP__END_DECLS
#endif /* JSTRP__SELECTOR_STRING_FZB_H */
