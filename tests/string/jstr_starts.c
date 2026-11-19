/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	assert(jstr_starts("hello", "he"));
	assert(!jstr_starts("hello", "He"));
	assert(jstr_starts("", ""));
	return 0;
}
