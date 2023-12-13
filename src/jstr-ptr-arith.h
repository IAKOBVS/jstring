#ifndef JSTR_PTR_ARITH_H
#define JSTR_PTR_ARITH_H 1

#include "jstr-config.h"

#define JSTR_ALIGN_DOWN(base, size)     ((base) & -((uintptr_t)(size)))
#define JSTR_ALIGN_UP(base, size)       JSTR_ALIGN_DOWN((base) + (size)-1, (size))
#define JSTR_ALIGN_UP_STR(base)         JSTR_ALIGN_UP((uintptr_t)base, JSTR_MALLOC_ALIGNMENT)
#define JSTR_ALIGN_DOWN_STR(base)       JSTR_ALIGN_DOWN((uintptr_t)base, JSTR_MALLOC_ALIGNMENT)
#define JSTR_PTR_TOPTR(p)               ((uintptr_t)p)
#define JSTR_PTR_DIFF(p1, p2)           ((size_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
#define JSTR_PTR_IS_NOT_ALIGNED(base, size) (((uintptr_t)(base)) & ((size)-1))
#define JSTR_PTR_IS_ALIGNED(base, size) (!(JSTR_PTR_IS_NOT_ALIGNED(base, size)))
#define JSTR_PTR_ALIGN_DOWN(base, size) JSTR_ALIGN_DOWN((uintptr_t)(base), (size))
#define JSTR_PTR_ALIGN_UP(base, size)   JSTR_ALIGN_UP((uintptr_t)(base), (size))
#define JSTR_PTR_IS_ALIGNED_STR(base)   JSTR_PTR_IS_ALIGNED(base, JSTR_MALLOC_ALIGNMENT)

#endif /* JSTR_PTR_ARITH_H */
