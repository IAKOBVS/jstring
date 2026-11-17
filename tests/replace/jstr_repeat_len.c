/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

/* Regression coverage: repeating an empty string must keep length 0 and
 * write exactly one NUL regardless of n; previously n NULs were written and
 * a preallocated capacity of 1 overflowed. */

#include <stdlib.h>
#include <string.h>

#include "../rigor.h"
#include "replace.h"

static char *
xnew(size_t cap)
{
	char *p = (char *)malloc(cap ? cap : 1);
	RCHECK(p != NULL);
	memset(p, 0xa5, cap ? cap : 1);
	return p;
}

static size_t
ref_repeat(char *out, const char *in, size_t sz, size_t n)
{
	if (n <= 1) {
		memcpy(out, in, sz + 1);
		return sz;
	}
	for (size_t i = 0; i < n; ++i)
		memcpy(out + i * sz, in, sz);
	out[n * sz] = '\0';
	return n * sz;
}

static void
check_inv(const char *s, size_t sz, size_t cap)
{
	RCHECK(cap == 0 || cap > sz);
	if (cap != 0)
		RCHECK_EQ(s[sz], 0);
}

int
main(void)
{
	static const size_t lens[] = { 0, 1, 2, 3, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
	char in[160];
	char exp[2048];
	size_t k;

	/* n == 0 and n == 1 are no-ops. */
	for (size_t n = 0; n <= 1; ++n) {
		char *s = xnew(6);
		size_t sz = 5;
		size_t cap = 6;
		memcpy(s, "hello", 6);
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, n), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 5);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "hello", 6);
		free(s);
	}

	/* Repetition of a normal string. */
	{
		char *s = xnew(7);
		size_t sz = 2;
		size_t cap = 7;
		memcpy(s, "ab", 3);
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, 3), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 6);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "ababab", 7);
		free(s);
	}
	{
		char *s = xnew(2);
		size_t sz = 1;
		size_t cap = 2;
		memcpy(s, "x", 2);
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, 5), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 5);
		check_inv(s, sz, cap);
		RCHECK_MEM(s, "xxxxx", 6);
		free(s);
	}

	/* Empty string: length stays zero at every n (cap 0 forces growth). */
	for (size_t n = 0; n <= 4; ++n) {
		char *s = xnew(1);
		size_t sz = 0;
		size_t cap = 0;
		s[0] = '\0';
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, n), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 0);
		check_inv(s, sz, cap);
		RCHECK_EQ(s[0], 0);
		free(s);
	}

	/* Regression: cap == 1 (exactly room for the NUL) is enough for an
	 * empty string; n must not cause n NUL bytes to be written. */
	for (size_t n = 2; n <= 8; ++n) {
		char *s = xnew(1);
		size_t sz = 0;
		size_t cap = 1;
		s[0] = '\0';
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, n), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 0);
		RCHECK_EQ(cap, 1);
		RCHECK_EQ(s[0], 0);
		free(s);
	}

	/* Large repetition crossing many realloc boundaries. */
	{
		char *s = xnew(3);
		size_t sz = 2;
		size_t cap = 3;
		memcpy(s, "xy", 3);
		RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, 100), JSTR_RET_SUCC);
		RCHECK_EQ(sz, 200);
		check_inv(s, sz, cap);
		for (size_t i = 0; i < sz; i += 2) {
			RCHECK_EQ(s[i], 'x');
			RCHECK_EQ(s[i + 1], 'y');
		}
		free(s);
	}

	/* All boundary lengths; n from 0 to 4; tight capacity. */
	for (k = 0; k < sizeof(lens) / sizeof(lens[0]); ++k) {
		const size_t n = lens[k];
		for (size_t i = 0; i < n; ++i)
			in[i] = (char)('a' + (int)((i + k) % 26));
		in[n] = '\0';
		for (size_t reps = 0; reps <= 4; ++reps) {
			char *s;
			size_t sz;
			size_t cap;
			size_t want;
			if (n == 0) {
				s = xnew(1);
				s[0] = '\0';
				sz = 0;
				cap = 0;
			} else {
				cap = n * (reps > 1 ? reps : 1) + 1;
				s = xnew(cap);
				memcpy(s, in, n + 1);
				sz = n;
			}
			want = ref_repeat(exp, in, n, reps);
			RCHECK_EQ(jstr_repeat_len(&s, &sz, &cap, reps), JSTR_RET_SUCC);
			RCHECK_EQ(sz, want);
			check_inv(s, sz, cap);
			RCHECK_MEM(s, exp, sz + 1);
			free(s);
		}
	}
	return 0;
}
