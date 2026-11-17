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
	const char *data = "stdin data\n";
	int p[2];
	assert(pipe(p) == 0);
	assert(write(p[1], data, strlen(data)) == (ssize_t)strlen(data));
	close(p[1]);
	int saved = dup(STDIN_FILENO);
	dup2(p[0], STDIN_FILENO);
	close(p[0]);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_readstdin(&s, &sz, &cap) == JSTR_RET_SUCC);
	assert(sz == strlen(data));
	assert(memcmp(s, data, sz) == 0);
	free(s);
	dup2(saved, STDIN_FILENO);
	close(saved);
	/* empty stdin */
	{
		int p2[2];
		assert(pipe(p2) == 0);
		close(p2[1]);
		int saved2 = dup(STDIN_FILENO);
		dup2(p2[0], STDIN_FILENO);
		close(p2[0]);
		char *s2 = NULL; size_t sz2=0, cap2=0;
		assert(jstr_io_readstdin(&s2, &sz2, &cap2) == JSTR_RET_SUCC);
		assert(sz2 == 0);
		free(s2);
		dup2(saved2, STDIN_FILENO);
		close(saved2);
	}
	SUCCESS();
	return 0;
}
