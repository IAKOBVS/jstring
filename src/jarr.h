#ifndef JARR_H
#define JARR_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "_jstr-pp-arrcpy-va-args.h"
#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-macros.h"

#define R JSTR_RESTRICT

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

#define PJARR_ALIGN_UP(new_cap)		 JSTR_ALIGN_UP(new_cap, PJSTR_MALLOC_ALIGNMENT)
#define PJARR_MIN_ALLOC(j, new_cap)	 JSTR_MIN_ALLOC(PJARR_ALIGN_UP((size_t)(((new_cap)*PJARR_ELEMSZ(j) * PJARR_ALLOC_MULTIPLIER))))
#define PJARR_MIN_ALLOCEXACT(j, new_cap) JSTR_MIN_ALLOCEXACT(PJARR_ALIGN_UP((new_cap)*PJARR_ELEMSZ(j)))

#define PJARR_CHECK_ARG(j)                         \
	do {                                       \
		JSTR_ASSERT_IS_SIZE(PJARR_SZ(j));  \
		JSTR_ASSERT_IS_SIZE(PJARR_CAP(j)); \
	} while (0)
#define PJARR_GROW(old_cap, new_cap)                                        \
	do {                                                                \
		JSTR_ASSERT_IS_SIZE(old_cap);                               \
		JSTR_ASSERT_IS_SIZE(new_cap);                               \
		if (jstr_unlikely(old_cap == 0))                            \
			old_cap = JSTR_MIN_CAP / PJARR_ALLOC_MULTIPLIER;    \
		do                                                          \
			(old_cap) *= PJARR_GROWTH;                          \
		while ((old_cap) < (new_cap));                              \
		(old_cap) = JSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define PJARR_REALLOC(j, new_cap, do_on_malloc_err)                                                                \
	do {                                                                                                       \
		PJARR_CHECK_ARG(j);                                                                                \
		PJARR_GROW(PJARR_CAP(j), new_cap);                                                                 \
		PJARR_CAP(j) = PJARR_ALIGN_UP(PJARR_CAP(j) * PJARR_ELEMSZ(j));                                     \
		PJARR_DATA(j) = PJSTR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j))); \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), do_on_malloc_err);                                                 \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                   \
	} while (0)
#define PJARR_REALLOCEXACT(j, new_cap, do_on_malloc_err)                                                           \
	do {                                                                                                       \
		PJARR_CHECK_ARG(j);                                                                                \
		PJARR_CAP(j) = PJARR_ALIGN_UP(PJARR_CAP(j) * PJARR_ELEMSZ);                                        \
		PJARR_DATA(j) = PJSTR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j))); \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), do_on_malloc_err);                                                 \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                   \
	} while (0)
#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define PJARR_CHECK_VAL(j, value) JSTR_ASSERT(JSTR_SAME_TYPE(value, *PJARR_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
#	define PJARR_CHECK_VAL(j, value) JSTR_ASSERT(sizeof(*PJARR_DATA(j)) == value, "Passing illegal value incompatible with the array type.")
#endif

#define PJARR_NULLIFY_MEMBERS(j) (PJARR_SZ(j) = 0, PJARR_CAP(j) = 0)
#define PJARR_NULLIFY(j)	 ((j)->data == NULL, PJARR_NULLIFY_MEMBERS(j))

#define JARR_INIT \
	{         \
		0 \
	}
#define jarr_ty(T, name)                    \
	typedef struct pjarr_##name##_ty {  \
		T *PJARR_DATA_NAME;         \
		size_t PJARR_SIZE_NAME;     \
		size_t PJARR_CAPACITY_NAME; \
	} jarr_##name##_ty;                 \
	jarr_##name##_ty name
#define jarr_free(j)                 \
	do {                         \
		free(PJARR_DATA(j)); \
		PJARR_NULLIFY(j);    \
	} while (0)
#define jarr_err_exit(j)                                                      \
	do {                                                                  \
		if (jstr_unlikely((j)->data == NULL))                         \
			pjstr_err_exit(__FILE__, __LINE__, JSTR_ASSERT_FUNC); \
	} while (0)
#undef PJARR_NULLIFY

