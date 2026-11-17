/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
{
		const char *p = jstr_strcasestr_len("hello", 5, "", 0);
		assert(p == (const char *)"hello");
	}
{
		const char *hs = "hello";
		const char *ne = "HELLO WORLD";
		char *r = jstr_strcasestr_len(hs, 5, ne, 11);
		assert(r == NULL);
	}
{
		const char *hs = "hello";
		char *r = jstr_strcasestr_len(hs, 5, "", 0);
		assert(r == hs);
	}
{
		const char *hs = "xxAbCdEfGhIjXx";
		const char *ne = "aBcDeFgHiJ";
		char *r = jstr_strcasestr_len(hs, 14, ne, 10);
		assert(r == hs + 2);
	}
{
		const char *hs = "anything";
		char *r = jstr_strcasestr_len(hs, 8, "", 0);
		assert(r == hs);
	}
{
		const char *hs = "xABy";
		char *r = jstr_strcasestr_len(hs, 4, "aB", 2);
		assert(r == hs + 1);
	}
{
		const char *hs = "xxABCDefghXx";
		char *r = jstr_strcasestr_len(hs, 12, "aBcD", 4);
		assert(r == hs + 2);
	}
	return 0;
}
