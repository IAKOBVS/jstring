#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
PJSTR_END_DECLS

#include "jstr-config.h"
#include "jstr-ctype.h"
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

#define jstr_foreach(j, ptr) for (char *ptr = ((j)->data), *const jstr_ty_end_ = ((j)->data) + ((j)->size); ptr < jstr_ty_end_; ++ptr)

#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				jstr_err_exit("");        \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#else
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#endif

#define PJSTR_REALLOC(p, old_cap, new_cap, do_on_malloc_err) \
	do {                                                 \
		JSTR_ASSERT_IS_SIZE(old_cap);                \
		JSTR_ASSERT_IS_SIZE(new_cap);                \
		old_cap = pjstr_grow(old_cap, new_cap);      \
		(p) = PJSTR_CAST(p, realloc(p, old_cap));    \
		PJSTR_MALLOC_ERR(p, do_on_malloc_err);       \
	} while (0)
#define PJSTR_REALLOCEXACT(p, old_cap, new_cap, do_on_malloc_err) \
	do {                                                      \
		JSTR_ASSERT_IS_SIZE(old_cap);                     \
		JSTR_ASSERT_IS_SIZE(new_cap);                     \
		(old_cap) = JSTR_ALIGN_UP_STR(new_cap);           \
		(p) = PJSTR_CAST(p, realloc(p, old_cap));         \
		PJSTR_MALLOC_ERR(p, do_on_malloc_err);            \
	} while (0)

#define PJSTR_REALLOC_MAY_MALLOC(p, old_cap, new_cap, do_on_malloc_err) \
	do {                                                            \
		old_cap = JSTR_MAX(old_cap, JSTR_MIN_CAP);              \
		PJSTR_REALLOC(p, old_cap, new_cap, do_on_malloc_err);   \
	} while (0)
#define PJSTR_REALLOCEXACT_MAY_MALLOC(p, old_cap, new_cap, do_on_malloc_err) \
	do {                                                                 \
		old_cap = JSTR_MAX(old_cap, JSTR_MIN_CAP);                   \
		PJSTR_REALLOCEXACT(p, old_cap, new_cap, do_on_malloc_err);   \
	} while (0)

#define JSTR_MIN_ALLOC(cap)	 (((cap) > JSTR_MIN_CAP) ? ((cap)*JSTR_ALLOC_MULTIPLIER) : (JSTR_MIN_CAP))
#define JSTR_MIN_ALLOCEXACT(cap) (((cap) > JSTR_MIN_CAP) ? (cap) : (JSTR_MIN_CAP))
#define jstr_err(msg)		 pjstr_err(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg);
#define jstr_err_exit(msg)	 pjstr_err_exit(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg);

#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
#	define PJSTR_EXIT_MAYBE() jstr_err_exit("")
#else
#	define PJSTR_EXIT_MAYBE() \
		do {               \
		} while (0)
#endif

#define PJSTR_RESERVE_FAIL(func, s, sz, cap, new_cap, do_on_malloc_err) \
	if (jstr_unlikely(!func(s, sz, cap, new_cap))) {                \
		PJSTR_EXIT_MAYBE();                                     \
		do_on_malloc_err;                                       \
	}

#define JSTR_RESERVE_ALWAYS_NOMALLOC(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserve_always_nomalloc, s, sz, cap, new_cap, do_on_malloc_err)
#define JSTR_RESERVEEXACT_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact_always, s, sz, cap, new_cap, do_on_malloc_err)
#define JSTR_RESERVEEXACT(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact, s, sz, cap, new_cap, do_on_malloc_err)
#define JSTR_RESERVE_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserve_always, s, sz, cap, new_cap, do_on_malloc_err)
#define JSTR_RESERVE(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserve, s, sz, cap, new_cap, do_on_malloc_err)

PJSTR_BEGIN_DECLS

JSTR_NOINLINE
JSTR_COLD
JSTR_FUNC_VOID
static void
pjstr_nullify_members(size_t *R sz,
		      size_t *R cap)
