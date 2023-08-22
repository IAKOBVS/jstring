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

#include "_config.h"
#include "_macros.h"
#include "_string.h"

#if JSTR_CFG_HAVE_ALLOCA && (defined __GNUC__ || defined __clang__)
#	define JSTR_USE_ALLOCA 1
#	include <alloca.h>
#else
#	define JSTR_USE_ALLOCA 0
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

typedef struct jstr_memmem_table {
	const char *ne;
	size_t nelen;
#if JSTR_USE_ALLOCA
	uint8_t *small_table;
	size_t *big_table;
#else
	uint8_t small_table[256];
	size_t big_table[256];
#endif /* USE_ALLOCA */
} jstr_memmem_table;

typedef jstr_memmem_table jstr_strstr_table;

JSTR_INLINE
JSTR_WARN_UNUSED
static unsigned char
priv_memmem_hash2(const unsigned char *JSTR_RST const _p) JSTR_NOEXCEPT
{
	return (((size_t)(_p)[0] - ((size_t)(_p)[-1] << 3)) % 256);
}

#if JSTR_USE_ALLOCA

JSTR_INLINE
static void
jstr_memmem_init(jstr_memmem_table *JSTR_RST const _ptable) JSTR_NOEXCEPT
{
	_ptable->small_table = NULL;
	_ptable->big_table = NULL;
}

#else

void
jstr_memmem_init(jstr_memmem_table *JSTR_RST const _ptable) JSTR_NOEXCEPT;

#	define jstr_memmem_init(_jstr_memmem_table)

#endif

#ifdef JSTR_HAVE_GENERIC
#	define JSTR_ASSERT_IS_MEMMEM_TABLE(expr) \
		JSTR_ASSERT(_Generic((expr), jstr_memmem_table * : 1), "Passing non-jstr_memmem_table as jstr_memmem_table argument!")
#else
#	define JSTR_ASSERT_IS_MEMMEM_TABLE(expr)
#endif /* HAVE_GENERIC */

#if JSTR_USE_ALLOCA

void
jstr_memmem_comp_mem(jstr_memmem_table *_ptable,
		     const char *_ne,
		     const size_t _nelen);

#	define jstr_memmem_comp_mem(_jstr_memmem_table, _ne, _nelen)                                                         \
		do {                                                                                                          \
			JSTR_ASSERT_IS_MEMMEM_TABLE(_jstr_memmem_table);                                                      \
			JSTR_ASSERT_IS_STR(_ne);                                                                              \
			JSTR_ASSERT_IS_SIZE(_nelen);                                                                          \
			(_jstr_memmem_table)->ne = _ne;                                                                       \
			(_jstr_memmem_table)->nelen = _nelen;                                                                 \
			if (jstr_unlikely(_nelen > 256)) {                                                                    \
				if ((_jstr_memmem_table)->big_table == NULL)                                                  \
					(_jstr_memmem_table)->big_table = (size_t *)alloca(256 * sizeof(size_t));             \
				memset((_jstr_memmem_table)->big_table, 0, 256 * sizeof(size_t));                             \
				for (size_t _i = 1; jstr_likely(_i < _nelen); ++_i)                                           \
					(_jstr_memmem_table)->big_table[priv_memmem_hash2((unsigned char *)_ne + _i)] = _i;   \
			} else {                                                                                              \
				if ((_jstr_memmem_table)->small_table == NULL)                                                \
					(_jstr_memmem_table)->small_table = (uint8_t *)alloca(256 * sizeof(uint8_t));         \
				memset((_jstr_memmem_table)->small_table, 0, 256 * sizeof(uint8_t));                          \
				for (int _i = 1; jstr_likely(_i < (int)_nelen); ++_i)                                         \
					(_jstr_memmem_table)->small_table[priv_memmem_hash2((unsigned char *)_ne + _i)] = _i; \
			}                                                                                                     \
		} while (0)

JSTR_NONNULL_ALL
void
jstr_memmem_comp(jstr_memmem_table *_ptable,
		 const char *_ne);

#	define jstr_memmem_comp(_ptable, _ne) \
		jstr_memmem_comp_mem(_ptable, _ne, strlen(_ne));

#else

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_memmem_comp_mem(jstr_memmem_table *JSTR_RST const _ptable,
		     const char *JSTR_RST const _ne,
		     const size_t _nelen)
{
	_ptable->ne = _ne;
	_ptable->nelen = _nelen;
	if (jstr_unlikely(_nelen > 256)) {
		memset(_ptable->big_table, 0, 256 * sizeof(size_t));
		for (size_t _i = 1; jstr_likely(_i < _nelen); ++_i)
			_ptable->big_table[priv_memmem_hash2((unsigned char *)_ne + _i)] = _i;
	} else {
		memset(_ptable->small_table, 0, 256 * sizeof(uint8_t));
		for (int _i = 1; jstr_likely(_i < (int)_nelen); ++_i)
			_ptable->small_table[priv_memmem_hash2((unsigned char *)_ne + _i)] = _i;
	}
}

JSTR_INLINE
JSTR_NONNULL_ALL
static void
jstr_memmem_comp(jstr_memmem_table *JSTR_RST const _ptable,
		 const char *JSTR_RST const _ne)
{
	jstr_memmem_comp_mem(_ptable, _ne, strlen(_ne));
}

