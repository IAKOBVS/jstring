/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

static void
check_guard(const char *p, size_t n, unsigned char value)
{
	for (size_t i = 0; i < n; ++i)
		assert((unsigned char)p[i] == value);
}

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	/* strset writes exactly strlen(s) bytes and leaves the NUL in place. */
	{
		char buf[64];
		memset(buf, 0xA5, sizeof buf);
		memcpy(buf, "hello", 6);
		jstr_strset(buf, 'z');
		assert(memcmp(buf, "zzzzz", 6) == 0);
		assert(buf[5] == '\0');
		check_guard(buf + 6, 16, 0xA5);
	}

	/* Empty string: nothing is written. */
	{
		char buf[8];
		memset(buf, 0xA5, sizeof buf);
		buf[0] = '\0';
		jstr_strset(buf, 'q');
		assert(buf[0] == '\0');
		check_guard(buf + 1, 7, 0xA5);
	}

	/* A NUL fill zeroes every byte that strlen observed. */
	{
		char buf[16];
		memcpy(buf, "abcdef", 7);
		jstr_strset(buf, '\0');
		assert(buf[0] == '\0');
		assert(memcmp(buf, "\0\0\0\0\0\0", 6) == 0);
		assert(buf[6] == '\0');
	}

	/* High-bit fill characters are stored as unsigned char. */
	{
		char buf[8];
		memcpy(buf, "abc", 4);
		jstr_strset(buf, 0x80);
		assert((unsigned char)buf[0] == 0x80);
		assert((unsigned char)buf[1] == 0x80);
		assert((unsigned char)buf[2] == 0x80);
		assert(buf[3] == '\0');
	}

	/* Managed assignnchr: grow, exact size, NUL, and capacity invariant. */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;

		assert(jstr_assignnchr(&s, &sz, &cap, 'x', 5) == JSTR_RET_SUCC);
		assert(sz == 5);
		assert(cap > sz);
		assert(memcmp(s, "xxxxx", 5) == 0);
		assert(s[5] == '\0');

		/* Shrinking does not need to reallocate or change capacity. */
		{
			char *const before = s;
			const size_t cap_before = cap;
			assert(jstr_assignnchr(&s, &sz, &cap, 'y', 2) == JSTR_RET_SUCC);
			assert(s == before);
			assert(cap == cap_before);
			assert(sz == 2);
			assert(s[0] == 'y' && s[1] == 'y');
			assert(s[2] == '\0');
		}

		assert(jstr_assignnchr(&s, &sz, &cap, 'z', 0) == JSTR_RET_SUCC);
		assert(sz == 0);
		assert(s[0] == '\0');

		/* A big assignment forces growth and fills every byte. */
		assert(jstr_assignnchr(&s, &sz, &cap, 'Q', 10000) == JSTR_RET_SUCC);
		assert(sz == 10000);
		assert(cap > sz);
		assert(s[0] == 'Q');
		assert(s[9999] == 'Q');
		assert(s[10000] == '\0');
		for (size_t i = 0; i < sz; ++i)
			assert(s[i] == 'Q');

		/* Shrinking a large buffer keeps the same allocation. */
		{
			char *const before = s;
			const size_t cap_before = cap;
			assert(jstr_assignnchr(&s, &sz, &cap, 'R', 1) == JSTR_RET_SUCC);
			assert(s == before);
			assert(cap == cap_before);
			assert(sz == 1);
			assert(s[0] == 'R');
			assert(s[1] == '\0');
		}

		/* strset on managed content uses strlen, not capacity. */
		assert(jstr_reserve(&s, &sz, &cap, 64) == JSTR_RET_SUCC);
		assert(jstr_assign_len(&s, &sz, &cap, "abcd", 4) == JSTR_RET_SUCC);
		jstr_strset(s, '#');
		assert(sz == 4);
		assert(memcmp(s, "####", 5) == 0);
		assert(cap > sz);

		jstr_free(&s, &sz, &cap);
		assert(s == NULL);
		assert(sz == 0);
		assert(cap == 0);
	}

	SUCCESS();
	return EXIT_SUCCESS;
}
