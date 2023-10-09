#ifndef PJSTR_SELECTOR_STRING_FZB_H
#define PJSTR_SELECTOR_STRING_FZB_H 1
#include "jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_SH
#	include "glibc-sh-string-fzb.h"
#elif JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-fzb.h"
#elif JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzb.h"
#else
#	include "glibc-generic-string-fzb.h"
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_FZB_H */
