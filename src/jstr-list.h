/* See LICENSE file for copyright and license details. */

#ifndef JSTRL_H
#define JSTRL_H

#include "jstr-macros.h"

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define PJSTRL_RESERVE_FAIL(func, list, new_cap, do_on_mallocerr) \
	if (jstr_unlikely(!func(list, new_cap))) {                \
		PJSTR_EXIT_MAYBE();                               \
		do_on_mallocerr;                                  \
	}

#define PJSTRL_RESERVE(list, new_cap, do_on_mallocerr) \
	PJSTRL_RESERVE_FAIL(jstrl_reserve, list, new_cap, do_on_mallocerr)
#define PJSTRL_RESERVEALWAYS(list, new_cap, do_on_mallocerr) \
	PJSTRL_RESERVE_FAIL(jstrl_reservealways, list, new_cap, do_on_mallocerr)

#define jstrl_foreach(l, p) for (jstr_ty *p = ((l)->data), *const pjstrl_foreach_end_##l##_##p = jstrl_end(l); \
	                         p < pjstrl_foreach_end_##l##_##p;                                             \
	                         ++p)

#define pjstrl_foreach_cap(l, p) for (jstr_ty *p = ((l)->data), *const pjstrl_foreach_end_##l##_##p = ((l)->data) + ((l)->capacity); \
	                              p < pjstrl_foreach_end_##l##_##p;                                                              \
	                              ++p)

#define jstrl_foreachi(l, i) for (size_t i = 0, const pjstrl_foreach_end_##l##_##p = ((l)->size); \
	                          i < pjstrl_foreach_end_##l##_##p;                               \
	                          ++i)

#define JSTRL_INIT \
	{          \
		0  \
	}

PJSTR_BEGIN_DECLS

typedef struct jstrlist_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstrlist_ty;

