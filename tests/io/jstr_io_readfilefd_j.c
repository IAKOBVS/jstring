#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int
main(void)
{
	char tmpdir[] = "/tmp/jstr_rfdj_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	assert(write(fd, data, 5) == 5);
	close(fd);
	fd = open(path, O_RDONLY);
	jstr_ty j = JSTR_INIT;
	struct stat st;
	assert(jstr_io_readfilefd_j(&j, fd, &st) == JSTR_RET_SUCC);
	assert(j.size == 5);
	assert(memcmp(j.data, data, 5) == 0);
	close(fd);
	free(j.data);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
