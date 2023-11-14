/* See LICENSE file for copyright and license details. */

#ifndef JSTR_LIST_H
#define JSTR_LIST_H

#include "jstr-macros.h"

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define JSTRLP_RESERVE_FAIL(func, list, new_cap, do_on_mallocerr) \
	if (jstr_unlikely(!func(list, new_cap))) {                 \
		JSTRP_EXIT_MAYBE();                                \
		do_on_mallocerr;                                   \
	}

#define JSTRLP_RESERVE(list, new_cap, do_on_mallocerr) \
	JSTRLP_RESERVE_FAIL(jstrl_reserve, list, new_cap, do_on_mallocerr)
#define JSTRLP_RESERVE_ALWAYS(list, new_cap, do_on_mallocerr) \
	JSTRLP_RESERVE_FAIL(jstrl_reservealways, list, new_cap, do_on_mallocerr)

#define jstrl_foreach(l, p) for (jstr_ty *p = ((l)->data), *const jstrl_ty_end_ = jstrl_end(l); \
	                          p < jstrl_ty_end_;                                              \
	                          ++p)

#define jstrl_foreachi(l, i) for (size_t i = 0, const jstrl_ty_end_ = ((l)->size); \
	                           i < jstrl_ty_end_;                               \
	                           ++i)

#define JSTRL_INIT \
	{           \
		0   \
	}

JSTRP_BEGIN_DECLS

typedef struct jstrl_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstrl_ty;

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstrl_start(const jstrl_ty *R l)
JSTR_NOEXCEPT
{
	return l->data;
}

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstrl_end(const jstrl_ty *R l)
JSTR_NOEXCEPT
{
	return l->data + l->size;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrl_free(jstrl_ty *R l)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->data != NULL)) {
		jstrl_foreach (l, p)
			free(p->data);
		free(l->data);
		l->data = NULL;
		l->size = 0;
		l->capacity = 0;
	}
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstrl_debug(const jstrl_ty *R l)
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

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstrl_at(const jstrl_ty *R l,
          const size_t idx)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(idx <= l->size, "Index out of bounds.");
	return l->data + idx;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstrl_index(jstrl_ty *R l,
             jstr_ty *R curr)
JSTR_NOEXCEPT
{
	return curr - l->data;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstrlp_grow(size_t cap,
             size_t new_cap)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(cap == 0))
		cap = JSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER;
	while ((cap *= JSTR_GROWTH) < new_cap)
		;
	return cap;
}

JSTR_FUNC
JSTR_INLINE
static int
jstrl_reservealways(jstrl_ty *R l,
                     size_t new_cap)
JSTR_NOEXCEPT
{
	new_cap = jstrlp_grow(l->capacity, new_cap);
	l->data = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	JSTRP_MALLOC_ERR(l->data, goto err);
	l->capacity = new_cap;
	return 1;
err:
	jstrp_nullify_members(&l->size, &l->capacity);
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
jstrl_reserve(jstrl_ty *R l,
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
jstrlp_allocassign_len(char *R *R s,
                        size_t *R sz,
                        size_t *R cap,
                        const char *R src,
                        const size_t src_len)
{
	*cap = JSTR_ALIGN_UP_STR(src_len + 1);
	*s = (char *)malloc(*cap);
	JSTRP_MALLOC_ERR(*s, goto err);
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return 1;
err:
	jstrp_nullify_members(sz, cap);
	return 0;
}

JSTR_FUNC_VOID
static void
jstrl_popback(jstrl_ty *R l)
{
	if (jstr_likely(l->size))
		free(jstrl_at(l, --l->size)->data);
}

JSTR_FUNC_VOID
static void
jstrl_popfront(jstrl_ty *R l)
{
	if (jstr_likely(l->size)) {
		free(l->data->data);
		if (jstr_likely(l->size > 1))
			memmove(l->data, l->data + 1, (jstrl_end(l) - (l->data + 1)) * sizeof(*l->data));
		--l->size;
	}
}

JSTR_FUNC
static int
jstrl_pushfront_len_unsafe(jstrl_ty *R l,
                            const char *R s,
                            const size_t s_len)
{
	if (jstr_likely(l->size))
		memmove(l->data + 1, l->data, (jstrl_end(l) - (l->data)) * sizeof(*l->data));
	++l->size;
	if (jstr_unlikely(
	    !jstrlp_allocassign_len(
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
jstrl_pushfront_len(jstrl_ty *R l,
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
jstrl_pushback_len_unsafe(jstrl_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(
	    !jstrlp_allocassign_len(
	    &jstrl_at(l, l->size)->data,
	    &jstrl_at(l, l->size)->size,
	    &jstrl_at(l, l->size)->capacity,
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
jstrl_pushback_len(jstrl_ty *R l,
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
jstrl_cat(jstrl_ty *R l,
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
		    !jstrlp_allocassign_len(&j->data, &j->size, &j->capacity, arg, strlen(arg))))
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
jstrl_assign_len(jstrl_ty *R l,
                  const size_t idx,
                  const char *R s,
                  const size_t s_len)
{
	return jstrlp_allocassign_len(&(l->data + idx)->data, &(l->data + idx)->size, &(l->data + idx)->capacity, s, s_len);
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_find_len(const jstrl_ty *R l,
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
jstrl_findcase_len(const jstrl_ty *R l,
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
jstrl_findstrchr(const jstrl_ty *R l,
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
jstrl_findstrchrinv(const jstrl_ty *R l,
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
jstrl_findstrstr_len(const jstrl_ty *R l,
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
jstrl_strcasestr_len(const jstrl_ty *R l,
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
jstrl_findstarts_len(const jstrl_ty *R l,
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
jstrl_findstartscase_len(const jstrl_ty *R l,
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
jstrl_findends_len(const jstrl_ty *R l,
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
jstrl_findendscase_len(const jstrl_ty *R l,
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
jstrlp_delete(jstrl_ty *l,
               jstr_ty *const p)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->size) && p) {
		free(p->data);
		if (jstr_likely(p != jstrl_end(l)))
			memmove(p, p + 1, (jstrl_end(l) - (p + 1)) * sizeof(jstrl_ty));
		--l->size;
	}
}

JSTR_FUNC_VOID
static void
jstrl_find_len_delete(jstrl_ty *R l,
                       const char *R s,
                       const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_find_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findstrchr_delete(jstrl_ty *R l,
                         const int c)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrchr(l, c));
}

JSTR_FUNC_VOID
static void
jstrl_findstrchrinv_delete(jstrl_ty *R l,
                            const int c)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrchrinv(l, c));
}

JSTR_FUNC_VOID
static void
jstrl_findstrstr_len_delete(jstrl_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstrstr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_strcasestr_len_delete(jstrl_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_strcasestr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findstarts_len_delete(jstrl_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstarts_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findends_len_delete(jstrl_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findends_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findcase_len_delete(jstrl_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findcase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findstartscase_len_delete(jstrl_ty *R l,
                                 const char *R s,
                                 const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findstartscase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstrl_findendscase_len_delete(jstrl_ty *R l,
                               const char *R s,
                               const size_t s_len)
JSTR_NOEXCEPT
{
	jstrlp_delete(l, jstrl_findendscase_len(l, s, s_len));
}

#undef JSTRP_DELETE

JSTRP_END_DECLS

#undef R

#endif /* JSTR_LIST_H */
