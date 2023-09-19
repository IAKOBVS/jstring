#ifndef P_JSTR_SELECTOR_STRING_FZA_H
#define P_JSTR_SELECTOR_STRING_FZA_H 1
#include "jstr-macros.h"
P_JSTR_BEGIN_DECLS
#if JSTR_ARCH_POWERPC6
#	include "powerpc-string-fza.h"
#elif JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-fza.h"
#elif JSTR_ARCH_RISCV
#	include "riscv-string-fza.h"
#elif JSTR_ARCH_ARM6T2
#	include "glibc-arm-armv6t2-string-fza.h"
#else
#	include "generic-string-fza.h"
#endif
P_JSTR_END_DECLS
#endif /* P_JSTR_SELECTOR_STRING_FZA_H */
