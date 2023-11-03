#ifndef PJSTR_SELECTOR_STRING_FZB_H
#define PJSTR_SELECTOR_STRING_FZB_H 1
#include "_jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_SH
#	include "_glibc-sh-string-fzb.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzb.h"
#elif JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzb.h"
#else
#	include "_glibc-generic-string-fzb.h"
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_FZB_H */