/* Add elements to end of PTR. */
#define jarr_cat(j, ...)                                                                       \
	do {                                                                                   \
		PJARR_CHECK_ARG(j);                                                            \
		PJARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));                           \
		if (jstr_unlikely(PJARR_CAP(j) <= (PJARR_SZ(j) + PJSTR_PP_NARG(__VA_ARGS__)))) \
			PJARR_REALLOC(j, PJARR_CAP(j), PJARR_NULLIFY_MEMBERS(j); break;);      \
		PJSTR_PP_ARRCPY_VA_ARGS(PJARR_DATA(j) + PJARR_SZ(j), __VA_ARGS__);             \
		PJARR_SZ(j) += PJSTR_PP_NARG(__VA_ARGS__);                                     \
	} while (0)
/* Pop PTR[0]. */
#define jarr_pop_front(j)                                                 \
	do {                                                              \
		PJARR_CHECK_ARG(j);                                       \
		if (jstr_unlikely(PJARR_CAP(j) == 0))                     \
			PJARR_NULLIFY_MEMBERS(j);                         \
		break;                                                    \
		;                                                         \
		memmove(PJARR_DATA(j), PJARR_DATA(j) + 1, --PJARR_SZ(j)); \
	} while (0)
/* Pop end of PTR. */
#define jarr_pop_back(j)                                 \
	do {                                             \
		PJARR_CHECK_ARG(j);                      \
		if (jstr_unlikely(PJARR_CAP(j) == 0))    \
			PJARR_NULLIFY_MEMBERS(j);        \
		break;                                   \
		;                                        \
		*(PJARR_DATA(j) + --PJARR_SZ(j)) = '\0'; \
	} while (0)
/* Push VAL to back of PTR. */
#define jarr_push_back(j, value)                                                                             \
	do {                                                                                                 \
		PJARR_CHECK_ARG(j);                                                                          \
		PJARR_CHECK_VAL(j, value);                                                                   \
		if (jstr_unlikely(PJARR_CAP(j) == PJARR_SZ(j)))                                              \
			PJARR_REALLOCEXACT(j, PJARR_SZ(j) * PJARR_GROWTH, PJARR_NULLIFY_MEMBERS(j); break;); \
		PJARR_DATA(j)                                                                                \
		[PJARR_SZ(j)++] = (value);                                                                   \
	} while (0)
/* Push VAL to front of P. */
#define jarr_push_front(j, value)                                                                            \
	do {                                                                                                 \
		PJARR_CHECK_ARG(j);                                                                          \
		PJARR_CHECK_VAL(j, value);                                                                   \
		if (jstr_unlikely(PJARR_CAP(j) == PJARR_SZ(j)))                                              \
			PJARR_REALLOCEXACT(j, PJARR_SZ(j) * PJARR_GROWTH, PJARR_NULLIFY_MEMBERS(j); break;); \
		PJARR_MEMMOVE(PJARR_DATA(j) + 1, PJARR_DATA(j), PJARR_SZ(j)++);                              \
		PJARR_DATA(j)                                                                                \
		[0] = (value);                                                                               \
	} while (0)
#define jarr_at(j, index) \
	((jstr_likely(index < (j)->size)) ? ((j)->data)[(index)] : (PJARR_ERR_EXIT_MSG(__FILE__, __LINE__, JSTR_ASSERT_FUNC, "Index out of bounds.")))
#define jarr_foreachi(j, iterator) \
	for (size_t iterator = 0, _max_elem_##iterator = (j)->size; iterator < _max_elem_##iterator; ++iterator)

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void
PJARR_ERR(const char *JSTR_RESTRICT FILE_,
	  const int LINE_,
	  const char *JSTR_RESTRICT func_)
{
	fprintf(stderr, "%s:%d:%s\n", FILE_, LINE_, func_);
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void
PJARR_ERR_EXIT_MSG(const char *JSTR_RESTRICT FILE_,
		   const int LINE_,
		   const char *JSTR_RESTRICT func_,
		   const char *JSTR_RESTRICT msg)
{
	fprintf(stderr, "%s:%d:%s:%s\n", FILE_, LINE_, func_, msg);
	exit(EXIT_FAILURE);
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void
PJARR_ERR_EXIT(const char *JSTR_RESTRICT FILE_,
	       const int LINE_,
	       const char *JSTR_RESTRICT func_)
{
	fprintf(stderr, "%s:%d:%s\n", FILE_, LINE_, func_);
	exit(EXIT_FAILURE);
}

PJSTR_END_DECLS

#undef R

#endif /* JARR_H */
