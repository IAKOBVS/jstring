/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(int argc, char **argv)
{
	(void)argc; (void)argv;
	char a[] = "abc";
	char b[] = "ABC";
	assert(jstr_memcasecmpeq(a, b, 3) == 0);
	char c[] = "abd";
	assert(jstr_memcasecmpeq(a, c, 3) == 1);
	/* n=0 */
	assert(jstr_memcasecmpeq(a, b, 0) == 0);
	/* embedded NULs compared */
	char d[] = "a\0c";
	char e[] = "A\0C";
	assert(jstr_memcasecmpeq(d, e, 3) == 0);
	return 0;
}
