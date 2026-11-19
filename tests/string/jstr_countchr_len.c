/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	assert(jstr_countchr_len("abac", 'a', 4) == 2);
	assert(jstr_countchr_len("abac", 'z', 4) == 0);
	assert(jstr_countchr_len("", 'a', 0) == 0);
	return 0;
}
