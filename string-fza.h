#ifndef PJSTR_STRING_FZA_H
#define PJSTR_STRING_FZA_H 1

#include "jstr_macros.h"

#if JSTR_ARCH_POWERPC
#	include "powerpc-string-fza.h"
#elif JSTR_ARCH_ARM6T2
#	include "arm-armv6t2-string-fza.h"
#else
#	include "generic-string-fza.h"
#endif

#endif /* PJSTR_STRING_FZA_H */
