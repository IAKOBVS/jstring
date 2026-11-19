/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char dst[64];
	assert(jstr_tolowerstrcpy_p(dst, "") == dst);
	assert(strcmp(dst, "") == 0);
	char *r = jstr_tolowerstrcpy_p(dst, "aBc");
	assert(r == dst + 3);
	assert(strcmp(dst, "abc") == 0);
	return 0;
}
