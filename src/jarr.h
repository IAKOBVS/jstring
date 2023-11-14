/* See LICENSE file for copyright and license details. */

#ifndef JARR_H
#define JARR_H 1

#include "jstr-macros.h"

JSTRP__BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
JSTRP__END_DECLS

#include "_jstr-pp-arrcpy-va-args.h"
#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-macros.h"

#define R JSTR_RESTRICT

#define JARRP_DATA_NAME     data
#define JARRP_SIZE_NAME     size
#define JARRP_CAPACITY_NAME capacity

#define JARR_INIT \
	{         \
		0 \
	}

#define jarr_ty(T, name)                    \
	typedef struct pjarr_##name##_ty {  \
		T *JARRP_DATA_NAME;         \
		size_t JARRP_SIZE_NAME;     \
		size_t JARRP_CAPACITY_NAME; \
	} jarr_##name##_ty;                 \
	jarr_##name##_ty name

#define JARRP_ELEMSZ(j) (sizeof((JARRP_DATA(j))[0]))
#define JARRP_ARRSZ(j)  (sizeof(JARRP_DATA(j)) / sizeof((JARRP_DATA(j))[0]))
#define JARRP_DATA(j)   ((j)->JARRP_DATA_NAME)
#define JARRP_SZ(j)     ((j)->JARRP_SIZE_NAME)
#define JARRP_CAP(j)    ((j)->JARRP_CAPACITY_NAME)

#define JARRP_MIN_CAP(j) (JSTRP__MIN_CAP / JARRP_ELEMSZ(j))

#define JARRP_MEMMOVE(j, dst, src, n) memmove(dst, src, (n)*JARRP_ELEMSZ(j))
#define JARRP_MEMCPY(j, dst, src, n)  memcpy(dst, src, (n)*JARRP_ELEMSZ(j))

#define jarr_chk(j)        (jstr_unlikely(JARRP_DATA(j) == NULL))
#define jarr_err(msg)      jstr_err(msg)
#define jarr_err_exit(msg) jstr_err_exit(msg)

#define JARRP_ALIGN_UP(j, base) ((JARRP_ELEMSZ(j) < (sizeof(size_t) + sizeof(size_t))) ? JSTR_ALIGN_UP_STR(base) : base)

#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
#	define JARRP_MALLOC_ERR(j, do_on_malloc_err)         \
		if (jstr_unlikely((JARRP_DATA(j)) == NULL)) { \
			JARRP_NULLIFY_MEMBERS(j);             \
			do_on_malloc_err;                     \
			jstr_err_exit("");                    \
		}
#else
#	define JARRP_MALLOC_ERR(j, do_on_malloc_err)         \
		if (jstr_unlikely((JARRP_DATA(j)) == NULL)) { \
			JARRP_NULLIFY_MEMBERS(j);             \
			do_on_malloc_err;                     \
		}
#endif

#define JARRP_CHECK_ARG(j)                         \
	do {                                       \
		JSTR_ASSERT_IS_SIZE(JARRP_SZ(j));  \
		JSTR_ASSERT_IS_SIZE(JARRP_CAP(j)); \
	} while (0)
#define JARRP_GROW(old_cap, new_cap)                                        \
	do {                                                                \
		JSTR_ASSERT_IS_SIZE(old_cap);                               \
		JSTR_ASSERT_IS_SIZE(new_cap);                               \
		if (jstr_unlikely(old_cap == 0))                            \
			old_cap = JSTRP__MIN_CAP / JARRP_ALLOC_MULTIPLIER;    \
		do                                                          \
			(old_cap) *= JARRP_GROWTH;                          \
		while ((old_cap) < (new_cap));                              \
		(old_cap) = JSTR_ALIGN_UP(old_cap, JSTRP__MALLOC_ALIGNMENT); \
	} while (0)
