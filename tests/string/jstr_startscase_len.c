/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	assert(jstr_startscase_len("hello", 5, "HE", 2));
	assert(!jstr_startscase_len("hello", 5, "xx", 2));
	assert(jstr_startscase_len("", 0, "", 0));
	return 0;
}
