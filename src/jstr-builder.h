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

#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
#	define PJSTR_MALLOC_ERR(p, malloc_fail)          \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				jstr_err_exit("");        \
				malloc_fail;              \
			}                                 \
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

#define JSTR_MIN_ALLOC(cap)	 (((cap) > JSTR_MIN_CAP) ? ((cap)*JSTR_ALLOC_MULTIPLIER) : (JSTR_MIN_CAP))
#define JSTR_MIN_ALLOCEXACT(cap) (((cap) > JSTR_MIN_CAP) ? (cap) : (JSTR_MIN_CAP))

PJSTR_BEGIN_DECLS

JSTR_NOINLINE
JSTR_COLD
JSTR_FUNC_VOID
static void
pjstr_nullify_members(size_t *R sz,
		      size_t *R cap)
{
	*sz = 0;
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

JSTR_FUNC_VOID_MAY_NULL
JSTR_NOINLINE
JSTR_COLD
static void
pjstr_err_exit(const char *R filename,
	       const unsigned int line,
	       const char *R func,
	       const char *R msg)
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_NOINLINE
JSTR_COLD
static void
pjstr_err(const char *R filename,
	  const unsigned int line,
	  const char *R func,
	  const char *R msg)
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_debug(const jstr_ty *R j) JSTR_NOEXCEPT
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", j->size, j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(j->data));
	fprintf(stderr, "data puts():%s\n", j->data);
	fputs("data:", stderr);
	fwrite(j->data, 1, j->size, stderr);
	fputc('\n', stderr);
}

#define jstr_err(msg)	   pjstr_err(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg);
#define jstr_err_exit(msg) pjstr_err_exit(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg);

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserve_always(char *R *R s,
		    size_t *R sz,
		    size_t *R cap,
		    const size_t new_cap) JSTR_NOEXCEPT
{
	PJSTR_REALLOC_MAY_MALLOC(*s, *cap, new_cap + 1, goto err);
	return 1;
err:
	pjstr_nullify_members(sz, cap);
	return 0;
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserveexact_always(char *R *R s,
			 size_t *R sz,
			 size_t *R cap,
			 const size_t new_cap) JSTR_NOEXCEPT
{
	PJSTR_REALLOCEXACT_MAY_MALLOC(*s, *cap, new_cap + 1, goto err);
	return 1;
err:
	pjstr_nullify_members(sz, cap);
	return 0;
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserve(char *R *R s,
	     size_t *R sz,
	     size_t *R cap,
	     const size_t new_cap) JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return 1;
	return jstr_reserve_always(s, sz, cap, new_cap);
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
static int
jstr_reserveexact(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const size_t new_cap) JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return 1;
	return jstr_reserveexact_always(s, sz, cap, new_cap);
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
jstr_free(char *R *R s,
	  size_t *R sz,
	  size_t *R cap) JSTR_NOEXCEPT
{
	free(*s);
	*s = NULL;
	*cap = 0;
	*sz = 0;
}

/*
  free(p) and set p to NULL.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_free_j(jstr_ty *R j) JSTR_NOEXCEPT
{
	jstr_free(&j->data, &j->size, &j->capacity);
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
		PJSTR_REALLOC_MAY_MALLOC(*s, *cap, *sz + arglen, return 0);
	p = *s + *sz;
	*sz += arglen;
	for (const char *R arg; (arg = va_arg(ap, char *)); p = jstr_stpcpy(p, arg))
		;
	return 1;
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
		PJSTR_REALLOC_MAY_MALLOC(*s, *cap, *sz + srclen, return 0);
	jstr_strcpy_len(*s + *sz, src, srclen);
	*sz += srclen;
	return 1;
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
		if (jstr_unlikely(!jstr_reserveexact_always(s, sz, cap, srclen * JSTR_ALLOC_MULTIPLIER)))
			return 0;
	jstr_strcpy_len(*s, src, srclen);
	*sz = srclen;
	return 1;
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
		if (jstr_unlikely(!jstr_reserveexact_always(s, sz, cap, *sz * JSTR_ALLOC_MULTIPLIER)))
			return 0;
	*(*s + *sz) = c;
	*(*s + ++*sz) = '\0';
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
jstr_push_front(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char c) JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap == *sz + 1))
		PJSTR_REALLOCEXACT_MAY_MALLOC(*s, *cap, *sz * JSTR_ALLOC_MULTIPLIER, return 0);
	jstr_strmove_len(*s + 1, *s, (*sz)++);
	**s = c;
	return 1;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_back(char *R s,
	      size_t *R sz) JSTR_NOEXCEPT
{
	if (jstr_likely(*sz != 0))
		*(s + --*sz) = '\0';
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_front(char *R s,
	       size_t *R sz) JSTR_NOEXCEPT
{
	if (jstr_likely(*sz != 0))
		memmove(s, s + 1, (*sz)--);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwrite(const char *R s,
	       const size_t sz,
	       FILE *R fp) JSTR_NOEXCEPT
{
	return fwrite(s, 1, sz, fp) == sz;
}

JSTR_FUNC
JSTR_INLINE
static unsigned int
pjstr_asprintf_strlen(va_list ap, const char *R fmt)
{
	enum { MAX_DIGITS = 19 + 19 };
	unsigned int arglen = 0;
	for (const char *f = fmt, *R arg;;) {
		if (*f == '%') {
			arg = va_arg(ap, const char *);
			switch (*++f) {
			case 's':
				arglen = strlen(arg);
				break;
			case '%':
				arglen += 2;
				break;
			case '\\':
				if (jstr_unlikely(*++f == '\0')) {
					errno = EINVAL;
					return -1;
				}
				/* fallthrough */
			case 'c':
				++arglen;
				break;
			default:
				arglen += MAX_DIGITS;
				break;
			case '\0':
				errno = EINVAL;
				return -1;
			}
		} else {
			++arglen;
			if (jstr_unlikely(*f++ == '\0'))
				break;
			va_arg(ap, const void *);
		}
	}
	return arglen;
}

JSTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static int
jstr_asprintf(char *R *R s,
	      size_t *R sz,
	      size_t *R cap,
	      const char *R fmt,
	      ...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	if (jstr_unlikely(!jstr_reserveexact(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz = arglen;
	return 1;
err_free:
	jstr_free(s, sz, cap);
err:
	return 0;
}

JSTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static int
jstr_asprintf_j(jstr_ty *R j,
		const char *R fmt,
		...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	if (jstr_unlikely(!jstr_reserveexact(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size = arglen;
	return 1;
err_free:
	jstr_free_j(j);
err:
	return 0;
}

/*
   Concatenate ... to end of S.
*/
JSTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static int
jstr_asprintf_cat(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const char *R fmt,
		  ...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	arglen += *sz;
	if (jstr_unlikely(!jstr_reserveexact(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz += arglen;
	return 1;
err_free:
	jstr_free(s, sz, cap);
err:
	return 0;
}

/*
   Concatenate ... to end of S.
*/
JSTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static int
jstr_asprintf_cat_j(jstr_ty *R j,
		    const char *R fmt,
		    ...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	arglen += j->size;
	if (jstr_unlikely(!jstr_reserveexact(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size += arglen;
	return 1;
err_free:
	jstr_free_j(j);
err:
	return 0;
}

JSTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static int
jstr_asprintf_from(char *R *R s,
		   size_t *R sz,
		   size_t *R cap,
		   const size_t start_idx,
		   const char *R fmt,
		   ...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	arglen += start_idx;
	if (jstr_unlikely(!jstr_reserveexact(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz = arglen + start_idx;
	return 1;
err_free:
	jstr_free(s, sz, cap);
err:
	return 0;
}

JSTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static int
jstr_asprintf_from_j(jstr_ty *R j,
		     const size_t start_idx,
		     const char *R fmt,
		     ...)
{
	va_list ap;
	va_start(ap, fmt);
	unsigned int arglen = pjstr_asprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err;
	arglen += start_idx;
	if (jstr_unlikely(!jstr_reserveexact(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER)))
		goto err;
	va_start(ap, fmt);
	arglen = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size = arglen + start_idx;
	return 1;
err_free:
	jstr_free_j(j);
err:
	return 0;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf to grow S.
*/
JSTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static int
jstr_sprintf(char *R *R s,
	     size_t *R sz,
	     size_t *R cap,
	     const char *R fmt,
	     ...)
{
	va_list ap;
	va_start(ap, fmt);
	const unsigned int ret = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)ret < 0))
		goto err_free;
	*sz = ret;
	return 1;
err_free:
	if (errno != EINVAL)
		jstr_free(s, sz, cap);
	return 0;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf to grow S.
*/
JSTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static int
jstr_sprintf_j(jstr_ty *R j,
	       const char *R fmt,
	       ...)
{
	va_list ap;
	va_start(ap, fmt);
	const unsigned int ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)ret < 0))
		goto err_free;
	j->size = ret;
	return 1;
err_free:
	if (errno != EINVAL)
		jstr_free_j(j);
	return 0;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf to grow S.
*/
JSTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static int
jstr_sprintf_from(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const size_t start_idx,
		  const char *R fmt,
		  ...)
{
	va_list ap;
	va_start(ap, fmt);
	const unsigned int ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)ret < 0))
		goto err_free;
	*sz = ret + start_idx;
	return 1;
err_free:
	if (errno != EINVAL)
		jstr_free(s, sz, cap);
	return 0;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf to grow S.
*/
JSTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static int
jstr_sprintf_from_j(jstr_ty *R j,
		    const size_t start_idx,
		    const char *R fmt,
		    ...)
{
	va_list ap;
	va_start(ap, fmt);
	const unsigned int ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)ret < 0))
		goto err_free;
	j->size = ret + start_idx;
	return 1;
err_free:
	if (errno != EINVAL)
		jstr_free_j(j);
	return 0;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
