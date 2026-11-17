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
		const char s[] = "/usr/local/bin";
		char *p = jstr_basename_len(s, 14);
		assert(!strcmp(p, "bin"));
		p = jstr_basename_len("foo", 3);
		assert(!strcmp(p, "foo"));
		p = jstr_basename_len("", 0);
		assert(!strcmp(p, ""));
	}
	return 0;
}
