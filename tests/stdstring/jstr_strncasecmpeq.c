/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../test.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;
	assert(jstr_strncasecmpeq("abc", "ABC", 3) == 0);
	assert(jstr_strncasecmpeq("abc", "abd", 3) != 0);
	assert(jstr_strncasecmpeq("a", "abc", 5) != 0); /* stops at NUL */
	assert(jstr_strncasecmpeq("x", "y", 0) == 0);
	return 0;
}
