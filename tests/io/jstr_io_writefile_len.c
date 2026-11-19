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
	char tmpdir[] = "/tmp/jstr_wfile_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	/* create/truncate write */
	assert(jstr_io_writefile_len(jstr_literal("hello"), path, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
	{
		char buf[64];
		int fd = open(path, O_RDONLY);
		ssize_t r = read(fd, buf, 64);
		buf[r] = '\0';
		assert(r == 6);
		assert(strcmp(buf, "hello\n") == 0);
		close(fd);
	}
	/* append */
	assert(jstr_io_writefile_len(jstr_literal("world"), path, O_APPEND | O_CREAT, 0644) == JSTR_RET_SUCC);
	{
		char buf[64];
		int fd = open(path, O_RDONLY);
		ssize_t r = read(fd, buf, 64);
		buf[r] = '\0';
		/* first hello\n + world\n appended */
		assert(strstr(buf, "hello") && strstr(buf, "world"));
		close(fd);
	}
	/* missing path error */
	assert(jstr_io_writefile_len(jstr_literal("x"), "/no/such/dir/f.txt", O_CREAT | O_TRUNC, 0644) != JSTR_RET_SUCC);
	/* sz == 0 success */
	assert(jstr_io_writefile_len("x", 0, path, O_APPEND, 0644) == JSTR_RET_SUCC);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
