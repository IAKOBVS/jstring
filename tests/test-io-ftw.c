#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "jstr.h"
#include "io.h"

static int count;
static int ftw_cb_all(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++count;
	return 0;
}

int main(void) {
	char tmpdir[] = "/tmp/jstr_ftw_XXXXXX";
	if (!mkdtemp(tmpdir)) {
		perror("mkdtemp");
		return 1;
	}
	/* Create: a, sub/, sub/b */
	char p[256];
	snprintf(p, sizeof p, "%s/a", tmpdir);
	FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/b", tmpdir);
	f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

	jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	int ret;

	/* Walk with REG|DIR, no filter */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(count == 4);

	snprintf(p, sizeof p, "rm -rf %s", tmpdir);
	system(p);
	return 0;
}
