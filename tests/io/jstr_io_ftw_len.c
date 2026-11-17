#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int gcount;
static int cb_count(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw; (void)args; ++gcount; return 0;
}

static int cb_fail(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw; (void)args; return -1;
}

int
main(int argc, char **argv)
{
	START();
	char tmpdir[] = "/tmp/jstr_ftwl_XXXXXX";
	assert(mkdtemp(tmpdir));
	char p[256];
	snprintf(p, sizeof p, "%s/a.txt", tmpdir);
	FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/b.txt", tmpdir);
	f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

	/* empty dirpath */
	{
		int r = jstr_io_ftw_len(tmpdir, 0, cb_count, NULL, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(r != JSTR_RET_SUCC);
	}
	/* basic walk REG|DIR counts root? Let's count files+dirs visited */
	gcount = 0;
	jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	int r = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(r == 0);
	assert(gcount >= 3);
	/* fail callback propagates */
	gcount = 0;
	r = jstr_io_ftw_len(tmpdir, dlen, cb_fail, NULL, JSTR_IO_FTW_REG, NULL, NULL);
	assert(r != JSTR_RET_SUCC);
	/* trailing slash trimmed */
	gcount = 0;
	char t2[256];
	snprintf(t2, sizeof t2, "%s/", tmpdir);
	r = jstr_io_ftw_len(t2, (jstr_io_path_size_ty)strlen(t2), cb_count, NULL, JSTR_IO_FTW_REG, NULL, NULL);
	assert(r == 0);

	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
