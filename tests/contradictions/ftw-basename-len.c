/* SPDX-License-Identifier: MIT */
/* STATE_MACHINES.md: IO-2 (FTW func_match basename length uses the wrong base)
 *
 * For a non-directory root, io.h computes:
 *   dirpath = jstr_memrchr(fulpath, '/', dirpath_len);
 *   end = dirpath + dirpath_len;          <-- wrong base
 * After *++dirpath, end - dirpath equals dirpath_len - 1, not the basename
 * length. A root file named "a" should yield fname_len == 1.
 *
 * EXPECTED TO FAIL until end is computed from fulpath. */

#ifndef _POSIX_C_SOURCE
#	define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jstr.h"
#include "io.h"

static jstr_io_path_size_ty seen_len;
static int seen_call;

static int
cb_noop(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	(void)args;
	return 0;
}

static int
match_record_len(const char *fname, jstr_io_path_size_ty fname_len, const void *args)
{
	(void)fname;
	(void)args;
	seen_len = fname_len;
	seen_call = 1;
	return 0;
}

int
main(void)
{
	char tmpdir[] = "/tmp/jstr_contradiction_basename_XXXXXX";
	assert(mkdtemp(tmpdir) != NULL);
	char path[256];
	assert(snprintf(path, sizeof(path), "%s/a", tmpdir) > 0);
	FILE *f = fopen(path, "w");
	assert(f != NULL);
	fputc('x', f);
	fclose(f);

	int ret = jstr_io_ftw_len(path, (jstr_io_path_size_ty)strlen(path), cb_noop, NULL, JSTR_IO_FTW_REG, match_record_len, NULL);
	assert(!jstr_chk(ret));
	assert(seen_call);
	assert(seen_len == (jstr_io_path_size_ty)1);

	assert(unlink(path) == 0);
	assert(rmdir(tmpdir) == 0);
	return 0;
}
