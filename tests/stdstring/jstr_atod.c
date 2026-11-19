/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_atod("3.5") == 3.5);
	assert(jstr_atod("0") == 0.0);
	assert(jstr_atod("abc") == 0.0);
	return 0;
}
