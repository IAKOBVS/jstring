/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char src[] = "abc";
	char dst[8];
	void *r = jstr_mempmove(dst, src, 3);
	assert(r == dst + 3);
	assert(memcmp(dst, "abc", 3) == 0);
	/* overlapping */
	char buf[8] = "abcd";
	jstr_mempmove(buf + 1, buf, 3);
	assert(memcmp(buf, "aabc", 4) == 0);
	return 0;
}
