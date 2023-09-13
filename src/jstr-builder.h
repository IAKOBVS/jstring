#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-templates.h"

#ifndef __cplusplus
#	include "jstr-pp-va-args-macros.h"
#endif /* !__cpluslus */

#include "jstr-config.h"
#include "jstr-macros.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_err(char *R const p) JSTR_NOEXCEPT
{
	if (jstr_unlikely(p == NULL))
		pjstr_err_exit();
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_alloc(char *R *R const s,
	   size_t *R const sz,
	   size_t *R const cap,
	   const size_t top)
{
	*sz = 0;
	*cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(top));
	*s = (char *)malloc(*cap);
	PJSTR_MALLOC_ERR(*s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const size_t top) JSTR_NOEXCEPT
{
	*sz = 0;
	*cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(top));
	*s = (char *)malloc(*cap);
	PJSTR_MALLOC_ERR(*s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact_assign_len(char *R *R const s,
			   size_t *R const sz,
			   size_t *R const cap,
			   const char *R const src,
			   const size_t srclen) JSTR_NOEXCEPT
{
	jstr_allocexact(s, sz, cap, srclen + 1);
	if (jstr_unlikely(*s == NULL))
		return;
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_assign(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const src) JSTR_NOEXCEPT
{
	return jstr_allocexact_assign_len(s, sz, cap, src, strlen(src));
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_alloc_assign_len(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const src,
		      const size_t srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*s, *cap, srclen, return);
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_assign(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const src) JSTR_NOEXCEPT
{
	return jstr_alloc_assign_len(s, sz, cap, src, strlen(src));
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocmore_assign_len(char *R *R const s,
			  size_t *R const sz,
			  size_t *R const cap,
			  const char *R const src,
			  const size_t srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*s, *cap, srclen * 2, return);
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocmore_assign(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const src) JSTR_NOEXCEPT
{
	return jstr_allocmore_assign_len(s, sz, cap, src, strlen(src));
}

/*
  free(p) and set p to NULL.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_free(char *R p) JSTR_NOEXCEPT
{
	free(p);
#if JSTR_NULLIFY_PTR_ON_FREE
	p = NULL;
#endif /* JSTR_NULLIFY_PTR_ON_FREE */
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_debug(const jstr_ty *R const j)
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", j->size, j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(j->data));
	fprintf(stderr, "data puts:%s\n", j->data);
	fputs("data:", stderr);
	fwrite(j->data, 1, j->size, stderr);
	fputc('\n', stderr);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_append_len(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char *R const src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + srclen)
		PJSTR_REALLOC(*s, *cap, *sz + srclen, return);
	memcpy(*s + *sz, src, srclen);
	*(*s + (*sz += srclen)) = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_append(char *R *R const s,
	    size_t *R const sz,
	    size_t *R const cap,
	    const char *R const src) JSTR_NOEXCEPT
{
	return jstr_append_len(s, sz, cap, src, strlen(src));
}

/*
   Assign SRC to DST.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_assign_len(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char *R const src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < srclen)
		PJSTR_REALLOC(*s, *cap, srclen * JSTR_ALLOC_MULTIPLIER, return);
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
	*sz = srclen;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_assign(char *R *R const s,
	    size_t *R const sz,
	    size_t *R const cap,
	    const char *R const src) JSTR_NOEXCEPT
{
	return jstr_assign_len(s, sz, cap, src, strlen(src));
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_back(char *R *R const s,
	       size_t *R const sz,
	       size_t *R const cap,
	       const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		PJSTR_REALLOCEXACT(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, return);
	(*s)[*sz] = c;
	(*s)[++*sz] = '\0';
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_front(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		PJSTR_REALLOCEXACT(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, return);
	memmove(*s + 1, *s, *sz);
	(*s)[++*sz] = '\0';
	**s = c;
}

/* Pop s[size]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back(char *R const s,
	      size_t *R const sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz == 0))
		return;
	s[--*sz] = '\0';
}

/*
   Pop s[size].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	jstr_pop_back(j->data, &j->size);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front(char *R const s,
	       size_t *R const sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz == 0))
		return;
	memmove(s, s + 1, (*sz)--);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	jstr_pop_front(j->data, &j->size);
}

PJSTR_END_DECLS

#ifndef __cplusplus

#	define jstr_appendmore_f(s, sz, ...)                                           \
		do {                                                                    \
			JSTR_ASSERT_IS_STR(*(s));                                       \
			JSTR_ASSERT_IS_SIZE(*(sz));                                     \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                 \
			size_t ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                 \
			*(sz) += PJSTR_PP_STRLEN_ARR_VA_ARGS(ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(s) + *(sz);                                         \
			PJSTR_PP_STRCPY_VA_ARGS(p, ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                      \
		} while (0)
#	define jstr_appendmore(s, sz, cap, ...)                                                             \
		do {                                                                                         \
			JSTR_ASSERT_IS_STR(*(s));                                                            \
			JSTR_ASSERT_IS_SIZE(*(sz));                                                          \
			JSTR_ASSERT_IS_SIZE(*(cap));                                                         \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                      \
			size_t ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                                      \
			const size_t NEW_SZ = *(sz) + PJSTR_PP_STRLEN_ARR_VA_ARGS(ARR_VA_ARGS, __VA_ARGS__); \
			if (*(cap) < NEW_SZ)                                                                 \
				PJSTR_REALLOC(*(s), *(cap), NEW_SZ + 1, break);                              \
			char *p = *(s) + *(sz);                                                              \
			PJSTR_PP_STRCPY_VA_ARGS(p, ARR_VA_ARGS, __VA_ARGS__);                                \
			*p = '\0';                                                                           \
			*(sz) = NEW_SZ;                                                                      \
		} while (0)
#	define jstr_alloc_appendmore(s, sz, cap, ...)                                 \
		do {                                                                   \
			JSTR_ASSERT_IS_STR(*(s));                                      \
			JSTR_ASSERT_IS_SIZE(*(sz));                                    \
			JSTR_ASSERT_IS_SIZE(*(cap));                                   \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                \
			size_t ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                \
			*(sz) = PJSTR_PP_STRLEN_ARR_VA_ARGS(ARR_VA_ARGS, __VA_ARGS__); \
			*(cap) = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(*(sz)));           \
			*(s) = malloc(*(cap));                                         \
			PJSTR_MALLOC_ERR(*((s)), break);                               \
			char *p = *(s);                                                \
			PJSTR_PP_STRCPY_VA_ARGS(p, ARR_VA_ARGS, __VA_ARGS__);          \
			*p = '\0';                                                     \
		} while (0)
#	define jstr_appendmore_j(j, ...)	jstr_appendmore(&((j)->data), &((j)->size), &((j)->capacity), __VA_ARGS__)
#	define jstr_alloc_appendmore_j(j, ...) jstr_alloc_appendmore(&((j)->data), &((j)->size), &((j)->capacity), __VA_ARGS__)

#endif /* __cplusplus */

#undef R

#endif /* JSTR_BUILDER H */
