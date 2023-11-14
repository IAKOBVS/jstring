/* See LICENSE file for copyright and license details. */

#ifndef JSTRL_H
#define JSTRL_H

#include "jstr-macros.h"

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define JSTRLP_RESERVE_FAIL(func, list, new_cap, do_on_mallocerr) \
	if (jstr_unlikely(!func(list, new_cap))) {                \
		JSTRP_EXIT_MAYBE();                               \
		do_on_mallocerr;                                  \
	}

#define JSTRLP_RESERVE(list, new_cap, do_on_mallocerr) \
	JSTRLP_RESERVE_FAIL(jstrl_reserve, list, new_cap, do_on_mallocerr)
#define JSTRLP_RESERVE_ALWAYS(list, new_cap, do_on_mallocerr) \
	JSTRLP_RESERVE_FAIL(jstrl_reservealways, list, new_cap, do_on_mallocerr)

#define jstrl_foreach(l, p) for (jstr_ty *p = ((l)->data), *const jstrlist_ty_end_ = jstrl_end(l); \
	                         p < jstrlist_ty_end_;                                             \
	                         ++p)

#define jstrlp_foreach_cap(l, p) for (jstr_ty *p = ((l)->data), *const jstrlist_ty_end_ = ((l)->data) + ((l)->capacity); \
	                              p < jstrlist_ty_end_;                                                              \
	                              ++p)

#define jstrl_foreachi(l, i) for (size_t i = 0, const jstrlist_ty_end_ = ((l)->size); \
	                          i < jstrlist_ty_end_;                               \
	                          ++i)

#define JSTRL_INIT \
	{          \
		0  \
	}

JSTRP_BEGIN_DECLS

typedef struct jstrlist_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstrlist_ty;

JSTR_FUNC_VOID
JSTR_NOINLINE
JSTR_COLD
static void
jstrlp_nullify_members(jstrlist_ty *R l)
{
	l->size = 0;
	l->capacity = 0;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrlp_elemstore(jstr_ty *R dst,
                 const jstr_ty *R src)
{
	dst->data = src->data;
	dst->capacity = src->capacity;
	dst->size = 0;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrlp_elemmove(jstrlist_ty *R l,
                const jstr_ty *R elem)
{
	jstr_ty *curr = l->data + l->size;
	const jstr_ty *const end = l->data + l->capacity;
	for (; curr < end && curr->data; ++curr)
		;
	*curr = *elem;
}

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstrl_start(const jstrlist_ty *R l)
JSTR_NOEXCEPT
{
	return l->data;
}

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstrl_end(const jstrlist_ty *R l)
JSTR_NOEXCEPT
{
	return l->data + l->size;
}

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstrl_at(const jstrlist_ty *R l,
         const size_t idx)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(idx <= l->size, "Index out of bounds.");
	return l->data + idx;
}

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstrlp_at(const jstrlist_ty *R l,
          const size_t idx)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(idx <= l->capacity, "Index out of bounds.");
	return l->data + idx;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstrl_index(jstrlist_ty *R l,
            jstr_ty *R curr)
JSTR_NOEXCEPT
{
	return curr - l->data;
}

JSTR_FUNC_VOID
static void
jstrl_free(jstrlist_ty *R l)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->data != NULL)) {
#if JSTRL_LAZY_FREE
		jstrlp_foreach_cap (l, p)
			free(p->data);
#else
		jstrl_foreach (l, p)
			free(p->data);
#endif
		free(l->data);
		l->data = NULL;
		l->size = 0;
		l->capacity = 0;
	}
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrl_debug(const jstrlist_ty *R l)
{
	fprintf(stderr, "size:%zu\n"
	                "cap:%zu\n"
	                "\n",
	        l->size,
	        l->capacity);
	jstrl_foreach (l, p) {
		fprintf(stderr,
		        "idx:%zu\n"
		        "size:%zu\n"
		        "cap:%zu\n",
		        p - l->data,
		        p->size,
		        p->capacity);
		fprintf(stderr, "data:%s\n"
		                "\n",
		        p->data);
	}
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstrlp_grow(size_t cap,
            size_t new_cap)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(cap == 0))
		cap = JSTRP_MIN_CAP / JSTRP_ALLOC_MULTIPLIER;
	while ((cap *= JSTRLP_GROWTH) < new_cap)
		;
	return cap;
}

