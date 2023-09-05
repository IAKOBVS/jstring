#ifndef PJSTR_SELECTOR_STRING_SHIFT_H
#define PJSTR_SELECTOR_STRING_SHIFT_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS

#if JSTR_ARCH_ALPHA
#	include "alpha-string-shift.h"
#else
#	include "generic-string-shift.h"
#endif
	
PJSTR_END_DECLS

#endif /* PJSTR_SELECTOR_STRING_SHIFT_H */
