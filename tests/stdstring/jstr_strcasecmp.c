/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strcasecmp("abc", "ABC") == 0);
	assert(jstr_strcasecmp("abc", "abd") < 0);
	assert(jstr_strcasecmp("abd", "abc") > 0);
	assert(jstr_strcasecmp("", "") == 0);
	return 0;
}
