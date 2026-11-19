/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../test.h"

int
main(void)
{

	assert(jstr_strcasecmpeq("abc", "ABC") == 0);
	int r = jstr_strcasecmpeq("abx", "aby");
	assert(r != 0);
	r = jstr_strcasecmpeq("ab", "abc");
	assert(r != 0);
	return 0;
}