#define jarr_reserve(j, new_cap)                                                                                   \
	do {                                                                                                       \
		JARRP_CHECK_ARG(j);                                                                                \
		if (jstr_unlikely(JARRP_CAP(j) == 0))                                                              \
			JARRP_CAP(j) = JSTRP__MIN_CAP / JARRP_ELEMSZ(j);                                             \
		JARRP_GROW(JARRP_CAP(j), new_cap);                                                                 \
		JARRP_CAP(j) = JARRP_ALIGN_UP(j, JARRP_CAP(j) * JARRP_ELEMSZ(j));                                  \
		JARRP_DATA(j) = JSTRP__CAST(JARRP_DATA(j), realloc(JARRP_DATA(j), JARRP_CAP(j) * JARRP_ELEMSZ(j))); \
		if (jarr_chk(j))                                                                                   \
			break;                                                                                     \
		JARRP_CAP(j) /= JARRP_ELEMSZ(j);                                                                   \
	} while (0)
#define jarr_reserveexact(j, new_cap)                                                                              \
	do {                                                                                                       \
		JARRP_CHECK_ARG(j);                                                                                \
		JARRP_CAP(j) = JARRP_ALIGN_UP(j, JARRP_CAP(j) * JARRP_ELEMSZ(j));                                  \
		JARRP_DATA(j) = JSTRP__CAST(JARRP_DATA(j), realloc(JARRP_DATA(j), JARRP_CAP(j) * JARRP_ELEMSZ(j))); \
		if (jarr_chk(j))                                                                                   \
			break;                                                                                     \
		JARRP_CAP(j) /= JARRP_ELEMSZ(j);                                                                   \
	} while (0)
#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define JARRP_CHECK_VAL(j, value) JSTR_ASSERT(JSTR_SAME_TYPE(value, *JARRP_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
#	define JARRP_CHECK_VAL(j, value) \
		do {                      \
		} while (0)
#endif

#define JARRP_NULLIFY_MEMBERS(j)  \
	do {                      \
		JARRP_SZ(j) = 0;  \
		JARRP_CAP(j) = 0; \
	} while (0)
#define JARRP_NULLIFY(j)                  \
	do {                              \
		JARRP_DATA(j) = NULL;     \
		JARRP_NULLIFY_MEMBERS(j); \
	} while (0)

#define jarr_free(j)                 \
	do {                         \
		free(JARRP_DATA(j)); \
		JARRP_NULLIFY(j);    \
	} while (0)

/* Add elements to end of PTR. */
#define jarr_assign(j, ...)                                                     \
	do {                                                                    \
		JARRP_CHECK_ARG(j);                                             \
		JARRP_CHECK_VAL(j, JSTRP__PP_FIRST_ARG(__VA_ARGS__));            \
		if (jstr_unlikely(JARRP_CAP(j) < JSTRP__PP_NARG(__VA_ARGS__))) { \
			jarr_reserve(j, JSTRP__PP_NARG(__VA_ARGS__));            \
			JARRP_MALLOC_ERR(j, break)                              \
		}                                                               \
		JSTRP__PP_ARRCPY_VA_ARGS(JARRP_SZ(j), __VA_ARGS__);              \
		JARRP_SZ(j) = JSTRP__PP_NARG(__VA_ARGS__);                       \
	} while (0)

/* Add elements to end of PTR. */
#define jarr_cat(j, ...)                                                                        \
	do {                                                                                    \
		JARRP_CHECK_ARG(j);                                                             \
		JARRP_CHECK_VAL(j, JSTRP__PP_FIRST_ARG(__VA_ARGS__));                            \
		if (jstr_unlikely(JARRP_CAP(j) < (JARRP_SZ(j) + JSTRP__PP_NARG(__VA_ARGS__)))) { \
			jarr_reserve(j, (JARRP_SZ(j) + JSTRP__PP_NARG(__VA_ARGS__)));            \
			JARRP_MALLOC_ERR(j, break)                                              \
		}                                                                               \
		JSTRP__PP_ARRCPY_VA_ARGS(JARRP_DATA(j) + JARRP_SZ(j), __VA_ARGS__);              \
		JARRP_SZ(j) += JSTRP__PP_NARG(__VA_ARGS__);                                      \
	} while (0)
