/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"
#include "internal/musl/ctype.h"

int
main(void)
{

	assert(jstr_internal_itoa_digit(0) == '0');
	assert(jstr_internal_itoa_digit(9) == '9');
	assert(jstr_internal_itoa_digit(10) == 'a');
	assert(jstr_internal_itoa_digit(35) == 'z');
	return 0;
}
