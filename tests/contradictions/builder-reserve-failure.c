/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: BU-2 / RP-7 (reserve growth transactional failure)
 *
 * Regression test: a failed jstr_reserveexactalways() must leave *cap and *s
 * unchanged. The original implementation wrote *cap before realloc, so a
 * failed allocation left data == NULL with a huge capacity. ASan's
 * allocator_may_return_null makes the failure path observable. */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include "jstr.h"

/* Let ASan return NULL for the deliberately impossible allocation so the test
 * observes the library's error path rather than ASan's own abort. */
const char *
__asan_default_options(void)
{
	return "allocator_may_return_null=1";
}

int
main(void)
{
	char *s = NULL;
	size_t sz = 0;
	size_t cap = 0;

	jstr_ret_ty ret = jstr_reserveexactalways(&s, &sz, &cap, (size_t)-1);
	assert(jstr_chk(ret));
	assert(s == NULL);
	assert(cap == 0);
	return 0;
}
