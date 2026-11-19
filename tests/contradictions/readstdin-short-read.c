/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: IO-8 (short read must be retried until EOF)
 *
 * A child writes two chunks with a delay. A single read() sees only the first
 * chunk; the old implementation treated that as EOF and lost the second one.
 * The fixed implementation keeps reading until the writer closes the pipe.
 */

#ifndef _POSIX_C_SOURCE
#	define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	close(p[0]);

	const pid_t child = fork();
	assert(child >= 0);
	if (child == 0) {
		(void)write(p[1], "hello", 5);
		sleep(1);
		(void)write(p[1], "world", 5);
		close(p[1]);
		_exit(0);
	}
	close(p[1]);

	jstr_ty s = JSTR_INIT;
	assert(!jstr_chk(jstr_io_readstdin(jstr_struct(&s))));
	assert(waitpid(child, NULL, 0) == child);

	assert(dup2(saved_stdin, STDIN_FILENO) == 0);
	close(saved_stdin);

	assert(s.size == 10);
	assert(memcmp(s.data, "helloworld", 10) == 0);
	jstr_free_j(&s);
	return 0;
}
