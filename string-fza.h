#ifndef PJSTR_SELECTOR_STRING_FZA_H
#define PJSTR_SELECTOR_STRING_FZA_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS

#if JSTR_ARCH_POWERPC6
#	include "powerpc-string-fza.h"
#elif JSTR_ARCH_ALPHA
#	include "alpha-string-fza.h"
#elif JSTR_ARCH_RISCV
#	include "riscv-string-fza.h"
#elif JSTR_ARCH_ARM6T2
#	include "arm-armv6t2-string-fza.h"
#else
#	include "generic-string-fza.h"
#endif

PJSTR_END_DECLS

#endif /* PJSTR_SELECTOR_STRING_FZA_H */
