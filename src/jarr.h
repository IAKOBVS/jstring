/* Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
   This file is part of the jstring library.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   MIT License (Expat) */

#ifndef JARR_H
#define JARR_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "_jstr-macros-pp-arrcp-va-args.h"
#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-macros.h"

#define R JSTR_RESTRICT

#define PJARR_DATA_NAME     data
#define PJARR_SIZE_NAME     size
#define PJARR_CAPACITY_NAME capacity

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

#define jarr_chk(j)      (jstr_unlikely(PJARR_DATA(j) == NULL))
#define jarr_err(msg)    jstr_err(msg)
#define jarr_errdie(msg) jstr_errdie(msg)
#define PJARR_ELEMSZ(j) (sizeof((PJARR_DATA(j))[0]))
#define PJARR_ARRSZ(j)  (sizeof(PJARR_DATA(j)) / sizeof((PJARR_DATA(j))[0]))
#define PJARR_DATA(j)   ((j)->PJARR_DATA_NAME)
#define PJARR_SZ(j)     ((j)->PJARR_SIZE_NAME)
#define PJARR_CAP(j)    ((j)->PJARR_CAPACITY_NAME)
#define PJARR_MIN_CAP(j) (JSTR_MIN_CAP / PJARR_ELEMSZ(j))
#define PJARR_MEMMOVE(j, dst, src, n) memmove(dst, src, (n)*PJARR_ELEMSZ(j))
#define PJARR_MEMCPY(j, dst, src, n)  memcpy(dst, src, (n)*PJARR_ELEMSZ(j))

#define PJARR_ALIGN_UP(j, base) ((PJARR_ELEMSZ(j) < (sizeof(size_t) + sizeof(size_t))) ? JSTR_ALIGN_UP_STR(base) : base)

#if JSTR_PANIC
#	define PJARR_MALLOC_ERR(j, do_on_malloc_err)         \
		if (jstr_unlikely((PJARR_DATA(j)) == NULL)) { \
			PJARR_NULLIFY_MEMBERS(j);             \
			do_on_malloc_err;                     \
			jstr_errdie("");                      \
		}
#else
#	define PJARR_MALLOC_ERR(j, do_on_malloc_err)         \
		if (jstr_unlikely((PJARR_DATA(j)) == NULL)) { \
			PJARR_NULLIFY_MEMBERS(j);             \
			do_on_malloc_err;                     \
		}
#endif

#define PJARR_CHECK_ARG(j)                                \
	do {                                              \
		JSTR_STATIC_ASSERT_IS_SIZE(PJARR_SZ(j));  \
		JSTR_STATIC_ASSERT_IS_SIZE(PJARR_CAP(j)); \
	} while (0)
#define PJARR_GROW(old_cap, new_cap)                                       \
	do {                                                               \
		JSTR_STATIC_ASSERT_IS_SIZE(old_cap);                       \
		JSTR_STATIC_ASSERT_IS_SIZE(new_cap);                       \
		if (jstr_unlikely(old_cap == 0))                           \
			old_cap = JSTR_MIN_CAP / JARR_ALLOC_MULTIPLIER;    \
		do                                                         \
			(old_cap) *= JARR_GROWTH;                          \
		while ((old_cap) < (new_cap));                             \
		(old_cap) = JSTR_ALIGN_UP(old_cap, JSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define jarr_reserve(j, new_cap)                                                                                   \
	do {                                                                                                       \
		PJARR_CHECK_ARG(j);                                                                                \
		if (jstr_unlikely(PJARR_CAP(j) == 0))                                                              \
			PJARR_CAP(j) = JSTR_MIN_CAP / PJARR_ELEMSZ(j);                                             \
		PJARR_GROW(PJARR_CAP(j), new_cap);                                                                 \
		PJARR_CAP(j) = PJARR_ALIGN_UP(j, PJARR_CAP(j) * PJARR_ELEMSZ(j));                                  \
		PJARR_DATA(j) = PJSTR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j))); \
		if (jarr_chk(j))                                                                                   \
			break;                                                                                     \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                   \
	} while (0)
#define jarr_reserveexact(j, new_cap)                                                                              \
	do {                                                                                                       \
		PJARR_CHECK_ARG(j);                                                                                \
		PJARR_CAP(j) = PJARR_ALIGN_UP(j, PJARR_CAP(j) * PJARR_ELEMSZ(j));                                  \
		PJARR_DATA(j) = PJSTR_CAST(PJARR_DATA(j), realloc(PJARR_DATA(j), PJARR_CAP(j) * PJARR_ELEMSZ(j))); \
		if (jarr_chk(j))                                                                                   \
			break;                                                                                     \
		PJARR_CAP(j) /= PJARR_ELEMSZ(j);                                                                   \
	} while (0)
#if JSTR_HAVE_GENERIC && JSTR_HAVE_TYPEOF
#	define PJARR_CHECK_VAL(j, value) JSTR_STATIC_ASSERT(JSTR_SAME_TYPE(value, *PJARR_DATA(j)), "Passing illegal value incompatible with the array type.")
#else
/* clang-format off */
#	define PJARR_CHECK_VAL(j, value) do {} while (0)
/* clang-format on */
#endif

#define PJARR_NULLIFY_MEMBERS(j)  \
	do {                      \
		PJARR_SZ(j) = 0;  \
		PJARR_CAP(j) = 0; \
	} while (0)
