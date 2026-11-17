#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "jstr.h"

int
main(void)
{
	const pid_t pid = fork();
	assert(pid >= 0);
	if (pid == 0) {
		jstr_errdie("expected error %d\n", 1);
		_exit(EXIT_SUCCESS);
	}
	int status = 0;
	assert(waitpid(pid, &status, 0) == pid);
	assert(WIFEXITED(status));
	assert(WEXITSTATUS(status) == EXIT_FAILURE);
	return 0;
}
