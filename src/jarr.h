#ifndef JARR_H
#define JARR_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jarr-macros.h"
#include "jstr-macros.h"
#include "jstr-pp-arrcpy-va-args.h"

#define R JSTR_RESTRICT

#if JARR_NULLIFY_MEMBERS_ON_FREE
#	define P_JARR_NULLIFY_MEMBERS(j) ((j)->size = 0, (j)->cap = 0)
#if JARR_DEBUG
#	define P_JARR_NULLIFY_MEMBERS_ERR(j) (P_JARR_NULLIFY_MEMBERS(j), P_JARR_ERR_EXIT(__FILE__, __LINE__, __func__))
#endif
#else
#	define P_JARR_NULLIFY_MEMBERS(j)
#	define P_JARR_NULLIFY_MEMBERS_ERR(j)
#endif

#if JARR_NULLIFY_PTR_ON_FREE
#	define P_JARR_NULLIFY(j) ((j)->data == NULL, P_JARR_NULLIFY_MEMBERS(j))
#else
#	define P_JARR_NULLIFY(j)
#endif

#define jarr_ty(T, name)                                                                                                                        \
	typedef struct pjarr_##name##_ty {                                                                                                      \
		T *P_JARR_DATA_NAME;                                                                                                            \
		size_t P_JARR_SIZE_NAME;                                                                                                        \
		size_t P_JARR_CAPACITY_NAME;                                                                                                    \
	} jarr_##name##_ty;                                                                                                                     \
	jarr_##name##_ty name;
#define jarr_free(j)                                                                                                                            \
	do {                                                                                                                                    \
		free(P_JARR_DATA(j));                                                                                                           \
		P_JARR_NULLIFY(j);                                                                                                              \
	} while (0)
#define jarr_err_exit(j)                                                                                                                        \
	do {                                                                                                                                    \
		if (jstr_unlikely((j)->data == NULL))                                                                                           \
			pjstr_err_exit(__FILE__, __LINE__, __func__);                                                                          \
	} while (0)
#undef P_JARR_NULLIFY

/* Allocate PTR. */
#define jarr_alloc(j, new_cap)                                                                                                                  \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_SZ(j) = 0;                                                                                                               \
		P_JARR_CAP(j) = P_JARR_MIN_ALLOC(j, new_cap);                                                                                   \
		P_JARR_DATA(j) = malloc(P_JARR_CAP(j));                                                                                         \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                                                       \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                                              \
	} while (0)
/* Allocate PTR and pushing VAL. */
#define jarr_alloc_push(j, value)                                                                                                               \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_SZ(j) = 1;                                                                                                               \
		P_JARR_CAP(j) = JSTR_MIN_CAP;                                                                                                   \
		P_JARR_DATA(j) = malloc(P_JARR_CAP(j));                                                                                         \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                                                       \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                                              \
		P_JARR_DATA(j)                                                                                                                  \
		[0] = (value);                                                                                                                  \
	} while (0)
/* Allocate and pushes to PTR. */
#define jarr_alloc_assign(j, ...)                                                                                                               \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));                                                                          \
		P_JARR_CAP(j) = P_JARR_MIN_ALLOC(j, PJSTR_PP_NARG(__VA_ARGS__));                                                               \
		P_JARR_DATA(j) = P_JARR_CAST(P_JARR_DATA(j), malloc(P_JARR_CAP(j)));                                                            \
		PJSTR_MALLOC_ERR(P_JARR_DATA(j), P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                                                       \
		P_JARR_CAP(j) /= P_JARR_ELEMSZ(j);                                                                                              \
		P_JARR_SZ(j) = PJSTR_PP_NARG(__VA_ARGS__);                                                                                     \
		PJSTR_PP_ARRCPY_VA_ARGS(P_JARR_DATA(j), __VA_ARGS__);                                                                          \
	} while (0)
/* Add elements to end of PTR. */
#define jarr_cat(j, ...)                                                                                                                 \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));                                                                          \
		if (jstr_unlikely(P_JARR_CAP(j) <= (P_JARR_SZ(j) + PJSTR_PP_NARG(__VA_ARGS__))))                                               \
			P_JARR_REALLOC(j, P_JARR_CAP(j), P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                                                \
		PJSTR_PP_ARRCPY_VA_ARGS(P_JARR_DATA(j) + P_JARR_SZ(j), __VA_ARGS__);                                                           \
		P_JARR_SZ(j) += PJSTR_PP_NARG(__VA_ARGS__);                                                                                    \
	} while (0)
/* Pop PTR[0]. */
#define jarr_pop_front(j)                                                                                                                       \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		if (jstr_unlikely(P_JARR_CAP(j) == 0))                                                                                          \
			P_JARR_NULLIFY_MEMBERS_ERR(j); break;;                                                                                  \
		memmove(P_JARR_DATA(j), P_JARR_DATA(j) + 1, --P_JARR_SZ(j));                                                                    \
	} while (0)
/* Pop end of PTR. */
#define jarr_pop_back(j)                                                                                                                        \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		if (jstr_unlikely(P_JARR_CAP(j) == 0))                                                                                          \
			P_JARR_NULLIFY_MEMBERS_ERR(j); break;;                                                                                  \
		*(P_JARR_DATA(j) + --P_JARR_SZ(j)) = '\0';                                                                                      \
	} while (0)
/* Push VAL to back of PTR. */
#define jarr_push_back(j, value)                                                                                                                \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_CHECK_VAL(j, value);                                                                                                     \
		if (jstr_unlikely(P_JARR_CAP(j) == P_JARR_SZ(j)))                                                                               \
			P_JARR_REALLOCEXACT(j, P_JARR_SZ(j) * P_JARR_GROWTH, P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                            \
		P_JARR_DATA(j)                                                                                                                  \
		[P_JARR_SZ(j)++] = (value);                                                                                                     \
	} while (0)
/* Push VAL to front of P. */
#define jarr_push_front(j, value)                                                                                                               \
	do {                                                                                                                                    \
		P_JARR_CHECK_ARG(j);                                                                                                            \
		P_JARR_CHECK_VAL(j, value);                                                                                                     \
		if (jstr_unlikely(P_JARR_CAP(j) == P_JARR_SZ(j)))                                                                               \
			P_JARR_REALLOCEXACT(j, P_JARR_SZ(j) * P_JARR_GROWTH, P_JARR_NULLIFY_MEMBERS_ERR(j); break;);                            \
		P_JARR_MEMMOVE(P_JARR_DATA(j) + 1, P_JARR_DATA(j), P_JARR_SZ(j)++);                                                             \
		P_JARR_DATA(j)                                                                                                                  \
		[0] = (value);                                                                                                                  \
	} while (0)
#define jarr_at(j, index)                                                                                                                       \
	((jstr_likely(index < (j)->size)) ? ((j)->data)[(index)] : (P_JARR_ERR_EXIT_MSG(__FILE__, __LINE__, __func__, "Index out of bounds.")))
#define jarr_foreach(j, iterator)                                                                                                               \
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
