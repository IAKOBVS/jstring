/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	const char s[] = "abac";
	assert(jstr_memrchr(s, 'a', 4) == s + 2);
	assert(jstr_memrchr(s, 'c', 4) == s + 3);
	assert(jstr_memrchr(s, 'z', 4) == NULL);
	return 0;
}
