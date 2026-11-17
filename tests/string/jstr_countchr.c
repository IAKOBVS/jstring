/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	assert(jstr_countchr("abacaba", 'a') == 4);
	assert(jstr_countchr("", 'a') == 0);
	assert(jstr_countchr("zzz", 'z') == 3);
	return 0;
}
