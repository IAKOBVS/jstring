/* See LICENSE file for copyright and license details. */

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
#include "jstr-stdstring.h"

#define R JSTR_RESTRICT

#define JSTR_INIT \
	{         \
		0 \
	}

#define jstr_err(msg)        pjstr_err(__FILE__, __LINE__, JSTR_STATIC_ASSERT_FUNC, msg)
#define jstr_err_exit(msg)   pjstr_err_exit(__FILE__, __LINE__, JSTR_STATIC_ASSERT_FUNC, msg)
#define jstr_foreach(j, ptr) for (char *ptr = ((j)->data), *const pjstr_foreach_end_##ptr = ((j)->data) + ((j)->size); \
	                          ptr < pjstr_foreach_end_##ptr;                                                       \
	                          ++ptr)
#define jstr_foreachi(j, i) for (size_t i = 0, const pjstr_foreach_end_##ptr = ((j)->size); \
	                         i < pjstr_foreach_end_##ptr;                               \
	                         ++i)

#if JSTR_DEBUG || JSTR_PANIC
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				jstr_err_exit("");        \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#	define JSTR_ASSERT_DEBUG(expr, msg)        \
		do {                                \
			if (jstr_unlikely(!(expr))) \
				jstr_err_exit(msg); \
		} while (0)
#else
#	define PJSTR_MALLOC_ERR(p, do_on_malloc_err)     \
		do {                                      \
			if (jstr_unlikely((p) == NULL)) { \
				do_on_malloc_err;         \
			}                                 \
		} while (0)
#	define JSTR_ASSERT_DEBUG(expr, msg) \
		do {                         \
		} while (0)
#endif

#if JSTR_DEBUG || JSTR_PANIC
#	define PJSTR_EXIT_MAYBE() jstr_err_exit("")
#	define PJSTR_ERR_MAYBE()  jstr_err("")
#else
#	define PJSTR_EXIT_MAYBE() \
		do {               \
		} while (0)
#	define PJSTR_ERR_MAYBE() \
		do {              \
		} while (0)
#endif

#define PJSTR_RESERVE_FAIL(func, s, sz, cap, new_cap, do_on_malloc_err) \
	if (jstr_chk(func(s, sz, cap, new_cap))) {                      \
		PJSTR_ERR_MAYBE();                                      \
		do_on_malloc_err;                                       \
	}

#define PJSTR_RESERVE(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserve, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVEEXACT(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVE_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserve_always, s, sz, cap, new_cap, do_on_malloc_err)
#define PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, new_cap, do_on_malloc_err) \
	PJSTR_RESERVE_FAIL(jstr_reserveexact_always, s, sz, cap, new_cap, do_on_malloc_err)

PJSTR_BEGIN_DECLS

JSTR_ATTR_NOINLINE
JSTR_ATTR_COLD
JSTR_FUNC_VOID
static void
pjstr_nullify_members(size_t *R sz,
                      size_t *R cap)
JSTR_NOEXCEPT
{
	*sz = 0;
	*cap = 0;
}

