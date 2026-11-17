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
	char tmpdir[] = "/tmp/jstr_frfullj_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	FILE *fp = fopen(path, "w");
	assert(fp);
	assert(fwrite(data, 1, 5, fp) == 5);
	fclose(fp);
	jstr_ty j = JSTR_INIT;
	struct stat st;
	assert(jstr_io_freadfile_j(&j, path, "r", &st) == JSTR_RET_SUCC);
	assert(j.size == 5);
	assert(memcmp(j.data, data, 5) == 0);
	free(j.data);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
