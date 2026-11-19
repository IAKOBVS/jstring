/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strcasecmpeq_loop("abc", "ABC") == 0);
	/* returns trailing byte of s2 at first diff */
	int r = jstr_strcasecmpeq_loop("abx", "aby");
	assert(r == 'y');
	r = jstr_strcasecmpeq_loop("ab", "abc");
	assert(r == 'c');
	return 0;
}
