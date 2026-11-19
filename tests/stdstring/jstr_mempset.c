/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[8];
	void *r = jstr_mempset(buf, 'A', 4);
	assert(r == buf + 4);
	assert(memcmp(buf, "AAAA", 4) == 0);
	/* n=0 */
	assert(jstr_mempset(buf, 'B', 0) == buf);
	return 0;
}
