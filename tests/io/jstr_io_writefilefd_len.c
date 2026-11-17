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
	char tmpdir[] = "/tmp/jstr_wfd_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	/* sz == 0 success no write */
	assert(jstr_io_writefilefd_len("x", 0, fd) == JSTR_RET_SUCC);
	/* write with newline appended */
	{
		const char *s = "hello";
		size_t sz = 5;
		assert(jstr_io_writefilefd_len(s, sz, fd) == JSTR_RET_SUCC);
	}
	assert(close(fd) == 0);
	/* verify file content */
	{
		char *buf = malloc(64);
		fd = open(path, O_RDONLY);
		ssize_t r = read(fd, buf, 64);
		buf[r] = '\0';
		assert(r == 6);
		assert(strcmp(buf, "hello\n") == 0);
		close(fd);
		free(buf);
	}
	/* already ends with newline no double */
	{
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		const char *s = "hi\n";
		assert(jstr_io_writefilefd_len(s, 3, fd) == JSTR_RET_SUCC);
		close(fd);
		char buf[64];
		fd = open(path, O_RDONLY);
		ssize_t r = read(fd, buf, 64);
		buf[r] = '\0';
		assert(r == 3);
		assert(strcmp(buf, "hi\n") == 0);
		close(fd);
	}
	/* invalid fd */
	{
		assert(jstr_io_writefilefd_len(jstr_literal("x"), -1) != JSTR_RET_SUCC);
	}
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
