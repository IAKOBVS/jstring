#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#include "jstr-config.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

P_JSTR_BEGIN_DECLS

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

P_JSTR_END_DECLS

#define R JSTR_RESTRICT

#define P_JSTR_MALLOC_ERR(p, malloc_fail)                         \
	do {                                                      \
		if (jstr_unlikely((p) == NULL)) {                 \
			p_jstr_err(__FILE__, __LINE__, __func__); \
			malloc_fail;                              \
		}                                                 \
	} while (0)
#define P_JSTR_GROW(old_cap, new_cap)                                        \
	do {                                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                                \
		JSTR_ASSERT_IS_SIZE(new_cap);                                \
		while ((old_cap *= JSTR_GROWTH) < (new_cap))                 \
			;                                                    \
		(old_cap) = JSTR_ALIGN_UP(old_cap, P_JSTR_MALLOC_ALIGNMENT); \
	} while (0)
#define P_JSTR_REALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                             \
		JSTR_ASSERT_IS_STR(p);                   \
		JSTR_ASSERT_IS_SIZE(old_cap);            \
		JSTR_ASSERT_IS_SIZE(new_cap);            \
		P_JSTR_GROW(old_cap, new_cap);           \
		(p) = (char *)realloc(p, old_cap);       \
		P_JSTR_MALLOC_ERR(p, malloc_fail);       \
	} while (0)
#define P_JSTR_REALLOCEXACT(p, old_cap, new_cap, malloc_fail) \
	do {                                                  \
		JSTR_ASSERT_IS_STR(p);                        \
		JSTR_ASSERT_IS_SIZE(old_cap);                 \
		JSTR_ASSERT_IS_SIZE(new_cap);                 \
		(old_cap) = JSTR_ALIGN_UP_STR(new_cap);       \
		(p) = (char *)realloc(p, old_cap);            \
		P_JSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)
#define P_JSTR_MIN_ALLOC(new_cap)                         \
	((new_cap < JSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER) \
	 ? (JSTR_MIN_CAP)                                 \
	 : (new_cap * JSTR_ALLOC_MULTIPLIER))
#define P_JSTR_MIN_ALLOCEXACT(new_cap) \
	((new_cap < JSTR_MIN_CAP)      \
	 ? (JSTR_MIN_CAP)              \
	 : (new_cap))
#define P_JSTR_ALLOC_ONLY(p, cap, new_cap, do_fail)                  \
	do {                                                         \
		(cap) = P_JSTR_MIN_ALLOC(new_cap);                   \
		(cap) = JSTR_ALIGN_UP(cap, P_JSTR_MALLOC_ALIGNMENT); \
		(p) = (char *)malloc((cap));                         \
		P_JSTR_MALLOC_ERR((p), do_fail);                     \
	} while (0)

P_JSTR_BEGIN_DECLS

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
JSTR_NOTHROW
static void
p_jstr_err_exit(const char *R FILE_,
		const int LINE_,
		const char *R func_) JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", FILE_, LINE_, func_);
	perror("");
	exit(1);
}

JSTR_MAYBE_UNUSED
JSTR_NOINLINE
JSTR_COLD
JSTR_NOTHROW
static void
p_jstr_err(const char *R FILE_,
	   const int LINE_,
	   const char *R func_) JSTR_NOEXCEPT
{
#if JSTR_ERR_MSG_ON_MALLOC_ERROR
	fprintf(stderr, "%s:%d:%s\n:Can't malloc:", FILE_, LINE_, func_);
	perror("");
#endif
#if JSTR_EXIT_ON_MALLOC_ERROR
	exit(1);
#endif
}

