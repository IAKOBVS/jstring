#ifndef JSTR_MEMMEM_H_DEF
#define JSTR_MEMMEM_H_DEF

#ifdef ___cplusplus
extern "C" {
#endif /* __cpluslus */
#include <alloca.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef ___cplusplus
}
#endif /* __cpluslus */

#include "_jstr_macros.h"
#include "_jstr_string.h"

#ifdef ___cplusplus
extern "C" {
#endif /* __cpluslus */

typedef struct jstr_memmem_table {
	const unsigned char *ne;
	size_t nelen;
	uint8_t *small_table;
	size_t *big_table;
} jstr_memmem_table;

JSTR_INLINE
JSTR_WARN_UNUSED
static unsigned char
private_jstr_memmem_hash2(const unsigned char *JSTR_RST const p) JSTR_NOEXCEPT
{
	return (((size_t)(p)[0] - ((size_t)(p)[-1] << 3)) % 256);
}

JSTR_NONNULL_ALL
void
jstr_memmem_comp_mem(jstr_memmem_table *_ptable,
		     const char *JSTR_RST _ne,
		     const size_t _nelen);

#if defined(JSTR_HAVE_GENERIC)
#	define JSTR_ASSERT_IS_MEMMEM_TABLE(expr) \
		JSTR_ASSERT(_Generic((expr), jstr_memmem_table : 1), "Passing non-jstr_memmem_table as jstr_memmem_table argument!")
#else
#	define JSTR_ASSERT_IS_MEMMEM_TABLE(expr)
#endif /* JSTR_HAVE_GENERIC */

#define jstr_memmem_comp_mem(_jstr_memmem_table, _ne, _nelen)                                                                         \
	do {                                                                                                                          \
		JSTR_ASSERT_IS_STR(_ne);                                                                                              \
		JSTR_ASSERT_IS_SIZE(_nelen);                                                                                          \
		(_jstr_memmem_table)->ne = (const unsigned char *)_ne;                                                                \
		(_jstr_memmem_table)->nelen = _nelen;                                                                                 \
		if (jstr_unlikely(_nelen < 5)) {                                                                                           \
			(_jstr_memmem_table)->big_table = NULL;                                                                       \
			(_jstr_memmem_table)->small_table = NULL;                                                                     \
		} else if (jstr_unlikely(_nelen > 256)) {                                                                                  \
			(_jstr_memmem_table)->small_table = NULL;                                                                     \
			(_jstr_memmem_table)->big_table = (size_t *)alloca(256 * sizeof(size_t));                                     \
			memset((_jstr_memmem_table)->big_table, 0, 256 * sizeof(size_t));                                             \
			for (size_t i = 1; i < _nelen - 1; ++i)                                                                       \
				(_jstr_memmem_table)->big_table[private_jstr_memmem_hash2((unsigned char *)_ne + i)] = i;             \
			(_jstr_memmem_table)->big_table[private_jstr_memmem_hash2((unsigned char *)_ne + _nelen - 1)] = _nelen - 1;   \
		} else {                                                                                                              \
			(_jstr_memmem_table)->big_table = NULL;                                                                       \
			(_jstr_memmem_table)->small_table = (uint8_t *)alloca(256 * sizeof(uint8_t));                                 \
			memset((_jstr_memmem_table)->small_table, 0, 256 * sizeof(uint8_t));                                          \
			for (int i = 1; i < (int)_nelen - 1; ++i)                                                                     \
				(_jstr_memmem_table)->small_table[private_jstr_memmem_hash2((unsigned char *)_ne + i)] = i;           \
			(_jstr_memmem_table)->small_table[private_jstr_memmem_hash2((unsigned char *)_ne + _nelen - 1)] = _nelen - 1; \
		}                                                                                                                     \
	} while (0)

JSTR_NONNULL_ALL
void
jstr_memmem_comp(jstr_memmem_table *_ptable,
		 const char *JSTR_RST _ne);

#define jstr_memmem_comp(_ptable, _ne) \
	jstr_memmem_comp_mem(_ptable, _ne, strlen(_ne));

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_CONST
static void *
private_jstr_pre_memmem2(const unsigned char *JSTR_RST hs,
			 const unsigned char *JSTR_RST const ne,
			 const size_t hslen) JSTR_NOEXCEPT
{
	const unsigned char *const end = hs + hslen;
	const uint32_t nw = ne[0] << 8 | ne[1];
	uint32_t hw = hs[0] << 8 | hs[1];
	for (hs += 2; hw != nw && hs <= end; hw = (hw | *hs++) << 8)
		;
	return (hw == nw) ? (void *)(hs - 2) : NULL;
}

JSTR_CONST
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static void *
private_jstr_pre_memmem3(const unsigned char *JSTR_RST hs,
			 const unsigned char *JSTR_RST const ne,
			 const size_t hslen) JSTR_NOEXCEPT
{
	const unsigned char *const end = hs + hslen;
	const uint32_t nw = ne[0] << 24 | ne[1] << 16 | ne[2] << 8;
	uint32_t hw = hs[0] << 24 | hs[1] << 16 | hs[2] << 8;
	for (hs += 3; hw != nw && hs <= end; hw = (hw | *hs++) << 8)
		;
	return (hw == nw) ? (void *)(hs - 3) : NULL;
}

JSTR_CONST
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static void *
private_jstr_pre_memmem4(const unsigned char *JSTR_RST hs,
			 const unsigned char *JSTR_RST const ne,
			 const size_t hslen) JSTR_NOEXCEPT
{
	const unsigned char *const end = hs + hslen;
	const uint32_t nw = ne[0] << 24 | ne[1] << 16 | ne[2] << 8 | ne[3];
	uint32_t hw = hs[0] << 24 | hs[1] << 16 | hs[2] << 8 | hs[3];
	for (; hw != nw && hs <= end; hw = (hw | *hs++) << 8)
		;
	return (hw == nw) ? (void *)(hs - 4) : NULL;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
JSTR_INLINE
static void *
jstr_memmem_exec_constexpr(const jstr_memmem_table *JSTR_RST const ptable,
			   const char *JSTR_RST const hs,
			   const size_t hslen) JSTR_NOEXCEPT
{
#define PRIVATE_JSTR_MEMMEM_EXEC(_table)                                                      \
	do {                                                                                  \
		const unsigned char *h = (unsigned char *)hs;                                 \
		const unsigned char *const end = h + hslen - ptable->nelen;                   \
		const size_t m1 = ptable->nelen - 1;                                          \
		const size_t mtc1 = ptable->nelen - 1;                                        \
		const size_t shft1 = m1 - _table[private_jstr_memmem_hash2(ptable->ne + m1)]; \
		size_t off = 0;                                                               \
		size_t tmp;                                                                   \
		do {                                                                          \
			do {                                                                  \
				h += mtc1;                                                    \
				tmp = _table[private_jstr_memmem_hash2(h)];                   \
			} while (!tmp && h <= end);                                           \
			h -= tmp;                                                             \
			if (tmp < mtc1)                                                       \
				continue;                                                     \
			if (mtc1 < 15 || !memcmp(h + off, ptable->ne + off, 8)) {             \
				if (!memcmp(h, ptable->ne, ptable->nelen))                    \
					return (void *)h;                                     \
				off = (off >= 8 ? off : mtc1) - 8;                            \
			}                                                                     \
			h += shft1;                                                           \
		} while (h <= end);                                                           \
	} while (0)
	if (jstr_unlikely(hslen < ptable->nelen))
		return NULL;
	switch (ptable->nelen) {
	case 0: return (void *)hs;
	case 1: return (void *)memchr(hs, *ptable->ne, hslen);
	case 2: return private_jstr_pre_memmem2((unsigned char *)hs, (unsigned char *)ptable->ne, hslen);
	case 3: return private_jstr_pre_memmem3((unsigned char *)hs, (unsigned char *)ptable->ne, hslen);
	case 4: return private_jstr_pre_memmem4((unsigned char *)hs, (unsigned char *)ptable->ne, hslen);
	}
	if (jstr_unlikely(ptable->nelen > 256))
		PRIVATE_JSTR_MEMMEM_EXEC(ptable->big_table);
	else
		PRIVATE_JSTR_MEMMEM_EXEC(ptable->small_table);
	return NULL;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
static void *
jstr_memmem_exec(const jstr_memmem_table *JSTR_RST const ptable,
		 const char *JSTR_RST const hs,
		 const size_t hslen) JSTR_NOEXCEPT
{
	return jstr_memmem_exec_constexpr(ptable, hs, hslen);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_CONST
static char *
jstr_strstr_exec(const jstr_memmem_table *JSTR_RST const ptable,
		 const char *JSTR_RST const hs) JSTR_NOEXCEPT
{
	return (char *)jstr_memmem_exec(ptable, hs, strlen(hs));
}

#ifdef ___cplusplus
}
#endif /* __cpluslus */

#endif /* JSTR_MEMMEM_H_DEF */
