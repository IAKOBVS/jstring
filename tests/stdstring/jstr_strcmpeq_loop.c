/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strcmpeq_loop("abc", "abc") == 0);
	int r = jstr_strcmpeq_loop("abx", "aby");
	assert(r == 'y');
	r = jstr_strcmpeq_loop("ab", "abc");
	assert(r == 'c');
	return 0;
}
