#ifndef PJSTR_STRING_FZB_H
#define PJSTR_STRING_FZB_H 1

#include "jstr_macros.h"

#if JSTR_ARCH_ALPHA
#	include "alpha-string-fzc.h"
#elif JSTR_ARCH_SH
#	include "sh-string-fzb.h"
#else
#	include "generic-string-fzc.h"
#endif

#endif /* PJSTR_STRING_FZC_H */
