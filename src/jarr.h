#ifndef JARR_DEF_H
#define JARR_DEF_H 1

#include "jstr_macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr_builder.h"
#include "jstr_config.h"
#include "jarr_macros.h"
#include "jstr_macros.h"
#include "jstr_pp_arrcpy_va_args.h"
#include "jstr_templates.h"

#define jarr(T, name)                                                             \
	typedef struct pjarr_##name##_ty {                                        \
		T *PJARR_DATA_NAME;                                               \
		size_t PJARR_SIZE_NAME;                                           \
		size_t PJARR_CAPACITY_NAME;                                       \
	} pjarr_##name##_ty;                                                      \
	pjarr_##name##_ty name;

#define jarr_free(j)                                                              \
	do {                                                                      \
		free(PJARR_DATA(j));                                              \
		PJARR_NULLIFY(j);                                                 \
	} while (0)

#if PJARR_NULLIFY_PTR_ON_DELETE
#	define PJARR_NULLIFY(j) ((j)->data == NULL)
#else
#	define PJARR_NULLIFY(j)
#endif

#define jarr_err_exit(j)                                                          \
	do {                                                                      \
		if (jstr_unlikely((j)->data == NULL))                                          \
			pjstr_err_exit(__FILE__, __LINE__, __func__);                  \
	} while (0)

#undef PJARR_NULLIFY

/* Allocate PTR. */
#define jarr_alloc(j, new_cap)                                                    \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		PJARR_SZ(j) = 0;                                                  \
		PJARR_CAP(j) = PJARR_MIN_ALLOC(j, new_cap);                       \
		PJARR_DATA(j) = malloc(PJARR_CAP(j));                             \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), break);                           \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                  \
	} while (0)

/* Allocate PTR. */
#define jarr_alloc_push(j, value)                                                 \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		PJARR_SZ(j) = 1;                                                  \
		PJARR_CAP(j) = PJSTR_MIN_CAP;                                     \
		PJARR_DATA(j) = malloc(PJARR_CAP(j));                             \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), break);                           \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                  \
		PJARR_DATA(j)                                                     \
		[0] = (value);                                                    \
	} while (0)

/* Allocate elements to PTR. */
#define jarr_alloc_cat(j, ...)                                                    \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		PJARR_CAP(j) = PJARR_MIN_ALLOC(j, PJSTR_PP_NARG(__VA_ARGS__));    \
		PJARR_DATA(j) = PJARR_CAST(PJARR_DATA(j), malloc(PJARR_CAP(j)));  \
		PJSTR_MALLOC_ERR(PJARR_DATA(j), break);                           \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                  \
		PJARR_SZ(j) = PJSTR_PP_NARG(__VA_ARGS__);                         \
		PJSTR_PP_ARRCPY_VA_ARGS(PJARR_DATA(j), __VA_ARGS__);              \
	} while (0)

/* Pop PTR[0]. */
#define jarr_pop_front(j)                                                         \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		if (jstr_unlikely(PJARR_CAP(j) == 0))                             \
			break;                                                    \
		memmove(PJARR_DATA(j), PJARR_DATA(j) + 1, --PJARR_SZ(j));         \
	} while (0)

/* Pop PTR[size]. */
#define jarr_pop_back(j)                                                          \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		if (jstr_unlikely(PJARR_CAP(j) == 0))                             \
			break;                                                    \
		*(PJARR_DATA(j) + --PJARR_SZ(j)) = '\0';                          \
	} while (0)

/* Push VAL to back of PTR. */
#define jarr_push_back(j, value)                                                  \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		PJARR_CHECK_VAL(j);                                               \
		if (jstr_unlikely(PJARR_CAP(j) == PJARR_SZ(j)))                   \
			PJARR_REALLOCEXACT(j, PJARR_SZ(j) * PJARR_GROWTH, break); \
		PJARR_DATA(j)                                                     \
		[PJARR_SZ(j)++] = (value);                                        \
	} while (0)

/* Push VAL to front of P. */
#define jarr_push_front(j, value)                                                 \
	do {                                                                      \
		PJARR_CHECK_ARG(j);                                               \
		PJARR_CHECK_VAL(j);                                               \
		if (jstr_unlikely(PJARR_CAP(j) == PJARR_SZ(j)))                   \
			PJARR_REALLOCEXACT(j, PJARR_SZ(j) * PJARR_GROWTH, break); \
		PJARR_MEMMOVE(PJARR_DATA(j) + 1, PJARR_DATA(j), PJARR_SZ(j)++);   \
		PJARR_DATA(j)                                                     \
		[0] = (value);                                                    \
	} while (0)

#endif /* JARR_DEF_H */
