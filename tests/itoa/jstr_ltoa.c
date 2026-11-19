/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"
#include "../test.h"

int
main(void)
{

	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_ltoa(&s, &sz, &cap, -123, 10) == JSTR_RET_SUCC);
	assert(strcmp(s, "-123") == 0);
	assert(jstr_ltoa(&s, &sz, &cap, 0, 10) == JSTR_RET_SUCC);
	/* invalid base */
	assert(jstr_ltoa(&s, &sz, &cap, 1, 1) == JSTR_RET_ERR);
	jstr_free(&s, &sz, &cap);
	return 0;
}