/* Set first char to NUL and size to zero. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_empty(char *R s,
           size_t *R sz)
JSTR_NOEXCEPT
{
	*s = '\0';
	*sz = 0;
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static size_t
pjstr_grow(size_t cap,
           size_t new_cap)
JSTR_NOEXCEPT
{
	while ((cap *= PJSTR_GROWTH) < new_cap)
		;
	return JSTR_ALIGN_UP(cap, PJSTR_ATTR_MALLOC_ALIGNMENT);
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((3))
JSTR_ATTR_NOINLINE
JSTR_ATTR_COLD
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
JSTR_NONNULL((1))
JSTR_NONNULL((3))
JSTR_ATTR_NOINLINE
JSTR_ATTR_COLD
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
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_debug(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	int ret;
	ret = fprintf(stderr, "size:%zu\ncapacity:%zu\n", j->size, j->capacity);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	const char *data;
	size_t size;
	if (jstr_likely(j->data != NULL)) {
		data = j->data;
		size = strlen(data);
	} else {
		data = "(null)";
		size = 0;
	}
	ret = fprintf(stderr, "strlen():%zu\n", size);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	ret = fprintf(stderr, "data puts():%s\n", data);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	ret = fputs("data:", stderr);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	fwrite(j->data, 1, j->size, stderr);
	if (jstr_unlikely(ferror(stderr)))
		goto err;
	if (jstr_unlikely(fputc('\n', stderr) == EOF))
		goto err;
	return JSTR_RET_SUCC;
err_set_errno:
	errno = ret;
err:
	PJSTR_EXIT_MAYBE();
	return JSTR_RET_ERR;
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static char *
jstr_at(const jstr_ty *R j,
        size_t idx)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(idx <= j->size, "Index out of bounds.");
	return j->data + idx;
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
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
JSTR_ATTR_INLINE
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
JSTR_ATTR_NOINLINE
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
JSTR_ATTR_INLINE
static void
jstr_free_j(jstr_ty *R j)
JSTR_NOEXCEPT
{
	jstr_free(&j->data, &j->size, &j->capacity);
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static jstr_ty *
jstr_start(const jstr_ty *R j)
{
	return (jstr_ty *)j->data;
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static jstr_ty *
jstr_end(const jstr_ty *R j)
{
	return (jstr_ty *)j->data + j->size;
}

/*
   Return JSTR_RET_ERR on malloc error.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserve_always(char *R *R s,
                    size_t *R sz,
                    size_t *R cap,
                    size_t new_cap)
JSTR_NOEXCEPT
{
	*s = (char *)realloc(*s, *cap = jstr_likely(*cap) ? pjstr_grow(*cap, new_cap) : new_cap * PJSTR_ALLOC_MULTIPLIER);
	PJSTR_MALLOC_ERR(*s, goto err);
	return JSTR_RET_SUCC;
err:
	pjstr_nullify_members(sz, cap);
	return JSTR_RET_ERR;
}

/*
   Do nothing if new_cap < cap.
   Return JSTR_RET_ERR on malloc error.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserve(char *R *R s,
             size_t *R sz,
             size_t *R cap,
             size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return JSTR_RET_SUCC;
	return jstr_reserve_always(s, sz, cap, new_cap);
}

/*
   Return JSTR_RET_ERR on malloc error.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserveexact_always(char *R *R s,
                         size_t *R sz,
                         size_t *R cap,
                         size_t new_cap)
JSTR_NOEXCEPT
{
	*s = (char *)realloc(*s, *cap = jstr_likely(*cap) ? new_cap + 1 : new_cap * PJSTR_ALLOC_MULTIPLIER);
	PJSTR_MALLOC_ERR(*s, goto err);
	return JSTR_RET_SUCC;
err:
	pjstr_nullify_members(sz, cap);
	return JSTR_RET_ERR;
}

/*
   Do nothing if new_cap < cap.
   Return JSTR_RET_ERR on malloc error.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_reserveexact(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap < *cap)
		return JSTR_RET_SUCC;
	return jstr_reserveexact_always(s, sz, cap, new_cap);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_shrink_to_fit(char *R *R s,
                   size_t *R sz,
                   size_t *R cap)
{
	PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz, return JSTR_RET_ERR)
	return JSTR_RET_SUCC;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_print(const jstr_ty *R j)
JSTR_NOEXCEPT
{
	fwrite(j->data, 1, j->size, stdout);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
pjstr_cat(char *R *R s,
          size_t *R sz,
          size_t *R cap,
          va_list ap,
          size_t arg_len)
JSTR_NOEXCEPT
{
	char *p;
	PJSTR_RESERVE(s, sz, cap, *sz + arg_len, return JSTR_RET_ERR)
	p = *s + *sz;
	*sz += arg_len;
	for (const char *R arg; (arg = va_arg(ap, const char *)); p = jstr_stpcpy(p, arg))
		;
	return JSTR_RET_SUCC;
}

/*
   Last arg must be NULL.
   Return value:
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_ATTR_SENTINEL
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
JSTR_NONNULL((2))
JSTR_NONNULL((3))
static jstr_ret_ty
jstr_cat(char *R *R s,
         size_t *R sz,
         size_t *R cap,
         ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, cap);
	size_t arg_len = 0;
	for (const char *R arg; (arg = va_arg(ap, const char *)); arg_len += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return JSTR_RET_SUCC;
	va_start(ap, cap);
	const jstr_ret_ty ret = pjstr_cat(s, sz, cap, ap, arg_len);
	va_end(ap);
	return ret;
}

/*
   Last arg must be NULL.
   Return value:
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_ATTR_SENTINEL
JSTR_FUNC_MAY_NULL
JSTR_NONNULL((1))
static jstr_ret_ty
jstr_cat_j(jstr_ty *R j,
           ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, j);
	size_t arg_len = 0;
	for (const char *R arg; (arg = va_arg(ap, const char *)); arg_len += strlen(arg))
		;
	va_end(ap);
	if (jstr_unlikely(arg_len == 0))
		return JSTR_RET_SUCC;
	va_start(ap, j);
	const jstr_ret_ty ret = pjstr_cat(&j->data, &j->size, &j->capacity, ap, arg_len);
	va_end(ap);
	return ret;
}

/* Return ptr to '\0' in S. */
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_append_len_unsafe_p(char *R s,
                         size_t sz,
                         const char *R src,
                         size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s + sz, src, src_len);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_append_unsafe(char *R s,
                   size_t sz,
                   const char *R src)
