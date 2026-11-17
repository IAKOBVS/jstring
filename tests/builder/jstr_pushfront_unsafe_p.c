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

	/* Exact-size buffer: 2 existing bytes + 1 prepended byte + NUL. */
	{
		struct {
			char buf[4];
			char guard[8];
		} x;
		memset(&x, 0xA5, sizeof x);
		memcpy(x.buf, "bc", 3);
		char *p = jstr_pushfront_unsafe_p(x.buf, 2, 'a');
		assert(p == x.buf + 3);
		assert(*p == '\0');
		assert(memcmp(x.buf, "abc", 4) == 0);
		check_guard(x.guard, sizeof x.guard, 0xA5);
	}

	/* Oversized buffer: bytes after the new terminator stay untouched. */
	{
		char buf[64];
		memset(buf, 0xA5, sizeof buf);
		memcpy(buf, "middle", 7);
		char *p = jstr_pushfront_unsafe_p(buf, 6, '>');
		assert(p == buf + 7);
		assert(*p == '\0');
		assert(memcmp(buf, ">middle", 8) == 0);
		check_guard(buf + 8, 16, 0xA5);
	}

	/* Empty source string. */
	{
		struct {
			char buf[2];
			char guard[4];
		} x;
		memset(&x, 0xA5, sizeof x);
		char *p = jstr_pushfront_unsafe_p(x.buf, 0, 'Z');
		assert(p == x.buf + 1);
		assert(x.buf[0] == 'Z');
		assert(x.buf[1] == '\0');
		check_guard(x.guard, sizeof x.guard, 0xA5);
	}

	/* A longer exact-size prepend, including embedded content bytes. */
	{
		char buf[18];
		memset(buf, 0xA5, sizeof buf);
		memcpy(buf, "0123456789abcdef", 17);
		char *p = jstr_pushfront_unsafe_p(buf, 16, '!');
		assert(p == buf + 17);
		assert(memcmp(buf, "!0123456789abcdef", 18) == 0);
	}

	/* Managed push/pop cycles that force growth in both directions. */
	{
		char expect[4096];
		char *s = NULL;
		size_t sz = 0, cap = 0, esz = 0;

		for (int i = 0; i < 400; ++i) {
			const char c = (char)('A' + (i % 26));
			if (i % 3 == 0) {
				assert(jstr_pushfront(&s, &sz, &cap, c) == JSTR_RET_SUCC);
				assert(esz + 1 < sizeof expect);
				memmove(expect + 1, expect, esz + 1);
				expect[0] = c;
				++esz;
			} else {
				assert(jstr_pushback(&s, &sz, &cap, c) == JSTR_RET_SUCC);
				assert(esz + 1 < sizeof expect);
				expect[esz++] = c;
				expect[esz] = '\0';
			}
			assert(sz == esz);
			assert(strlen(s) == sz);
			assert(s[sz] == '\0');
			assert(cap == 0 || cap > sz);
			assert(memcmp(s, expect, esz + 1) == 0);

			if (i % 5 == 4 && esz >= 4) {
				size_t old = esz;
				char *pb = jstr_popback_p(s, sz);
				assert(pb == s + old - 1);
				assert(*pb == '\0');
				--sz;
				expect[--esz] = '\0';
				assert(sz == esz);
				assert(memcmp(s, expect, esz + 1) == 0);

				old = esz;
				char *pf = jstr_popfront_p(s, sz);
				assert(pf == s + old - 1);
				assert(*pf == '\0');
				memmove(expect, expect + 1, old);
				--sz;
				--esz;
				assert(sz == esz);
				assert(memcmp(s, expect, esz + 1) == 0);
			}
		}
		assert(esz > 0);
		assert(cap > sz);
		assert(strcmp(s, expect) == 0);

		/* pushfrontn on a managed buffer. */
		assert(jstr_pushfrontn(&s, &sz, &cap, 'x', 5) == JSTR_RET_SUCC);
		{
			char next[4096];
			memset(next, 'x', 5);
			memcpy(next + 5, expect, esz + 1);
			memcpy(expect, next, esz + 6);
			esz += 5;
		}
		assert(sz == esz);
		assert(cap > sz);
		assert(memcmp(s, expect, esz + 1) == 0);

		assert(jstr_shrink_to_fit(&s, &sz, &cap) == JSTR_RET_SUCC);
		assert(cap == sz + 1);
		assert(memcmp(s, expect, esz + 1) == 0);

		jstr_free(&s, &sz, &cap);
		assert(s == NULL);
		assert(sz == 0);
		assert(cap == 0);
	}

	SUCCESS();
	return EXIT_SUCCESS;
}
