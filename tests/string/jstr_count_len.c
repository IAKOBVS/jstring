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
	assert(jstr_count_len("abcabc", 6, "abc", 3) == 2);
	assert(jstr_count_len("abcabc", 6, "xyz", 3) == 0);
	assert(jstr_count_len("", 0, "abc", 3) == 0);
	return 0;
}
