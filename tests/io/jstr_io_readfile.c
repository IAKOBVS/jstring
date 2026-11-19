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
	char tmpdir[] = "/tmp/jstr_rf_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello\n";
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(fd >= 0);
	assert(write(fd, data, 6) == 6);
	close(fd);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_readfile(&s, &sz, &cap, path, O_RDONLY) == JSTR_RET_SUCC);
	assert(sz == 6);
	assert(memcmp(s, data, 6) == 0);
	free(s);
	/* empty file */
	{
		char epath[256];
		snprintf(epath, sizeof epath, "%s/empty.txt", tmpdir);
		fd = open(epath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		close(fd);
		char *s2 = NULL;
		size_t sz2 = 0, cap2 = 0;
		assert(jstr_io_readfile(&s2, &sz2, &cap2, epath, O_RDONLY) == JSTR_RET_SUCC);
		assert(sz2 == 0);
		assert(s2[0] == '\0');
		free(s2);
	}
	/* missing path */
	{
		char *s2 = NULL;
		size_t sz2 = 0, cap2 = 0;
		assert(jstr_io_readfile(&s2, &sz2, &cap2, "/no/such/file", O_RDONLY) != JSTR_RET_SUCC);
		free(s2);
	}
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