#endif /* USE_ALLOCA */

JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_PURE
static void *
priv_jstr_pre_memmem2(const unsigned char *JSTR_RST _hs,
		      const unsigned char *JSTR_RST const _ne,
		      const size_t _hslen) JSTR_NOEXCEPT
{
	const unsigned char *const _end = _hs + _hslen;
	const uint32_t nw = _ne[0] << 8 | _ne[1];
	uint32_t hw = _hs[0] << 8 | _hs[1];
	for (_hs += 2; hw != nw && jstr_likely(_hs <= _end); hw = (hw | *_hs++) << 8)
		;
	return (hw == nw) ? (void *)(_hs - 2) : NULL;
}

JSTR_PURE
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static void *
priv_jstr_pre_memmem3(const unsigned char *JSTR_RST _hs,
		      const unsigned char *JSTR_RST const _ne,
		      const size_t _hslen) JSTR_NOEXCEPT
{
	const unsigned char *const _end = _hs + _hslen;
	const uint32_t nw = _ne[0] << 24 | _ne[1] << 16 | _ne[2] << 8;
	uint32_t hw = _hs[0] << 24 | _hs[1] << 16 | _hs[2] << 8;
	for (_hs += 3; hw != nw && jstr_likely(_hs <= _end); hw = (hw | *_hs++) << 8)
		;
	return (hw == nw) ? (void *)(_hs - 3) : NULL;
}

JSTR_PURE
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static void *
priv_jstr_pre_memmem4(const unsigned char *JSTR_RST _hs,
		      const unsigned char *JSTR_RST const _ne,
		      const size_t _hslen) JSTR_NOEXCEPT
{
	const unsigned char *const _end = _hs + _hslen;
	const uint32_t nw = _ne[0] << 24 | _ne[1] << 16 | _ne[2] << 8 | _ne[3];
	uint32_t hw = _hs[0] << 24 | _hs[1] << 16 | _hs[2] << 8 | _hs[3];
	for (; hw != nw && jstr_likely(_hs <= _end); hw = (hw | *_hs++) << 8)
		;
	return (hw == nw) ? (void *)(_hs - 4) : NULL;
}

JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_INLINE
static void *
jstr_memmem_exec(const jstr_memmem_table *JSTR_RST const _ptable,
		 const char *JSTR_RST const _hs,
		 const size_t _hslen) JSTR_NOEXCEPT
{
#define PRIV_JSTR_MEMMEM_EXEC(_table)                                                                          \
	do {                                                                                                   \
		const unsigned char *_h = (unsigned char *)_hs;                                                \
		const unsigned char *const _end = _h + _hslen - _ptable->nelen;                                \
		const size_t _mtc1 = _ptable->nelen - 1;                                                       \
		const size_t _shft1 = _mtc1 - _table[priv_memmem_hash2((unsigned char *)_ptable->ne + _mtc1)]; \
		size_t off = 0;                                                                                \
		size_t _tmp;                                                                                   \
		do {                                                                                           \
			do {                                                                                   \
				_h += _mtc1;                                                                   \
				_tmp = _table[priv_memmem_hash2(_h)];                                          \
			} while (!_tmp && _h <= _end);                                                         \
			_h -= _tmp;                                                                            \
			if (_tmp < _mtc1)                                                                      \
				continue;                                                                      \
			if (_mtc1 < 15 || !memcmp(_h + off, _ptable->ne + off, 8)) {                           \
				if (!memcmp(_h, _ptable->ne, _ptable->nelen))                                  \
					return (void *)_h;                                                     \
				off = (off >= 8 ? off : _mtc1) - 8;                                            \
			}                                                                                      \
			_h += _shft1;                                                                          \
		} while (_h <= _end);                                                                          \
	} while (0)
	if (jstr_unlikely(_hslen < _ptable->nelen))
		return NULL;
	switch (_ptable->nelen) {
	case 0: return (void *)_hs;
	case 1: return (void *)memchr(_hs, *_ptable->ne, _hslen);
	case 2: return priv_jstr_pre_memmem2((unsigned char *)_hs, (unsigned char *)_ptable->ne, _hslen);
	case 3: return priv_jstr_pre_memmem3((unsigned char *)_hs, (unsigned char *)_ptable->ne, _hslen);
	case 4: return priv_jstr_pre_memmem4((unsigned char *)_hs, (unsigned char *)_ptable->ne, _hslen);
	}
	if (jstr_unlikely(_ptable->nelen > 256))
		PRIV_JSTR_MEMMEM_EXEC(_ptable->big_table);
	else
		PRIV_JSTR_MEMMEM_EXEC(_ptable->small_table);
	return NULL;
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_WARN_UNUSED
JSTR_PURE
static char *
jstr_strstr_exec(const jstr_memmem_table *JSTR_RST const _ptable,
		 const char *JSTR_RST const _hs) JSTR_NOEXCEPT
{
	return (char *)jstr_memmem_exec(_ptable, _hs, strlen(_hs));
}

#ifdef __cplusplus
}
#endif /* __cpluslus */

#endif /* JSTR_MEMMEM_H_DEF */
