/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2024 James Tirta Halim <tirtajames45 at gmail dot com>
 * This file is part of the jstring library.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef JSTR_LIST_H
#define JSTR_LIST_H 1

#include "jstr-macros.h"

#include "jstr-builder.h"
#include "jstr-config.h"
#include "jstr-replace.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

#define JSTRL_INIT                                                             \
	{                                                                      \
		0                                                              \
	}

#define jstrl_foreach(l, p)                                                    \
	for (jstr_ty *p = ((l)->data),                                         \
	             *const _pjstrl_foreach_end_##p = jstrl_end(l);            \
	     p < _pjstrl_foreach_end_##p;                                      \
	     ++p)
#define pjstrl_foreach_cap(l, p)                                               \
	for (jstr_ty *p = ((l)->data),                                         \
	             *const _pjstrl_foreach_end_##p                            \
	             = ((l)->data) + ((l)->capacity);                          \
	     p < _pjstrl_foreach_end_##p;                                      \
	     ++p)
#define jstrl_foreachi(l, i)                                                   \
	for (size_t i = 0, const _pjstrl_foreachi_end_##i = ((l)->size);       \
	     i < _pjstrl_foreachi_end_##i;                                     \
	     ++i)
#define jstrl_index(l, curr) JSTR_PTR_DIFF(curr, (l)->data)
#define pjstrl_at(l, i)      ((l)->data + (i))
#define jstrl_start(l)       ((l)->data)
#define jstrl_end(l)         ((l)->data + (l)->size)
#ifdef JSTR_DEBUG
#	define jstrl_at(l, i)                                                 \
		((i <= (l)->size) ? ((l)->data + (i))                          \
		                  : assert(i <= (l)->size),                    \
		 ((l)->data))
#else
#	define jstrl_at(l, i) ((l)->data + (i))
#endif

JSTR__BEGIN_DECLS

