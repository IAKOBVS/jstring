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
	assert(jstr_starts("hello", "he"));
	assert(!jstr_starts("hello", "He"));
	assert(jstr_starts("", ""));
	return 0;
}
