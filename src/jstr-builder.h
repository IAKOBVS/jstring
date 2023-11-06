#ifndef JSTR_BUILDER_H
#define JSTR_BUILDER_H 1

#include "jstr-macros.h"
#include "jstr-struct.h"

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

#define R JSTR_RESTRICT

#define JSTR_INIT \
	{         \
		0 \
	}

#define jstr_foreach(j, ptr) for (char *ptr = ((j)->data), *const jstr_ty_end_ = ((j)->data) + ((j)->size); \
				  ptr < jstr_ty_end_;                                                       \
				  ++ptr)

#define jstr_foreachi(j, i) for (size_t i = 0, const jstr_j_ty_end_ = ((j)->size); \
				 i < jstr_j_ty_end_;                               \
				 ++i)

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

#define JSTR_MIN_ALLOC(cap)	 (((cap) > JSTR_MIN_CAP) ? ((cap)*JSTR_ALLOC_MULTIPLIER) : (JSTR_MIN_CAP))
#define JSTR_MIN_ALLOCEXACT(cap) (((cap) > JSTR_MIN_CAP) ? (cap) : (JSTR_MIN_CAP))
#define jstr_err(msg)		 pjstr_err(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg)
#define jstr_err_exit(msg)	 pjstr_err_exit(__FILE__, __LINE__, JSTR_ASSERT_FUNC, msg)

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

#define PJSTR_RESERVE_ALWAYS_NOMALLOC(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reservealways_nomalloc, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact_always, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVEEXACT(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVE_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reservealways, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVE(s, sz, cap, new_cap, do_on_malloc_err) \
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

JSTR_FUNC_CONST
JSTR_INLINE
static char *
jstr_at(const jstr_ty *R j,
	const size_t idx)