/*
  exit(1) if ptr is NULL.
*/
JSTR_INLINE
JSTR_NOTHROW
static void
jstr_err(char *R const p) JSTR_NOEXCEPT
{
	if (jstr_unlikely(p == NULL))
		p_jstr_err_exit(__FILE__, __LINE__, __func__);
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_alloc(char *R *R const s,
	   size_t *R const sz,
	   size_t *R const cap,
	   const size_t top)
{
	*sz = 0;
	*cap = P_JSTR_MIN_ALLOC(JSTR_ALIGN_UP_STR(top));
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, return 0);
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_allocexact(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const size_t top) JSTR_NOEXCEPT
{
	*sz = 0;
	*cap = P_JSTR_MIN_ALLOC(JSTR_ALIGN_UP_STR(top));
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, return 0);
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_allocexact_assign_len(char *R *R const s,
			   size_t *R const sz,
			   size_t *R const cap,
			   const char *R const src,
			   const size_t srclen) JSTR_NOEXCEPT
{
	if (jstr_unlikely(!jstr_allocexact(s, sz, cap, srclen + 1)))
		return 0;
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_allocexact_assign(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const src) JSTR_NOEXCEPT
{
	return jstr_allocexact_assign_len(s, sz, cap, src, strlen(src));
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_alloc_assign_len(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      const char *R const src,
		      const size_t srclen) JSTR_NOEXCEPT
{
	P_JSTR_ALLOC_ONLY(*s, *cap, srclen, return 0);
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_alloc_assign(char *R *R const s,
		  size_t *R const sz,
		  size_t *R const cap,
		  const char *R const src) JSTR_NOEXCEPT
{
	return jstr_alloc_assign_len(s, sz, cap, src, strlen(src));
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC
static int
jstr_alloc_assignmore(char *R *R const s,
		      size_t *R const sz,
		      size_t *R const cap,
		      ...) JSTR_NOEXCEPT
{
	const char *arg;
	*sz = 0;
	va_list ap;
	va_start(ap, cap);
	while ((arg = va_arg(ap, char *)))
		*sz += strlen(arg);
	va_end(ap);
	*cap = *sz;
	P_JSTR_ALLOC_ONLY(*s, *cap, *sz, return 0);
	char *p = *s;
	va_start(ap, cap);
	while ((arg = va_arg(ap, char *)))
		p = jstr_stpcpy(p, arg);
	va_end(ap);
	*p = '\0';
	return 1;
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC
static int
jstr_alloc_assignmore_j(jstr_ty *R const j,
			...) JSTR_NOEXCEPT
{
	const char *arg;
	j->size = 0;
	va_list ap;
	va_start(ap, j);
	while ((arg = va_arg(ap, char *)))
		j->size += strlen(arg);
	va_end(ap);
	j->capacity = j->size;
	P_JSTR_ALLOC_ONLY(j->data, j->capacity, j->size, return 0);
	char *p = j->data;
	va_start(ap, j);
	while ((arg = va_arg(ap, char *)))
		p = jstr_stpcpy(p, arg);
	va_end(ap);
	*p = '\0';
	return 1;
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC
static int
jstr_appendmore(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		...) JSTR_NOEXCEPT
{
	const char *arg;
	size_t arglen = 0;
	va_list ap;
	va_start(ap, cap);
	while ((arg = va_arg(ap, char *)))
		arglen += strlen(arg);
	va_end(ap);
	if (*cap < *sz + arglen)
		P_JSTR_REALLOC(*s, *cap, *sz + arglen, return 0);
	char *p = *s + *sz;
	*sz += arglen;
	va_start(ap, cap);
	while ((arg = va_arg(ap, char *)))
		p = jstr_stpcpy(p, arg);
	va_end(ap);
	return 1;
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC
static int
jstr_appendmore_j(jstr_ty *R const j,
		  ...) JSTR_NOEXCEPT
{
	const char *arg;
	size_t arglen = 0;
	va_list ap;
	va_start(ap, j);
	while ((arg = va_arg(ap, char *)))
		arglen += strlen(arg);
	va_end(ap);
	if (j->capacity < j->size + arglen)
		P_JSTR_REALLOC(j->data, j->capacity, j->size + arglen, return 0);
	char *p = j->data + j->size;
	j->size += arglen;
	va_start(ap, j);
	while ((arg = va_arg(ap, char *)))
		p = jstr_stpcpy(p, arg);
	va_end(ap);
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_allocmore_assign_len(char *R *R const s,
			  size_t *R const sz,
			  size_t *R const cap,
			  const char *R const src,
			  const size_t srclen) JSTR_NOEXCEPT
{
	P_JSTR_ALLOC_ONLY(*s, *cap, srclen * 2, return 0);
	*sz = srclen;
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
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
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_free(char *R p) JSTR_NOEXCEPT
{
	free(p);
#if JSTR_NULLIFY_PTR_ON_FREE
	p = NULL;
#endif /* JSTR_NULLIFY_PTR_ON_FREE */
}

JSTR_FUNC_VOID
JSTR_INLINE
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
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_append_len(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char *R const src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + srclen)
		P_JSTR_REALLOC(*s, *cap, *sz + srclen, return 0);
	memcpy(*s + *sz, src, srclen);
	*(*s + (*sz += srclen)) = '\0';
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
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
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_assign_len(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char *R const src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < srclen)
		P_JSTR_REALLOC(*s, *cap, srclen * JSTR_ALLOC_MULTIPLIER, return 0);
	memcpy(*s, src, srclen);
	(*s)[srclen] = '\0';
	*sz = srclen;
	return 1;
}

/*
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
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
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
jstr_push_back(char *R *R const s,
	       size_t *R const sz,
	       size_t *R const cap,
	       const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		P_JSTR_REALLOCEXACT(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, return 0);
	(*s)[*sz] = c;
	(*s)[++*sz] = '\0';
	return 1;
}

/*
   Push C to end of S.
   S is NUL terminated.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static int
jstr_push_front(char *R *R const s,
		size_t *R const sz,
		size_t *R const cap,
		const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		P_JSTR_REALLOCEXACT(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, return 0);
	memmove(*s + 1, *s, *sz);
	(*s)[++*sz] = '\0';
	**s = c;
	return 1;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_INLINE
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
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_back_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	jstr_pop_back(j->data, &j->size);
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_front(char *R const s,
	       size_t *R const sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz == 0))
		return;
	memmove(s, s + 1, (*sz)--);
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_front_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	jstr_pop_front(j->data, &j->size);
}

JSTR_FUNC
static int
jstr_reserve(char *R *R const s,
	     size_t *R const cap,
	     const size_t new_cap)
{
	if (new_cap <= *cap)
		return 1;
	P_JSTR_REALLOC(*s, *cap, new_cap, return 0);
	return 1;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_reserve_j(jstr_ty *R const j,
	       const size_t new_cap)
{
	return jstr_reserve(&j->data, &j->capacity, new_cap);
}

JSTR_FUNC
static int
jstr_reserveexact(char *R *R const s,
		  size_t *R const cap,
		  const size_t new_cap)
{
	if (new_cap <= *cap)
		return 1;
	P_JSTR_REALLOCEXACT(*s, *cap, new_cap, return 0);
	return 1;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_reserveexact_j(jstr_ty *R const j,
		    const size_t new_cap)
{
	return jstr_reserveexact(&j->data, &j->capacity, new_cap);
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
