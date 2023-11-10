#ifndef JSTR_LIST_H
#define JSTR_LIST_H

#include "jstr-macros.h"

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define PJSTR_L_RESERVE_FAIL(func, list, new_cap, do_on_mallocerr) \
	if (jstr_unlikely(!func(list, new_cap))) {                 \
		PJSTR_EXIT_MAYBE();                                \
		do_on_mallocerr;                                   \
	}

#define PJSTR_L_RESERVE(list, new_cap, do_on_mallocerr) \
	PJSTR_L_RESERVE_FAIL(jstr_l_reserve, list, new_cap, do_on_mallocerr)
#define PJSTR_L_RESERVE_ALWAYS(list, new_cap, do_on_mallocerr) \
	PJSTR_L_RESERVE_FAIL(jstr_l_reservealways, list, new_cap, do_on_mallocerr)

#define jstr_l_foreach(l, p) for (jstr_ty *p = ((l)->data), *const jstr_l_ty_end_ = jstr_l_end(l); \
	                          p < jstr_l_ty_end_;                                              \
	                          ++p)

#define jstr_l_foreachi(l, i) for (size_t i = 0, const jstr_l_ty_end_ = ((l)->size); \
	                           i < jstr_l_ty_end_;                               \
	                           ++i)

#define JSTR_L_INIT \
	{           \
		0   \
	}

PJSTR_BEGIN_DECLS

typedef struct jstr_l_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstr_l_ty;

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstr_l_start(const jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	return l->data;
}

