/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	const char *p = (const char *)jstr_memmem("abXcd", 5, "X", 1);
	assert(p && *p == 'X');
	assert(jstr_memmem("abcd", 4, "xyz", 3) == NULL);
	assert(jstr_memmem("aaaa", 4, "", 0) != NULL);
	return 0;
}