/* Pop PTR[0]. */
#define jarr_popfront(j)                                                  \
	do {                                                              \
		JARRP_CHECK_ARG(j);                                       \
		if (jstr_unlikely(JARRP_CAP(j) == 0)) {                   \
			JARRP_NULLIFY_MEMBERS(j);                         \
			break;                                            \
		}                                                         \
		memmove(JARRP_DATA(j), JARRP_DATA(j) + 1, --JARRP_SZ(j)); \
	} while (0)
/* Pop end of PTR. */
#define jarr_popback(j)                                  \
	do {                                             \
		JARRP_CHECK_ARG(j);                      \
		if (jstr_unlikely(JARRP_CAP(j) == 0)) {  \
			JARRP_NULLIFY_MEMBERS(j);        \
			break;                           \
		}                                        \
		*(JARRP_DATA(j) + --JARRP_SZ(j)) = '\0'; \
	} while (0)
/* Push VAL to back of PTR. */
#define jarr_pushback(j, value)                                            \
	do {                                                               \
		JARRP_CHECK_ARG(j);                                        \
		JARRP_CHECK_VAL(j, value);                                 \
		if (jstr_unlikely(JARRP_CAP(j) < JARRP_SZ(j) + 1)) {       \
			if (jstr_unlikely(JARRP_CAP(j) == 0))              \
				JARRP_CAP(j) = JARRP_MIN_CAP(j);           \
			jarr_reserveexact(j, JARRP_CAP(j) * JARRP_GROWTH); \
			JARRP_MALLOC_ERR(j, break)                         \
		}                                                          \
		*(JARRP_DATA(j) + JARRP_SZ(j)++) = (value);                \
	} while (0)
/* Push VAL to front of P. */
#define jarr_pushfront(j, value)                                                   \
	do {                                                                       \
		JARRP_CHECK_ARG(j);                                                \
		JARRP_CHECK_VAL(j, value);                                         \
		if (jstr_unlikely(JARRP_CAP(j) < JARRP_SZ(j) + 1)) {               \
			if (jstr_unlikely(JARRP_CAP(j) == 0))                      \
				JARRP_CAP(j) = JARRP_MIN_CAP(j);                   \
			jarr_reserveexact(j, JARRP_CAP(j) * JARRP_GROWTH);         \
			JARRP_MALLOC_ERR(j, break)                                 \
		}                                                                  \
		JARRP_MEMMOVE(j, JARRP_DATA(j) + 1, JARRP_DATA(j), JARRP_SZ(j)++); \
		*JARRP_DATA(j) = (value);                                          \
	} while (0)

#define jarr_foreachi(j, iterator) \
	for (size_t iterator = 0, _max_elem_##iterator = (j)->size; iterator < _max_elem_##iterator; ++iterator)

#define jarr_start(j, p) JARRP_DATA(j)
#define jarr_end(j, p)   (JARRP_DATA(j) + JARRP_SZ(j))

#if JSTR_DEBUG
#	define jarr_at(j, idx) \
		(jstr_likely(idx < JARRP_SZ(j)) ? (JARRP_DATA(j) + (idx)) : (jstr_err_exit("Index out of bounds."), JARRP_DATA(j)))
#else
#	define jarr_at(j, idx) \
		(JARRP_DATA(j) + (idx))
#endif

JSTRP__BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void
JARRP_ERR(const char *JSTR_RESTRICT FILE_,
          const int LINE_,
          const char *JSTR_RESTRICT func_)
{
	fprintf(stderr, "%s:%d:%s\n", FILE_, LINE_, func_);
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
static void
JARRP_ERR_EXIT(const char *JSTR_RESTRICT FILE_,
               const int LINE_,
               const char *JSTR_RESTRICT func_,
               const char *JSTR_RESTRICT msg)
{
	fprintf(stderr, "%s:%d:%s:%s\n", FILE_, LINE_, func_, msg);
	exit(EXIT_FAILURE);
}

JSTRP__END_DECLS

#undef R

#endif /* JARR_H */