JSTR_FUNC_CONST
JSTR_INLINE
static jstr_ty *
jstr_l_end(const jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	return l->data + l->size;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_l_free(jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->data != NULL)) {
		jstr_l_foreach (l, p)
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
jstr_l_debug(const jstr_l_ty *R l)
{
	fprintf(stderr, "size:%zu\n"
	                "cap:%zu\n"
	                "\n",
	        l->size,
	        l->capacity);
	jstr_l_foreach (l, p) {
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
jstr_l_at(const jstr_l_ty *R l,
          const size_t idx)
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(idx <= l->size, "Index out of bounds.");
	return l->data + idx;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
jstr_l_index(jstr_l_ty *R l,
             jstr_ty *R curr)
JSTR_NOEXCEPT
{
	return curr - l->data;
}

JSTR_FUNC_CONST
JSTR_INLINE
static size_t
pjstr_l_grow(size_t cap,
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
jstr_l_reservealways(jstr_l_ty *R l,
                     size_t new_cap)
JSTR_NOEXCEPT
{
	new_cap = pjstr_l_grow(l->capacity, new_cap);
	l->data = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	PJSTR_MALLOC_ERR(l->data, goto err);
	l->capacity = new_cap;
	return 1;
err:
	pjstr_nullify_members(&l->size, &l->capacity);
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_l_reserve(jstr_l_ty *R l,
               size_t new_cap)
JSTR_NOEXCEPT
{
	if (new_cap > l->capacity)
		PJSTR_L_RESERVE_ALWAYS(l, new_cap, return 0);
	return 1;
}

JSTR_FUNC
JSTR_INLINE
static int
pjstr_l_allocassign_len(char *R *R s,
                        size_t *R sz,
                        size_t *R cap,
                        const char *R src,
                        const size_t src_len)
{
	*cap = JSTR_ALIGN_UP_STR(src_len + 1);
	*s = (char *)malloc(*cap);
	PJSTR_MALLOC_ERR(*s, goto err);
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return 1;
err:
	pjstr_nullify_members(sz, cap);
	return 0;
}

JSTR_FUNC_VOID
static void
jstr_l_popback(jstr_l_ty *R l)
{
	if (jstr_likely(l->size))
		free(jstr_l_at(l, --l->size)->data);
}

JSTR_FUNC_VOID
static void
jstr_l_popfront(jstr_l_ty *R l)
{
	if (jstr_likely(l->size)) {
		free(l->data->data);
		if (jstr_likely(l->size > 1))
			memmove(l->data, l->data + 1, (jstr_l_end(l) - (l->data + 1)) * sizeof(*l->data));
		--l->size;
	}
}

JSTR_FUNC
static int
jstr_l_pushfront_len_unsafe(jstr_l_ty *R l,
                            const char *R s,
                            const size_t s_len)
{
	if (jstr_likely(l->size))
		memmove(l->data + 1, l->data, (jstr_l_end(l) - (l->data)) * sizeof(*l->data));
	++l->size;
	if (jstr_unlikely(
	    !pjstr_l_allocassign_len(
	    &l->data->data,
	    &l->data->size,
	    &l->data->capacity,
	    s,
	    s_len)))
		goto err;
	return 1;
err:
	jstr_l_free(l);
	return 0;
}

JSTR_FUNC
static int
jstr_l_pushfront_len(jstr_l_ty *R l,
                     const char *R s,
                     const size_t s_len)
{
	PJSTR_L_RESERVE(l, l->size + 1, goto err)
	return jstr_l_pushfront_len_unsafe(l, s, s_len);
err:
	jstr_l_free(l);
	return 0;
}

JSTR_FUNC
static int
jstr_l_pushback_len_unsafe(jstr_l_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(
	    !pjstr_l_allocassign_len(
	    &jstr_l_at(l, l->size)->data,
	    &jstr_l_at(l, l->size)->size,
	    &jstr_l_at(l, l->size)->capacity,
	    s,
	    s_len)))
		goto err;
	++l->size;
	return 1;
err:
	jstr_l_free(l);
	return 0;
}

JSTR_FUNC
static int
jstr_l_pushback_len(jstr_l_ty *R l,
                    const char *R s,
                    const size_t s_len)
JSTR_NOEXCEPT
{
	PJSTR_L_RESERVE(l, l->size + 1, goto err)
	return jstr_l_pushback_len_unsafe(l, s, s_len);
err:
	jstr_l_free(l);
	return 0;
}

/* Last arg must be NULL. */
JSTR_SENTINEL
JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
static int
jstr_l_cat(jstr_l_ty *R l,
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
	PJSTR_L_RESERVE(l, l->size + argc, return 0)
	va_start(ap, l);
	for (jstr_ty *j = l->data + l->size; (arg = va_arg(ap, char *)); ++j, ++l->size)
		if (jstr_unlikely(
		    !pjstr_l_allocassign_len(&j->data, &j->size, &j->capacity, arg, strlen(arg))))
			goto err_free_l;
	va_end(ap);
	return 1;
err_free_l:
	jstr_l_free(l);
	va_end(ap);
	return 0;
}

JSTR_FUNC
static int
jstr_l_assign_len(jstr_l_ty *R l,
                  const size_t idx,
                  const char *R s,
                  const size_t s_len)
{
	return pjstr_l_allocassign_len(&(l->data + idx)->data, &(l->data + idx)->size, &(l->data + idx)->capacity, s, s_len);
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_find_len(const jstr_l_ty *R l,
                const char *R s,
                const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (!jstr_cmpeq(s, s_len, j->data, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findcase_len(const jstr_l_ty *R l,
                    const char *R s,
                    const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (!jstr_cmpcaseeq(s, s_len, j->data, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findstrchr(const jstr_l_ty *R l,
                  const int c)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (memchr(j->data, c, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findstrchrinv(const jstr_l_ty *R l,
                     const int c)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_strchrinv(j->data, c))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findstrstr_len(const jstr_l_ty *R l,
                      const char *R s,
                      const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_strstr_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_strcasestr_len(const jstr_l_ty *R l,
                      const char *R s,
                      const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_strcasestr_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findstarts_len(const jstr_l_ty *R l,
                      const char *R s,
                      const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_starts_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findstartscase_len(const jstr_l_ty *R l,
                          const char *R s,
                          const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_startscase_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findends_len(const jstr_l_ty *R l,
                    const char *R s,
                    const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_ends_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_findendscase_len(const jstr_l_ty *R l,
                        const char *R s,
                        const size_t s_len)
JSTR_NOEXCEPT
{
	jstr_l_foreach (l, j)
		if (jstr_endscase_len(j->data, j->size, s, s_len))
			return j;
	return NULL;
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_INLINE
static void
pjstr_l_delete(jstr_l_ty *l,
               jstr_ty *const p)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->size) && p) {
		free(p->data);
		if (jstr_likely(p != jstr_l_end(l)))
			memmove(p, p + 1, (jstr_l_end(l) - (p + 1)) * sizeof(jstr_l_ty));
		--l->size;
	}
}

JSTR_FUNC_VOID
static void
jstr_l_find_len_delete(jstr_l_ty *R l,
                       const char *R s,
                       const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_find_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findstrchr_delete(jstr_l_ty *R l,
                         const int c)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findstrchr(l, c));
}

JSTR_FUNC_VOID
static void
jstr_l_findstrchrinv_delete(jstr_l_ty *R l,
                            const int c)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findstrchrinv(l, c));
}

JSTR_FUNC_VOID
static void
jstr_l_findstrstr_len_delete(jstr_l_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findstrstr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_strcasestr_len_delete(jstr_l_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_strcasestr_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findstarts_len_delete(jstr_l_ty *R l,
                             const char *R s,
                             const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findstarts_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findends_len_delete(jstr_l_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findends_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findcase_len_delete(jstr_l_ty *R l,
                           const char *R s,
                           const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findcase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findstartscase_len_delete(jstr_l_ty *R l,
                                 const char *R s,
                                 const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findstartscase_len(l, s, s_len));
}

JSTR_FUNC_VOID
static void
jstr_l_findendscase_len_delete(jstr_l_ty *R l,
                               const char *R s,
                               const size_t s_len)
JSTR_NOEXCEPT
{
	pjstr_l_delete(l, jstr_l_findendscase_len(l, s, s_len));
}

#undef PJSTR_DELETE

PJSTR_END_DECLS

#undef R

#endif /* JSTR_LIST_H */