JSTR_NOEXCEPT
{
	strcpy(s + sz, src);
}

/* Return ptr to '\0' in S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_append_unsafe_p(char *R s,
                     size_t sz,
                     const char *R src)
JSTR_NOEXCEPT
{
	return jstr_stpcpy(s + sz, src);
}

/*
   Append SRC to DST.
   Return value:
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_append_len(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R src,
                size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return JSTR_RET_ERR)
	*sz = jstr_append_len_unsafe_p(*s, *sz, src, src_len) - *s;
	return JSTR_RET_SUCC;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
jstr_strset(char *R s,
            int c)
JSTR_NOEXCEPT
{
	const size_t len = strlen(s);
	memset(s, c, len);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_assignnchr_unsafe_p(char *R s,
                         size_t sz,
                         int c,
                         size_t n)
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
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_assignnchr(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	if (n > *sz) {
		PJSTR_RESERVE_ALWAYS(s, sz, cap, n, return JSTR_RET_ERR)
		memset(*s, c, n);
		*(*s + n) = '\0';
		*sz = n;
	} else {
		memset(*s, c, n);
	}
	return JSTR_RET_SUCC;
}

/* Append N Cs to end of S. */
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_pushbackn_len_unsafe_p(char *R s,
                            size_t sz,
                            int c,
                            size_t n)
JSTR_NOEXCEPT
{
	return (char *)memset(s + sz, c, n) + n;
}

/*
   Append N Cs to end of S.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushbackn(char *R *R s,
               size_t *R sz,
               size_t *R cap,
               int c,
               size_t n)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + n, return JSTR_RET_ERR)
	*sz = jstr_pushbackn_len_unsafe_p(*s, *sz, c, n) - *s;
	return JSTR_RET_SUCC;
}

/*
   Prepend N Cs to S.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_pushfrontn_len_unsafe_p(char *R s,
                             size_t sz,
                             int c,
                             size_t n)
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
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushfrontn(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                int c,
                size_t n)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + n, return JSTR_RET_ERR)
	*sz = jstr_pushfrontn_len_unsafe_p(*s, *sz, c, n) - *s;
	return JSTR_RET_SUCC;
}

/*
   Only prepend NUL if S is empty.
   Return value:
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_ATTR_ACCESS((__read_only__, 3, 4))
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_prepend_len_unsafe_p(char *R s,
                          size_t sz,
                          const char *R src,
                          size_t src_len)
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
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_prepend_len(char *R *R s,
                 size_t *R sz,
                 size_t *R cap,
                 const char *R src,
                 size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return JSTR_RET_ERR)
	*sz = jstr_prepend_len_unsafe_p(*s, *sz, src, src_len) - *s;
	return JSTR_RET_SUCC;
}

/*
   Assign SRC to DST.
   S is NUL terminated.
   Return ptr to '\0' in S.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static char *
jstr_assign_len_unsafe_p(char *R s,
                         const char *R src,
                         size_t src_len)
JSTR_NOEXCEPT
{
	return jstr_stpcpy_len(s, src, src_len);
}

/*
   Assign SRC to DST.
   S is NUL terminated.
   Return value:
   JSTR_RET_ERR on malloc error
   otherwise JSTR_RET_SUCC.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_assign_len(char *R *R s,
                size_t *R sz,
                size_t *R cap,
                const char *R src,
                size_t src_len)
JSTR_NOEXCEPT
{
	PJSTR_RESERVE(s, sz, cap, *sz + src_len, return JSTR_RET_ERR)
	*sz = jstr_assign_len_unsafe_p(*s, src, src_len) - *s;
	return JSTR_RET_SUCC;
}

JSTR_ATTR_INLINE
JSTR_FUNC_VOID
static char *
jstr_pushback_unsafe_p(char *R s,
                       size_t sz,
                       char c)
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
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_ATTR_INLINE
JSTR_FUNC
static jstr_ret_ty
jstr_pushback(char *R *R s,
              size_t *R sz,
              size_t *R cap,
              char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * PJSTR_GROWTH, return JSTR_RET_ERR)
	*sz = jstr_pushback_unsafe_p(*s, *sz, c) - *s;
	return JSTR_RET_SUCC;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_pushfront_unsafe_p(char *R s,
                        size_t sz,
                        char c)
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
   JSTR_RET_ERR on malloc error;
   otherwise JSTR_RET_SUCC.
*/
JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstr_pushfront(char *R *R s,
               size_t *R sz,
               size_t *R cap,
               char c)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(*cap <= *sz))
		PJSTR_RESERVEEXACT_ALWAYS(s, sz, cap, *sz * PJSTR_GROWTH, return JSTR_RET_ERR)
	*sz = jstr_pushfront_unsafe_p(*s, *sz, c) - *s;
	return JSTR_RET_SUCC;
}

