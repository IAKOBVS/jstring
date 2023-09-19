#ifndef P_JSTR_SELECTOR_STRING_SHIFT_H
#define P_JSTR_SELECTOR_STRING_SHIFT_H 1
#include "jstr-macros.h"
P_JSTR_BEGIN_DECLS
#if JSTR_ARCH_ALPHA
#	include "glibc-alpha-string-shift.h"
#else
#	include "generic-string-shift.h"
#endif
P_JSTR_END_DECLS
#endif /* P_JSTR_SELECTOR_STRING_SHIFT_H */
