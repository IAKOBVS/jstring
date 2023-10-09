#ifndef PJSTR_SELECTOR_STRING_FZI_H
#define PJSTR_SELECTOR_STRING_FZI_H 1
#include "jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "glibc-riscv-string-fzi.h"
#else
#	include "glibc-generic-string-fzi.h"
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_FZI_H */