/* Pop s[size]. */
JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static char *
jstr_popback_p(char *R s,
               size_t sz)
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
JSTR_ATTR_INLINE
static char *
jstr_popfront_p(char *R s,
                size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_likely(sz != 0)) {
		memmove(s, s + 1, sz);
		return s + sz - 1;
	}
	return s;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_fwrite(const char *R s,
              size_t sz,
              FILE *R fp)
JSTR_NOEXCEPT
{
	return fwrite(s, 1, sz, fp) == sz ? JSTR_RET_SUCC : JSTR_RET_ERR;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrio_fwriteln(const char *R s,
                size_t sz,
                FILE *R fp)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(fwrite(s, 1, sz, fp) != sz))
		return JSTR_RET_ERR;
	return fputc('\n', fp) == '\n' ? JSTR_RET_SUCC : JSTR_RET_ERR;
}

/*
   Return size of allocation needed for sprintf.
   Return value:
   size of allocation;
   -1 on error and errno is set.
*/
JSTR_FUNC
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) \
|| (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 200112L)
JSTR_ATTR_INLINE
#endif
static int
jstr_vsprintfstrlen(va_list ap, const char *R fmt)
JSTR_NOEXCEPT
{
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) \
|| (defined _POSIX_C_SOURCE && _POSIX_C_SOURCE >= 200112L)
	const int ret = vsnprintf(NULL, 0, fmt, ap);
	if (jstr_likely(ret > 0))
		return ret + 1;
	errno = ret;
	return -1;
#else
#	define PJSTR_COUNTDIGITS(lflag, base)               \
		if (lflag == L_INT)                          \
			arg_len += INT / base;               \
		else {                                       \
			if (lflag == L_LONG)                 \
				arg_len += LONG / base;      \
			else if (lflag == L_LONG_LONG)       \
				arg_len += LONG_LONG / base; \
			else                                 \
				goto einval;                 \
			lflag = L_INT;                       \
		}
