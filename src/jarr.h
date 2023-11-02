#ifndef JARR_H
#define JARR_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-macros.h"
#include "_jstr-pp-arrcpy-va-args.h"

#define R JSTR_RESTRICT

#define P_JARR_DATA_NAME     data
#define P_JARR_SIZE_NAME     size
#define P_JARR_CAPACITY_NAME capacity

#define P_JARR_ELEMSZ(j) (sizeof((P_JARR_DATA(j))[0]))
#define P_JARR_ARRSZ(j)	 (sizeof(P_JARR_DATA(j)) / sizeof((P_JARR_DATA(j))[0]))
#define P_JARR_DATA(j)	 ((j)->P_JARR_DATA_NAME)
#define P_JARR_SZ(j)	 ((j)->P_JARR_SIZE_NAME)
#define P_JARR_CAP(j)	 ((j)->P_JARR_CAPACITY_NAME)

#define P_JARR_MEMMOVE(j, dst, src, n) memmove(dst, src, (n)*P_JARR_ELEMSZ(j))
#define P_JARR_MEMCPY(j, dst, src, n)  memcpy(dst, src, (n)*P_JARR_ELEMSZ(j))

#define P_JARR_ALIGN_UP(new_cap)	  JSTR_ALIGN_UP(new_cap, PJSTR_MALLOC_ALIGNMENT)
#define P_JARR_MIN_ALLOC(j, new_cap)	  JSTR_MIN_ALLOC(P_JARR_ALIGN_UP((size_t)(((new_cap)*P_JARR_ELEMSZ(j) * P_JARR_ALLOC_MULTIPLIER))))
#define P_JARR_MIN_ALLOCEXACT(j, new_cap) JSTR_MIN_ALLOCEXACT(P_JARR_ALIGN_UP((new_cap)*P_JARR_ELEMSZ(j)))

#define P_JARR_CHECK_ARG(j)                         \
	do {                                        \
		JSTR_ASSERT_IS_SIZE(P_JARR_SZ(j));  \
		JSTR_ASSERT_IS_SIZE(P_JARR_CAP(j)); \
	} while (0)
#define P_JARR_GROW(old_cap, new_cap)                                       \
	do {                                                                \
		JSTR_ASSERT_IS_SIZE(old_cap);                               \
		JSTR_ASSERT_IS_SIZE(new_cap);                               \
		if (jstr_unlikely(old_cap == 0))                            \
			old_cap = JSTR_MIN_CAP / P_JARR_ALLOC_MULTIPLIER;   \
		do                                                          \
			(old_cap) *= P_JARR_GROWTH;                         \
		while ((old_cap) < (new_cap));                              \
		(old_cap) = JSTR_ALIGN_UP(old_cap, PJSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define P_JARR_REALLOC(j, new_cap, do_on_malloc_err)                                                                    \
	do {                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                    \
		P_JARR_GROW(P_JARR_CAP(j), new_cap);                                                                    \
		P_JARR_CAP(j) = P_JARR_ALIGN_UP(P_JARR_CAP(j) * P_JARR_ELEMSZ(j));                                      \
		P_JARR_DATA(j) = PJSTR_CAST(P_JARR_DATA(j), realloc(P_JARR_DATA(j), P_JARR_CAP(j) * P_JARR_ELEMSZ(j))); \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), do_on_malloc_err);                                                     \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                      \
	} while (0)
#define P_JARR_REALLOCEXACT(j, new_cap, do_on_malloc_err)                                                               \
	do {                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                    \
		P_JARR_CAP(j) = P_JARR_ALIGN_UP(P_JARR_CAP(j) * P_JARR_ELEMSZ);                                         \
		P_JARR_DATA(j) = PJSTR_CAST(P_JARR_DATA(j), realloc(P_JARR_DATA(j), P_JARR_CAP(j) * P_JARR_ELEMSZ(j))); \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), do_on_malloc_err);                                                     \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                      \
	} while (0)
#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define P_JARR_CHECK_VAL(j, value) JSTR_ASSERT(JSTR_SAME_TYPE(value, *P_JARR_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
#	define P_JARR_CHECK_VAL(j, value) JSTR_ASSERT(sizeof(*P_JARR_DATA(j)) == value, "Passing illegal value incompatible with the array type.")
#endif

#define P_JARR_NULLIFY_MEMBERS(j) (P_JARR_SZ(j) = 0, P_JARR_CAP(j) = 0)
#define P_JARR_NULLIFY(j) ((j)->data == NULL, P_JARR_NULLIFY_MEMBERS(j))

#define JARR_INIT {0}
#define jarr_ty(T, name)                                                                                                                                \
	typedef struct pjarr_##name##_ty {                                                                                                              \
		T *P_JARR_DATA_NAME;                                                                                                                    \
		size_t P_JARR_SIZE_NAME;                                                                                                                \
		size_t P_JARR_CAPACITY_NAME;                                                                                                            \
	} jarr_##name##_ty;                                                                                                                             \
	jarr_##name##_ty name
