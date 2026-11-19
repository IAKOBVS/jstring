/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{

	char buf[64];
	jstr_toupperstr_len(NULL, 0);
	strcpy(buf, "abc");
	jstr_toupperstr_len(buf, 0);
	assert(strcmp(buf, "abc") == 0);
	buf[0] = 'a';
	buf[1] = '\0';
	buf[2] = 'b';
	buf[3] = 'c';
	jstr_toupperstr_len(buf, 4);
	assert(buf[0] == 'A' && buf[1] == '\0' && buf[2] == 'B' && buf[3] == 'C');
	char b2[8];
	strcpy(b2, "abc");
	jstr_toupperstr_len(b2, 3);
	assert(strcmp(b2, "ABC") == 0);
	return 0;
}
