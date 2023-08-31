#ifndef JARR_DEF_H
#define JARR_DEF_H

#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdlib.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "_builder.h"
#include "_config.h"
#include "_jarr_macros.h"
#include "_macros.h"
#include "_pp_arrcpy_va_args.h"
#include "_templates.h"

#define jarr(T, name)                       \
	typedef struct jarr_##name##_ty {   \
		T *PJARR_DATA_NAME;         \
		size_t PJARR_SIZE_NAME;     \
		size_t PJARR_CAPACITY_NAME; \
	} jarr_##name##_ty;                 \
	jarr_##name##_ty name;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define jarr_free(p)        \
	do {                \
		free(p);    \
		(p) = NULL; \
	} while (0)

/* Allocate PTR. */
#define jarr_alloc(PJARR_JARR, newcap)               \
	do {                                         \
		PJARR_CHECK_ARG();                   \
		PJARR_SZ = 0;                        \
		PJARR_CAP = PJARR_MIN_ALLOC(newcap); \
		PJARR_DATA = malloc(PJARR_CAP);      \
		PJSTR_MALLOC_ERR(PJARR_DATA, break); \
		PJARR_CAP /= PJARR_ELEMSZ;           \
	} while (0)

/* Allocate PTR. */
#define jarr_alloc_push(PJARR_JARR, PJARR_VAL)       \
	do {                                         \
		PJARR_CHECK_ARG();                   \
		PJARR_SZ = 1;                        \
		PJARR_CAP = PJARR_MIN_ALLOC(1);      \
		PJARR_DATA = malloc(PJARR_CAP);      \
		PJSTR_MALLOC_ERR(PJARR_DATA, break); \
		PJARR_CAP /= PJARR_ELEMSZ;           \
		PJARR_DATA[0] = PJARR_VAL;           \
	} while (0)

/* Allocate elements to PTR. */
#define jarr_alloc_cat(PJARR_JARR, ...)                                      \
	do {                                                                 \
		PJARR_CHECK_ARG();                                           \
		PJARR_CAP = PJARR_MIN_ALLOC(PJSTR_PP_NARG(__VA_ARGS__));     \
		PJARR_DATA = pjarr_auto_cast(PJARR_DATA, malloc(PJARR_CAP)); \
		PJSTR_MALLOC_ERR(PJARR_DATA, break);                         \
		PJARR_CAP /= PJARR_ELEMSZ;                                   \
		PJARR_SZ = PJSTR_PP_NARG(__VA_ARGS__);                       \
		PJSTR_PP_ARRCPY_VA_ARGS(PJARR_DATA, __VA_ARGS__);            \
	} while (0)

/* Pop PTR[0]. */
#define jarr_pop_front(PJARR_JARR)                               \
	do {                                                     \
		PJARR_CHECK_ARG();                               \
		if (jstr_unlikely(PJARR_CAP == 0))               \
			break;                                   \
		memmove(PJARR_DATA, PJARR_DATA + 1, --PJARR_SZ); \
	} while (0)

/* Pop PTR[size]. */
#define jarr_pop_back(PJARR_JARR)                  \
	do {                                       \
		PJARR_CHECK_ARG();                 \
		if (jstr_unlikely(PJARR_CAP == 0)) \
			break;                     \
		*(PJARR_DATA + --PJARR_SZ) = '\0'; \
	} while (0)

/* Push VAL to back of PTR. */
#define jarr_push_back(PJARR_JARR, PJARR_VAL)                              \
	do {                                                               \
		PJARR_CHECK_ARG();                                         \
		PJARR_CHECK_VAL();                                         \
		if (jstr_unlikely(PJARR_CAP == PJARR_SZ + 1))              \
			PJARR_REALLOC_EXACT(PJARR_SZ *JARR_GROWTH, break); \
		PJARR_DATA[PJARR_SZ++] = PJARR_VAL;                        \
	} while (0)

/* Push VAL to front of P. */
#define jarr_push_front(PJARR_JARR, PJARR_VAL)                             \
	do {                                                               \
		PJARR_CHECK_ARG();                                         \
		PJARR_CHECK_VAL();                                         \
		if (jstr_unlikely(PJARR_CAP == PJARR_SZ + 1))              \
			PJARR_REALLOC_EXACT(PJARR_SZ *JARR_GROWTH, break); \
		PJARR_MEMMOVE(PJARR_DATA + 1, PJARR_DATA, PJARR_SZ++);     \
		PJARR_DATA[0] = PJARR_VAL;                                 \
	} while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JARR_DEF_H */