#define PJARR_NULLIFY(j)                  \
	do {                              \
		PJARR_DATA(j) = NULL;     \
		PJARR_NULLIFY_MEMBERS(j); \
	} while (0)

#define jarr_free(j)                 \
	do {                         \
		free(PJARR_DATA(j)); \
		PJARR_NULLIFY(j);    \
	} while (0)

/* Add elements to end of PTR. */
#define jarr_assign(j, ...)                                                     \
	do {                                                                    \
		PJARR_CHECK_ARG(j);                                             \
		PJARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));            \
		if (jstr_unlikely(PJARR_CAP(j) < PJSTR_PP_NARG(__VA_ARGS__))) { \
			jarr_reserve(j, PJSTR_PP_NARG(__VA_ARGS__));            \
			PJARR_MALLOC_ERR(j, break)                              \
		}                                                               \
		PJSTR_PP_ARRCPY_VA_ARGS(PJARR_SZ(j), __VA_ARGS__);              \
		PJARR_SZ(j) = PJSTR_PP_NARG(__VA_ARGS__);                       \
	} while (0)

/* Add elements to end of PTR. */
#define jarr_cat(j, ...)                                                                        \
	do {                                                                                    \
		PJARR_CHECK_ARG(j);                                                             \
		PJARR_CHECK_VAL(j, PJSTR_PP_FIRST_ARG(__VA_ARGS__));                            \
		if (jstr_unlikely(PJARR_CAP(j) < (PJARR_SZ(j) + PJSTR_PP_NARG(__VA_ARGS__)))) { \
			jarr_reserve(j, (PJARR_SZ(j) + PJSTR_PP_NARG(__VA_ARGS__)));            \
			PJARR_MALLOC_ERR(j, break)                                              \
		}                                                                               \
		PJSTR_PP_ARRCPY_VA_ARGS(PJARR_DATA(j) + PJARR_SZ(j), __VA_ARGS__);              \
		PJARR_SZ(j) += PJSTR_PP_NARG(__VA_ARGS__);                                      \
	} while (0)
/* Pop PTR[0]. */
#define jarr_popfront(j)                                                  \
	do {                                                              \
		PJARR_CHECK_ARG(j);                                       \
		if (jstr_unlikely(PJARR_CAP(j) == 0)) {                   \
			PJARR_NULLIFY_MEMBERS(j);                         \
			break;                                            \
		}                                                         \
		memmove(PJARR_DATA(j), PJARR_DATA(j) + 1, --PJARR_SZ(j)); \
	} while (0)
/* Pop end of PTR. */
#define jarr_popback(j)                                  \
	do {                                             \
		PJARR_CHECK_ARG(j);                      \
		if (jstr_unlikely(PJARR_CAP(j) == 0)) {  \
			PJARR_NULLIFY_MEMBERS(j);        \
			break;                           \
		}                                        \
		*(PJARR_DATA(j) + --PJARR_SZ(j)) = '\0'; \
	} while (0)
/* Push VAL to back of PTR. */
#define jarr_pushback(j, value)                                           \
	do {                                                              \
		PJARR_CHECK_ARG(j);                                       \
		PJARR_CHECK_VAL(j, value);                                \
		if (jstr_unlikely(PJARR_CAP(j) < PJARR_SZ(j) + 1)) {      \
			if (jstr_unlikely(PJARR_CAP(j) == 0))             \
				PJARR_CAP(j) = PJARR_MIN_CAP(j);          \
			jarr_reserveexact(j, PJARR_CAP(j) * JARR_GROWTH); \
			PJARR_MALLOC_ERR(j, break)                        \
		}                                                         \
		*(PJARR_DATA(j) + PJARR_SZ(j)++) = (value);               \
	} while (0)
/* Push VAL to front of P. */
#define jarr_pushfront(j, value)                                                   \
	do {                                                                       \
		PJARR_CHECK_ARG(j);                                                \
		PJARR_CHECK_VAL(j, value);                                         \
		if (jstr_unlikely(PJARR_CAP(j) < PJARR_SZ(j) + 1)) {               \
			if (jstr_unlikely(PJARR_CAP(j) == 0))                      \
				PJARR_CAP(j) = PJARR_MIN_CAP(j);                   \
			jarr_reserveexact(j, PJARR_CAP(j) * JARR_GROWTH);          \
			PJARR_MALLOC_ERR(j, break)                                 \
		}                                                                  \
		PJARR_MEMMOVE(j, PJARR_DATA(j) + 1, PJARR_DATA(j), PJARR_SZ(j)++); \
		*PJARR_DATA(j) = (value);                                          \
	} while (0)

#define jarr_foreachi(j, i) \
	for (size_t i = 0, pjarr_max_elem_##i = (j)->size; i < pjarr_max_elem_##i; ++i)

#define jarr_start(j) PJARR_DATA(j)
#define jarr_end(j)   (PJARR_DATA(j) + PJARR_SZ(j))

#if JSTR_DEBUG
#	define jarr_at(j, idx) \
		(jstr_likely(idx < PJARR_SZ(j)) ? (PJARR_DATA(j) + (idx)) : (jstr_errdie("Index out of bounds."), PJARR_DATA(j)))
#else
#	define jarr_at(j, idx) \
		(PJARR_DATA(j) + (idx))
#endif

PJSTR_BEGIN_DECLS

PJSTR_END_DECLS

#undef R

#endif /* JARR_H */
