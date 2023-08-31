#ifndef JARRAY_DEF_H
#define JARRAY_DEF_H

#include "_builder.h"
#include "_config.h"
#include "_macros.h"
#include <stdlib.h>

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

JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_init(const unsigned char **JSTR_RST const _p,
	   size_t *JSTR_RST const _size,
	   size_t *JSTR_RST const _cap)
{
	*_p = NULL;
	*_size = 0;
	*_cap = 0;
}

#define jarr_init(jarr, newcap) \
	pjarr_init((unsigned char **)&((jarr)->data), &((jarr)->size), &((jarr)->capacity))

JSTR_INLINE
static void
pjarr_free(unsigned char *JSTR_RST _p)
{
	free(_p);
	_p = NULL;
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_alloc(unsigned char **JSTR_RST const _p,
	    size_t *JSTR_RST const _sz,
	    size_t *JSTR_RST const _cap,
	    const size_t _sizeof_elem,
	    const size_t _newcap)
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(_newcap) / _sizeof_elem;
	*_p = (unsigned char *)malloc(*_cap * _sizeof_elem);
}

#define jarr_alloc(jarr, newcap) \
	pjarr_alloc((unsigned char **)&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), newcap)

#if 0

JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_alloc_append(unsigned char **JSTR_RST const _p,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const size_t _sizeof_elem,
		  const size_t _newcap)
{
	*_sz = 0;
	*_cap = PJSTR_MIN_ALLOC(_newcap) / _sizeof_elem;
	*_p = malloc(*_cap * _sizeof_elem);
}

#	define jarr_alloc_append(jarr, newcap) \
		pjarr_alloc_append((unsigned char **)&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), newcap)

#endif

/* Pop p[0]. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_pop_front(unsigned char *JSTR_RST const _p,
		size_t *JSTR_RST const _sz,
		const size_t _sizeof_elem)
{
	if (jstr_unlikely(*_sz == 0))
		return;
	memmove(_p,
		_p + 1 * _sizeof_elem,
		((*_sz)-- - 1) * _sizeof_elem);
}

#define jarr_pop_front(jarr) \
	pjarr_pop_front((unsigned char *)((jarr)->data), &((jarr)->size), sizeof(*((jarr)->data)))

/* Pop p[size]. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_pop_back(unsigned char *JSTR_RST const _p,
	       size_t *JSTR_RST const _sz,
	       const size_t _sizeof_elem)
{
	if (jstr_unlikely(*_sz == 0))
		return;
	*(_p + (*_sz)-- * _sizeof_elem - 1 * _sizeof_elem) = '\0';
}

#define jarr_pop_back(jarr) \
	pjarr_pop_back((unsigned char *)((jarr)->data), &((jarr)->size), sizeof(*((jarr)->data)))

/* Push VAL to back of P. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_push_back(unsigned char **JSTR_RST const _p,
		size_t *JSTR_RST const _sz,
		size_t *JSTR_RST const _cap,
		const size_t _sizeof_elem,
		const unsigned char *JSTR_RST const _val)
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJARR_REALLOC_EXACT(*(void **)_p, _sizeof_elem, *_cap, *_sz * JSTR_CFG_ALLOC_MULTIPLIER, return);
	memcpy(_p + (*_sz)++ * _sizeof_elem, _val, _sizeof_elem);
}

#define jarr_push_back(jarr, c) \
	pjarr_push_back((unsigned char **)&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), c)

/* Push VAL to front of P. */
JSTR_INLINE
JSTR_NONNULL_ALL
static void
pjarr_push_front(unsigned char **JSTR_RST const _p,
		 size_t *JSTR_RST const _sz,
		 size_t *JSTR_RST const _cap,
		 const size_t _sizeof_elem,
		 const unsigned char *JSTR_RST const _val)
{
	if (jstr_unlikely(*_cap == *_sz + 1))
		PJARR_REALLOC_EXACT(*(void **)_p, _sizeof_elem, *_cap, *_sz * JSTR_CFG_ALLOC_MULTIPLIER, return);
	memmove(*_p + 1 * _sizeof_elem, *_p, (*_sz)++ * _sizeof_elem);
	memcpy(_p, _val, _sizeof_elem);
}

#define jarr_push_front(jarr, c) \
	pjarr_push_front((unsigned char **)&((jarr)->data), &((jarr)->size), &((jarr)->capacity), sizeof(*((jarr)->data)), c)

#endif /* JARRAY_DEF_H */
