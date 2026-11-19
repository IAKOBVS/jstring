/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: BU-2 / RP-7 (reserve growth transactional failure)
 *
 * jstr_reserveexactalways() writes *cap = new_cap before realloc. On failure
 * the buffer keeps the old pointer while capacity claims the new size, so the
 * caller sees data == NULL with a huge capacity and later writes go out of
 * bounds. This repro forces the failure without dereferencing the result.
 *
 * EXPECTED TO FAIL until reserve failure is made transactional. */

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
