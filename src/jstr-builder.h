#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
PJSTR_END_DECLS

#include "jstr-config.h"
#include "jstr-macros.h"
#include "jstr-std-string.h"

PJSTR_BEGIN_DECLS

typedef struct jstr_ty {
	char *data;
	size_t size;
	size_t capacity;
} jstr_ty;

PJSTR_END_DECLS

#define R JSTR_RESTRICT

#if JSTR_DEBUG
#	define PJSTR_MALLOC_ERR(p, malloc_fail)                                    \
		do {                                                                \
			if (jstr_unlikely((p) == NULL)) {                           \
				pjstr_err_exit_debug(__FILE__, __LINE__, __func__); \
				malloc_fail;                                        \
			}                                                           \
		} while (0)
#else
#	define PJSTR_MALLOC_ERR(p, malloc_fail)          \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				malloc_fail;              \
			}                                 \
		} while (0)
#endif

#define PJSTR_REALLOC(p, old_cap, new_cap, malloc_fail)   \
	do {                                              \
		JSTR_ASSERT_IS_SIZE(old_cap);             \
		JSTR_ASSERT_IS_SIZE(new_cap);             \
		old_cap = pjstr_grow(old_cap, new_cap);   \
		(p) = PJSTR_CAST(p, realloc(p, old_cap)); \
		PJSTR_MALLOC_ERR(p, malloc_fail);         \
	} while (0)
#define PJSTR_REALLOCEXACT(p, old_cap, new_cap, malloc_fail) \
	do {                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                \
		JSTR_ASSERT_IS_SIZE(new_cap);                \
		(old_cap) = JSTR_ALIGN_UP_STR(new_cap);      \
		(p) = PJSTR_CAST(p, realloc(p, old_cap));    \
		PJSTR_MALLOC_ERR(p, malloc_fail);            \
	} while (0)

#define PJSTR_REALLOC_MAY_MALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                                       \
		old_cap = JSTR_MAX(old_cap, JSTR_MIN_CAP);         \
		PJSTR_REALLOC(p, old_cap, new_cap, malloc_fail);   \
	} while (0)
#define PJSTR_REALLOCEXACT_MAY_MALLOC(p, old_cap, new_cap, malloc_fail) \
	do {                                                            \
		old_cap = JSTR_MAX(old_cap, JSTR_MIN_CAP);              \
		PJSTR_REALLOCEXACT(p, old_cap, new_cap, malloc_fail);   \
	} while (0)

#if JSTR_NULLIFY_MEMBERS_ON_FREE
#	define PJSTR_NULLIFY_MEMBERS(sz, cap) (pjstr_nullify_members(sz, cap))
#else
#	define PJSTR_NULLIFY_MEMBERS(sz, cap)
#endif

PJSTR_BEGIN_DECLS

JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_nullify_members(size_t *R size,
		      size_t *R cap) JSTR_NOEXCEPT
{
	*size = 0;
	*cap = 0;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
pjstr_grow(size_t cap,
	   const size_t new_cap) JSTR_NOEXCEPT
{
	while ((cap *= JSTR_GROWTH) < new_cap)
		;
	return JSTR_ALIGN_UP(cap, PJSTR_MALLOC_ALIGNMENT);
}

JSTR_MAYBE_UNUSED
JSTR_COLD
JSTR_NOTHROW
JSTR_NOINLINE
static void
pjstr_err_exit_debug(const char *R FILE_,
		     const int LINE_,
		     const char *R func_) JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%d:%s:%s", FILE_, LINE_, func_, strerror(errno));
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_err_debug(const char *R FILE_,
		const int LINE_,
		const char *R func_) JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%d:%s:%s", FILE_, LINE_, func_, strerror(errno));
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstr_err(void) JSTR_NOEXCEPT
{
	fputs(strerror(errno), stderr);
}

JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_err_exit(void) JSTR_NOEXCEPT
{
	fputs(strerror(errno), stderr);
	exit(EXIT_FAILURE);
}

/* Print error message: __FILE__:__LINE__:__func__:strerror(errno). */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_err_debug(void) JSTR_NOEXCEPT
{
	pjstr_err_debug(__FILE__, __LINE__, __func__);
}

/* Print error message: __FILE__:__LINE__:__func__:strerror(errno) and exit. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_err_exit_debug(void) JSTR_NOEXCEPT
{
	pjstr_err_exit_debug(__FILE__, __LINE__, __func__);
}

/* Print error message. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_err(void) JSTR_NOEXCEPT
{
#if JSTR_DEBUG
	jstr_err_debug();
#else
	pjstr_err();
#endif
}

/* Print error message and exit. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_err_exit(void) JSTR_NOEXCEPT
{
#if JSTR_DEBUG
	jstr_err_exit_debug();
#else
	pjstr_err_exit();
#endif
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_debug(const jstr_ty *R j) JSTR_NOEXCEPT
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", j->size, j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(j->data));
	fprintf(stderr, "data puts:%s\n", j->data);
	fputs("data:", stderr);
	fwrite(j->data, 1, j->size, stderr);
	fputc('\n', stderr);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_print(const jstr_ty *R j) JSTR_NOEXCEPT
{
	fwrite(j->data, 1, j->size, stdout);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_init(char *R *R s,
	  size_t *R sz,
	  size_t *R cap) JSTR_NOEXCEPT
{
	*s = NULL;
	*sz = 0;
	*cap = 0;
}

/*
  free(p) and set p to NULL.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_free(jstr_ty *const j) JSTR_NOEXCEPT
{
	free(j->data);
#if JSTR_NULLIFY_PTR_ON_FREE
	j->data = NULL;
#endif
#if JSTR_NULLIFY_MEMBERS_ON_FREE
	j->capacity = 0;
	j->size = 0;
#endif
}

JSTR_FUNC_MAY_NULL
JSTR_INLINE
static int
pjstr_cat(char *R *R s,
	  size_t *R sz,
	  size_t *R cap,
	  va_list ap,
	  const size_t arglen) JSTR_NOEXCEPT
{
	char *p;
	if (*cap < *sz + arglen)
		PJSTR_REALLOC_MAY_MALLOC(*s, *cap, *sz + arglen, goto err);
	p = *s + *sz;
	*sz += arglen;
	for (const char *R arg; (arg = va_arg(ap, char *)); p = jstr_stpcpy(p, arg))
		;
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC_MAY_NULL
static int
jstr_cat(char *R *R s,
	 size_t *R sz,
	 size_t *R cap,
	 ...) JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, cap);
	size_t arglen = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arglen += strlen(arg))
		;
	va_end(ap);
	va_start(ap, cap);
	arglen = pjstr_cat(s, sz, cap, ap, arglen);
	va_end(ap);
	return arglen;
}

/*
   Last arg must be NULL.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_SENTINEL
JSTR_FUNC_MAY_NULL
static int
jstr_cat_j(jstr_ty *R j,
	   ...) JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, j);
	size_t arglen = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arglen += strlen(arg))
		;
	va_end(ap);
	va_start(ap, j);
	arglen = pjstr_cat(&j->data, &j->size, &j->capacity, ap, arglen);
	va_end(ap);
	return arglen;
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
jstr_append_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char *R src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < *sz + srclen)
		PJSTR_REALLOC_MAY_MALLOC(*s, *cap, *sz + srclen, goto err);
	jstr_strcpy_len(*s + *sz, src, srclen);
	*sz += srclen;
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
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
jstr_assign_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char *R src,
		const size_t srclen) JSTR_NOEXCEPT
{
	if (*cap < srclen)
		PJSTR_REALLOC_MAY_MALLOC(*s, *cap, srclen * JSTR_ALLOC_MULTIPLIER, goto err);
	jstr_strcpy_len(*s, src, srclen);
	*sz = srclen;
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
}

/*
   Push C to end of S.
   S is NUL terminated.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_INLINE
JSTR_FUNC
static int
jstr_push_back(char *R *R s,
	       size_t *R sz,
	       size_t *R cap,
	       const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		PJSTR_REALLOCEXACT_MAY_MALLOC(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, goto err);
	(*s)[*sz] = c;
	(*s)[++*sz] = '\0';
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
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
jstr_push_front(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		PJSTR_REALLOCEXACT_MAY_MALLOC(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, goto err);
	jstr_strmove_len(*s + 1, *s, (*sz)++);
	**s = c;
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_back(char *R s,
	      size_t *R sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz == 0))
		return;
	s[--*sz] = '\0';
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_front(char *R s,
	       size_t *R sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz == 0))
		return;
	memmove(s, s + 1, (*sz)--);
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserve(char *R *R s,
	     size_t *R cap,
	     const size_t new_cap) JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return 1;
	PJSTR_REALLOC_MAY_MALLOC(*s, *cap, new_cap + 1, goto err);
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserveexact(char *R *R s,
		  size_t *R cap,
		  const size_t new_cap) JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return 1;
	PJSTR_REALLOCEXACT_MAY_MALLOC(*s, *cap, new_cap + 1, goto err);
	return 1;
err:
	PJSTR_NULLIFY_MEMBERS(sz, cap);
	return 0;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