JSTR_FUNC_VOID
JSTR_NOINLINE
JSTR_COLD
static void
pjstrl_nullify_members(jstrlist_ty *R l)
{
	l->size = 0;
	l->capacity = 0;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstrl_elemstore(jstr_ty *R dst,
                 const jstr_ty *R src)
{
	dst->data = src->data;
	dst->capacity = src->capacity;
	dst->size = 0;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstrl_elemmove(jstrlist_ty *R l,
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
pjstrl_at(const jstrlist_ty *R l,
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
		pjstrl_foreach_cap(l, p)
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
pjstrl_grow(size_t cap,
            size_t new_cap)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(cap == 0))
		cap = PJSTR_MIN_CAP / PJSTR_ALLOC_MULTIPLIER;
	while ((cap *= PJSTRL_GROWTH) < new_cap)
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
	new_cap = pjstrl_grow(l->capacity, new_cap);
	jstr_ty *tmp = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	PJSTR_MALLOC_ERR(tmp, goto err);
	l->data = tmp;
#if JSTRL_LAZY_FREE
	memset(l->data + l->capacity, 0, (new_cap - l->capacity) * sizeof(*l->data));
#endif
	l->capacity = new_cap;
	return 1;
err:
	jstrl_free(l);
	PJSTR_EXIT_MAYBE();
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
		PJSTRL_RESERVEALWAYS(l, new_cap, return 0);
	return 1;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstrl_assign_len(char *R *R s,
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
	PJSTR_MALLOC_ERR(*s, goto err);
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return 1;
err:
	pjstr_nullify_members(sz, cap);
	return 0;
#endif
}

JSTR_FUNC_VOID
static void
jstrl_popback(jstrlist_ty *R l)
{
	if (jstr_likely(l->size)) {
#if JSTRL_LAZY_FREE
		pjstrl_at(l, --l->size)->size = 0;
#else
		free(pjstrl_at(l, --l->size)->data);
#endif
	}
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
pjstrl_memmove(jstr_ty *const dst,
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
		pjstrl_elemstore(&tmp, l->data);
#else
		free(l->data->data);
#endif
		if (jstr_likely(l->size > 1))
			pjstrl_memmove(l->data, l->data + 1, (l->size - 1) * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		pjstrl_elemmove(l, &tmp);
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
		pjstrl_memmove(l->data + 1, l->data, l->size * sizeof(*l->data));
	++l->size;
	if (jstr_unlikely(
	    !pjstrl_assign_len(
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
	PJSTRL_RESERVE(l, l->size + 1, goto err)
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
	    !pjstrl_assign_len(
	    &pjstrl_at(l, l->size)->data,
	    &pjstrl_at(l, l->size)->size,
	    &pjstrl_at(l, l->size)->capacity,
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
	PJSTRL_RESERVE(l, l->size + 1, goto err)
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
	va_start(ap, l);
	int argc = 0;
	for (; va_arg(ap, void *); ++argc)
		;
	va_end(ap);
	if (jstr_unlikely(argc == 0))
		return 1;
	PJSTRL_RESERVE(l, l->size + argc, return 0)
	va_start(ap, l);
	const char *R arg;
	for (jstr_ty *j = l->data + l->size; (arg = va_arg(ap, const char *)); ++j, ++l->size)
		if (jstr_unlikely(
		    !pjstrl_assign_len(&j->data, &j->size, &j->capacity, arg, strlen(arg))))
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
	const int ret = pjstrl_assign_len(&pjstrl_at(l, idx)->data, &pjstrl_at(l, idx)->size, &pjstrl_at(l, idx)->capacity, s, s_len);
	if (jstr_likely(ret == 1))
		return 1;
	jstrl_free(l);
	return 0;
}

#define PJSTRL_DEFINE_FIND_LEN(name, func)                    \
	JSTR_FUNC_PURE                                        \
	static jstr_ty *                                      \
	jstrl_##name(const jstrlist_ty *R l,                  \
	             const char *R s,                         \
	             const size_t s_len)                      \
	JSTR_NOEXCEPT                                         \
	{                                                     \
		jstrl_foreach (l, j)                          \
			if (func(s, s_len, j->data, j->size)) \
				return j;                     \
		return NULL;                                  \
	}

PJSTRL_DEFINE_FIND_LEN(find_len, !jstr_cmpeq)
PJSTRL_DEFINE_FIND_LEN(findcase_len, !jstr_cmpcaseeq)
PJSTRL_DEFINE_FIND_LEN(findstrstr_len, jstr_strstr_len)
PJSTRL_DEFINE_FIND_LEN(findstrcasestr_len, jstr_strcasestr_len)
PJSTRL_DEFINE_FIND_LEN(findstarts_len, jstr_starts_len)
PJSTRL_DEFINE_FIND_LEN(findstartscase_len, jstr_startscase_len)
PJSTRL_DEFINE_FIND_LEN(findends_len, jstr_ends_len)
PJSTRL_DEFINE_FIND_LEN(findendscase_len, jstr_endscase_len)

#undef PJSTRL_DEFINE_FIND_LEN

#define PJSTRL_DEFINE_FIND(name, func)                        \
	JSTR_FUNC_PURE                                        \
	static jstr_ty *                                      \
	jstrl_##name(const jstrlist_ty *R l,                  \
	             const char *R s)                         \
	JSTR_NOEXCEPT                                         \
	{                                                     \
		const size_t s_len = strlen(s);               \
		jstrl_foreach (l, j)                          \
			if (func(s, s_len, j->data, j->size)) \
				return j;                     \
		return NULL;                                  \
	}

PJSTRL_DEFINE_FIND(findcase, !jstr_cmpcaseeq)
PJSTRL_DEFINE_FIND(findstrcasestr, jstr_strcasestr_len)
PJSTRL_DEFINE_FIND(find, !jstr_cmpeq)
PJSTRL_DEFINE_FIND(findstrstr, jstr_strstr_len)
PJSTRL_DEFINE_FIND(findstarts, jstr_starts_len)
PJSTRL_DEFINE_FIND(findstartscase, jstr_startscase_len)
PJSTRL_DEFINE_FIND(findends, jstr_ends_len)
PJSTRL_DEFINE_FIND(findendscase, jstr_endscase_len)

#undef PJSTRL_DEFINE_FIND

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
jstrl_findstrcasechr(const jstrlist_ty *R l,
                     const int c)
JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_strcasechr_len(j->data, c, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_INLINE
static void
pjstrl_delete(jstrlist_ty *R l,
              jstr_ty *R p)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->size) && p) {
#if JSTRL_LAZY_FREE
		jstr_ty tmp;
		pjstrl_elemstore(&tmp, p);
#else
		free(p->data);
#endif
		if (jstr_likely(p != jstrl_end(l)))
			pjstrl_memmove(p, p + 1, (jstrl_end(l) - (p + 1)) * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		pjstrl_elemmove(l, &tmp);
#endif
	}
}

#define PJSTRL_DEFINE_DELETE_LEN(name, func) \
	JSTR_FUNC_VOID                       \
	static void                          \
	jstrl_##name(jstrlist_ty *R l,       \
	             const char *R s,        \
	             const size_t s_len)     \
	JSTR_NOEXCEPT                        \
	{                                    \
		pjstrl_delete(l, func);      \
	}

PJSTRL_DEFINE_DELETE_LEN(delete_len, jstrl_find_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletecase_len, jstrl_findcase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestrstr_len, jstrl_findstrstr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestrcasestr_len, jstrl_findstrcasestr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestarts_len, jstrl_findstarts_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestartscase_len, jstrl_findstartscase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deleteends_len, jstrl_findends_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deleteendscase_len, jstrl_findendscase_len(l, s, s_len))

#undef PJSTRL_DEFINE_DELETE_LEN

#define PJSTRL_DEFINE_DELETE(name, func)        \
	JSTR_FUNC_VOID                          \
	static void                             \
	jstrl_##name(jstrlist_ty *R l,          \
	             const char *R s)           \
	JSTR_NOEXCEPT                           \
	{                                       \
		const size_t s_len = strlen(s); \
		pjstrl_delete(l, func);         \
	}

PJSTRL_DEFINE_DELETE(delete, jstrl_find_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deletecase, jstrl_findcase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deletestrstr, jstrl_findstrstr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deletestrcasestr, jstrl_findstrcasestr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deletestarts, jstrl_findstarts_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deletestartscase, jstrl_findstartscase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deleteends, jstrl_findends_len(l, s, s_len))
PJSTRL_DEFINE_DELETE(deleteendscase, jstrl_findendscase_len(l, s, s_len))

#undef PJSTRL_DEFINE_DELETE

JSTR_FUNC_VOID
static void
jstrl_deletestrchr(jstrlist_ty *R l,
                   const int c)
JSTR_NOEXCEPT
{
	pjstrl_delete(l, jstrl_findstrchr(l, c));
}

PJSTR_END_DECLS

#undef R

#endif /* JSTRL_H */
