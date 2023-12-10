#ifndef PJSTR_SELECTOR_STRING_FZI_H
#define PJSTR_SELECTOR_STRING_FZI_H 1
#include "jstr-macros.h"
PJSTR_BEGIN_DECLS
#if JSTR_ARCH_HPPA
#	include "_glibc-hppa-string-fzi.h"
#elif JSTR_ARCH_ALPHA
#	include "_glibc-alpha-string-fzi.h"
#elif JSTR_ARCH_RISCV
#	include "_glibc-riscv-string-fzi.h"
#else
#	if defined __GNUC__ || defined __clang__
#		pragma GCC diagnostic ignored "-Wsign-conversion"
#		pragma GCC diagnostic push

#		include "_glibc-generic-string-fzi.h"

#		pragma GCC diagnostic pop
#	endif
#endif
PJSTR_END_DECLS
#endif /* PJSTR_SELECTOR_STRING_FZI_H */