JSTR_FUNC
JSTR_INLINE
static int
jstrl_reservealways(jstrlist_ty *R l,
                    size_t new_cap)
JSTR_NOEXCEPT
{
	new_cap = jstrlp_grow(l->capacity, new_cap);
	jstr_ty *tmp = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	JSTRP_MALLOC_ERR(tmp, goto err);
	l->data = tmp;
#if JSTRL_LAZY_FREE
	memset(l->data + l->capacity, 0, (new_cap - l->capacity) * sizeof(*l->data));
#endif
	l->capacity = new_cap;
	return 1;
err:
	jstrl_free(l);
#if JSTR_DEBUG || JSTR_EXIT_ON_ERROR
	jstr_err_exit("");
#endif
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
jstrl_reserve(jstrlist_ty *R l,
              size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap > l->capacity)
		JSTRLP_RESERVE_ALWAYS(l, new_cap, return 0);
	return 1;
}

JSTR_FUNC
JSTR_INLINE
static int
jstrlp_assign_len(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  const char *R src,
                  const size_t src_len)
{
#if JSTRL_LAZY_FREE
	return jstr_assign_len(s, sz, cap, src, src_len);
#else
	*cap = JSTR_ALIGN_UP_STR(src_len + 1);
	*s = (char *)malloc(*cap);
	JSTRP_MALLOC_ERR(*s, goto err);
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return 1;
err:
	jstrp_nullify_members(sz, cap);
	return 0;
#endif
}

