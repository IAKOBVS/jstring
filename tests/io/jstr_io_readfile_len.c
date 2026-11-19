#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main(void)
{
	char tmpdir[] = "/tmp/jstr_rfl_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	assert(write(fd, data, 5) == 5);
	close(fd);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_readfile_len(&s, &sz, &cap, path, O_RDONLY, 5) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(memcmp(s, data, 5) == 0);
	free(s);
	/* error bad path */
	s = NULL;
	sz = 0;
	cap = 0;
	assert(jstr_io_readfile_len(&s, &sz, &cap, "/no/such/file", O_RDONLY, 5) != JSTR_RET_SUCC);
	free(s);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
