/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	{
		const char *p = jstr_linenext("abc\ndef");
		assert(p != NULL && *p == 'd');
		p = jstr_linenext("abc");
		assert(p == NULL);
		p = jstr_linenext("abc\n");
		assert(p == NULL);
	}
	return 0;
}
