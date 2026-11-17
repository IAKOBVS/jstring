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
	char tmpdir[] = "/tmp/jstr_fwfile_XXXXXX";
	assert(mkdtemp(tmpdir));
	char path[256];
	snprintf(path, sizeof path, "%s/f.txt", tmpdir);
	assert(jstr_io_fwritefile_len(jstr_literal("hello"), path, "w") == JSTR_RET_SUCC);
	{
		FILE *fp = fopen(path, "r");
		char buf[64];
		size_t r = fread(buf, 1, 64, fp);
		buf[r] = '\0';
		assert(r == 6);
		assert(strcmp(buf, "hello\n") == 0);
		fclose(fp);
	}
	/* append mode */
	assert(jstr_io_fwritefile_len(jstr_literal("world"), path, "a") == JSTR_RET_SUCC);
	/* error on bad path */
	assert(jstr_io_fwritefile_len(jstr_literal("x"), "/no/such/dir/f.txt", "w") != JSTR_RET_SUCC);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
