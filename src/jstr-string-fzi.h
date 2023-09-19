#ifndef P_JSTR_SELECTOR_STRING_FZI_H
#define P_JSTR_SELECTOR_STRING_FZI_H 1
#include "jstr-macros.h"
P_JSTR_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "glibc-hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "glibc-riscv-string-fzi.h"
#else
#	include "glibc-generic-string-fzi.h"
#endif
P_JSTR_END_DECLS
#endif /* P_JSTR_SELECTOR_STRING_FZI_H */
