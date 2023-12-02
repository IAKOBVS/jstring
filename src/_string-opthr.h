#ifndef PJSTR_SELECTOR_STRING_OPTHR_H
#define PJSTR_SELECTOR_STRING_OPTHR_H 1

#ifdef JSTR_ARCH_x86_32
#	include "_glibc-i386-string-opthr.h"
#else
#	include "_glibc-generic-string-opthr.h"
#endif

#endif /* PJSTR_SELECTOR_STRING_OPTHR_H */