typedef struct jstrlist_ty {
	jstr_ty *data;
	size_t size;
	size_t capacity;
} jstrlist_ty;

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjstrl_elemstore(jstr_ty *R dst, const jstr_ty *R src)
{
	dst->data = src->data;
	dst->capacity = src->capacity;
	dst->size = 0;
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjstrl_elemmove(jstrlist_ty *R l, const jstr_ty *R elem)
{
	jstr_ty *curr = l->data + l->size;
	for (size_t n = l->capacity; n-- && curr->data; ++curr) {}
	*curr = *elem;
}

JSTR_FUNC_VOID
static void
jstrl_free(jstrlist_ty *R l) JSTR_NOEXCEPT
{
	if (jstr_likely(l->data != NULL)) {
#if JSTRL_LAZY_FREE
		pjstrl_foreach_cap (l, p)
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
JSTR_ATTR_COLD
JSTR_ATTR_NOINLINE
static void
jstrl_free_noinline(jstrlist_ty *R l) JSTR_NOEXCEPT
{
	jstrl_free(l);
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrl_debug(const jstrlist_ty *R l)
{
	int ret;
	ret = fprintf(stderr,
	              "size:%zu\n"
	              "cap:%zu\n"
	              "\n",
	              l->size,
	              l->capacity);
	if (jstr_unlikely(ret < 0))
		goto err_set_errno;
	jstrl_foreach (l, p) {
		ret = fprintf(stderr,
		              "idx:%zu\n"
		              "size:%zu\n"
		              "cap:%zu\n",
		              p - l->data,
		              p->size,
		              p->capacity);
		if (jstr_unlikely(ret < 0))
			goto err_set_errno;
		ret = fprintf(stderr,
		              "data:%s\n"
		              "\n",
		              p->data);
		if (jstr_unlikely(ret < 0))
			goto err_set_errno;
	}
	return JSTR_RET_SUCC;
err_set_errno:
	errno = ret;
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC_CONST
JSTR_ATTR_INLINE
static size_t
pjstrl_grow(size_t cap, size_t new_cap) JSTR_NOEXCEPT
{
	if (jstr_unlikely(cap == 0))
		cap = JSTR_MIN_CAP / JSTR_ALLOC_MULTIPLIER;
	while ((cap = (size_t)(cap * JSTRL_GROWTH)) < new_cap) {}
	return cap;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrl_reservealways(jstrlist_ty *R l, size_t new_cap) JSTR_NOEXCEPT
{
	new_cap = pjstrl_grow(l->capacity, new_cap);
	jstr_ty *tmp = (jstr_ty *)realloc(l->data, new_cap * sizeof(*l->data));
	if (jstr_nullchk(tmp))
		goto err;
	l->data = tmp;
#if JSTRL_LAZY_FREE
	memset(
	l->data + l->capacity, 0, (new_cap - l->capacity) * sizeof(*l->data));
#endif
	l->capacity = new_cap;
	return JSTR_RET_SUCC;
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
jstrl_reserve(jstrlist_ty *R l, size_t new_cap) JSTR_NOEXCEPT
{
	if (new_cap > l->capacity)
		if (jstr_chk(jstrl_reservealways(l, new_cap)))
			return JSTR_RET_ERR;
	return JSTR_RET_SUCC;
}

JSTR_FUNC
JSTR_ATTR_INLINE
static jstr_ret_ty
pjstrl_assign_len(
char *R *R s, size_t *R sz, size_t *R cap, const char *R src, size_t src_len)
{
#if JSTRL_LAZY_FREE
	return jstr_assign_len(s, sz, cap, src, src_len);
#else
	*cap = JSTR_ALIGN_UP_STR(src_len + 1);
	*s = (char *)malloc(*cap);
	if (jstr_nullchk(*s))
		goto err;
	jstr_strcpy_len(*s, src, src_len);
	*sz = src_len;
	return JSTR_RET_SUCC;
err:
	jstr_free_noinline(s, sz, cap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
#endif
}

JSTR_FUNC_VOID
static void
jstrl_popback(jstrlist_ty *R l)
{
	if (jstr_likely(l->size != 0)) {
#if JSTRL_LAZY_FREE
		pjstrl_at(l, --l->size)->size = 0;
#else
		free(pjstrl_at(l, --l->size)->data);
#endif
	}
}

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
pjstrl_memmove(jstr_ty *const dst, jstr_ty *const src, size_t n)
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
	if (jstr_likely(l->size != 0)) {
#if JSTRL_LAZY_FREE
		jstr_ty tmp;
		pjstrl_elemstore(&tmp, l->data);
#else
		free(l->data->data);
#endif
		if (jstr_likely(l->size > 1))
			pjstrl_memmove(
			l->data, l->data + 1, (l->size - 1) * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		pjstrl_elemmove(l, &tmp);
#endif
	}
}

JSTR_ATTR_NO_SANITIZE_ADDRESS
JSTR_FUNC
static jstr_ret_ty
jstrl_pushfront_len_unsafe(jstrlist_ty *R l, const char *R s, size_t s_len)
{
	jstr_ty *const lp = l->data;
	if (jstr_likely(l->size != 0))
		pjstrl_memmove(lp + 1, lp, l->size * sizeof(*lp));
	++l->size;
	if (jstr_chk(
	    pjstrl_assign_len(&lp->data, &lp->size, &lp->capacity, s, s_len)))
		goto err;
	return JSTR_RET_SUCC;
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrl_pushfront_len(jstrlist_ty *R l, const char *R s, size_t s_len)
{
	if (jstr_chk(jstrl_reserve(l, l->size + 1)))
		goto err;
	return jstrl_pushfront_len_unsafe(l, s, s_len);
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrl_pushback_len_unsafe(jstrlist_ty *R l,
                          const char *R s,
                          size_t s_len) JSTR_NOEXCEPT
{
	jstr_ty *const p = l->data + l->size;
	if (jstr_chk(
	    pjstrl_assign_len(&p->data, &p->size, &p->capacity, s, s_len)))
		goto err;
	++l->size;
	return JSTR_RET_SUCC;
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrl_pushback_len(jstrlist_ty *R l,
                   const char *R s,
                   size_t s_len) JSTR_NOEXCEPT
{
	if (jstr_chk(jstrl_reserve(l, l->size + 1)))
		goto err;
	return jstrl_pushback_len_unsafe(l, s, s_len);
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

/* Last arg must be NULL. */
JSTR_ATTR_SENTINEL
JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL((1))
static jstr_ret_ty
jstrl_cat(jstrlist_ty *R l, ...) JSTR_NOEXCEPT
{
	va_list ap;
	va_start(ap, l);
	int argc = 0;
	for (; va_arg(ap, void *); ++argc) {}
	va_end(ap);
	if (jstr_unlikely(argc == 0))
		return JSTR_RET_SUCC;
	if (jstr_chk(jstrl_reserve(l, l->size + (size_t)argc)))
		return JSTR_RET_ERR;
	va_start(ap, l);
	const char *R arg;
	jstr_ty *j = l->data + l->size;
	for (; (arg = va_arg(ap, const char *)); ++j, ++l->size)
		if (jstr_chk(pjstrl_assign_len(
		    &j->data, &j->size, &j->capacity, arg, strlen(arg))))
			goto err_free_l;
	va_end(ap);
	return JSTR_RET_SUCC;
err_free_l:
	jstrl_free_noinline(l);
	va_end(ap);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR_FUNC
static jstr_ret_ty
jstrl_assign_len(jstrlist_ty *R l, size_t idx, const char *R s, size_t s_len)
{
	jstr_ty *const p = l->data + idx;
	if (jstr_likely(
	    pjstrl_assign_len(&p->data, &p->size, &p->capacity, s, s_len)
	    == JSTR_RET_SUCC))
		return JSTR_RET_SUCC;
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

#define PJSTRL_DEFINE_FIND_LEN(name, func)                                     \
	JSTR_FUNC_PURE                                                         \
	static jstr_ty *jstrl_##name(                                          \
	const jstrlist_ty *R l, const char *R s, size_t s_len) JSTR_NOEXCEPT   \
	{                                                                      \
		jstrl_foreach (l, j)                                           \
			if (func(s, s_len, j->data, j->size))                  \
				return j;                                      \
		return NULL;                                                   \
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

#define PJSTRL_DEFINE_FIND(name, func)                                         \
	JSTR_FUNC_PURE                                                         \
	static jstr_ty *jstrl_##name(const jstrlist_ty *R l, const char *R s)  \
	JSTR_NOEXCEPT                                                          \
	{                                                                      \
		const size_t s_len = strlen(s);                                \
		jstrl_foreach (l, j)                                           \
			if (func(s, s_len, j->data, j->size))                  \
				return j;                                      \
		return NULL;                                                   \
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
jstrl_findstrchr(const jstrlist_ty *R l, int c) JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (memchr(j->data, c, j->size))
			return j;
	return NULL;
}

JSTR_FUNC_PURE
static jstr_ty *
jstrl_findstrcasechr(const jstrlist_ty *R l, int c) JSTR_NOEXCEPT
{
	jstrl_foreach (l, j)
		if (jstr_strcasechr(j->data, c))
			return j;
	return NULL;
}

JSTR_FUNC_VOID_MAY_NULL
JSTR_ATTR_INLINE
static void
pjstrl_delete(jstrlist_ty *R l, jstr_ty *R p) JSTR_NOEXCEPT
{
	if (p) {
#if JSTRL_LAZY_FREE
		jstr_ty tmp;
		pjstrl_elemstore(&tmp, p);
#else
		free(p->data);
#endif
		if (jstr_likely(p != jstrl_end(l)))
			pjstrl_memmove(p,
			               p + 1,
			               JSTR_PTR_DIFF(jstrl_end(l), p + 1)
			               * sizeof(*l->data));
		--l->size;
#if JSTRL_LAZY_FREE
		pjstrl_elemmove(l, &tmp);
#endif
	}
}

#define PJSTRL_DEFINE_DELETE_LEN(name, func)                                   \
	JSTR_FUNC_VOID                                                         \
	static void jstrl_##name(                                              \
	jstrlist_ty *R l, const char *R s, size_t s_len) JSTR_NOEXCEPT         \
	{                                                                      \
		pjstrl_delete(l, func);                                        \
	}

PJSTRL_DEFINE_DELETE_LEN(delete_len, jstrl_find_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletecase_len, jstrl_findcase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestrstr_len, jstrl_findstrstr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestrcasestr_len,
                         jstrl_findstrcasestr_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestarts_len, jstrl_findstarts_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deletestartscase_len,
                         jstrl_findstartscase_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deleteends_len, jstrl_findends_len(l, s, s_len))
PJSTRL_DEFINE_DELETE_LEN(deleteendscase_len,
                         jstrl_findendscase_len(l, s, s_len))

#undef PJSTRL_DEFINE_DELETE_LEN

#define PJSTRL_DEFINE_DELETE(name, func)                                       \
	JSTR_FUNC_VOID                                                         \
	static void jstrl_##name(jstrlist_ty *R l, const char *R s)            \
	JSTR_NOEXCEPT                                                          \
	{                                                                      \
		const size_t s_len = strlen(s);                                \
		pjstrl_delete(l, func);                                        \
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
jstrl_deletestrchr(jstrlist_ty *R l, int c) JSTR_NOEXCEPT
{
	pjstrl_delete(l, jstrl_findstrchr(l, c));
}

JSTR_FUNC_VOID
static void
jstrl_deletestrcasechr(jstrlist_ty *R l, int c) JSTR_NOEXCEPT
{
	pjstrl_delete(l, jstrl_findstrcasechr(l, c));
}

/* Split SRC into a string list with SPLIT as delimiter. */
JSTR_FUNC
static jstr_ret_ty
jstrl_split_len(jstrlist_ty *R l,
                const char *src,
                size_t src_len,
                const char *split,
                size_t split_len)
{
	const char *save = src;
	const char *end = src + src_len;
	const char *tok;
	while ((tok = jstr_strtok_ne_len(&save, end, split, split_len)))
		if (jstr_chk(jstrl_pushback_len(
		    l, tok, JSTR_PTR_DIFF(save, tok - split_len))))
			goto err;
	return JSTR_RET_SUCC;
err:
	jstrl_free_noinline(l);
	JSTR_RETURN_ERR(JSTR_RET_ERR);
}

JSTR__END_DECLS

#undef R

#endif /* JSTR_LIST_H */
