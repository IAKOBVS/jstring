/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	assert(jstr_strcasecmpeq_len("abc", "ABC", 3) == 0);
	assert(jstr_strcasecmpeq_len("abc", "abd", 3) != 0);
	assert(jstr_strcasecmpeq_len("a", "b", 0) == 0);
	return 0;
}
