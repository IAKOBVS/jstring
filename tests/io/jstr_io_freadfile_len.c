#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	char tmpdir[] = "/tmp/jstr_frf_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	FILE *fp = fopen(path, "w");
	assert(fp);
	assert(fwrite(data, 1, 5, fp) == 5);
	fclose(fp);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_freadfile_len(&s, &sz, &cap, path, "r", 5) == JSTR_RET_SUCC);
	assert(sz == 5);
	assert(memcmp(s, data, 5) == 0);
	free(s);
	/* error bad path */
	s = NULL; sz = 0; cap = 0;
	assert(jstr_io_freadfile_len(&s, &sz, &cap, "/no/such/file", "r", 5) != JSTR_RET_SUCC);
	free(s);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
