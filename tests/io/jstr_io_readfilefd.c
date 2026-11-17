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
main(int argc, char **argv)
{
	START();
	char tmpdir[] = "/tmp/jstr_rfd_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	assert(write(fd, data, 5) == 5);
	close(fd);
	fd = open(path, O_RDONLY);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	struct stat st;
	assert(jstr_io_readfilefd(&s, &sz, &cap, fd, &st) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(st.st_size == 5);
	assert(memcmp(s, data, 5) == 0);
	close(fd);
	free(s);
	/* invalid fd */
	{
		char *s2 = NULL; size_t sz2 = 0, cap2 = 0; struct stat st2;
		assert(jstr_io_readfilefd(&s2, &sz2, &cap2, -1, &st2) != JSTR_RET_SUCC);
	}
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
