#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
	START();
	const char *data = "stdin j\n";
	int p[2];
	assert(pipe(p) == 0);
	assert(write(p[1], data, strlen(data)) == (ssize_t)strlen(data));
	close(p[1]);
	int saved = dup(STDIN_FILENO);
	dup2(p[0], STDIN_FILENO);
	close(p[0]);
	jstr_ty j = JSTR_INIT;
	assert(jstr_io_readstdin_j(&j) == JSTR_RET_SUCC);
	assert(j.size == strlen(data));
	assert(memcmp(j.data, data, j.size) == 0);
	free(j.data);
	dup2(saved, STDIN_FILENO);
	close(saved);
	SUCCESS();
	return 0;
}
