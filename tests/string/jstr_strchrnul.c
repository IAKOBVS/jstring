/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{

	assert(jstr_strchrnul("abc", 'b') == (char *)"abc" + 1);
	assert(jstr_strchrnul("abc", 'z') == (char *)"abc" + 3);
	assert(jstr_strchrnul("", 'z') == (char *)"");
	return 0;
}
