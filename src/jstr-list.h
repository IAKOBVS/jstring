#ifndef JSTR_LIST_H
#define JSTR_LIST_H

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-macros.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

typedef struct jstr_l_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstr_l_ty;

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstr_l_start(jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	return l->data;
}

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstr_l_end(jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	return l->data + l->size;
}

JSTR_CONST
JSTR_INLINE
static jstr_ty *
jstr_l_at(jstr_l_ty *R l,
	  const size_t at)
JSTR_NOEXCEPT
{
	return l->data + at;
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
jstr_l_reserve_always(jstr_l_ty *R l,
		      size_t new_cap)
JSTR_NOEXCEPT
{
	new_cap = pjstr_l_grow(l->capacity, new_cap);
	l->data = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	if (jstr_unlikely(l->data == NULL))
		goto err;
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
	return (new_cap > l->capacity) ? jstr_l_reserve_always(l, new_cap) : 1;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_l_init(jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	l->data = NULL;
	l->size = 0;
	l->capacity = 0;
}

JSTR_FUNC
static int
jstr_l_add_len(jstr_l_ty *R l,
	       const char *R s,
	       const size_t slen)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(!jstr_l_reserve(l, l->capacity + slen)))
		goto err;
	if (jstr_unlikely(!jstr_append_len(&l->data[l->size].data,
					   &l->data[l->size].size,
					   &l->data[l->size].capacity,
					   s,
					   slen)))
		goto err_str;
	++l->size;
	return 1;
err_str:
	pjstr_nullify_members(&l->data[l->size].size, &l->data[l->size].capacity);
err:
	pjstr_nullify_members(&l->size, &l->capacity);
	return 0;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_find_len(const jstr_l_ty *R l,
		const char *R s,
		const size_t slen)
JSTR_NOEXCEPT
{
	for (size_t i = 0; i < l->size; ++i)
		if (slen == l->data[i].size
		    && *s == *(l->data[i].data)
		    && !memcmp(s, l->data[i].data, slen))
			return l->data + i;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_starts_len(const jstr_l_ty *R l,
		  const char *R s,
		  const size_t slen)
JSTR_NOEXCEPT
{
	for (size_t i = 0; i < l->size; ++i)
		if (jstr_starts_len(l->data[i].data, l->data[i].size, s, slen))
			return l->data + i;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstr_l_ends_len(const jstr_l_ty *R l,
		const char *R s,
		const size_t slen)
JSTR_NOEXCEPT
{
	for (size_t i = 0; i < l->size; ++i)
		if (jstr_ends_len(l->data[i].data, l->data[i].size, s, slen))
			return l->data + i;
	return NULL;
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_INLINE
static void
pjstr_l_delete(jstr_l_ty *R l,
	       jstr_ty *R p)
JSTR_NOEXCEPT
{
	if (p) {
		free(p->data);
		if (l->size > 1)
			memmove(p, p + 1, jstr_l_end(l) - p + 1);
		--l->size;
	}
}

JSTR_FUNC_VOID
static void
jstr_l_delete_len(jstr_l_ty *R l,
		  const char *R s,
		  const size_t slen)
JSTR_NOEXCEPT
{
	jstr_ty *p = jstr_l_find_len(l, s, slen);
	pjstr_l_delete(l, p);
}

JSTR_FUNC_VOID
static void
jstr_l_delete_starts_len(jstr_l_ty *R l,
			 const char *R s,
			 const size_t slen)
JSTR_NOEXCEPT
{
	jstr_ty *p = jstr_l_starts_len(l, s, slen);
	pjstr_l_delete(l, p);
}

JSTR_FUNC_VOID
static void
jstr_l_delete_ends_len(jstr_l_ty *R l,
		       const char *R s,
		       const size_t slen)
JSTR_NOEXCEPT
{
	jstr_ty *p = jstr_l_ends_len(l, s, slen);
	pjstr_l_delete(l, p);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_l_free(jstr_l_ty *R l)
JSTR_NOEXCEPT
{
	if (jstr_likely(l->data != NULL)) {
		for (size_t i = 0; i < l->size; ++i)
			free(l->data[i].data);
		free(l->data);
		l->data = NULL;
		l->size = 0;
		l->capacity = 0;
	}
}

PJSTR_END_DECLS

#undef JSTR_RESTRICT

#endif /* JSTR_LIST_H */
