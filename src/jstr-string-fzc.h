#ifndef PJSTR_SELECTOR_STRING_FZC_H
#define PJSTR_SELECTOR_STRING_FZC_H 1
#include "jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzc.h"
#else
#	include "glibc-generic-string-fzc.h"
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_FZC_H */
