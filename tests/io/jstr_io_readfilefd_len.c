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
	char tmpdir[] = "/tmp/jstr_rfdl_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	size_t dlen = 5;
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	assert(write(fd, data, dlen) == (ssize_t)dlen);
	close(fd);
	/* read exact */
	fd = open(path, O_RDONLY);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_readfilefd_len(&s, &sz, &cap, fd, dlen) == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	assert(s[dlen] == '\0');
	close(fd);
	free(s);
	/* file_size 0 */
	fd = open(path, O_RDONLY);
	s = NULL; sz = 0; cap = 0;
	/* use empty file */
	char epath[256];
	snprintf(epath, sizeof epath, "%s/empty.txt", tmpdir);
	fd = open(epath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	close(fd);
	fd = open(epath, O_RDONLY);
	assert(jstr_io_readfilefd_len(&s, &sz, &cap, fd, 0) == JSTR_RET_SUCC);
	assert(sz == 0);
	assert(s[0] == '\0');
	close(fd);
	free(s);
	/* premature EOF: request more than available */
	fd = open(path, O_RDONLY);
	s = NULL; sz = 0; cap = 0;
	assert(jstr_io_readfilefd_len(&s, &sz, &cap, fd, dlen + 5) != JSTR_RET_SUCC);
	close(fd);
	free(s);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