#define jarr_free(j)                                                                                                                                    \
	do {                                                                                                                                            \
		free(P_JARR_DATA(j));                                                                                                                   \
		P_JARR_NULLIFY(j);                                                                                                                      \
	} while (0)
#define jarr_err_exit(j)                                                                                                                                \
	do {                                                                                                                                            \
		if (jstr_unlikely((j)->data == NULL))                                                                                                   \
			pjstr_err_exit(__FILE__, __LINE__, JSTR_ASSERT_FUNC);                                                                           \
	} while (0)
#undef P_JARR_NULLIFY

/* Add elements to end of PTR. */
#define jarr_cat(j, ...)                                                                                                                                \
	do {                                                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                                                    \
		P_JARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));                                                                                   \
		if (jstr_unlikely(P_JARR_CAP(j) <= (P_JARR_SZ(j) + PJSTR_PP_NARG(__VA_ARGS__))))                                                        \
			P_JARR_REALLOC(j, P_JARR_CAP(j), P_JARR_NULLIFY_MEMBERS(j); break;);                                                        \
		PJSTR_PP_ARRCPY_VA_ARGS(P_JARR_DATA(j) + P_JARR_SZ(j), __VA_ARGS__);                                                                    \
		P_JARR_SZ(j) += PJSTR_PP_NARG(__VA_ARGS__);                                                                                             \
	} while (0)
/* Pop PTR[0]. */
#define jarr_pop_front(j)                                                                                                                               \
	do {                                                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                                                    \
		if (jstr_unlikely(P_JARR_CAP(j) == 0))                                                                                                  \
			P_JARR_NULLIFY_MEMBERS(j); break;;                                                                                          \
		memmove(P_JARR_DATA(j), P_JARR_DATA(j) + 1, --P_JARR_SZ(j));                                                                            \
	} while (0)
/* Pop end of PTR. */
#define jarr_pop_back(j)                                                                                                                                \
	do {                                                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                                                    \
		if (jstr_unlikely(P_JARR_CAP(j) == 0))                                                                                                  \
			P_JARR_NULLIFY_MEMBERS(j); break;;                                                                                          \
		*(P_JARR_DATA(j) + --P_JARR_SZ(j)) = '\0';                                                                                              \
	} while (0)
/* Push VAL to back of PTR. */
#define jarr_push_back(j, value)                                                                                                                        \
	do {                                                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                                                    \
		P_JARR_CHECK_VAL(j, value);                                                                                                             \
		if (jstr_unlikely(P_JARR_CAP(j) == P_JARR_SZ(j)))                                                                                       \
			P_JARR_REALLOCEXACT(j, P_JARR_SZ(j) * P_JARR_GROWTH, P_JARR_NULLIFY_MEMBERS(j); break;);                                    \
		P_JARR_DATA(j)                                                                                                                          \
		[P_JARR_SZ(j)++] = (value);                                                                                                             \
	} while (0)
/* Push VAL to front of P. */
#define jarr_push_front(j, value)                                                                                                                       \
	do {                                                                                                                                            \
		P_JARR_CHECK_ARG(j);                                                                                                                    \
		P_JARR_CHECK_VAL(j, value);                                                                                                             \
		if (jstr_unlikely(P_JARR_CAP(j) == P_JARR_SZ(j)))                                                                                       \
			P_JARR_REALLOCEXACT(j, P_JARR_SZ(j) * P_JARR_GROWTH, P_JARR_NULLIFY_MEMBERS(j); break;);                                    \
		P_JARR_MEMMOVE(P_JARR_DATA(j) + 1, P_JARR_DATA(j), P_JARR_SZ(j)++);                                                                     \
		P_JARR_DATA(j)                                                                                                                          \
		[0] = (value);                                                                                                                          \
	} while (0)
#define jarr_at(j, index)                                                                                                                               \
	((jstr_likely(index < (j)->size)) ? ((j)->data)[(index)] : (P_JARR_ERR_EXIT_MSG(__FILE__, __LINE__, JSTR_ASSERT_FUNC, "Index out of bounds.")))
#define jarr_foreachi(j, iterator)                                                                                                                       \
	for (size_t iterator = 0, _max_elem_##iterator = (j)->size; iterator < _max_elem_##iterator; ++iterator)

PJSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void P_JARR_ERR(const char *JSTR_RESTRICT FILE_,
		const int LINE_,
		const char *JSTR_RESTRICT func_)
{
	fprintf(stderr, "%s:%d:%s\n", FILE_, LINE_, func_);
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void P_JARR_ERR_EXIT_MSG(const char *JSTR_RESTRICT FILE_,
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
static void P_JARR_ERR_EXIT(const char *JSTR_RESTRICT FILE_,
		const int LINE_,
		const char *JSTR_RESTRICT func_)
{
	fprintf(stderr, "%s:%d:%s\n", FILE_, LINE_, func_);
	exit(EXIT_FAILURE);
}

PJSTR_END_DECLS

#undef R

#endif /* JARR_H */
