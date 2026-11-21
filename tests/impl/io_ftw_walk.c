/* SPDX-License-Identifier: MIT */
/* Direct tests for the walk/len internal functions documented by the
 * @anchor spec_jstr_internal_io_ftw_walk and
 * @anchor spec_jstr_internal_io_ftw_len spec comments in include/io.h.
 * These statics are only visible when JSTR_IMPLEMENTATION is defined. */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "jstr.h"
#include "io.h"

struct fixture {
	char root[128];
	char sub[160];
	char deep[200];
	char file[240];
	char walkpath[512];
	struct stat st;
};

static void
fixture_make(struct fixture *f)
{
	char tpl[] = "/tmp/jstr_ftw_walk_XXXXXX";
	assert(mkdtemp(tpl) != NULL);
	assert(strlen(tpl) < sizeof(f->root));
	strcpy(f->root, tpl);
	assert(snprintf(f->sub, sizeof(f->sub), "%s/sub", f->root) > 0);
	assert(snprintf(f->deep, sizeof(f->deep), "%s/deep", f->sub) > 0);
	assert(snprintf(f->file, sizeof(f->file), "%s/x", f->deep) > 0);
	assert(mkdir(f->sub, 0755) == 0);
	assert(mkdir(f->deep, 0755) == 0);
	FILE *fp = fopen(f->file, "w");
	assert(fp != NULL);
	fclose(fp);
}

static void
fixture_free(struct fixture *f)
{
	assert(unlink(f->file) == 0);
	assert(rmdir(f->deep) == 0);
	assert(rmdir(f->sub) == 0);
	assert(rmdir(f->root) == 0);
}

static int
count_fds(void)
{
	DIR *d = opendir("/proc/self/fd");
	if (d == NULL)
		return -1;
	int n = 0;
	while (readdir(d) != NULL)
		++n;
	closedir(d);
	return n;
}

static void
init_data(struct jstr_internal_io_ftw_data *a, struct fixture *f, jstr_io_ftw_func_ty func, const void *args, int flags)
{
	memset(a, 0, sizeof(*a));
	a->func = func;
	a->func_args = args;
	strcpy(f->walkpath, f->root);
	a->ftw.dirpath = f->walkpath;
	a->ftw.dirpath_len = (jstr_io_path_size_ty)strlen(f->walkpath);
	a->ftw.st = &f->st;
	a->ftw_flags = flags;
}

static void
assert_ownership(const struct jstr_internal_io_ftw_data *a, struct fixture *f, jstr_io_ftw_func_ty func, const void *args)
{
	assert(a->func == func);
	assert(a->func_args == args);
	assert(a->ftw.dirpath == f->walkpath);
	assert(a->ftw.dirpath_len <= (jstr_io_path_size_ty)sizeof(f->walkpath));
}

static int
cb_stop(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	++*(int *)args;
	return JSTR_IO_FTW_RET_STOP;
}

static int
cb_continue(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	++*(int *)args;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int
cb_error(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	++*(int *)args;
	return -1;
}

static int
open_rootdir(const struct jstr_internal_io_ftw_data *a)
{
	int dirfd = open(a->ftw.dirpath, O_RDONLY);
	assert(dirfd >= 0);
	return dirfd;
}

static void
test_walk_stop(void)
{
	struct fixture f;
	fixture_make(&f);
	int calls = 0;
	struct jstr_internal_io_ftw_data a;
	init_data(&a, &f, cb_stop, &calls, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL);
	jstr_internal_io_ftw_state_ty st = JSTR_INTERNAL_IO_FTW_FINISH;
	int before = count_fds();
	int dirfd = open_rootdir(&a);
	int ret = jstr_internal_io_ftw_walk(&a, a.ftw.dirpath_len, dirfd, &st);
	int after = count_fds();
	assert(ret == JSTR_RET_SUCC);
	assert(st == JSTR_INTERNAL_IO_FTW_STOP);
	assert(calls >= 1);
	if (before >= 0 && after >= 0)
		assert(before == after);
	assert_ownership(&a, &f, cb_stop, &calls);
	fixture_free(&f);
}

static void
test_walk_finish(void)
{
	struct fixture f;
	fixture_make(&f);
	int calls = 0;
	struct jstr_internal_io_ftw_data a;
	init_data(&a, &f, cb_continue, &calls, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR);
	jstr_internal_io_ftw_state_ty st = JSTR_INTERNAL_IO_FTW_FINISH;
	int before = count_fds();
	int dirfd = open_rootdir(&a);
	int ret = jstr_internal_io_ftw_walk(&a, a.ftw.dirpath_len, dirfd, &st);
	int after = count_fds();
	assert(ret == JSTR_RET_SUCC);
	assert(st == JSTR_INTERNAL_IO_FTW_FINISH);
	assert(calls >= 2);
	if (before >= 0 && after >= 0)
		assert(before == after);
	assert_ownership(&a, &f, cb_continue, &calls);
	fixture_free(&f);
}

static void
test_walk_error(void)
{
	struct fixture f;
	fixture_make(&f);
	int calls = 0;
	struct jstr_internal_io_ftw_data a;
	init_data(&a, &f, cb_error, &calls, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR);
	jstr_internal_io_ftw_state_ty st = JSTR_INTERNAL_IO_FTW_FINISH;
	int before = count_fds();
	int dirfd = open_rootdir(&a);
	int ret = jstr_internal_io_ftw_walk(&a, a.ftw.dirpath_len, dirfd, &st);
	int after = count_fds();
	assert(ret == JSTR_RET_ERR);
	assert(st == JSTR_INTERNAL_IO_FTW_ERROR);
	assert(calls >= 1);
	if (before >= 0 && after >= 0)
		assert(before == after);
	assert_ownership(&a, &f, cb_error, &calls);
	fixture_free(&f);
}

static void
test_ftw_len_direct(void)
{
	struct fixture f;
	fixture_make(&f);
	int calls = 0;
	struct jstr_internal_io_ftw_data a;
	init_data(&a, &f, cb_stop, &calls, JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL);
	int before = count_fds();
	int dirfd = open_rootdir(&a);
	int ret = jstr_internal_io_ftw_len(&a, a.ftw.dirpath_len, dirfd);
	int after = count_fds();
	assert(ret == JSTR_RET_SUCC);
	assert(calls >= 1);
	if (before >= 0 && after >= 0)
		assert(before == after);
	assert_ownership(&a, &f, cb_stop, &calls);
	fixture_free(&f);
}

int
main(void)
{
	test_walk_stop();
	test_walk_finish();
	test_walk_error();
	test_ftw_len_direct();
	return 0;
}
