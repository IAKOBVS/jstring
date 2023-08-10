#ifndef JSTR_MEMMEM_H_DEF
#define JSTR_MEMMEM_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_string.h"

#define JSTR_HASH2(p) (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256)

#ifdef __cplusplus
namespace jstr {
#endif /* __cpluslus */

typedef struct jstr_memmem_table {
	size_t *big_tbl;
	size_t shft1;
	size_t nelen;
	const char *ne;
	uint8_t smal_tbl[256];
} jstr_memmem_table;

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
static int jstr_memmem_comp_mem(jstr_memmem_table *JSTR_RST const ptable,
				const char *JSTR_RST const ne,
				const size_t nelen)
{
	ptable->ne = ne;
	ptable->nelen = nelen;
	if (unlikely(nelen < 5)) {
		ptable->big_tbl = NULL;
	} else if (unlikely(nelen > 256)) {
		ptable->big_tbl = (size_t *)calloc(256, sizeof(size_t));
		if (unlikely(!ptable->big_tbl))
			return 1;
		for (size_t i = 1; i < nelen - 1; ++i)
			ptable->big_tbl[JSTR_HASH2(ne + i)] = i;
		ptable->shft1 = nelen - 1 - ptable->big_tbl[JSTR_HASH2(ne + nelen - 1)];
		ptable->smal_tbl[JSTR_HASH2(ne + nelen - 1)] = nelen - 1;
	} else {
		ptable->big_tbl = NULL;
		memset(ptable->smal_tbl, 0, 256 * sizeof(uint8_t));
		for (int i = 1; i < (int)nelen - 1; ++i)
			ptable->smal_tbl[JSTR_HASH2(ne + i)] = i;
		ptable->shft1 = nelen - 1 - ptable->smal_tbl[JSTR_HASH2(ne + nelen - 1)];
		ptable->smal_tbl[JSTR_HASH2(ne + nelen - 1)] = nelen - 1;
	}
	return 0;
}

JSTR_INLINE
JSTR_WARN_UNUSED
static int jstr_memmem_comp(jstr_memmem_table *ptable, const char *JSTR_RST const ne)
{
	return jstr_memmem_comp_mem(ptable, ne, strlen(ne));
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void jstr_memmem_free(jstr_memmem_table *JSTR_RST const ptable)
{
	free(ptable->big_tbl);
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
static void *jstr_memmem_exec_constexpr(const jstr_memmem_table *JSTR_RST const ptable,
					const char *JSTR_RST const hs,
					const size_t hslen) JSTR_NOEXCEPT
{
#define PRIVATE_JSTR_MEMMEM_EXEC(shift_type, ne_iterator_type)                    \
	do {                                                                      \
		const size_t mtc1 = ptable->nelen - 1;                            \
		const unsigned char *h = (unsigned char *)hs;                     \
		const unsigned char *const end = h + hslen - ptable->nelen;       \
		size_t off = 0;                                                   \
		size_t tmp;                                                       \
		do {                                                              \
			do {                                                      \
				h += mtc1;                                        \
				tmp = shift[JSTR_HASH2(h)];                       \
			} while (!tmp && h <= end);                               \
			h -= tmp;                                                 \
			if (tmp < mtc1)                                           \
				continue;                                         \
			if (mtc1 < 15 || !memcmp(h + off, ptable->ne + off, 8)) { \
				if (!memcmp(h, ptable->ne, ptable->nelen))        \
					return (void *)h;                         \
				off = (off >= 8 ? off : mtc1) - 8;                \
			}                                                         \
			h += ptable->shft1;                                       \
		} while (h <= end);                                               \
	} while (0)
	if (unlikely(hslen < ptable->nelen))
		return NULL;
	switch (ptable->nelen) {
	case 0: return (void *)hs;
	case 1: return (void *)memchr(hs, *ptable->ne, hslen);
	case 2: {
		const unsigned char *h = (unsigned char *)hs + 2;
		const unsigned char *const end = h + hslen;
		const uint32_t nw = ptable->ne[0] << 8 | ptable->ne[1];
		uint32_t hw = hs[0] << 8 | hs[1];
		for (; hw != nw && h <= end; hw = (hw | *h++) << 8)
			;
		return (hw == nw) ? (void *)(h - 2) : NULL;
	}
	case 3: {
		const unsigned char *h = (unsigned char *)hs + 3;
		const unsigned char *const end = h + hslen;
		const uint32_t nw = ptable->ne[0] << 24 | ptable->ne[1] << 16 | ptable->ne[2] << 8;
		uint32_t hw = hs[0] << 24 | hs[1] << 16 | hs[2] << 8;
		for (; hw != nw && h <= end; hw = (hw | *h++) << 8)
			;
		return (hw == nw) ? (void *)(h - 3) : NULL;
	}
	case 4: {
		const unsigned char *h = (unsigned char *)hs + 4;
		const unsigned char *const end = h + hslen;
		const uint32_t nw = ptable->ne[0] << 24 | ptable->ne[1] << 16 | ptable->ne[2] << 8 | ptable->ne[3];
		uint32_t hw = hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
		for (; hw != nw && h <= end; hw = (hw | *h++) << 8)
			;
		return (hw == nw) ? (void *)(h - 4) : NULL;
	}
	}
	if (unlikely(ptable->nelen > 256)) {
		const size_t *const shift = (size_t *)ptable->big_tbl;
		PRIVATE_JSTR_MEMMEM_EXEC(size_t, size_t);
	} else {
		const uint8_t *const shift = ptable->smal_tbl;
		PRIVATE_JSTR_MEMMEM_EXEC(uint8_t, int);
	}
	return NULL;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
static void *jstr_memmem_exec(const jstr_memmem_table *JSTR_RST const ptable,
			      const char *JSTR_RST const hs,
			      const size_t hslen) JSTR_NOEXCEPT
{
	return jstr_memmem_exec_constexpr(ptable, hs, hslen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
static char *jstr_strstr_exec(const jstr_memmem_table *JSTR_RST const ptable,
			      const char *JSTR_RST const hs) JSTR_NOEXCEPT
{
	return (char *)jstr_memmem_exec(ptable, hs, strlen(hs));
}

#ifdef __cplusplus
}
#endif /* __cpluslus */

#endif /* JSTR_MEMMEM_H_DEF */
