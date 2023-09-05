#ifndef PJSTR_SELECTOR_STRING_FZC_H
#define PJSTR_SELECTOR_STRING_FZC_H 1

#include "jstr_macros.h"

JSTR_BEGIN_DECLS

#if JSTR_ARCH_HPPA
#	include "hppa-string-fzc.h"
#else
#	include "generic-string-fzc.h"
#endif

JSTR_END_DECLS

#endif /* PJSTR_SELECTOR_STRING_FZC_H */
