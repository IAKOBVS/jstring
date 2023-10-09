#ifndef PJSTR_SELECTOR_STRING_OPTYPE_H
#define PJSTR_SELECTOR_STRING_OPTYPE_H 1

#include "jstr-macros.h"

#if JSTR_ARCH_X86_32
#	include "glibc-x86_64-x32-string-optype.h"
#else
#	include "glibc-generic-string-optype.h"
#endif

#endif /* PJSTR_SELECTOR_STRING_OPTYPE_H */
