/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: BU-5 (jstr_at debug/release divergence)
 *
 * The "debug" definition of jstr_at parses as
 *   (i <= size ? data + i : assert(...)), data
 * so the comma operator discards the computed address and always yields data.
 * It is guarded by #ifdef JSTR_DEBUG, and config.h defines JSTR_DEBUG to 0,
 * so this broken macro is active even in default builds.
 *
 * EXPECTED TO FAIL until the macro is parenthesized and the #ifdef is fixed. */

#include <assert.h>
#include <stddef.h>

#include "jstr.h"

int
main(void)
{
	jstr_ty j = JSTR_INIT;
	assert(!jstr_chk(jstr_assign_len(jstr_struct(&j), "abcdef", 6)));
	assert(jstr_at(&j, 4) == j.data + 4);
	jstr_free_j(&j);
	return 0;
}
