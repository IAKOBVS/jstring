#ifndef PJSTR_SELECTOR_STRING_FZI_H
#define PJSTR_SELECTOR_STRING_FZI_H 1

#include "jstr_macros.h"

#if JSTR_ARCH_HPPA
#	include "hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "riscv-string-fzi.h"
#else
#	include "generic-string-fzi.h"
#endif

#endif /* PJSTR_SELECTOR_STRING_FZI_H */
