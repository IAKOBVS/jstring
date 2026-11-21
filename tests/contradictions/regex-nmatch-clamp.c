/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: RE-6 (defensive nmatch clamp)
 *
 * The fixed-size regmatch_t rm[10] can hold at most ten match slots. Passing a
 * larger nmatch must be clamped rather than forwarded to regexec, which would
 * write past the array. nmatch = 1000 makes the pre-fix failure reliable. */

#define __STDC_NO_VLA__ 1

#include <assert.h>
#include <regex.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "jstr.h"
#include "regex.h"

int
main(void)
{
	jstr_re_ty preg;
	assert(!jstr_re_chkcomp(jstr_re_comp(&preg, "\\(a\\)", 0)));

	const size_t values[] = { 10, 11, 1000, SIZE_MAX };
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
		char buf[32] = "aa";
		char *p = buf;
		size_t sz = 2;
		size_t cap = sizeof(buf);

		jstr_re_off_ty ret = jstr_re_rplcall_backref_len_exec(&preg, &p, &sz, &cap, "\\1\\1", 4, 0, values[i]);
		assert(!jstr_chk(ret));
		assert(sz == 4);
		assert(memcmp(p, "aaaa", 4) == 0);
	}

	jstr_re_free(&preg);
	return 0;
}
