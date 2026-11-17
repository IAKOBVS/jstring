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
	assert(jstr_ends("hello", "lo"));
	assert(!jstr_ends("hello", "lO"));
	assert(jstr_ends("", ""));
	return 0;
}
