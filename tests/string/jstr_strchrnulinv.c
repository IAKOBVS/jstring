/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		const char *p = jstr_strchrnulinv("aaaabc", 'a');
		assert(*p == 'b');
		p = jstr_strchrnulinv("aaaa", 'a');
		assert(*p == '\0');
		p = jstr_strchrnulinv("abc", '\0');
		assert(p == (const char *)"abc");
	}
	return 0;
}
