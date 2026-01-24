/* SPDX-License-Identifier: MIT */
/* Based on musl's strstr() and memmem().
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "../../macros.h"

#ifdef JSTR_INTERNAL_MUSL_CANON
#	define CANON JSTR_INTERNAL_MUSL_CANON
#else
#	define CANON(c) (c)
#endif
#ifndef JSTR_INTERNAL_MUSL_CMP_FUNC
#	define JSTR_INTERNAL_MUSL_CMP_FUNC memcmp
#endif
#ifndef JSTR_INTERNAL_MUSL_FUNC_NAME
#	define JSTR_INTERNAL_MUSL_FUNC_NAME jstr_internal_memmem_musl
#endif

#define BITOP(a, b, op) ((a)[(size_t)(b) / (8 * sizeof *(a))] op(size_t) 1 << ((size_t)(b) % (8 * sizeof *(a))))

#ifndef JSTR_INTERNAL_MUSL_TWOWAY_STRUCT
#	define JSTR_INTERNAL_MUSL_TWOWAY_STRUCT

typedef struct jstr_twoway_ty {
	size_t needle_len;
	union u {
		struct _t {
			size_t _shift[256];
			size_t _byteset[32 / sizeof(size_t)];
			size_t _suffix;
			size_t _global_period;
			size_t _memory0;
		} _t;
		char _buf[sizeof(struct _t)];
	} u;
} jstr_twoway_ty;

#endif

JSTR_FUNC_VOID
JSTR_ATTR_INLINE
static void
JSTR_CONCAT(JSTR_INTERNAL_MUSL_FUNC_NAME, _comp)(jstr_twoway_ty *t, const unsigned char *ne
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
                                         ,
                                         size_t needle_len
#endif
                                         )
JSTR_NOEXCEPT
{
	size_t max_suffix, j, k, period;
	int a, b;
	memset(t->u._t._byteset, 0, sizeof(t->u._t._byteset));
	/* Computing length of ne and fill shift table. */
#ifdef JSTR_INTERNAL_MUSL_CHECK_EOL
	for (max_suffix = 0; ne[max_suffix]; ++max_suffix)
#else
	for (max_suffix = 0; max_suffix < needle_len; ++max_suffix)
#endif
		a = CANON(ne[max_suffix]), BITOP(t->u._t._byteset, a, |=), t->u._t._shift[a] = max_suffix + 1;
	t->needle_len = max_suffix;
	/* Compute maximal suffix. */
	max_suffix = (size_t)-1, j = 0, k = t->u._t._global_period = 1;
	while (j + k < t->needle_len) {
		a = CANON(ne[max_suffix + k]);
		b = CANON(ne[j + k]);
		if (a > b) {
			j += k;
			k = 1;
			t->u._t._global_period = j - max_suffix;
		} else if (a == b) {
			if (k == t->u._t._global_period) {
				j += t->u._t._global_period;
				k = 1;
			} else {
				++k;
			}
		} else /* a < b */ {
			max_suffix = j++;
			k = t->u._t._global_period = 1;
		}
	}
	t->u._t._suffix = max_suffix;
	period = t->u._t._global_period;
	/* And with the opposite comparison. */
	max_suffix = (size_t)-1, j = 0, k = t->u._t._global_period = 1;
	while (j + k < t->needle_len) {
		a = CANON(ne[max_suffix + k]);
		b = CANON(ne[j + k]);
		if (a < b) {
			j += k;
			k = 1;
			t->u._t._global_period = j - max_suffix;
		} else if (a == b) {
			if (k == t->u._t._global_period) {
				j += t->u._t._global_period;
				k = 1;
			} else {
				++k;
			}
		} else /* a > b */ {
			max_suffix = j++;
			k = t->u._t._global_period = 1;
		}
	}
	if (max_suffix + 1 > t->u._t._suffix + 1)
		t->u._t._suffix = max_suffix;
	else
		t->u._t._global_period = period;
	/* Periodic ne? */
	if (JSTR_INTERNAL_MUSL_CMP_FUNC((const char *)ne, (const char *)ne + t->u._t._global_period, t->u._t._suffix + 1)) {
		t->u._t._memory0 = 0;
		t->u._t._global_period = JSTR_MAX(t->u._t._suffix, t->needle_len - t->u._t._suffix - 1) + 1;
	} else {
		t->u._t._memory0 = t->needle_len - t->u._t._global_period;
	}
}

