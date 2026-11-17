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
	char src[] = "a,b,c";
	char dst[8];
	void *r = jstr_memccpy(dst, src, ',', 6);
	assert(r == dst + 2);
	assert(memcmp(dst, "a,", 2) == 0);
	/* not found */
	memset(dst, 0, 8);
	r = jstr_memccpy(dst, src, 'z', 6);
	assert(r == NULL);
	assert(memcmp(dst, src, 6) == 0);
	/* n=0 */
	r = jstr_memccpy(dst, src, ',', 0);
	assert(r == NULL);
	return 0;
}
