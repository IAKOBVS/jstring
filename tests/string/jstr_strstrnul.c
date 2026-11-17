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
		const char hs[] = "hello world";
		const char *p = jstr_strstrnul(hs, "wo");
		assert(!strcmp(p, "world"));
		p = jstr_strstrnul(hs, "xyz");
		assert(p == hs + 11);
		p = jstr_strstrnul(hs, "h");
		assert(p == hs);
		p = jstr_strstrnul(hs, "d");
		assert(!strcmp(p, "d"));
	}
	return 0;
}
