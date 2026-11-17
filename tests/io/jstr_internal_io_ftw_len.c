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

int
main(int argc, char **argv)
{
	START();
	(void)jstr_internal_io_ftw_len;
	char tmpdir[] = "/tmp/jstr_iftwl_XXXXXX";
	assert(mkdtemp(tmpdir));
	char p[256];
	snprintf(p, sizeof p, "%s/a.txt", tmpdir);
	FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
	gcount = 0;
	jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	int r = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL, JSTR_IO_FTW_REG, NULL, NULL);
	assert(r == 0);
	assert(gcount >= 1);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	SUCCESS();
	return 0;
}
