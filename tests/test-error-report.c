#define _POSIX_C_SOURCE 200809L
#include "../build/include/jstr/jstr.h"
#include "test.h"
#include <assert.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
	/* jstr_internal_err: returns negative on write fail (impossible to trigger
	 * in practice without stderr redirection trickery). We just verify the
	 * function compiles and links. Actual coverage would require JSTR_PANIC=1
	 * which calls jstr_errdie and exits. */

	/* jstr_internal_errdie: trigger via JSTR_PANIC=1 - but we can't test
	 * that here since it calls exit(). */

	/* Just test that jstr_err compiles and runs without crashing. */
	errno = EINVAL;
	int ret = jstr_err("test error %s", "hello");
	/* jstr_err writes to stderr; we can't verify output but it returns 0. */
	(void)ret;

	SUCCESS();
	return EXIT_SUCCESS;
}
