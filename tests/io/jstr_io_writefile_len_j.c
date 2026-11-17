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
	jstr_ty j = JSTR_INIT;
	j.data = malloc(8);
	strcpy(j.data, "hello");
	j.size = 5;
	j.capacity = 8;
	char tmpdir[] = "/tmp/jstr_wflj_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	assert(jstr_io_writefile_len_j(&j, path, O_CREAT | O_TRUNC, 0644) == JSTR_RET_SUCC);
	FILE *fp = fopen(path, "r");
	char buf[64];
	size_t r = fread(buf, 1, 64, fp);
	buf[r] = '\0';
	assert(r == 6);
	assert(strcmp(buf, "hello\n") == 0);
	fclose(fp);
	free(j.data);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
