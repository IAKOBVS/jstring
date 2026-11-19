#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	char tmpdir[] = "/tmp/jstr_frfj_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	const char *data = "hello";
	FILE *fp = fopen(path, "w");
	assert(fp);
	assert(fwrite(data, 1, 5, fp) == 5);
	fclose(fp);
	jstr_ty j = JSTR_INIT;
	assert(jstr_io_freadfile_len_j(&j, path, "r", 5) == JSTR_RET_SUCC);
	assert(j.size == 5);
	assert(memcmp(j.data, data, 5) == 0);
	free(j.data);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
