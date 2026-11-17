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
	const char s[] = "aBcD";
	assert(jstr_memcasechr(s, 'b', 4) == s + 1);
	assert(jstr_memcasechr(s, 'd', 4) == s + 3);
	assert(jstr_memcasechr(s, 'z', 4) == NULL);
	return 0;
}
