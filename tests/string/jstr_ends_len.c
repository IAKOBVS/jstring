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
	assert(jstr_ends_len("hello", 5, "lo", 2));
	assert(!jstr_ends_len("hello", 5, "lO", 2));
	assert(jstr_ends_len("", 0, "", 0));
	return 0;
}
