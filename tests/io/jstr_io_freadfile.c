#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int
main(int argc, char **argv)
{
	START();
	char tmpdir[] = "/tmp/jstr_frfull_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "full freadfile test data\n";
	size_t dlen = 24;
	FILE *fp = fopen(path, "w");
	assert(fp);
	assert(fwrite(data, 1, dlen, fp) == dlen);
	fclose(fp);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	struct stat st;
	assert(jstr_io_freadfile(&s, &sz, &cap, path, "r", &st) == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	free(s);
	/* bad path */
	s = NULL; sz = 0; cap = 0;
	assert(jstr_io_freadfile(&s, &sz, &cap, "/no/such/file", "r", &st) != JSTR_RET_SUCC);
	free(s);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
