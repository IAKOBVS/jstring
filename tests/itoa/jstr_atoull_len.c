/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[] = "12345";
	assert(jstr_atoull_len(buf, 0) == 0);
	assert(jstr_atoull_len(buf, 3) == 123);
	assert(jstr_atoull_len(buf, 5) == 12345);
	char buf2[] = "0";
	assert(jstr_atoull_len(buf2, 1) == 0);
	return 0;
}
