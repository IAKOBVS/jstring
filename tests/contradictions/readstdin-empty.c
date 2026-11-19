/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: IO-8 (empty stdin must be an error, like readsystem)
 *
 * jstr_io_readsystem() treats zero bytes as an error. jstr_io_readstdin()
 * currently treats an immediately closed stdin as success with an empty
 * string; the two helpers should agree. */

#ifndef _POSIX_C_SOURCE
#	define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <unistd.h>

#include "jstr.h"
#include "io.h"

int
main(void)
{
	int p[2];
	assert(pipe(p) == 0);
	const int saved_stdin = dup(STDIN_FILENO);
	assert(saved_stdin >= 0);
	assert(dup2(p[0], STDIN_FILENO) == 0);
	close(p[1]);

	jstr_ty s = JSTR_INIT;
	jstr_ret_ty ret = jstr_io_readstdin(jstr_struct(&s));
	assert(jstr_chk(ret));

	close(p[0]);
	assert(dup2(saved_stdin, STDIN_FILENO) == 0);
	close(saved_stdin);
	jstr_free_j(&s);
	return 0;
}
