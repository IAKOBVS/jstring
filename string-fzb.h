#ifndef PJSTR_SELECTOR_STRING_FZB_H
#define PJSTR_SELECTOR_STRING_FZB_H 1
#include "jstr_macros.h"

#if JSTR_ARCH_ALPHA
#	include "alpha-string-fzb.h"
#elif JSTR_ARCH_SH
#	include "sh-string-fzb.h"
#else
#	include "generic-string-fzb.h"
#endif

#endif /* PJSTR_STRING_FZB_H */
