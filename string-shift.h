#ifndef PJSTR_SELECTOR_STRING_SHIFT_H
#define PJSTR_SELECTOR_STRING_SHIFT_H 1

#include "jstr_macros.h"

#if JSTR_ARCH_ALPHA
#	include "alpha-string-shift.h"
#else
#	include "generic-string-shift.h"
#endif

#endif /* PJSTR_SELECTOR_STRING_SHIFT_H */
