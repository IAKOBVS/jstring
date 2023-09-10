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
jstr_alloc(char *R *R const _s,
	   size_t *R const _sz,
	   size_t *R const _cap,
	   const size_t _top)
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(_top));
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact(char *R *R const _s,
		size_t *R const _sz,
		size_t *R const _cap,
		const size_t _top) JSTR_NOEXCEPT
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(_top));
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, return);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocexact_assign_len(char *R *R const _s,
			   size_t *R const _sz,
			   size_t *R const _cap,
			   const char *R const _src,
			   const size_t _srclen) JSTR_NOEXCEPT
{
	jstr_allocexact(_s, _sz, _cap, _srclen + 1);
	if (jstr_unlikely(*_s == NULL))
		return;
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocexact_assign(char *R *R const _s,
		       size_t *R const _sz,
		       size_t *R const _cap,
		       const char *R const _src) JSTR_NOEXCEPT
{
	return jstr_allocexact_assign_len(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_alloc_assign_len(char *R *R const _s,
		      size_t *R const _sz,
		      size_t *R const _cap,
		      const char *R const _src,
		      const size_t _srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*_s, *_cap, _srclen, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_alloc_assign(char *R *R const _s,
		  size_t *R const _sz,
		  size_t *R const _cap,
		  const char *R const _src) JSTR_NOEXCEPT
{
	return jstr_alloc_assign_len(_s, _sz, _cap, _src, strlen(_src));
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_allocmore_assign_len(char *R *R const _s,
			  size_t *R const _sz,
			  size_t *R const _cap,
			  const char *R const _src,
			  const size_t _srclen) JSTR_NOEXCEPT
{
	PJSTR_ALLOC_ONLY(*_s, *_cap, _srclen * 2, return);
	*_sz = _srclen;
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_allocmore_assign(char *R *R const _s,
		      size_t *R const _sz,
		      size_t *R const _cap,
		      const char *R const _src) JSTR_NOEXCEPT
{
	return jstr_allocmore_assign_len(_s, _sz, _cap, _src, strlen(_src));
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
#if JSTR_NULLIFY_PTR_ON_DELETE
	p = NULL;
#endif /* JSTR_NULLIFY_PTR_ON_DELETE */
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_debug(const jstr_ty *R const _j)
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", _j->size, _j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(_j->data));
	fprintf(stderr, "data puts:%s\n", _j->data);
	fputs("data:", stderr);
	fwrite(_j->data, 1, _j->size, stderr);
	fputc('\n', stderr);
}

/*
   Append SRC to DST.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_append_len(char *R *R const _s,
		size_t *R const _sz,
		size_t *R const _cap,
		const char *R const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*_cap < *_sz + _srclen)
		PJSTR_REALLOC(*_s, *_cap, *_sz + _srclen, return);
	memcpy(*_s + *_sz, _src, _srclen);
	*(*_s + (*_sz += _srclen)) = '\0';
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_append(char *R *R const _s,
	    size_t *R const _sz,
	    size_t *R const _cap,
	    const char *R const _src) JSTR_NOEXCEPT
{
	return jstr_append_len(_s, _sz, _cap, _src, strlen(_src));
}

/*
   Assign SRC to DST.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_assign_len(char *R *R const _s,
		size_t *R const _sz,
		size_t *R const _cap,
		const char *R const _src,
		const size_t _srclen) JSTR_NOEXCEPT
{
	if (*_cap < _srclen)
		PJSTR_REALLOC(*_s, *_cap, _srclen * JSTR_ALLOC_MULTIPLIER, return);
	memcpy(*_s, _src, _srclen);
	*(*_s + _srclen) = '\0';
	*_sz = _srclen;
}

JSTR_INLINE
JSTR_MAYBE_UNUSED
static void
jstr_assign(char *R *R const _s,
	    size_t *R const _sz,
	    size_t *R const _cap,
	    const char *R const _src) JSTR_NOEXCEPT
{
	return jstr_assign_len(_s, _sz, _cap, _src, strlen(_src));
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_back(char *R *R const _s,
	       size_t *R const _sz,
	       size_t *R const _cap,
	       const char _c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJSTR_REALLOCEXACT(*_s, *_cap, *_sz * JSTR_ALLOC_MULTIPLIER, return);
	*(*_s + *_sz) = _c;
	*(*_s + ++*_sz) = '\0';
}

/*
   Push C to end of S.
   S is NUL terminated.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_push_front(char *R *R const _s,
		size_t *R const _sz,
		size_t *R const _cap,
		const char _c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJSTR_REALLOCEXACT(*_s, *_cap, *_sz * JSTR_ALLOC_MULTIPLIER, return);
	memmove(*_s + 1, *_s, (*_sz)++ + 1);
	**_s = _c;
}

/* Pop s[size]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back(char *R const _s,
	      size_t *R const _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_sz == 0))
		return;
	*(_s + (*_sz)-- - 1) = '\0';
}

/*
   Pop s[size].
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_back_j(jstr_ty *R const _j) JSTR_NOEXCEPT
{
	jstr_pop_back(_j->data, &_j->size);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front(char *R const _s,
	       size_t *R const _sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*_sz == 0))
		return;
	memmove(_s, _s + 1, (*_sz)--);
}

/* Pop s[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static void
jstr_pop_front_j(jstr_ty *R const _j) JSTR_NOEXCEPT
{
	jstr_pop_front(_j->data, &_j->size);
}

PJSTR_END_DECLS

#ifndef __cplusplus

#	define jstr_appendmore_f(_s, _sz, ...)                                           \
		do {                                                                      \
			JSTR_ASSERT_IS_STR(*(_s));                                        \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                      \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                   \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                  \
			*(_sz) += PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			char *p = *(_s) + *(_sz);                                         \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);            \
			*p = '\0';                                                        \
		} while (0)
#	define jstr_appendmore(_s, _sz, _cap, ...)                                                             \
		do {                                                                                            \
			JSTR_ASSERT_IS_STR(*(_s));                                                              \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                                            \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                                           \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                                         \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                                        \
			const size_t _NEW_SZ = *(_sz) + PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			if (*(_cap) < _NEW_SZ)                                                                  \
				PJSTR_REALLOC(*(_s), *(_cap), _NEW_SZ + 1, break);                              \
			char *p = *(_s) + *(_sz);                                                               \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);                                  \
			*p = '\0';                                                                              \
			*(_sz) = _NEW_SZ;                                                                       \
		} while (0)
#	define jstr_alloc_appendmore(_s, _sz, _cap, ...)                                \
		do {                                                                     \
			JSTR_ASSERT_IS_STR(*(_s));                                       \
			JSTR_ASSERT_IS_SIZE(*(_sz));                                     \
			JSTR_ASSERT_IS_SIZE(*(_cap));                                    \
			PJSTR_PP_ST_ASSERT_IS_STR_VA_ARGS(__VA_ARGS__);                  \
			size_t _ARR_VA_ARGS[PJSTR_PP_NARG(__VA_ARGS__)];                 \
			*(_sz) = PJSTR_PP_STRLEN_ARR_VA_ARGS(_ARR_VA_ARGS, __VA_ARGS__); \
			*(_cap) = PJSTR_MIN_ALLOC(PJSTR_ALIGN_UP_STR(*(_sz)));           \
			*(_s) = malloc(*(_cap));                                         \
			PJSTR_MALLOC_ERR(*((_s)), break);                                \
			char *p = *(_s);                                                 \
			PJSTR_PP_STRCPY_VA_ARGS(p, _ARR_VA_ARGS, __VA_ARGS__);           \
			*p = '\0';                                                       \
		} while (0)
#	define jstr_appendmore_j(_j, ...)	 jstr_appendmore(&((_j)->data), &((_j)->size), &((_j)->capacity), __VA_ARGS__)
#	define jstr_alloc_appendmore_j(_j, ...) jstr_alloc_appendmore(&((_j)->data), &((_j)->size), &((_j)->capacity), __VA_ARGS__)

#endif /* __cplusplus */

#undef R

#endif /* JSTR_BUILDER _H */
