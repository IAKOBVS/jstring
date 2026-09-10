#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "jstr.h"
#include "io.h"

static int count;
static int ftw_cb_all(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++count;
	return 0;
}

static int ftw_cb_count_stat(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	if (S_ISREG(ftw->st->st_mode))
		++count;
	return 0;
}

static int ftw_cb_fail(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	return -1;
}

static int ftw_cb_stop_on_dir(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	if (S_ISDIR(ftw->st->st_mode))
		return JSTR_IO_FTW_RET_STOP;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_cb_invalid_ret(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	return 42;
}

static int ftw_cb_fail_deep(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	if (S_ISREG(ftw->st->st_mode) && ftw->dirpath_len > 4)
		return -1;
	return 0;
}

static int ftw_match_skip_a(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name_len;
	(void)args;
	return name[0] == 'a' ? 1 : 0;
}

static int ftw_match_skip_a_matchpath(const char *path, jstr_io_path_size_ty path_len, const void *args) {
	(void)path_len;
	(void)args;
	size_t plen = strlen(path);
	return plen > 0 && path[plen - 1] == 'a' ? 1 : 0;
}

static int ftw_match_accept_all(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name;
	(void)name_len;
	(void)args;
	return 0;
}

int main(void) {
	char tmpdir[] = "/tmp/jstr_ftw2_XXXXXX";
	if (!mkdtemp(tmpdir)) {
		perror("mkdtemp");
		return 1;
	}

	jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	int ret;
	char p[512];

	/* Create: a (file), sub/ (dir), sub/b (file), link (symlink to a) */
	snprintf(p, sizeof p, "%s/a", tmpdir);
	FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/b", tmpdir);
	f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);
	snprintf(p, sizeof p, "%s/link", tmpdir);
	assert(symlink("a", p) == 0);

	/* Test 1: STATREG - stat regular files, process directories */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_count_stat, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_STATREG, NULL, NULL);
	assert(ret == 0);
	/* root(dir) + a(reg) + sub(dir) + b(reg) + link(reg-like) = 3 reg */
	assert(count == 3);

	/* Test 2: Symlink processed as reg file with REG|DIR */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	/* root + a + sub + b + link = 5 */
	assert(count == 5);

	/* Test 3: Symlink + only REG */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG, NULL, NULL);
	assert(ret == 0);
	/* root(dir) + a + link(reg-like) = 3; sub not visited */
	assert(count == 3);

	/* Test 4: func returns -1 (error) without ACTIONRETVAL for reg file */
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_fail, NULL,
	    JSTR_IO_FTW_REG, NULL, NULL);
	assert(ret == -1);

	/* Test 4b: error only on regular files deeper than the root */
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_fail_deep, NULL,
	    JSTR_IO_FTW_REG, NULL, NULL);
	assert(ret == -1);

	/* Test 5: func stops on directory with ACTIONRETVAL */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_stop_on_dir, NULL,
	    JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == JSTR_IO_FTW_RET_STOP);

	/* Test 6: func returns invalid ACTIONRETVAL value at root */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_invalid_ret, NULL,
	    JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == -1);

	/* Test 7: func_match + MATCHPATH - skip entries ending with 'a' */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH,
	    ftw_match_skip_a_matchpath, NULL);
	assert(ret == 0);
	/* a ends with 'a' → skipped; b and link → counted */
	assert(count == 2);

	/* Test 8: func_match without MATCHPATH */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG,
	    ftw_match_skip_a, NULL);
	assert(ret == 0);
	/* a is skipped, b and link processed */
	assert(count == 2);

	/* Test 8b: func_match that accepts everything (identical to NULL) */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG,
	    ftw_match_accept_all, NULL);
	assert(ret == 0);
	/* root skipped, a + link processed (same as Test 3) */
	assert(count == 3);

	/* Test 9: Very long dirpath (ENAMETOOLONG) */
	char longpath[4097];
	memset(longpath, 'a', 4096);
	longpath[4096] = '\0';
	ret = jstr_io_ftw_len(longpath, 4096, ftw_cb_all, NULL, 0, NULL, NULL);
	assert(ret == -1);

	/* Test 10: dirpath is "/" with NOSUBDIR to avoid ELOOP */
	count = 0;
	ret = jstr_io_ftw_len("/", 1, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_NOSUBDIR, NULL, NULL);
	assert(ret == 0);
	assert(count >= 0);

	/* Test 11: non-directory file path + func_match */
	snprintf(p, sizeof p, "%s/a", tmpdir);
	dlen = (jstr_io_path_size_ty)strlen(p);
	count = 0;
	ret = jstr_io_ftw_len(p, dlen, ftw_cb_all, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH,
	    ftw_match_skip_a_matchpath, NULL);
	assert(ret == 0);
	assert(count == 0);

	/* Test 12: func error at root (non-ACTIONRETVAL) */
	ret = jstr_io_ftw_len(tmpdir, strlen(tmpdir), ftw_cb_fail, NULL,
	    JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == -1);

	/* Test 13: ACTIONRETVAL root func returns STOP */
	count = 0;
	ret = jstr_io_ftw_len(tmpdir, strlen(tmpdir), ftw_cb_stop_on_dir, NULL,
	    JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == JSTR_IO_FTW_RET_STOP);

	snprintf(p, sizeof p, "rm -rf %s", tmpdir);
	system(p);
	return 0;
}