JSTR_FUNC_VOID
static void
jstrl_popback(jstrlist_ty *R l)
{
	if (jstr_likely(l->size)) {
#if JSTRL_LAZY_FREE
		jstrlp_at(l, --l->size)->size = 0;
#else
		free(jstrlp_at(l, --l->size)->data);
#endif
	}
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrlp_memmove(jstr_ty *const dst,
               jstr_ty *const src,
               const size_t n)
{
	memmove(dst, src, n);
#if JSTRL_LAZY_FREE
	src->data = NULL;
	src->capacity = 0;
	src->size = 0;
#endif
}

JSTR_FUNC_VOID
static void
jstrl_popfront(jstrlist_ty *R l)
{
	if (jstr_likely(l->size)) {
#if JSTRL_LAZY_FREE
		jstr_ty tmp;
		jstrlp_elemstore(&tmp, l->data);
#else
		free(l->data->data);
#endif
		if (jstr_likely(l->size > 1))
			jstrlp_memmove(l->data, l->data + 1, (l->size - 1) * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		jstrlp_elemmove(l, &tmp);
#endif
	}
}

JSTR_FUNC
static int
jstrl_pushfront_len_unsafe(jstrlist_ty *R l,
                           const char *R s,
                           const size_t s_len)
{
	if (jstr_likely(l->size))
		jstrlp_memmove(l->data + 1, l->data, l->size * sizeof(*l->data));
	++l->size;
	if (jstr_unlikely(
	    !jstrlp_assign_len(
	    &l->data->data,
	    &l->data->size,
	    &l->data->capacity,
	    s,
	    s_len)))
		goto err;
	return 1;
err:
	jstrl_free(l);
	return 0;
}

JSTR_FUNC
static int
jstrl_pushfront_len(jstrlist_ty *R l,
                    const char *R s,
                    const size_t s_len)
{
	JSTRLP_RESERVE(l, l->size + 1, goto err)
	return jstrl_pushfront_len_unsafe(l, s, s_len);
err:
	jstrl_free(l);
	return 0;
}

JSTR_FUNC
static int
jstrl_pushback_len_unsafe(jstrlist_ty *R l,
                          const char *R s,
                          const size_t s_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(
	    !jstrlp_assign_len(
	    &jstrlp_at(l, l->size)->data,
	    &jstrlp_at(l, l->size)->size,
	    &jstrlp_at(l, l->size)->capacity,
	    s,
	    s_len)))
		goto err;
	++l->size;
	return 1;
err:
	jstrl_free(l);
	return 0;
}

JSTR_FUNC
static int
jstrl_pushback_len(jstrlist_ty *R l,
                   const char *R s,
                   const size_t s_len)
JSTR_NOEXCEPT
{
	JSTRLP_RESERVE(l, l->size + 1, goto err)
	return jstrl_pushback_len_unsafe(l, s, s_len);
err:
	jstrl_free(l);
	return 0;
}

/* Last arg must be NULL. */
JSTR_SENTINEL
JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
static int
jstrl_cat(jstrlist_ty *R l,
          ...)
JSTR_NOEXCEPT
{
	va_list ap;
	const char *R arg;
	va_start(ap, l);
	int argc = 0;
	for (; va_arg(ap, char *); ++argc)
		;
	va_end(ap);
	if (jstr_unlikely(argc == 0))
		return 1;
	JSTRLP_RESERVE(l, l->size + argc, return 0)
	va_start(ap, l);
	for (jstr_ty *j = l->data + l->size; (arg = va_arg(ap, char *)); ++j, ++l->size)
		if (jstr_unlikely(
		    !jstrlp_assign_len(&j->data, &j->size, &j->capacity, arg, strlen(arg))))
			goto err_free_l;
	va_end(ap);
	return 1;
err_free_l:
	jstrl_free(l);
	va_end(ap);
	return 0;
}

JSTR_FUNC
static int
jstrl_assign_len(jstrlist_ty *R l,
                 const size_t idx,
                 const char *R s,
                 const size_t s_len)
{
	const int ret = jstrlp_assign_len(&jstrlp_at(l, idx)->data, &jstrlp_at(l, idx)->size, &jstrlp_at(l, idx)->capacity, s, s_len);
	if (jstr_likely(ret == 1))
		return 1;
	jstrl_free(l);
	return 0;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_find_len(const jstrlist_ty *R l,
               const char *R s,
               const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (!jstr_cmpeq(s, s_len, j->data, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findcase_len(const jstrlist_ty *R l,
                   const char *R s,
                   const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (!jstr_cmpcaseeq(s, s_len, j->data, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstrchr(const jstrlist_ty *R l,
                 const int c)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (memchr(j->data, c, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstrchrinv(const jstrlist_ty *R l,
                    const int c)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_strchrinv(j->data, c))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstrstr_len(const jstrlist_ty *R l,
                     const char *R s,
                     const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_strstr_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_strcasestr_len(const jstrlist_ty *R l,
                     const char *R s,
                     const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_strcasestr_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstarts_len(const jstrlist_ty *R l,
                     const char *R s,
                     const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_starts_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstartscase_len(const jstrlist_ty *R l,
                         const char *R s,
                         const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_startscase_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findends_len(const jstrlist_ty *R l,
                   const char *R s,
                   const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_ends_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findendscase_len(const jstrlist_ty *R l,
                       const char *R s,
                       const size_t s_len)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_endscase_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_INLINE
static void
jstrlp_delete(jstrlist_ty *l,
              jstr_ty *p)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->size) && p) {
#if JSTRL_LAZY_FREE
		jstr_ty tmp;
		jstrlp_elemstore(&tmp, p);
#else
		free(p->data);
#endif
		if (jstr_likely(p != jstrl_end(l)))
			jstrlp_memmove(p, p + 1, (jstrl_end(l) - (p + 1)) * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		jstrlp_elemmove(l, &tmp);
#endif
	}
}

JSTR_FUNC_VOID
static void
jstrl_delete_len(jstrlist_ty *R l,
                 const char *R s,
                 const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_find_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deletestrchr(jstrlist_ty *R l,
                   const int c)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrchr(l, c));
}

JSTR_FUNC_VOID
static void
jstrl_deletestrchrinv(jstrlist_ty *R l,
                      const int c)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrchrinv(l, c));
}

JSTR_FUNC_VOID
static void
jstrl_deletestrstr_len(jstrlist_ty *R l,
                       const char *R s,
                       const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrstr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deletestrcasestr_len(jstrlist_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_strcasestr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deletestarts_len(jstrlist_ty *R l,
                       const char *R s,
                       const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstarts_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deleteends_len(jstrlist_ty *R l,
                     const char *R s,
                     const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findends_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deletecase_len(jstrlist_ty *R l,
                     const char *R s,
                     const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findcase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deletestartscase_len(jstrlist_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstartscase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_deleteendscase_len(jstrlist_ty *R l,
                         const char *R s,
                         const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findendscase_len(l, s, s_len));
}

#undef JSTRP_DELETE

JSTRP_END_DECLS

#undef R

#endif /* JSTRL_H */