JSTR_FUNC_PURE
JSTR_ATTR_INLINE
static char *
JSTR_CONCAT(JSTR_INTERNAL_MUSL_FUNC_NAME, _exec)(const jstr_twoway_ty *t, const unsigned char *hs
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
                                         ,
                                         const size_t hs_len
#endif
                                         ,
                                         const unsigned char *ne
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
                                         ,
                                         const size_t needle_len
#endif
                                         )
JSTR_NOEXCEPT
{
	JSTR_ASSERT_DEBUG(t->needle_len != (size_t)-1, "Trying to call jstr_*exec() before compiling with jstr_*comp().");
	int c;
	size_t k;
	size_t memory = 0;
	/* Initialize end-of-haystack pointer. */
#ifdef JSTR_INTERNAL_MUSL_CHECK_EOL
	const unsigned char *end = hs + jstr_strnlen((const char *)hs, t->needle_len | 512);
#else
	const unsigned char *end = hs + hs_len;
	(void)needle_len;
#endif
	/* Search loop */
	for (;;) {
#ifdef JSTR_INTERNAL_MUSL_CHECK_EOL
		/* Update incremental end-of-haystack pointer. */
		if (jstr_unlikely(JSTR_DIFF(end, hs) < t->needle_len)) {
			/* Fast estimate for MAX(t->needle_len, 2048). */
			end += jstr_strnlen((const char *)end, t->needle_len | 2048);
			if (jstr_unlikely(JSTR_DIFF(end, hs) < t->needle_len))
				break;
		}
#else
		/* If remainder of haystack is shorter than needle, done. */
		if (jstr_unlikely(JSTR_DIFF(end, hs) < t->needle_len))
			break;
#endif
		/* Check last byte first; advance by shift on mismatch. */
		c = CANON(hs[t->needle_len - 1]);
		if (BITOP(t->u._t._byteset, c, &)) {
			k = t->needle_len - t->u._t._shift[c];
			if (k) {
				if (k < memory)
					k = memory;
				hs += k;
				memory = 0;
				continue;
			}
		} else {
			hs += t->needle_len;
			memory = 0;
			continue;
		}
		/* Compare right half. */
		for (k = JSTR_MAX(t->u._t._suffix + 1, memory); k < t->needle_len && CANON(ne[k]) == CANON(hs[k]); ++k) {}
		if (k < t->needle_len) {
			hs += k - t->u._t._suffix;
			memory = 0;
			continue;
		}
		/* Compare left half. */
		for (k = t->u._t._suffix + 1; k > memory && CANON(ne[k - 1]) == CANON(hs[k - 1]); --k) {}
		if (k <= memory)
			return (char *)hs;
		hs += t->u._t._global_period;
		memory = t->u._t._memory0;
	}
	return NULL;
}

JSTR_FUNC_PURE
static char *
JSTR_INTERNAL_MUSL_FUNC_NAME(const unsigned char *haystack
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
                     ,
                     const size_t haystack_len
#endif
                     ,
                     const unsigned char *needle
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
                     ,
                     const size_t needle_len
#endif
                     )
JSTR_NOEXCEPT
{
	jstr_twoway_ty t;
	JSTR_CONCAT(JSTR_INTERNAL_MUSL_FUNC_NAME, _comp)
	(&t, (const unsigned char *)needle
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
	 ,
	 needle_len
#endif
	);
	return JSTR_CONCAT(JSTR_INTERNAL_MUSL_FUNC_NAME, _exec)(&t, (const unsigned char *)haystack
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
	                                                ,
	                                                haystack_len
#endif
	                                                ,
	                                                (const unsigned char *)needle
#ifndef JSTR_INTERNAL_MUSL_CHECK_EOL
	                                                ,
	                                                needle_len
#endif
	);
}

#undef BITOP
#undef CANON

#undef JSTR_INTERNAL_MUSL_CHECK_EOL
#undef JSTR_INTERNAL_MUSL_CANON
#undef JSTR_INTERNAL_MUSL_FUNC_NAME
#undef JSTR_INTERNAL_MUSL_CMP_FUNC
