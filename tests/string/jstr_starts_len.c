/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	assert(jstr_starts_len("hello", 5, "he", 2));
	assert(!jstr_starts_len("hello", 5, "HE", 2));
	assert(jstr_starts_len("", 0, "", 0));
	return 0;
}
