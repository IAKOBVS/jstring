/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../test.h"

int
main(void)
{

	char buf[64];
	strcpy(buf, "hi");
	size_t sz = 2;

	char *p = jstr_pushfrontn_len_unsafe_p(buf, sz, 'z', 3);
	/* shifts existing content, fills front with z */
	assert(memcmp(buf, "zzzhi", 6) == 0);
	assert(p == buf + sz + 3);

	/* sz == 0: writes NUL at buf[n], fills front */
	char buf2[64];
	char *p2 = jstr_pushfrontn_len_unsafe_p(buf2, 0, 'x', 2);
	assert(buf2[2] == '\0');
	assert(memcmp(buf2, "xx", 3) == 0);
	assert(p2 == buf2 + 2);

	return EXIT_SUCCESS;
}
