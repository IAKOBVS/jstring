/* Taken from or based on musl's str*, mem* functions with modifications.
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

#if !PJSTR_MUSL_CHECK_EOL
#	define NE_LEN_ARG , needle_len
#	define HS_LEN_ARG haystack_len,
#else
#	define NE_LEN_ARG
#	define HS_LEN_ARG
#endif

#if PJSTR_MUSL_USE_N
#	define N_PARAM , const size_t n_limit
#	define N_ARG   , n_limit
#else
#	define N_PARAM
#	define N_ARG
#endif

#ifndef PJSTR_MUSL_TWOWAY_STRUCT
#	define PJSTR_MUSL_TWOWAY_STRUCT

typedef struct jstr_twoway_ty {
	size_t shift[256];
	size_t byteset[32 / sizeof(size_t)];
	size_t needle_len;
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
{
	int c;
	memset(t->byteset, 0, sizeof(t->byteset));
	/* Computing length of n and fill shift table */
	size_t i;
	for (i = 0;
#if PJSTR_MUSL_CHECK_EOL
	     *n;
#else
	     i < n_len;
#endif
	     ++i, ++n)
		c = CANON(*n), BITOP(t->byteset, c, |=), t->shift[c] = i + 1;
	t->needle_len = i;
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _exec)(const jstr_twoway_ty *const t,
                                         const unsigned char *h
#if !PJSTR_MUSL_CHECK_EOL
                                         ,
                                         const size_t haystack_len
#endif
                                         ,
                                         const unsigned char *n N_PARAM)
{
#if PJSTR_MUSL_CHECK_EOL
	if (jstr_unlikely(n[t->needle_len] != '\0'))
		return NULL;
#	if PJSTR_MUSL_USE_N
	if (jstr_unlikely(t->needle_len > n_limit))
		return NULL;
	const unsigned char *const end = h + n_limit;
#	endif
#endif
	size_t ip, jp, k, l, p, ms, p0, mem, mem0;
	int c0, c1;
	const unsigned char *z;
#if !PJSTR_MUSL_CHECK_EOL
	z = h + haystack_len;
#endif
	l = t->needle_len;
	/* Compute maximal suffix */
	ip = (size_t)-1, jp = 0, k = p = 1;
	while (jp + k < l) {
		c0 = CANON(n[ip + k]);
		c1 = CANON(n[jp + k]);
		if (c0 == c1) {
			if (k == p) {
				jp += p;
				k = 1;
			} else {
				++k;
			}
		} else if (c0 > c1) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else /* c0 < c1 */ {
			ip = jp++;
			k = p = 1;
		}
	}
	ms = ip;
	p0 = p;
	/* And with the opposite comparison */
	ip = (size_t)-1;
	jp = 0;
	k = p = 1;
	while (jp + k < l) {
		c0 = CANON(n[ip + k]);
		c1 = CANON(n[jp + k]);
		if (c0 == c1) {
			if (k == p) {
				jp += p;
				k = 1;
			} else {
				++k;
			}
		} else if (c0 < c1) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else /* c0 > c1 */ {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip + 1 > ms + 1)
		ms = ip;
	else
		p = p0;
	/* Periodic n? */
	if (PJSTR_MUSL_CMP_FUNC((const char *)n, (const char *)n + p, ms + 1)) {
		mem0 = 0;
		p = JSTR_MAX(ms, l - ms - 1) + 1;
	} else {
		mem0 = l - p;
	}
	mem = 0;
#if PJSTR_MUSL_CHECK_EOL
	/* Initialize incremental end-of-haystack pointer */
	z = h;
#endif
	/* Search loop */
	for (;;) {
#if PJSTR_MUSL_CHECK_EOL
		/* Update incremental end-of-haystack pointer */
		if (JSTR_PTR_DIFF(z, h) < l) {
			/* Fast estimate for MAX(l,63) */
			const size_t grow =
#	if PJSTR_MUSL_USE_N
			JSTR_MIN(l | 63, JSTR_PTR_DIFF(end, h));
#	else
			l | 63;
#	endif
			const unsigned char *const z2 = z + jstr_strnlen((const char *)z, grow);
			if (z2) {
				z = z2;
				if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < l))
					return NULL;
			} else {
				z += grow;
			}
		}
#else
		/* If remainder of haystack is shorter than n, done */
		if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < l))
			return NULL;
#endif
		/* Check last byte first; advance by shift on mismatch */
		c0 = CANON(h[l - 1]);
		if (BITOP(t->byteset, c0, &)) {
			k = l - t->shift[c0];
			if (k) {
				if (k < mem)
					k = mem;
				h += k;
				mem = 0;
				continue;
			}
		} else {
			h += l;
			mem = 0;
			continue;
		}
		/* Compare right half */
		for (k = JSTR_MAX(ms + 1, mem); k < l && CANON(n[k]) == CANON(h[k]); ++k) {}
		if (k < l) {
			h += k - ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k = ms + 1; k > mem && CANON(n[k - 1]) == CANON(h[k - 1]); --k) {}
		if (k <= mem)
			return (char *)h;
		h += p;
		mem = mem0;
	}
}

JSTR_FUNC_PURE
static char *
PJSTR_MUSL_FUNC_NAME(const unsigned char *h
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
                     N_PARAM)
{
	jstr_twoway_ty t;
	JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _comp)
	(&t, (const unsigned char *)needle NE_LEN_ARG);
	return JSTR_CONCAT(PJSTR_MUSL_FUNC_NAME, _exec)(&t, (const unsigned char *)h,
#if !PJSTR_MUSL_CHECK_EOL
	                                                HS_LEN_ARG
#endif
	                                                (const unsigned char *) needle N_ARG);
}

#undef NE_LEN_PARAM
#undef NE_LEN_ARG
#undef HS_LEN_PARAM
#undef HS_LEN_ARG
#undef N_PARAM
#undef N_ARG

#undef BITOP
#undef CANON
#undef PJSTR_MUSL_CHECK_EOL
#undef PJSTR_MUSL_CANON
#undef PJSTR_MUSL_FUNC_NAME
#undef PJSTR_MUSL_CMP_FUNC
#undef PJSTR_MUSL_USE_N
