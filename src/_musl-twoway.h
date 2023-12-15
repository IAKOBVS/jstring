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

JSTR_FUNC_PURE
static char *
PJSTR_MUSL_FUNC_NAME(const unsigned char *h,
#if !PJSTR_MUSL_CHECK_EOL
                     const size_t hs_len,
#endif
                     const unsigned char *n
#if !PJSTR_MUSL_CHECK_EOL
                     ,
                     size_t l
#endif
#if PJSTR_MUSL_USE_N
                     ,
                     const size_t n_limit
#endif
)
{
#if PJSTR_MUSL_USE_N
	const unsigned char *const end = h + n_limit;
#endif
#if PJSTR_MUSL_CHECK_EOL
	size_t l;
#endif
	const unsigned char *z;
	size_t ip, jp, k, p, ms, p0, mem, mem0;
	size_t byteset[32 / sizeof(size_t)];
	size_t shift[256];
	int c0, c1;
#if !PJSTR_MUSL_CHECK_EOL
	z = h + hs_len;
#endif
	memset(byteset, 0, sizeof(byteset));
	/* Computing length of needle and fill shift table */
#if PJSTR_MUSL_CHECK_EOL
	for (l = 0;
#	if PJSTR_MUSL_USE_N
	     l <= n_limit &&
#	endif
	     n[l] && h[l];
	     ++l) {
		c0 = CANON(n[l]);
		BITOP(byteset, c0, |=), shift[c0] = l + 1;
	}
#	if PJSTR_MUSL_USE_N
	if (jstr_unlikely(l > n_limit))
		return NULL;
#	endif
	if (jstr_unlikely(n[l] != '\0'))
		return NULL; /* hit the end of h */
#else
	for (size_t i = 0; i < l; ++i) {
		c0 = CANON(n[i]);
		BITOP(byteset, c0, |=), shift[c0] = i + 1;
	}
#endif
	/* Compute maximal suffix */
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
		} else if (c0 > c1) {
			jp += k;
			k = 1;
			p = jp - ip;
		} else {
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
		} else {
			ip = jp++;
			k = p = 1;
		}
	}
	if (ip + 1 > ms + 1)
		ms = ip;
	else
		p = p0;
	/* Periodic needle? */
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
#	if PJSTR_MUSL_USE_N
			const size_t grow = JSTR_MIN(l | 63, JSTR_PTR_DIFF(end, h));
#	else
			const size_t grow = l | 63;
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
		/* If remainder of haystack is shorter than needle, done */
		if (jstr_unlikely(JSTR_PTR_DIFF(z, h) < l))
			return NULL;
#endif
		/* Check last byte first; advance by shift on mismatch */
		c0 = CANON(h[l - 1]);
		if (BITOP(byteset, c0, &)) {
			k = l - shift[c0];
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
		for (k = JSTR_MAX(ms + 1, mem); k < l && CANON(n[k]) == CANON(h[k]); ++k)
			;
		if (k < l) {
			h += k - ms;
			mem = 0;
			continue;
		}
		/* Compare left half */
		for (k = ms + 1; k > mem && CANON(n[k - 1]) == CANON(h[k - 1]); --k)
			;
		if (k <= mem)
			return (char *)h;
		h += p;
		mem = mem0;
	}
}

#undef BITOP
#undef CANON
#undef PJSTR_MUSL_CHECK_EOL
#undef PJSTR_MUSL_CANON
#undef PJSTR_MUSL_FUNC_NAME
#undef PJSTR_MUSL_CMP_FUNC
#undef PJSTR_MUSL_USE_N
