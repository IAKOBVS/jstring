/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_atol("-42") == -42);
	assert(jstr_atol("0") == 0);
	assert(jstr_atol("abc") == 0);
	return 0;
}
