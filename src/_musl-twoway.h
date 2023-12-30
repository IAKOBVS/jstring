/* Based on musl's strstr() and memmem().
   Copyright © 2005-2020 Rich Felker, et al.

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "jstr-macros.h"

#ifdef PJSTR_MUSL_CANON
#	define CANON PJSTR_MUSL_CANON
#else
#	define CANON(c) (c)
#endif
#ifndef PJSTR_MUSL_CMP_FUNC
#	define PJSTR_MUSL_CMP_FUNC memcmp
#endif
#ifndef PJSTR_MUSL_FUNC_NAME
#	define PJSTR_MUSL_FUNC_NAME pjstr_memmem_musl
#endif

#define BITOP(a, b, op) \
	((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 << ((size_t)(b) % (8 * sizeof *(a))))

#ifndef PJSTR_MUSL_TWOWAY_STRUCT
#	define PJSTR_MUSL_TWOWAY_STRUCT

typedef struct jstr_twoway_ty {
	size_t needle_len;
	size_t _shift[256];
	size_t _byteset[32 / sizeof(size_t)];
	size_t _mem0, _ms, _p;
} jstr_twoway_ty;

#endif

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _comp)(jstr_twoway_ty *const t,
                                         const unsigned char *n
#if !PJSTR_MUSL_CHECK_EOL
                                         ,
                                         size_t n_len
#endif
                                         )
JSTR_NOEXCEPT
{
	size_t ip, jp, k, p0;
	int c0, c1;
	memset(t->_byteset, 0, sizeof(t->_byteset));
	/* Computing length of n and fill _shift table. */
	for (ip = 0;
#if PJSTR_MUSL_CHECK_EOL
	     n[ip];
#else
	     ip < n_len;
#endif
	     ++ip)
		c0 = CANON(n[ip]), BITOP(t->_byteset, c0, |=), t->_shift[c0] = ip + 1;
	t->needle_len = ip;
	/* Compute maximal suffix. */
	ip = (size_t)-1, jp = 0, k = t->_p = 1;
	while (jp + k < t->needle_len) {
		c0 = CANON(n[ip + k]);
		c1 = CANON(n[jp + k]);
		if (c0 == c1) {
			if (k == t->_p) {
				jp += t->_p;
				k = 1;
			} else {
				++k;
			}
		} else if (c0 > c1) {
			jp += k;
			k = 1;
			t->_p = jp - ip;
		} else /* c0 < c1 */ {
			ip = jp++;
			k = t->_p = 1;
		}
	}
	t->_ms = ip;
	p0 = t->_p;
	/* And with the opposite comparison. */
	ip = (size_t)-1, jp = 0, k = t->_p = 1;
	while (jp + k < t->needle_len) {
		c0 = CANON(n[ip + k]);
		c1 = CANON(n[jp + k]);
		if (c0 == c1) {
			if (k == t->_p) {
				jp += t->_p;
				k = 1;
			} else {
				++k;
			}
		} else if (c0 < c1) {
			jp += k;
			k = 1;
			t->_p = jp - ip;
		} else /* c0 > c1 */ {
			ip = jp++;
			k = t->_p = 1;
		}
	}
	if (ip + 1 > t->_ms + 1)
		t->_ms = ip;
	else
		t->_p = p0;
	/* Periodic needle? */
	if (PJSTR_MUSL_CMP_FUNC((const char *)n, (const char *)n + t->_p, t->_ms + 1)) {
		t->_mem0 = 0;
		t->_p = JSTR_MAX(t->_ms, t->needle_len - t->_ms - 1) + 1;
	} else {
		t->_mem0 = t->needle_len - t->_p;
	}
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _exec)(const jstr_twoway_ty *const t,
                                         const unsigned char *h
#if !PJSTR_MUSL_CHECK_EOL
                                         ,
                                         const size_t h_len
#endif
                                         ,
                                         const unsigned char *n
#if PJSTR_MUSL_USE_N
                                         ,
                                         size_t n_limit
#endif
                                         )
JSTR_NOEXCEPT
{
#if PJSTR_MUSL_CHECK_EOL
	if (jstr_unlikely(n[t->needle_len] != '\0'))
		return NULL;
#	if PJSTR_MUSL_USE_N
	if (jstr_unlikely(t->needle_len > n_limit))
		return NULL;
	const unsigned char *const z_n = h + n_limit;
#	endif
#endif
	int c0;
	size_t k;
	size_t mem = 0;
	/* Initialize end-of-haystack pointer. */
#if PJSTR_MUSL_CHECK_EOL
	const unsigned char *z = h + jstr_strnlen((const char *)h, t->needle_len | 512);
#else
	const unsigned char *const z = h + h_len;
#endif
	/* Search loop */
	for (;;) {
#if PJSTR_MUSL_CHECK_EOL
		/* Update incremental end-of-haystack pointer. */
		if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < t->needle_len)) {
			/* Fast estimate for MAX(t->needle_len, 2048). */
#	if PJSTR_MUSL_USE_N
			z += jstr_strnlen((const char *)z, JSTR_MIN(t->needle_len | 2048, JSTR_PTR_DIFF(z_n, h)));
#	else
			z += jstr_strnlen((const char *)z, t->needle_len | 2048);
#	endif
			if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < t->needle_len))
				break;
		}
#else
		/* If remainder of haystack is shorter than needle, done. */
		if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < t->needle_len))
			break;
#endif
		/* Check last byte first; advance by shift on mismatch. */
		c0 = CANON(h[t->needle_len - 1]);
		if (BITOP(t->_byteset, c0, &)) {
			k = t->needle_len - t->_shift[c0];
			if (k) {
				if (k < mem)
					k = mem;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += t->needle_len;
			mem = 0;
			continue;
		}
		/* Compare right half. */
		for (k = JSTR_MAX(t->_ms + 1, mem); k < t->needle_len && CANON(n[k]) == CANON(h[k]); ++k) {}
		if (k < t->needle_len) {
			h += k - t->_ms;
			mem = 0;
			continue;
		}
		/* Compare left half. */
		for (k = t->_ms + 1; k > mem && CANON(n[k - 1]) == CANON(h[k - 1]); --k) {}
		if (k <= mem)
			return (char *)h;
		h += t->_p;
		mem = t->_mem0;
	}
	return NULL;
}

JSTR_FUNC_PURE
static char *
PJSTR_MUSL_FUNC_NAME(const unsigned char *haystack
#if !PJSTR_MUSL_CHECK_EOL
                     ,
                     const size_t haystack_len
#endif
                     ,
                     const unsigned char *needle
#if !PJSTR_MUSL_CHECK_EOL
                     ,
                     const size_t needle_len
#endif
#if PJSTR_MUSL_USE_N
                     ,
                     size_t n_limit
#endif
                     )
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _comp)
	(&t, (const unsigned char *)needle
#if !PJSTR_MUSL_CHECK_EOL
	 ,
	 needle_len
#endif
	);
	return JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _exec)(&t, (const unsigned char *)haystack
#if !PJSTR_MUSL_CHECK_EOL
	                                                ,
	                                                haystack_len
#endif
	                                                ,
	                                                (const unsigned char *)needle
#if PJSTR_MUSL_USE_N
	                                                ,
	                                                n_limit
#endif
	);
}

#undef BITOP
#undef CANON

#undef PJSTR_MUSL_CHECK_EOL
#undef PJSTR_MUSL_CANON
#undef PJSTR_MUSL_FUNC_NAME
#undef PJSTR_MUSL_CMP_FUNC
#undef PJSTR_MUSL_USE_N
