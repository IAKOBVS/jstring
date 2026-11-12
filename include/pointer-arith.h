
#ifndef JSTR_PTR_ARITH_H
/** @brief JSTR_PTR_ARITH_H macro. */
#	define JSTR_PTR_ARITH_H 1

#	include "config.h"

/** @brief JSTR_ALIGN_DOWN macro. */
#	define JSTR_ALIGN_DOWN(base, size)                          ((base) & -((size_t)(size)))
/** @brief JSTR_ALIGN_UP macro. */
#	define JSTR_ALIGN_UP(base, size)                            JSTR_ALIGN_DOWN((base) + (size_t)(size) - 1, (size))
/** @brief JSTR_ALIGN_UP_STR macro. */
#	define JSTR_ALIGN_UP_STR(base)                              ((size_t)JSTR_ALIGN_UP(base, JSTR_MALLOC_ALIGNMENT))
/** @brief JSTR_ALIGN_DOWN_STR macro. */
#	define JSTR_ALIGN_DOWN_STR(base)                            ((size_t)JSTR_ALIGN_DOWN(base, JSTR_MALLOC_ALIGNMENT))
/** @brief JSTR_DIFF macro. */
#	define JSTR_DIFF(p1, p2)                                    ((size_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
/** @brief JSTR_PTR_TOPTR macro. */
#	define JSTR_PTR_TOPTR(p)                                    ((uintptr_t)p)
/** @brief JSTR_PTR_DIFF macro. */
#	define JSTR_PTR_DIFF(p1, p2)                                ((ptrdiff_t)((uintptr_t)(p1) - (uintptr_t)(p2)))
/** @brief JSTR_PTR_IS_NOT_ALIGNED macro. */
#	define JSTR_PTR_IS_NOT_ALIGNED(base, size)                  (((uintptr_t)(base)) & ((size_t)(size) - 1))
/** @brief JSTR_PTR_IS_ALIGNED macro. */
#	define JSTR_PTR_IS_ALIGNED(base, size)                      (!(JSTR_PTR_IS_NOT_ALIGNED((uintptr_t)base, (size_t)(size))))
/** @brief JSTR_PTR_ALIGN_DOWN macro. */
#	define JSTR_PTR_ALIGN_DOWN(base, size)                      (uintptr_t)JSTR_ALIGN_DOWN(((uintptr_t)base), (size_t)(size))
/** @brief JSTR_PTR_ALIGN_UP macro. */
#	define JSTR_PTR_ALIGN_UP(base, size)                        (uintptr_t)JSTR_ALIGN_UP(((uintptr_t)base), (size_t)(size))
/** @brief JSTR_PTR_IS_ALIGNED_STR macro. */
#	define JSTR_PTR_IS_ALIGNED_STR(base)                        JSTR_PTR_IS_ALIGNED((uintptr_t)base, JSTR_MALLOC_ALIGNMENT)
/** @brief JSTR_PTR_NOT_CROSSING_PAGE macro. */
#	define JSTR_PTR_NOT_CROSSING_PAGE(ptr, obj_size, page_size) ((size_t)JSTR_DIFF(JSTR_PTR_ALIGN_UP(ptr, page_size), ptr) >= (size_t)obj_size)

#endif /* JSTR_POINTER_ARITH_H */