JSTR_NOEXCEPT
{
	*sz = 0;
	*cap = 0;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
pjstr_grow(size_t cap,
	   const size_t new_cap)
JSTR_NOEXCEPT
{
	while ((cap *= JSTR_GROWTH) < new_cap)
		;
	return JSTR_ALIGN_UP(cap, PJSTR_MALLOC_ALIGNMENT);
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
JSTR_NOINLINE
JSTR_COLD
static void
pjstr_err_exit(const char *R filename,
	       const unsigned int line,
	       const char *R func,
	       const char *R msg)
JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
	exit(EXIT_FAILURE);
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
JSTR_NOINLINE
JSTR_COLD
static void
pjstr_err(const char *R filename,
	  const unsigned int line,
	  const char *R func,
	  const char *R msg)
JSTR_NOEXCEPT
{
	fprintf(stderr, "%s:%u:%s:%s:%s\n", filename, line, func, strerror(errno), msg);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_debug(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	fprintf(stderr, "size:%zu\ncapacity:%zu\n", j->size, j->capacity);
	fprintf(stderr, "strlen():%zu\n", strlen(j->data));
	fprintf(stderr, "data puts():%s\n", j->data);
	fputs("data:", stderr);
	fwrite(j->data, 1, j->size, stderr);
	fputc('\n', stderr);
}

/*
  free(p) and set p to NULL.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_free(char *R *R s,
	  size_t *R sz,
	  size_t *R cap)
JSTR_NOEXCEPT
{
	free(*s);
	*s = NULL;
	*cap = 0;
	*sz = 0;
}

JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_free_err(char *R *R s,
	       size_t *R sz,
	       size_t *R cap)
{
	jstr_free(s, sz, cap);
}

/*
  free(p) and set p to NULL.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_free_j(jstr_ty *R j)
JSTR_NOEXCEPT
{
	jstr_free(&j->data, &j->size, &j->capacity);
}

JSTR_FUNC
JSTR_INLINE
static jstr_ty *
jstr_end(const jstr_ty *R j)
{
	return (jstr_ty *)j->data + j->size;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_reallocexact(char *R *R s,
		   size_t *R sz,
		   size_t *R cap,
		   size_t new_cap)
JSTR_NOEXCEPT
{
	*cap = JSTR_MAX(JSTR_MIN_CAP, new_cap);
	*cap = JSTR_ALIGN_UP_STR(*cap);
	*s = (char *)realloc(*s, *cap);
	if (jstr_likely(*s != NULL))
		return 1;
	pjstr_nullify_members(sz, cap);
#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
	jstr_err_exit("");
#endif
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_realloc(char *R *R s,
	      size_t *R sz,
	      size_t *R cap,
	      size_t new_cap)
JSTR_NOEXCEPT
{
	*cap = pjstr_grow(*cap, new_cap);
	*s = (char *)realloc(*s, *cap);
	if (jstr_likely(*s != NULL))
		return 1;
	pjstr_nullify_members(sz, cap);
#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
	jstr_err_exit("");
#endif
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_realloc_may_zero(char *R *R s,
		       size_t *R sz,
		       size_t *R cap,
		       size_t new_cap)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*sz != 0))
		*cap = JSTR_MIN_CAP / 1.5;
	return pjstr_reallocexact(s, sz, cap, new_cap + 1);
}

/*
   Return 0 on malloc error.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_reserve_always(char *R *R s,
		    size_t *R sz,
		    size_t *R cap,
		    const size_t new_cap)
JSTR_NOEXCEPT
{
	return pjstr_realloc_may_zero(s, sz, cap, new_cap + 1);
}

/*
   Return 0 on malloc error.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_reserve_always_nomalloc(char *R *R s,
			     size_t *R sz,
			     size_t *R cap,
			     const size_t new_cap)
JSTR_NOEXCEPT
{
	return pjstr_realloc(s, sz, cap, new_cap + 1);
}

/*
   Return 0 on malloc error.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_reserveexact_always(char *R *R s,
			 size_t *R sz,
			 size_t *R cap,
			 const size_t new_cap)
JSTR_NOEXCEPT
{
	return pjstr_reallocexact(s, sz, cap, new_cap + 1);
}

/*
   Do nothing if new_cap < cap.
   Return 0 on malloc error.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_reserve(char *R *R s,
	     size_t *R sz,
	     size_t *R cap,
	     const size_t new_cap)
JSTR_NOEXCEPT
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
JSTR_INLINE
static int
jstr_reserveexact(char *R *R s,
		  size_t *R sz,
		  size_t *R cap,
		  const size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return 1;
	return jstr_reserveexact_always(s, sz, cap, new_cap);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_print(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	fwrite(j->data, 1, j->size, stdout);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_init(char *R *R s,
	  size_t *R sz,
	  size_t *R cap)
JSTR_NOEXCEPT
{
	*s = NULL;
	*sz = 0;
	*cap = 0;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_cat(char *R *R s,
	  size_t *R sz,
	  size_t *R cap,
	  va_list ap,
	  const size_t arglen)
JSTR_NOEXCEPT
{
	char *p;
	JSTR_RESERVE(s, sz, cap, *sz + arglen, return 0);
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
JSTR_NONNULL(1)
JSTR_NONNULL(2)
JSTR_NONNULL(3)
static int
jstr_cat(char *R *R s,
	 size_t *R sz,
	 size_t *R cap,
	 ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, cap);
	size_t arglen = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arglen += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arglen == 0))
		return 1;
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
JSTR_NONNULL(1)
static int
jstr_cat_j(jstr_ty *R j,
	   ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, j);
	size_t arglen = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arglen += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arglen == 0))
		return 1;
	va_start(ap, j);
	arglen = pjstr_cat(&j->data, &j->size, &j->capacity, ap, arglen);
	va_end(ap);
	return arglen;
}

/* Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_append_len_p_unsafe(char *R s,
			 const size_t sz,
			 const char *R src,
			 const size_t srclen)
JSTR_NOEXCEPT
{
	return jstr_strcpy_len(s + sz, src, srclen) + srclen;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_append_unsafe(char *R s,
		   const size_t sz,
		   const char *R src)
JSTR_NOEXCEPT
{
	strcpy(s + sz, src);
}

/* Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_append_p_unsafe(char *R s,
		     const size_t sz,
		     const char *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy(s + sz, src);
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
		const size_t srclen)
JSTR_NOEXCEPT
{
	JSTR_RESERVE(s, sz, cap, *sz + srclen, return 0);
	*sz = jstr_append_len_p_unsafe(*s, *sz, src, srclen) - *s;
	return 1;
}

/* Append N Cs to end of S. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_appendnchr_p_unsafe(char *R s,
			 const size_t sz,
			 const int c,
			 const size_t n)
JSTR_NOEXCEPT
{
	return (char *)memset(s + sz, c, n) + n;
}

/*
   Append N Cs to end of S.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_appendnchr(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const int c,
		const size_t n)
JSTR_NOEXCEPT
{
	JSTR_RESERVE(s, sz, cap, *sz + n, return 0);
	*sz = jstr_appendnchr_p_unsafe(*s, *sz, c, n) - *s;
	return 1;
}

/*
   Prepend N Cs to S.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_prependnchr_p_unsafe(char *R s,
			  const size_t sz,
			  const int c,
			  const size_t n)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0))
		jstr_strmove_len(s + n, s, sz);
	else
		*(s + n) = '\0';
	return (char *)memset(s, c, n) + n;
}

/*
   Prepend N Cs to S.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_prependnchr(char *R *R s,
		 size_t *R sz,
		 size_t *R cap,
		 const int c,
		 const size_t n)
JSTR_NOEXCEPT
{
	JSTR_RESERVE(s, sz, cap, *sz + n, return 0);
	*sz = jstr_prependnchr_p_unsafe(*s, *sz, c, n) - *s;
	return 1;
}

/*
   Only prepend NUL if S is empty.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_prepend_len_unsafe(char *R s,
			size_t *R sz,
			const char *R src,
			const size_t srclen)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s != 0))
		jstr_strmove_len(s + srclen, s, *sz);
	else
		*(s + srclen) = '\0';
	memcpy(s, src, srclen);
	*sz += srclen;
}

/*
   Only prepend NUL if S is empty.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_prepend_len(char *R *R s,
		 size_t *R sz,
		 size_t *R cap,
		 const char *R src,
		 const size_t srclen)
JSTR_NOEXCEPT
{
	JSTR_RESERVE(s, sz, cap, *sz + srclen, return 0);
	jstr_prepend_len_unsafe(*s, sz, src, srclen);
	return 1;
}

/*
   Assign SRC to DST.
   S is NUL terminated.
   Return ptr to '\0' in S.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_assign_len_p_unsafe(char *R s,
			 const char *R src,
			 const size_t srclen)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s, src, srclen);
}

/*
   Assign SRC to DST.
   S is NUL terminated.
   Return value:
   0 on malloc error
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_assign_len(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char *R src,
		const size_t srclen)
JSTR_NOEXCEPT
{
	if (*cap < srclen)
		JSTR_RESERVEEXACT_ALWAYS(s, sz, cap, srclen * JSTR_ALLOC_MULTIPLIER, return 0);
	*sz = jstr_assign_len_p_unsafe(*s, src, srclen) - *s;
	return 1;
}

JSTR_INLINE
JSTR_FUNC_VOID
static void
jstr_push_back_unsafe(char *R s,
		      size_t *R sz,
		      const char c)
JSTR_NOEXCEPT
{
	*(s + *sz) = c;
	*(s + ++*sz) = '\0';
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
	       const char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		JSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * JSTR_GROWTH, return 0);
	jstr_push_back_unsafe(*s, sz, c);
	return 1;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_push_front_unsafe(char *R s,
		       size_t *R sz,
		       const char c)
JSTR_NOEXCEPT
{
	jstr_strmove_len(s + 1, s, (*sz)++);
	*s = c;
}

/*
   Push C to end of S.
   S is NUL terminated.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static int
jstr_push_front(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		JSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * JSTR_GROWTH, return 0);
	jstr_push_front_unsafe(*s, sz, c);
	return 1;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_back(char *R s,
	      size_t *R sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(*sz != 0))
		*(s + --*sz) = '\0';
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_pop_front(char *R s,
	       size_t *R sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(*sz != 0))
		memmove(s, s + 1, (*sz)--);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwrite(const char *R s,
	       const size_t sz,
	       FILE *R fp)
JSTR_NOEXCEPT
{
	return fwrite(s, 1, sz, fp) == sz;
}

#if 0

JSTR_FUNC
static int
pjstr_asprintf_count_udigits(unsigned long long number)
{
	int len;
	if (sizeof(unsigned long long) == 8) {
		if (number < 9)
			len = 1;
		else if (number <= 99)
			len = 2;
		else if (number <= 999)
			len = 3;
		else if (number <= 9999)
			len = 4;
		else if (number <= 99999)
			len = 5;
		else if (number <= 999999)
			len = 6;
		else if (number <= 9999999)
			len = 7;
		else if (number <= 99999999)
			len = 8;
		else if (number <= 999999999)
			len = 9;
		else if (number <= 9999999999)
			len = 10;
		else if (number <= 99999999999)
			len = 11;
		else if (number <= 999999999999)
			len = 12;
		else if (number <= 9999999999999)
			len = 13;
		else if (number <= 99999999999999)
			len = 14;
		else if (number <= 999999999999999)
			len = 15;
		else if (number <= 9999999999999999)
			len = 16;
		else if (number <= 99999999999999999)
			len = 17;
		else if (number <= 999999999999999999)
			len = 18;
		else
			len = 19;
	} else {
		len = 1;
		while (number /= 10)
			++len;
	}
	return len * 2 + 1;
}

JSTR_FUNC
static int
pjstr_asprintf_count_digits(long long number)
{
	if (number < 0)
		number = -number;
	return pjstr_asprintf_count_udigits(number);
}

#endif

/*
   Return size of allocation needed for vsprintf.
   Return value:
   size of allocation;
   -1 on error.
*/
JSTR_FUNC
static int
jstr_vsprintf_strlen(va_list ap, const char *R fmt)
JSTR_NOEXCEPT
{
	enum {
		NOT_LONG = 0,
		LONG,
		LONG_LONG,
	}; /* lflag */
	enum {
		DEFAULT = 0,
		PAD,
	}; /* state */
	enum {
		MAX_WINT = CHAR_BIT * sizeof(wint_t),
		MAX_INT = CHAR_BIT * sizeof(int),
		MAX_LONG = CHAR_BIT * sizeof(long),
		MAX_LONG_LONG = CHAR_BIT * sizeof(long long),
		MAX_FLT = CHAR_BIT * sizeof(float) * 2 + 1,
		MAX_DBL = CHAR_BIT * sizeof(double) * 2 + 1,
		MAX_LDBL = CHAR_BIT * sizeof(long double) * 2 + 1,
		SIZE_T = (sizeof(size_t) == sizeof(unsigned long)) ? LONG : LONG_LONG,
		PTR_T = (sizeof(uintptr_t) == sizeof(unsigned long)) ? LONG : LONG_LONG
	};
	/* Use size_t because we are calculating the length for the buffer conservatively
	   and it may be much larger than the actual length. */
	size_t arglen = 0;
	const char *arg;
	int errno_len = 0;
	int lflag = NOT_LONG; /* long flag */
	int state = DEFAULT;
	unsigned int padlen;
	for (;; ++fmt) {
		if (*fmt == '%') {
cont_switch:
			switch (*++fmt) {
			case 's':
				arg = va_arg(ap, const char *);
				arglen = jstr_likely(arg != NULL) ? strlen(arg) : sizeof("(null)") - 1;
				break;
			case 'c':
				if (jstr_likely(lflag == NOT_LONG)) {
					++arglen;
				} else if (lflag & LONG) {
					arglen += MAX_INT;
					lflag = NOT_LONG;
				} else {
					goto einval;
				}
				goto get_arg;
				/* int */
			case 'u':
				/* fallthrough */
			case 'd':
			case 'i':
			case 'x':
			case 'X':
				if (lflag == NOT_LONG) {
					arglen += MAX_INT;
				} else {
					if (lflag & LONG)
						arglen = MAX_LONG;
					else if (lflag & LONG_LONG)
						arglen = MAX_LONG_LONG;
					else
						goto einval;
					lflag = NOT_LONG;
				}
				goto get_arg;
			case 'o':
				if (lflag == NOT_LONG) {
					arglen += MAX_LONG;
				} else {
					if (lflag & LONG)
						arglen += MAX_LONG;
					else if (lflag & LONG_LONG)
						arglen += MAX_LONG_LONG;
					else
						goto einval;
					lflag = NOT_LONG;
				}
				goto get_arg;
				/* ptr */
			case 'p':
				arg = va_arg(ap, const char *);
				/* max + 2 to make room for the 0x at the start of the number. */
				arglen = jstr_likely(arg != NULL) ? (PTR_T + 2) : sizeof("(nil)") - 1;
				break;
				/* chars written */
			case 'n':
				arglen += MAX_INT;
				break;
				/* double */
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				if (lflag == NOT_LONG) {
					arglen += MAX_DBL;
				} else if (lflag & LONG) {
					arglen += MAX_LDBL;
					lflag = NOT_LONG;
				} else {
					goto einval;
				}
				goto get_arg;
			case '%':
				arglen += 2;
				break;
			case 'm':
				if (errno_len == 0)
					arglen += strlen(strerror(errno));
				else
					arglen += errno_len;
				break;
			/* padding */
			case '*':
				/* Get length of padding from argument. */
				arglen += va_arg(ap, int);
				goto cont_switch;
			case '-':
				state = PAD;
			/* flags */
			case '+':
			case '#':
			case '0':
			case ' ':
			case '\'':
			/* precision */
			case '.':
				goto cont_switch;
			/* length modifier */
			case 'l':
				++lflag;
				goto cont_switch;
			case 't':
				lflag = PTR_T;
				goto cont_switch;
			case 'z':
				lflag = SIZE_T;
				goto cont_switch;
			case 'h':
			case 'j':
				goto cont_switch;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (state == PAD) {
					padlen = *fmt - '0' + (*fmt != 9);
					for (; jstr_isdigit(*fmt); ++fmt, padlen *= 10)
						;
					if (jstr_unlikely(padlen > INT_MAX)) {
						errno = EOVERFLOW;
						return -1;
					}
					arglen += padlen;
					state = DEFAULT;
				}
				goto cont_switch;
get_arg:
				va_arg(ap, void *);
				break;
einval:
			/* case '\0': */
			default:
				if (jstr_unlikely(*(fmt - 1) == '%')) {
					errno = EINVAL;
					return -1;
				}
				state = DEFAULT;
				goto string;
			}
			state = DEFAULT;
		} else {
string:
			++arglen;
			if (jstr_unlikely(*fmt == '\0'))
				break;
		}
	}
	if (jstr_unlikely(arglen > INT_MAX))
		arglen = INT_MAX;
	return arglen;
}

JSTR_FUNC_VOID
JSTR_NOINLINE
static void
pjstr_sprintf_err(char *R *R s,
		  size_t *R sz,
		  size_t *R cap)
{

	jstr_free(s, sz, cap);
	PJSTR_EXIT_MAYBE();
}

/*
   Return 0 on error.
   Supported conversions:
   %s - string.
   %c - char.
   %% - %.
   %d, %i, %u, %o, %x, %X - int.
   %a, %A, %e, %E, %f, %F, %g, %G - float/double.
   %zu - size_t.
   %l - long.
   %ll - long long.
   %lu - unsigned long.
   %llu - unsigned long long.
   %p - ptr.
   %n - chars written.
   %m - strlen of errno string.
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static int
jstr_asprintf(char *R *R s,
	      size_t *R sz,
	      size_t *R cap,
	      const char *R fmt,
	      ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	JSTR_RESERVEEXACT(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz = arglen;
	return 1;
err_free:
	pjstr_sprintf_err(s, sz, cap);
err:
	return 0;
}

/*
   Return 0 on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static int
jstr_asprintf_j(jstr_ty *R j,
		const char *R fmt,
		...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	JSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size = arglen;
	return 1;
err_free:
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return 0;
}

/*
   Append ... to end of S.
   Return 0 on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static int
jstr_asprintf_append(char *R *R s,
		     size_t *R sz,
		     size_t *R cap,
		     const char *R fmt,
		     ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	arglen += *sz;
	JSTR_RESERVEEXACT(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz += arglen;
	return 1;
err_free:
	pjstr_sprintf_err(s, sz, cap);
err:
	return 0;
}

/*
   Append ... to end of S.
   Return 0 on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static int
jstr_asprintf_append_j(jstr_ty *R j,
		       const char *R fmt,
		       ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	arglen += j->size;
	JSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size += arglen;
	return 1;
err_free:
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return 0;
}

/*
   Return 0 on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static int
jstr_asprintf_from(char *R *R s,
		   size_t *R sz,
		   size_t *R cap,
		   const size_t start_idx,
		   const char *R fmt,
		   ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	arglen += start_idx;
	JSTR_RESERVEEXACT(s, sz, cap, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	*sz = arglen + start_idx;
	return 1;
err_free:
	pjstr_sprintf_err(s, sz, cap);
err:
	return 0;
}

/*
   Return 0 on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return 0 and set errno to EINVAL.
*/
JSTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static int
jstr_asprintf_from_j(jstr_ty *R j,
		     const size_t start_idx,
		     const char *R fmt,
		     ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	size_t arglen = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely((int)arglen == -1))
		goto err;
	arglen += start_idx;
	JSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arglen * JSTR_ALLOC_MULTIPLIER, goto err);
	va_start(ap, fmt);
	arglen = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely((int)arglen < 0))
		goto err_free;
	j->size = arglen + start_idx;
	return 1;
err_free:
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
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
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free;
	*sz = ret;
	return 1;
err_free:
	if (errno != EINVAL)
		pjstr_sprintf_err(s, sz, cap);
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
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free;
	j->size = ret;
	return 1;
err_free:
	if (errno != EINVAL)
		pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
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
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free;
	*sz = ret + start_idx;
	return 1;
err_free:
	if (errno != EINVAL)
		pjstr_sprintf_err(s, sz, cap);
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
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free;
	j->size = ret + start_idx;
	return 1;
err_free:
	if (errno != EINVAL)
		pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
	return 0;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
