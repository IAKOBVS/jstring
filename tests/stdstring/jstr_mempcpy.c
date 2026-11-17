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
	char src[] = "abc";
	char dst[8];
	void *r = jstr_mempcpy(dst, src, 3);
	assert(r == dst + 3);
	assert(memcmp(dst, "abc", 3) == 0);
	/* n=0 */
	assert(jstr_mempcpy(dst, src, 0) == dst);
	return 0;
}
