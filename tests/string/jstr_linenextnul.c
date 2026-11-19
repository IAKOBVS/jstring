/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		const char *p = jstr_linenextnul("abc\ndef");
		assert(p != NULL && *p == 'd');
		p = jstr_linenextnul("abc");
		assert(p != NULL && *p == '\0');
		p = jstr_linenextnul("abc\n");
		assert(p != NULL && *p == '\n');
	}
	return 0;
}
