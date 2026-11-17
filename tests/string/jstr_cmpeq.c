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
		assert(jstr_cmpeq("abc", 3, "abc", 3) == 0);
		assert(jstr_cmpeq("abc", 3, "abd", 3) != 0);
		assert(jstr_cmpeq("abc", 3, "ab", 2) == 1);
	}
	return 0;
}
