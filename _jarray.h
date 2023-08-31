#ifndef JARR_DEF_H
#define JARR_DEF_H

#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <stdlib.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "_builder.h"
#include "_config.h"
#include "_macros.h"
#include "_pp_arrcpy_va_args.h"
#include "_templates.h"

#define PJARR_REALLOC(p, sizeof_elem, old_cap, new_cap, malloc_fail) \
	do {                                                         \
		JSTR_ASSERT_IS_SIZE(old_cap);                        \
		JSTR_ASSERT_IS_SIZE(new_cap);                        \
		PJSTR_GROW(old_cap, new_cap);                        \
		(p) = (char *)realloc(p, old_cap * sizeof_elem);     \
		PJSTR_MALLOC_ERR(p, malloc_fail);                    \
	} while (0)

#define PJARR_REALLOC_EXACT(p, sizeof_elem, old_cap, new_cap, malloc_fail) \
	do {                                                               \
		JSTR_ASSERT_IS_SIZE(old_cap);                              \
		JSTR_ASSERT_IS_SIZE(new_cap);                              \
		old_cap = PJSTR_ALIGN_UP(new_cap, PJSTR_MALLOC_ALIGNMENT); \
		(p) = (char *)realloc(p, old_cap * sizeof_elem);           \
		PJSTR_MALLOC_ERR(p, malloc_fail);                          \
	} while (0)

#define jarr_define(T, typename)          \
	typedef struct jarr_ty_##typename \
	{                                 \
		T *data;                  \
		size_t size;              \
		size_t capacity;          \
	}                                 \
	jarr_##typename##_ty;

#ifdef __cplusplus

template <typename T, typename Other>
static T
pjarr_auto_cast(T, Other other)
{
	return (T)other;
}

#else

#	define pjarr_auto_cast(T, other) other

#endif

#ifdef __cplusplus

extern "C" {
#endif /* __cplusplus */

JSTR_INLINE
static void
pjarr_free(void *JSTR_RST _p)
{
	free(_p);
	_p = NULL;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_alloc(void **JSTR_RST const _p,
	    size_t *JSTR_RST const _sz,
	    size_t *JSTR_RST const _cap,
	    const size_t _sizeof_elem,
	    const size_t _newcap)
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(_newcap * _sizeof_elem);
	*_p = malloc(*_cap);
	PJSTR_MALLOC_ERR(*_p, return);
	*_cap /= _sizeof_elem;
}

#define jarr_alloc(jarr, newcap) \
	pjarr_alloc(&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), newcap)

#define jarr_alloc_cat(jarr, ...)                                                              \
	do {                                                                                   \
		(jarr)->capacity = PJSTR_MIN_ALLOC(PJSTR_PP_NARG(__VA_ARGS__) * _sizeof_elem); \
		(jarr)->data = pjarr_auto_cast((jarr)->data, malloc((jarr)->capacity));        \
		PJSTR_MALLOC_ERR((jarr)->data, return);                                        \
		(jarr)->capacity /= _sizeof_elem;                                              \
		(jarr)->size = PJSTR_PP_NARG(__VA_ARGS__);                                     \
		PJSTR_PP_ARRCPY_VA_ARGS((jarr)->data, __VA_ARGS__);                            \
	} while (0)

/* Pop p[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_pop_front(void *JSTR_RST const _p,
		size_t *JSTR_RST const _sz,
		const size_t _sizeof_elem)
{
	if (jstr_unlikely(*_sz == 0))
		return;
	typedef unsigned char uc;
	memmove(_p,
		(uc *)_p + 1 * _sizeof_elem,
		((*_sz)-- - 1) * _sizeof_elem);
}

#define jarr_pop_front(jarr) \
	pjarr_pop_front(((jarr)->data), &((jarr)->size), sizeof(*((jarr)->data)))

/* Pop p[size]. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_pop_back(void *JSTR_RST const _p,
	       size_t *JSTR_RST const _sz,
	       const size_t _sizeof_elem)
{
	if (jstr_unlikely(*_sz == 0))
		return;
	typedef unsigned char uc;
	*((uc *)_p + (*_sz)-- * _sizeof_elem - 1 * _sizeof_elem) = '\0';
}

#define jarr_pop_back(jarr) \
	pjarr_pop_back(((jarr)->data), &((jarr)->size), sizeof(*((jarr)->data)))

/* Push VAL to back of P. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_push_back(void **JSTR_RST const _p,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _sizeof_elem,
		const void *JSTR_RST const _val)
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJARR_REALLOC_EXACT(*(void **)_p, _sizeof_elem, *_cap, *_sz * JSTR_CFG_ALLOC_MULTIPLIER, return);
	typedef unsigned char uc;
	memcpy(*(uc **)_p + (*_sz)++ * _sizeof_elem, _val, _sizeof_elem);
}

#define jarr_push_back(jarr, c) \
	pjarr_push_back(&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), c)

/* Push VAL to front of P. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_push_front(void **JSTR_RST const _p,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const size_t _sizeof_elem,
		 const void *JSTR_RST const _val)
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJARR_REALLOC_EXACT(*(void **)_p, _sizeof_elem, *_cap, *_sz * JSTR_CFG_ALLOC_MULTIPLIER, return);
	typedef unsigned char uc;
	memmove(*(uc **)_p + 1 * _sizeof_elem, *_p, (*_sz)++ * _sizeof_elem);
	memcpy(*_p, _val, _sizeof_elem);
}

#define jarr_push_front(jarr, c) \
	pjarr_push_front(&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), c)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JARR_DEF_H */
