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
main(void)
{

	/* Exact-size destination: 2 + 3 + 1 bytes only. */
	{
		struct {
			char buf[6];
			char guard[8];
		} x;
		memset(&x, 0xA5, sizeof x);
		memcpy(x.buf, "hi", 3);
		jstr_append_unsafe(x.buf, 2, "XYZ");
		assert(memcmp(x.buf, "hiXYZ", 6) == 0);
		assert(x.buf[5] == '\0');
		check_guard(x.guard, sizeof x.guard, 0xA5);
	}

	/* Oversized destination: nothing past the copied NUL may change. */
	{
		char buf[64];
		memset(buf, 0xA5, sizeof buf);
		memcpy(buf, "hi", 3);
		jstr_append_unsafe(buf, 2, "XYZ");
		assert(memcmp(buf, "hiXYZ", 6) == 0);
		check_guard(buf + 6, 16, 0xA5);
	}

	/* Empty source writes only a terminator at the append offset. */
	{
		char buf[16];
		memset(buf, 0xA5, sizeof buf);
		memcpy(buf, "hi", 3);
		jstr_append_unsafe(buf, 2, "");
		assert(memcmp(buf, "hi", 3) == 0);
		check_guard(buf + 3, 8, 0xA5);
	}

	/* Appending to an empty destination. */
	{
		char buf[8];
		memset(buf, 0xA5, sizeof buf);
		jstr_append_unsafe(buf, 0, "abc");
		assert(memcmp(buf, "abc", 4) == 0);
		check_guard(buf + 4, 4, 0xA5);
	}

	/* Appending at a nonzero offset leaves the prefix untouched. */
	{
		char buf[32];
		memcpy(buf, jstr_literal("prefix:"));
		jstr_append_unsafe(buf, 7, "tail");
		assert(strcmp(buf, "prefix:tail") == 0);
	}

	/* Many managed appends that force repeated growth. */
	{
		static const char *const pieces[] = { "a", "BC", "d", "", "ef" };
		char expect[4096];
		char *s = NULL;
		size_t sz = 0, cap = 0, esz = 0;

		for (int i = 0; i < 500; ++i) {
			const char *piece = pieces[i % 5];
			const size_t n = strlen(piece);
			assert(esz + n < sizeof expect);
			memcpy(expect + esz, piece, n);
			esz += n;
			expect[esz] = '\0';
			assert(jstr_append_len(&s, &sz, &cap, piece, n) == JSTR_RET_SUCC);
			assert(sz == esz);
			assert(strlen(s) == sz);
			assert(s[sz] == '\0');
			assert(cap == 0 || cap > sz);
		}
		assert(esz > 0);
		assert(memcmp(s, expect, esz + 1) == 0);
		assert(cap > sz);

		/* Repeated reserve calls must not lose content or change size. */
		{
			const size_t saved_cap = cap;
			const size_t saved_sz = sz;
			assert(jstr_reserve(&s, &sz, &cap, sz + 1) == JSTR_RET_SUCC);
			assert(cap == saved_cap);
			assert(sz == saved_sz);
			assert(memcmp(s, expect, esz + 1) == 0);
			assert(jstr_reserve(&s, &sz, &cap, sz + 4096) == JSTR_RET_SUCC);
			assert(cap >= sz + 4096);
			assert(sz == saved_sz);
			assert(memcmp(s, expect, esz + 1) == 0);
		}

		/* Shrinking preserves bytes and gives an exact-fit capacity. */
		assert(jstr_shrink_to_fit(&s, &sz, &cap) == JSTR_RET_SUCC);
		assert(cap == sz + 1);
		assert(sz == esz);
		assert(memcmp(s, expect, esz + 1) == 0);

		/* Growing again after the shrink must still preserve the prefix. */
		memcpy(expect + esz, "TAIL", 4);
		esz += 4;
		expect[esz] = '\0';
		assert(jstr_append_len(&s, &sz, &cap, "TAIL", 4) == JSTR_RET_SUCC);
		assert(sz == esz);
		assert(cap > sz);
		assert(memcmp(s, expect, esz + 1) == 0);
		assert(strlen(s) == esz);

		jstr_free(&s, &sz, &cap);
		assert(s == NULL);
		assert(sz == 0);
		assert(cap == 0);
	}

	return EXIT_SUCCESS;
}
