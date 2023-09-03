#ifndef PJSTR_SELECTOR_STRING_FZC_H
#define PJSTR_SELECTOR_STRING_FZC_H 1

#include "jstr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if JSTR_ARCH_HPPA
#	include "hppa-string-fzc.h"
#else
#	include "generic-string-fzc.h"
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PJSTR_SELECTOR_STRING_FZC_H */
