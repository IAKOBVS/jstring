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
	assert(jstr_endscase_len("hello", 5, "LO", 2));
	assert(!jstr_endscase_len("hello", 5, "LX", 2));
	assert(jstr_endscase_len("", 0, "", 0));
	return 0;
}
