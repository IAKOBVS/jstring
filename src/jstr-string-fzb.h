#ifndef P_JSTR_SELECTOR_STRING_FZB_H
#define P_JSTR_SELECTOR_STRING_FZB_H 1
#include "jstr-macros.h"
P_JSTR_BEGIN_DECLS
#if JSTR_ARCH_SH
#	include "glibc-sh-string-fzb.h"
#elif JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-fzb.h"
#elif JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzb.h"
#else
#	include "generic-string-fzb.h"
#endif
P_JSTR_END_DECLS
#endif /* P_JSTR_SELECTOR_STRING_FZB_H */
