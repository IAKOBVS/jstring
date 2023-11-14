/* See LICENSE file for copyright and license details. */

#ifndef JSTRP__SELECTOR_STRING_FZC_H
#define JSTRP__SELECTOR_STRING_FZC_H 1
#include "jstr-macros.h"
JSTRP__BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzc.h"
#else
#	include "_glibc-generic-string-fzc.h"
#endif
JSTRP__END_DECLS
#endif /* JSTRP__SELECTOR_STRING_FZC_H */
