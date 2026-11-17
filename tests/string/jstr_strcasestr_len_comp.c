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
		jstr_twoway_ty tw;
		jstr_strcasestr_len_comp(&tw, "abc", 3);
		const char *p = jstr_strcasestr_len_exec(&tw, "xxABCxx", 7, "abc", 3);
		assert(p != NULL && !strncasecmp(p, "ABC", 3));
	}
{
		jstr_twoway_ty t;
		jstr_strcasestr_len_comp(&t, "aB", 2);
		const char *hs = "xABy";
		char *r = jstr_strcasestr_len_exec(&t, hs, 4, "aB", 2);
		assert(r == hs + 1);
	}
{
		jstr_twoway_ty t;
		jstr_strcasestr_len_comp(&t, "X", 1);
		const char *hs = "abcXyz";
		char *r = jstr_strcasestr_len_exec(&t, hs, 6, "X", 1);
		assert(r == hs + 3);
	}
{
		jstr_twoway_ty t;
		jstr_strcasestr_len_comp(&t, "", 0);
		const char *hs = "anything";
		char *r = jstr_strcasestr_len_exec(&t, hs, 8, "", 0);
		assert(r == hs);
	}
	return 0;
}
