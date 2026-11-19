/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strncasecmp("abc", "ABC", 3) == 0);
	assert(jstr_strncasecmp("abc", "abd", 3) < 0);
	assert(jstr_strncasecmp("abd", "abc", 3) > 0);
	assert(jstr_strncasecmp("ab", "abc", 2) == 0); /* stops at NUL */
	assert(jstr_strncasecmp("ab", "abc", 5) < 0);
	assert(jstr_strncasecmp("", "", 1) == 0);
	return 0;
}