JSTR_NOEXCEPT
{
#if JSTR_DEBUG
	if (jstr_unlikely(idx >= j->size))
		jstr_err_exit("Index out of bounds.");
#endif
	return j->data + idx;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstr_index(const jstr_ty *R j,
	   const char *R curr)
{
	return curr - j->data;
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
jstr_start(const jstr_ty *R j)
{
	return (jstr_ty *)j->data;
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
	PJSTR_EXIT_MAYBE();
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
jstr_reservealways(char *R *R s,
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
jstr_reservealways_nomalloc(char *R *R s,
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
	return jstr_reservealways(s, sz, cap, new_cap);
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

JSTR_FUNC
JSTR_INLINE
static int
jstr_shrink_to_fit(char *R *R s,
		   size_t *R sz,
		   size_t *R cap)
{
	PJSTR_RESERVEEXACT(s, sz, cap, *sz + 1, return 0)
	return 1;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_print(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	fwrite(j->data, 1, j->size, stdout);
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_cat(char *R *R s,
	  size_t *R sz,
	  size_t *R cap,
	  va_list ap,
	  const size_t arg_len)
JSTR_NOEXCEPT
{
	char *p;
	PJSTR_RESERVE(s, sz, cap, *sz + arg_len, return 0)
	p = *s + *sz;
	*sz += arg_len;
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
	size_t arg_len = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arg_len += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return 1;
	va_start(ap, cap);
	arg_len = pjstr_cat(s, sz, cap, ap, arg_len);
	va_end(ap);
	return arg_len;
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
	size_t arg_len = 0;
	for (const char *R arg; (arg = va_arg(ap, char *)); arg_len += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return 1;
	va_start(ap, j);
	arg_len = pjstr_cat(&j->data, &j->size, &j->capacity, ap, arg_len);
	va_end(ap);
	return arg_len;
}

/* Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_append_len_unsafe_p(char *R s,
			 const size_t sz,
			 const char *R src,
			 const size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s + sz, src, src_len);
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
jstr_append_unsafe_p(char *R s,
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
		const size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return 0)
	*sz = jstr_append_len_unsafe_p(*s, *sz, src, src_len) - *s;
	return 1;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_strset(char *R s,
	    const int c)
JSTR_NOEXCEPT
{
	const size_t len = strlen(s);
	memset(s, c, len);
}

JSTR_FUNC
JSTR_INLINE
static char *
jstr_assignnchr_unsafe_p(char *R s,
			 const size_t sz,
			 const int c,
			 const size_t n)
JSTR_NOEXCEPT
{
	memset(s, c, n);
	if (n > sz) {
		*(s + n) = '\0';
		return s + n;
	}
	return s + sz;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_assignnchr(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const int c,
		const size_t n)
JSTR_NOEXCEPT
{
	if (n > *sz) {
		PJSTR_RESERVE(s, sz, cap, n, return 0)
		memset(*s, c, n);
		*(*s + n) = '\0';
		*sz = n;
	} else {
		memset(*s, c, n);
	}
	return 1;
}

/* Append N Cs to end of S. */
JSTR_FUNC
JSTR_INLINE
static char *
jstr_pushbackn_len_unsafe_p(char *R s,
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
jstr_pushbackn(char *R *R s,
	       size_t *R sz,
	       size_t *R cap,
	       const int c,
	       const size_t n)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + n, return 0)
	*sz = jstr_pushbackn_len_unsafe_p(*s, *sz, c, n) - *s;
	return 1;
}

/*
   Prepend N Cs to S.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_pushfrontn_len_unsafe_p(char *R s,
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
jstr_pushfrontn(char *R *R s,
		size_t *R sz,
		size_t *R cap,
		const int c,
		const size_t n)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + n, return 0)
	*sz = jstr_pushfrontn_len_unsafe_p(*s, *sz, c, n) - *s;
	return 1;
}

/*
   Only prepend NUL if S is empty.
   Return value:
   0 on malloc error;
   otherwise 1.
*/
JSTR_FUNC
JSTR_INLINE
static char *
jstr_prepend_len_unsafe_p(char *R s,
			  const size_t sz,
			  const char *R src,
			  const size_t src_len)
JSTR_NOEXCEPT
{
	if (jstr_likely(*s != 0))
		jstr_strmove_len(s + src_len, s, sz);
	else
		*(s + src_len) = '\0';
	memcpy(s, src, src_len);
	return s + sz + src_len;
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
		 const size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return 0)
	*sz = jstr_prepend_len_unsafe_p(*s, *sz, src, src_len) - *s;
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
jstr_assign_len_unsafe_p(char *R s,
			 const char *R src,
			 const size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s, src, src_len);
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
		const size_t src_len)
JSTR_NOEXCEPT
{
	if (*cap < src_len)
		PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, src_len * JSTR_ALLOC_MULTIPLIER, return 0)
	*sz = jstr_assign_len_unsafe_p(*s, src, src_len) - *s;
	return 1;
}

JSTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_pushback_unsafe_p(char *R s,
		       const size_t sz,
		       const char c)
JSTR_NOEXCEPT
{
	*(s + sz) = c;
	*(s + sz) = '\0';
	return s + sz + 1;
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
jstr_pushback(char *R *R s,
	      size_t *R sz,
	      size_t *R cap,
	      const char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * JSTR_GROWTH, return 0)
	*sz = jstr_pushback_unsafe_p(*s, *sz, c) - *s;
	return 1;
}

JSTR_FUNC_VOID
JSTR_INLINE
static char *
jstr_pushfront_unsafe_p(char *R s,
			const size_t sz,
			const char c)
JSTR_NOEXCEPT
{
	jstr_strmove_len(s + 1, s, sz);
	*s = c;
	return s + sz + 1;
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
jstr_pushfront(char *R *R s,
	       size_t *R sz,
	       size_t *R cap,
	       const char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * JSTR_GROWTH, return 0)
	*sz = jstr_pushfront_unsafe_p(*s, *sz, c) - *s;
	return 1;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_INLINE
static char *
jstr_popback_p(char *R s,
	       const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0)) {
		*(s + sz - 1) = '\0';
		return s + sz - 1;
	}
	return s;
}

/* Pop s[0]. */
JSTR_FUNC_VOID
JSTR_INLINE
static char *
jstr_popfront_p(char *R s,
		const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0)) {
		memmove(s, s + 1, sz);
		return s + sz - 1;
	}
	return s;
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

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwriteln(const char *R s,
		 const size_t sz,
		 FILE *R fp)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(fwrite(s, 1, sz, fp) != sz))
		return 0;
	return fputc('\n', fp) == '\n';
}

#if 0

JSTR_FUNC
static int
pjstr_sprintf_count_udigits(unsigned long long number,
			    const unsigned int base)
{
#	define LOOP_BASE(base)                                                              \
		for (len = 1, ldigit = 1; (number /= base) != 0; len *= 10, ldigit = number) \
			;                                                                    \
		return ldigit * number
	int len;
	int ldigit;
	switch (base) {
	case 10:
		if (number <= 9)
			return number;
		LOOP_BASE(10);
	case 2: LOOP_BASE(2);
	case 8: LOOP_BASE(8);
	case 16: LOOP_BASE(16);
	default: LOOP_BASE(base);
	}
#	undef LOOP_BASE
}

JSTR_FUNC
static int
pjstr_sprintf_count_digits(long long number,
			   const unsigned int base)
{
	if (number < 0)
		number = -number;
	return pjstr_sprintf_count_udigits(number, base);
}

#endif

/*
   Return size of allocation needed for sprintf.
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
	size_t arg_len = 0;
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
				arg_len = jstr_likely(arg != NULL) ? strlen(arg) : sizeof("(null)") - 1;
				break;
			case 'c':
				if (jstr_likely(lflag == NOT_LONG)) {
					++arg_len;
				} else if (lflag == LONG) {
					arg_len += MAX_INT;
					lflag = NOT_LONG;
				} else {
					goto einval;
				}
				goto get_arg;
				/* int */
			case 'u':
			case 'd':
			case 'i':
			case 'x':
			case 'X':
				if (lflag == NOT_LONG) {
					arg_len += MAX_INT;
				} else {
islong:
					if (lflag == LONG)
						arg_len = MAX_LONG;
					else if (lflag == LONG_LONG)
						arg_len = MAX_LONG_LONG;
					else
						goto einval;
					lflag = NOT_LONG;
				}
				goto get_arg;
			case 'o':
				if (lflag == NOT_LONG) {
					arg_len += MAX_LONG;
				} else {
					goto islong;
				}
				goto get_arg;
				/* ptr */
			case 'p':
				arg = va_arg(ap, const char *);
				/* max + 2 to make room for the 0x at the start of the number. */
				arg_len = jstr_likely(arg != NULL) ? (PTR_T + 2) : sizeof("(nil)") - 1;
				break;
				/* chars written */
			case 'n':
				arg_len += MAX_INT;
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
					arg_len += MAX_DBL;
				} else if (lflag == LONG) {
					arg_len += MAX_LDBL;
					lflag = NOT_LONG;
				} else {
					goto einval;
				}
				goto get_arg;
			case '%':
				arg_len += 2;
				break;
			case 'm':
				if (errno_len == 0)
					arg_len += strlen(strerror(errno));
				else
					arg_len += errno_len;
				break;
			/* padding */
			case '*':
				/* Get length of padding from argument. */
				arg_len += va_arg(ap, int);
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
					arg_len += padlen;
					state = DEFAULT;
				}
				goto cont_switch;
einval:
			/* case '\0': */
			default:
				errno = EINVAL;
				return -1;
get_arg:
				va_arg(ap, void *);
				break;
			}
			state = DEFAULT;
		} else {
			++arg_len;
			if (jstr_unlikely(*fmt == '\0'))
				break;
		}
	}
	if (jstr_unlikely(arg_len > INT_MAX))
		arg_len = INT_MAX;
	return arg_len;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	PJSTR_RESERVEEXACT(s, sz, cap, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	*sz = arg_len;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	PJSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	j->size = arg_len;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	arg_len += *sz;
	PJSTR_RESERVEEXACT(s, sz, cap, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	*sz += arg_len;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	arg_len += j->size;
	PJSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	j->size += arg_len;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	arg_len += start_idx;
	PJSTR_RESERVEEXACT(s, sz, cap, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	*sz = arg_len + start_idx;
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
	int arg_len = jstr_vsprintf_strlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len == -1))
		goto err;
	arg_len += start_idx;
	PJSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, arg_len * JSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	j->size = arg_len + start_idx;
	return 1;
err_free:
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return 0;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf() to grow S.
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
   Use jstr_asprintf() to grow S.
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
   Use jstr_asprintf() to grow S.
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
   Use jstr_asprintf() to grow S.
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
