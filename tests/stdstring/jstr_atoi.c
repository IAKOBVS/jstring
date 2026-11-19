/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_atoi("-42") == -42);
	assert(jstr_atoi("0") == 0);
	assert(jstr_atoi("abc") == 0);
	return 0;
}