#	define PJSTR_GETMAXDIGITS(length)                                                 \
		do {                                                                       \
			if (base == B_DEC)                                                 \
				arg_len += (is_thousep) ? DEC_##length : 2 * DEC_##length; \
			else if (base == B_HEX)                                            \
				arg_len += (is_thousep) ? HEX_##length : 2 * DEC_##length; \
			else                                                               \
				arg_len += (is_thousep) ? OCT_##length : 2 * DEC_##length; \
		} while (0)
	enum {
		B_DEC = 10,
		B_OCT = 8,
		B_HEX = 16,
	}; /* base */
	enum {
		L_INT = 0,
		L_LONG,
		L_LONG_LONG,
	}; /* lflag */
	enum {
		WINT = CHAR_BIT * sizeof(wint_t),
		INT = CHAR_BIT * sizeof(int),
		LONG = CHAR_BIT * sizeof(long),
		LONG_LONG = CHAR_BIT * sizeof(long long),
		SIZE = CHAR_BIT * sizeof(size_t),
		UINTPTR = CHAR_BIT * sizeof(uintptr_t),
		FLOAT = 1 + CHAR_BIT * sizeof(float),
		DOUBLE = 1 + CHAR_BIT * sizeof(double),
		DOUBLE_LONG = 1 + CHAR_BIT * sizeof(long double),
		DEC_WINT = 1 + WINT / 10,
		DEC_INT = 1 + INT / 10,
		DEC_LONG = 1 + LONG / 10,
		DEC_LONG_LONG = 1 + LONG_LONG / 10,
		DEC_SIZE = 1 + SIZE / 10,
		DEC_UINTPTR = 1 + UINTPTR / 10,
		OCT_WINT = 1 + WINT / 8,
		OCT_INT = 1 + INT / 8,
		OCT_LONG = 1 + LONG / 8,
		OCT_LONG_LONG = 1 + LONG_LONG / 8,
		OCT_SIZE = 1 + SIZE / 8,
		OCT_UINTPTR = 1 + UINTPTR / 8,
		HEX_WINT = 1 + WINT / 16,
		HEX_INT = 1 + INT / 16,
		HEX_LONG = 1 + LONG / 16,
		HEX_LONG_LONG = 1 + LONG_LONG / 16,
		HEX_SIZE = 1 + SIZE / 16,
		HEX_UINTPTR = 1 + UINTPTR / 16,
	}; /* sizeof */
	/* Use size_t because we are calculating the length for the buffer conservatively
	   and it may be much larger than the actual length. */
	size_t arg_len = 0;
	const char *arg;
	int errno_len = 0;
	int lflag = L_INT; /* long flag */
	int is_pad = 0;
	int is_thousep = 0;
	unsigned int padlen;
	unsigned int base;
	for (;; ++fmt) {
		if (*fmt == '%') {
cont_switch:
			switch (*++fmt) {
			case 's':
				arg = va_arg(ap, const char *);
				arg_len += jstr_likely(arg != NULL) ? strlen(arg) : sizeof("(null)") - 1;
				break;
			case 'c':
				if (jstr_likely(lflag == L_INT)) {
					++arg_len;
				} else if (lflag == LONG) {
					arg_len += INT;
					lflag = L_INT;
				} else {
					goto einval;
				}
				goto get_arg;
				/* int */
			case 'u':
			case 'd':
			case 'i':
				base = B_DEC;
				goto check_integer;
			case 'x':
			case 'X':
				base = B_HEX;
				goto check_integer;
			case 'o':
				base = B_OCT;
				goto check_integer;
check_integer:
				if (lflag == L_INT) {
					PJSTR_GETMAXDIGITS(INT);
				} else {
					if (lflag == LONG)
						PJSTR_GETMAXDIGITS(LONG);
					else if (lflag == LONG_LONG)
						PJSTR_GETMAXDIGITS(LONG_LONG);
					else
						goto einval;
					lflag = L_INT;
				}
				goto get_arg;
				/* ptr */
			case 'p':
				/* max + 2 to make room for the 0x at the start of the number. */
				arg_len += jstr_likely(va_arg(ap, const void *) != NULL) ? (HEX_UINTPTR + sizeof("0x") - 1) : sizeof("(nil)") - 1;
				break;
				/* chars written */
			case 'n':
				arg_len += INT;
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
				if (lflag == L_INT) {
					arg_len += DOUBLE;
				} else if (lflag == LONG) {
					arg_len += DOUBLE_LONG;
					lflag = L_INT;
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
				is_pad = 1;
			/* flags */
			case '+':
			case '#':
			case '0':
			case ' ':
				goto cont_switch;
			case '\'':
				is_thousep = 1;
			/* precision */
			case '.':
				goto cont_switch;
			/* length modifier */
			case 'l':
				++lflag;
				goto cont_switch;
			case 't':
				lflag = UINTPTR;
				goto cont_switch;
			case 'z':
				lflag = SIZE;
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
				if (is_pad) {
					padlen = *fmt - '0' + (*fmt != 9);
					for (; jstr_isdigit(*fmt); ++fmt, padlen *= 10)
						;
					if (jstr_unlikely(padlen > INT_MAX)) {
						errno = EOVERFLOW;
						return -1;
					}
					arg_len += padlen;
					is_pad = 0;
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
			is_pad = 0;
			is_thousep = 0;
		} else {
			++arg_len;
			if (jstr_unlikely(*fmt == '\0'))
				break;
		}
	}
	++arg_len;
	if (jstr_unlikely(arg_len > INT_MAX))
		arg_len = INT_MAX;
	return arg_len;
#	undef PJSTR_COUNTDIGITS
#	undef PJSTR_GETMAXDIGITS
#endif
}

JSTR_FUNC_VOID
JSTR_ATTR_NOINLINE
static void
pjstr_sprintf_err(char *R *R s,
                  size_t *R sz,
                  size_t *R cap)
{

	jstr_free(s, sz, cap);
	PJSTR_EXIT_MAYBE();
}

/*
   Return JSTR_RET_ERR on error.
   Referencing arguments with $ is not supported.
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
   %m - errno string.
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf(char *R *R s,
              size_t *R sz,
              size_t *R cap,
              const char *R fmt,
              ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int arg_len = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err;
	PJSTR_RESERVEEXACT(s, sz, cap, arg_len * PJSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	arg_len = vsprintf(*s, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(arg_len < 0))
		goto err_free;
	*sz = arg_len;
	return JSTR_RET_SUCC;
err_free:
	pjstr_sprintf_err(s, sz, cap);
err:
	return JSTR_RET_ERR;
}

/*
   Return JSTR_RET_ERR on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_j(jstr_ty *R j,
                const char *R fmt,
                ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	PJSTR_RESERVEEXACT(&j->data, &j->size, &j->capacity, ret * PJSTR_ALLOC_MULTIPLIER, goto err)
	va_start(ap, fmt);
	ret = vsprintf(j->data, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return JSTR_RET_ERR;
}

/*
   Append ... to end of S.
   Return JSTR_RET_ERR on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_append(char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const char *R fmt,
                     ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	ret += *sz;
	PJSTR_RESERVE(s, sz, cap, ret, goto err)
	va_start(ap, fmt);
	ret = vsprintf(*s + *sz, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz += ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(s, sz, cap);
err:
	return JSTR_RET_ERR;
}

/*
   Append ... to end of S.
   Return JSTR_RET_ERR on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_append_j(jstr_ty *R j,
                       const char *R fmt,
                       ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	ret += j->size;
	PJSTR_RESERVE(&j->data, &j->size, &j->capacity, ret, goto err)
	va_start(ap, fmt);
	ret = vsprintf(j->data + j->size, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size += ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return JSTR_RET_ERR;
}

/*
   Return JSTR_RET_ERR on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_from(char *R *R s,
                   size_t *R sz,
                   size_t *R cap,
                   size_t start_idx,
                   const char *R fmt,
                   ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	ret += start_idx;
	PJSTR_RESERVE(s, sz, cap, ret, goto err)
	va_start(ap, fmt);
	ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz = ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(s, sz, cap);
err:
	return JSTR_RET_ERR;
}

/*
   Return JSTR_RET_ERR on error.
   Supports only some conversions: see jstr_asprintf().
   Otherwise, return JSTR_RET_ERR and set errno to EINVAL.
*/
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static jstr_ret_ty
jstr_asprintf_from_j(jstr_ty *R j,
                     size_t start_idx,
                     const char *R fmt,
                     ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	int ret = jstr_vsprintfstrlen(ap, fmt);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err;
	ret += start_idx;
	PJSTR_RESERVE(&j->data, &j->size, &j->capacity, ret, goto err)
	va_start(ap, fmt);
	ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
err:
	return JSTR_RET_ERR;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf() to grow S.
*/
JSTR_ATTR_FORMAT(printf, 4, 5)
JSTR_FUNC
static jstr_ret_ty
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
		goto err_free_set_errno;
	*sz = ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(s, sz, cap);
	return JSTR_RET_ERR;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf() to grow S.
*/
JSTR_ATTR_FORMAT(printf, 2, 3)
JSTR_FUNC
static jstr_ret_ty
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
		goto err_free_set_errno;
	j->size = ret;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
	return JSTR_RET_ERR;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf() to grow S.
*/
JSTR_ATTR_FORMAT(printf, 5, 6)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf_from(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  size_t start_idx,
                  const char *R fmt,
                  ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(*s + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	*sz = ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(s, sz, cap);
	return JSTR_RET_ERR;
}

/*
   Assume that S has enough space.
   Use jstr_asprintf() to grow S.
*/
JSTR_ATTR_FORMAT(printf, 3, 4)
JSTR_FUNC
static jstr_ret_ty
jstr_sprintf_from_j(jstr_ty *R j,
                    size_t start_idx,
                    const char *R fmt,
                    ...)
JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, fmt);
	const int ret = vsprintf(j->data + start_idx, fmt, ap);
	va_end(ap);
	if (jstr_unlikely(ret < 0))
		goto err_free_set_errno;
	j->size = ret + start_idx;
	return JSTR_RET_SUCC;
err_free_set_errno:
	errno = ret;
	pjstr_sprintf_err(&j->data, &j->size, &j->capacity);
	return JSTR_RET_ERR;
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_BUILDER H */
