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
	char tmpdir[] = "/tmp/jstr_wfdlj_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	jstr_ty j = JSTR_INIT;
	j.data = malloc(8);
	strcpy(j.data, "hello");
	j.size = 5;
	j.capacity = 8;
	assert(jstr_io_writefilefd_len_j(&j, fd) == JSTR_RET_SUCC);
	close(fd);
	char buf[64];
	fd = open(path, O_RDONLY);
	ssize_t r = read(fd, buf, 64);
	buf[r] = '\0';
	assert(r == 6);
	assert(strcmp(buf, "hello\n") == 0);
	close(fd);
	free(j.data);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
