#ifndef JARR_MACROS_H
#define JARR_MACROS_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"

#define P_JARR_DATA_NAME	    data
#define P_JARR_SIZE_NAME	    size
#define P_JARR_CAPACITY_NAME capacity

#define P_JARR_ELEMSZ(j) (sizeof((P_JARR_DATA(j))[0]))
#define P_JARR_ARRSZ(j)	(sizeof(P_JARR_DATA(j)) / sizeof((P_JARR_DATA(j))[0]))
#define P_JARR_DATA(j)	((j)->P_JARR_DATA_NAME)
#define P_JARR_SZ(j)	((j)->P_JARR_SIZE_NAME)
#define P_JARR_CAP(j)	((j)->P_JARR_CAPACITY_NAME)

#define P_JARR_MEMMOVE(j, dst, src, n) memmove(dst, src, (n)*P_JARR_ELEMSZ(j))
#define P_JARR_MEMCPY(j, dst, src, n)  memcpy(dst, src, (n)*P_JARR_ELEMSZ(j))

#define P_JARR_ALIGN_UP(new_cap)		 JSTR_ALIGN_UP(new_cap, PJSTR_MALLOC_ALIGNMENT)
#define P_JARR_MIN_ALLOC(j, new_cap)	 PJSTR_MIN_ALLOCEXACTEXACT(P_JARR_ALIGN_UP((size_t)(((new_cap)*P_JARR_ELEMSZ(j) * P_JARR_ALLOC_MULTIPLIER))))
#define P_JARR_MIN_ALLOCEXACT(j, new_cap) PJSTR_MIN_ALLOCEXACTEXACT(P_JARR_ALIGN_UP((new_cap)*P_JARR_ELEMSZ(j)))

#define P_JARR_CHECK_ARG(j)                         \
	do {                                       \
		JSTR_ASSERT_IS_SIZE(P_JARR_SZ(j));  \
		JSTR_ASSERT_IS_SIZE(P_JARR_CAP(j)); \
	} while (0)
#define P_JARR_GROW(old_cap, new_cap)                                         \
	do {                                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                                \
		JSTR_ASSERT_IS_SIZE(new_cap);                                \
		while (((old_cap) *= P_JARR_GROWTH) < (new_cap))              \
			;                                                    \
		(old_cap) = JSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define P_JARR_REALLOC(j, new_cap, malloc_fail)                                                                    \
	do {                                                                                                      \
		P_JARR_CHECK_ARG(j);                                                                               \
		P_JARR_GROW(P_JARR_CAP(j), new_cap);                                                                \
		P_JARR_CAP(j) = P_JARR_ALIGN_UP(P_JARR_CAP(j) * P_JARR_ELEMSZ(j));                                    \
		P_JARR_DATA(j) = PJSTR_CAST(P_JARR_DATA(j), realloc(P_JARR_DATA(j), P_JARR_CAP(j) * P_JARR_ELEMSZ(j));                                   \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), malloc_fail);                                                                                       \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                  \
	} while (0)
#define P_JARR_REALLOCEXACT(j, new_cap, malloc_fail)                                                                \
	do {                                                                                                       \
		P_JARR_CHECK_ARG(j);                                                                                \
		P_JARR_CAP(j) = P_JARR_ALIGN_UP(P_JARR_CAP(j) * P_JARR_ELEMSZ);                                        \
		P_JARR_DATA(j) = PJSTR_CAST(P_JARR_DATA(j), realloc(P_JARR_DATA(j), P_JARR_CAP(j) * P_JARR_ELEMSZ(j))); \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), malloc_fail);                                                     \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                   \
	} while (0)
#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define P_JARR_CHECK_VAL(j, value) JSTR_ASSERT(JSTR_SAME_TYPE(value, *P_JARR_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
#	define P_JARR_CHECK_VAL(j, value) JSTR_ASSERT(sizeof(*P_JARR_DATA(j)) == value, "Passing illegal value incompatible with the array type.")
#endif

#endif /* JARR_MACROS_H */
