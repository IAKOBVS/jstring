#ifndef JARR_MACROS_DEF_H
#define JARR_MACROS_DEF_H 1

#include "_builder.h"
#include "_config.h"
#include "_macros.h"
#include "_templates.h"

#ifdef __cplusplus
template <typename T, typename Other>
static T
PJARR_CAST(T, Other other)
{
	return (T)other;
}
#else
#	define PJARR_CAST(T, other) other
#endif /* __cpluslus */

#define PJARR_DATA_NAME	    data
#define PJARR_SIZE_NAME	    size
#define PJARR_CAPACITY_NAME capacity

#define PJARR_ELEMSZ(j) (sizeof((PJARR_DATA(j))[0]))
#define PJARR_ARRSZ(j)	(sizeof(PJARR_DATA(j)) / sizeof((PJARR_DATA(j))[0]))
#define PJARR_DATA(j)	((j)->PJARR_DATA_NAME)
#define PJARR_SZ(j)	((j)->PJARR_SIZE_NAME)
#define PJARR_CAP(j)	((j)->PJARR_CAPACITY_NAME)

#define PJARR_MEMMOVE(j, dst, src, n) memmove(dst, src, (n)*PJARR_ELEMSZ(j))
#define PJARR_MEMCPY(j, dst, src, n)  memcpy(dst, src, (n)*PJARR_ELEMSZ(j))

#define PJARR_MIN_ALLOC_BASE(new_cap)                                                                                                               \
	(((new_cap) < PJSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER)                                                                                        \
	 ? (PJSTR_MIN_CAP)                                                                                                                          \
	 : ((new_cap)*PJARR_ALLOC_MULTIPLIER))

#define PJARR_ALIGN_UP(new_cap)		 PJSTR_ALIGN_UP(new_cap, PJSTR_MALLOC_ALIGNMENT)
#define PJARR_MIN_ALLOC(j, new_cap)	 PJARR_MIN_ALLOC_BASE(PJSTR_ALIGN_UP((new_cap)*PJARR_ELEMSZ(j) * PJARR_ALLOC_MULTIPLIER))
#define PJARR_MIN_ALLOCEXACT(j, new_cap) PJSTR_MIN_ALLOCEXACT(PJSTR_ALIGN_UP(new_cap *PJARR_ELEMSZ(j)))

#define PJARR_CHECK_ARG(j)                                                                                                                          \
	do {                                                                                                                                        \
		JSTR_ASSERT_IS_SIZE(PJARR_SZ(j));                                                                                                   \
		JSTR_ASSERT_IS_SIZE(PJARR_CAP(j));                                                                                                  \
	} while (0)

#define PJARR_GROW(old_cap, new_cap)                                                                                                                \
	do {                                                                                                                                        \
		JSTR_ASSERT_IS_SIZE(old_cap);                                                                                                       \
		JSTR_ASSERT_IS_SIZE(new_cap);                                                                                                       \
		while (((old_cap) *= PJARR_GROWTH) < (new_cap))                                                                                     \
			;                                                                                                                           \
		(old_cap) = PJSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT);                                                                        \
	} while (0)

#define PJARR_REALLOC(j, new_cap, malloc_fail)                                                                                                      \
	do {                                                                                                                                        \
		PJARR_CHECK_ARG(j);                                                                                                                 \
		PJARR_GROW(PJARR_CAP(j), new_cap);                                                                                                  \
		PJARR_CAP(j) = PJARR_ALIGN_UP(PJARR_CAP(j) * PJARR_ELEMSZ(j));                                                                      \
		PJARR_DATA(j) = PJARR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j));                                   \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), malloc_fail);                                                                                       \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                                                    \
	} while (0)

#define PJARR_REALLOCEXACT(j, new_cap, malloc_fail)                                                                                                 \
	do {                                                                                                                                        \
		PJARR_CHECK_ARG(j);                                                                                                                 \
		PJARR_CAP(j) = PJARR_ALIGN_UP(PJARR_CAP(j) * PJARR_ELEMSZ);                                                                         \
		PJARR_DATA(j) = PJARR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j)));                                  \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), malloc_fail);                                                                                       \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                                                    \
	} while (0)

#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define PJARR_CHECK_VAL(j) JSTR_ASSERT(JSTR_SAME_TYPE(PJARR_VAL, *PJARR_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
#	define PJARR_CHECK_VAL(j) JSTR_ASSERT(sizeof(*PJARR_DATA(j)) == PJARR_VAL, "Passing illegal value incompatible with the array type.")
#endif

#endif /* JARR_MACROS_DEF_H */
