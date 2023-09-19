#ifndef P_JSTR_SELECTOR_STRING_FZC_H
#define P_JSTR_SELECTOR_STRING_FZC_H 1
#include "jstr-macros.h"
P_JSTR_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzc.h"
#else
#	include "glibc-generic-string-fzc.h"
#endif
P_JSTR_END_DECLS
#endif /* P_JSTR_SELECTOR_STRING_FZC_H */
