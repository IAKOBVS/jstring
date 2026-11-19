#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int gcount;
static int
cb_count(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	(void)args;
	++gcount;
	return 0;
}

int
main(void)
{
	(void)jstr_internal_io_ftw;
	char tmpdir[] = "/tmp/jstr_iftw_XXXXXX";
	assert(mkdtemp(tmpdir));
	char p[256];
	snprintf(p, sizeof p, "%s/a.txt", tmpdir);
	FILE *f = fopen(p, "w");
	assert(f);
	fputc('x', f);
	fclose(f);
	gcount = 0;
	int r = jstr_io_ftw(tmpdir, cb_count, NULL, JSTR_IO_FTW_REG, NULL, NULL);
	assert(r == 0);
	assert(gcount >= 1);
	char cmd[256];
	snprintf(cmd, sizeof cmd, "rm -rf %s", tmpdir);
	system(cmd);
	return 0;
}
