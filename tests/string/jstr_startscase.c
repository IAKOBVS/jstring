/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		assert(jstr_startscase("Hello World", "hello"));
		assert(!jstr_startscase("Hello World", "world"));
		assert(jstr_startscase("Hello", ""));
	}
	return 0;
}
