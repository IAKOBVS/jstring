/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Tests for jstr_io_ftw_len and other io.h I/O functions
 * not covered by test-fuzz-io.c (readfile, writefile, expandtilde). */

#define _POSIX_C_SOURCE 200809L

#include "test.h"
#include "../include/io.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum { MAX_ENTRIES = 64 };

/* ---------- test helpers ---------- */

static char tmpdir_root[JSTR_IO_PATH_MAX];
static size_t tmpdir_root_len;

/* Append SRC (with known length SRC_LEN) to DST starting at offset OFF.
 * DST must have space.  NUL-terminates.  Returns new end offset. */
static size_t
pathcat(char *dst, size_t off, const char *src, size_t src_len)
{
	memcpy(dst + off, src, src_len);
	off += src_len;
	dst[off] = '\0';
	return off;
}

static int
setup_tree(void)
{
	memcpy(tmpdir_root, "/tmp/jstr_test_io_XXXXXX", 25);
	if (!mkdtemp(tmpdir_root)) {
		perror("mkdtemp");
		return -1;
	}
	tmpdir_root_len = strlen(tmpdir_root);

	/* Tree:
	 * root/
	 *   .hidden
	 *   file_a.txt
	 *   file_b.bin
	 *   subdir/
	 *     file_c.txt
	 *     subsubdir/
	 *       file_d.txt
	 *   empty_subdir/
	 */
	char p[JSTR_IO_PATH_MAX];
	size_t rl = tmpdir_root_len;
	memcpy(p, tmpdir_root, rl + 1);

	size_t off;
	off = pathcat(p, rl, "/subdir", 7);
	if (mkdir(p, 0755) && errno != EEXIST) return -1;
	off = pathcat(p, rl, "/subdir/subsubdir", 17);
	if (mkdir(p, 0755) && errno != EEXIST) return -1;
	off = pathcat(p, rl, "/empty_subdir", 13);
	if (mkdir(p, 0755) && errno != EEXIST) return -1;

	int fd;
	off = pathcat(p, rl, "/file_a.txt", 11);
	fd = creat(p, 0644);
	if (fd >= 0) close(fd);
	off = pathcat(p, rl, "/file_b.bin", 11);
	fd = creat(p, 0644);
	if (fd >= 0) close(fd);
	off = pathcat(p, rl, "/.hidden", 8);
	fd = creat(p, 0644);
	if (fd >= 0) close(fd);
	off = pathcat(p, rl, "/subdir/file_c.txt", 18);
	fd = creat(p, 0644);
	if (fd >= 0) close(fd);
	off = pathcat(p, rl, "/subdir/subsubdir/file_d.txt", 28);
	fd = creat(p, 0644);
	if (fd >= 0) close(fd);
	(void)off;
	return 0;
}

static void
teardown_tree(void)
{
	char cmd[JSTR_IO_PATH_MAX + 20];
	memcpy(cmd, "rm -rf '", 8);
	size_t off = 8;
	memcpy(cmd + off, tmpdir_root, tmpdir_root_len);
	off += tmpdir_root_len;
	memcpy(cmd + off, "'", 2);
	system(cmd);
}

/* ---------- ftw callback harness ---------- */

struct ftw_result {
	char paths[MAX_ENTRIES][JSTR_IO_PATH_MAX];
	size_t path_lens[MAX_ENTRIES];
	int is_dirs[MAX_ENTRIES];
	size_t count;
};

static int
collect_cb(const struct JSTR_IO_FTW *ftw, const void *args)
{
	struct ftw_result *r = (struct ftw_result *)args;
	assert(r->count < MAX_ENTRIES);
	size_t i = r->count++;
	memcpy(r->paths[i], ftw->dirpath, ftw->dirpath_len);
	r->paths[i][ftw->dirpath_len] = '\0';
	r->path_lens[i] = ftw->dirpath_len;
	r->is_dirs[i] = S_ISDIR(ftw->st->st_mode);
	return JSTR_RET_SUCC;
}

static int
skip_cb(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	(void)args;
	return JSTR_IO_FTW_RET_STOP;
}

/* match files ending in ".txt" — return 0 to accept */
static int
match_txt(const char *fname, jstr_io_path_size_ty fname_len, const void *args)
{
	(void)args;
	return fname_len < 4 || memcmp(fname + fname_len - 4, ".txt", 4) != 0;
}

/* match files ending in ".bin" */
static int
match_bin(const char *fname, jstr_io_path_size_ty fname_len, const void *args)
{
	(void)args;
	return fname_len < 4 || memcmp(fname + fname_len - 4, ".bin", 4) != 0;
}

/* ---------- ftw tests ---------- */

static void
test_basic_traversal(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		0, NULL, NULL);
	ASSERT_ERRFUNC(test_basic_traversal, ret == JSTR_RET_SUCC);
	/* root + 5 files + 3 subdirs = 9 */
	ASSERT_ERRFUNC(test_basic_traversal, r.count == 9);
}

static void
test_nohidden(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_NOHIDDEN, NULL, NULL);
	ASSERT_ERRFUNC(test_nohidden, ret == JSTR_RET_SUCC);
	/* root + 5 files (no .hidden) + 3 subdirs = 8 */
	ASSERT_ERRFUNC(test_nohidden, r.count == 8);
	for (size_t i = 0; i < r.count; ++i) {
		const char *base = r.paths[i] + tmpdir_root_len + 1;
		ASSERT_ERRFUNC(test_nohidden, strcmp(base, ".hidden") != 0);
	}
}

static void
test_reg_only(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_REG, NULL, NULL);
	ASSERT_ERRFUNC(test_reg_only, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_reg_only, r.count == 5);
	for (size_t i = 0; i < r.count; ++i)
		ASSERT_ERRFUNC(test_reg_only, !r.is_dirs[i]);
}

static void
test_dir_only(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_DIR, NULL, NULL);
	ASSERT_ERRFUNC(test_dir_only, ret == JSTR_RET_SUCC);
	/* root + subdir + subsubdir + empty_subdir = 4 */
	ASSERT_ERRFUNC(test_dir_only, r.count == 4);
	for (size_t i = 0; i < r.count; ++i)
		ASSERT_ERRFUNC(test_dir_only, r.is_dirs[i]);
}

static void
test_nosubdir(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_NOSUBDIR, NULL, NULL);
	ASSERT_ERRFUNC(test_nosubdir, ret == JSTR_RET_SUCC);
	/* root + .hidden + file_a.txt + file_b.bin + subdir + empty_subdir = 6 */
	ASSERT_ERRFUNC(test_nosubdir, r.count == 6);
}

static void
test_nosubdir_reg(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_NOSUBDIR | JSTR_IO_FTW_REG, NULL, NULL);
	ASSERT_ERRFUNC(test_nosubdir_reg, ret == JSTR_RET_SUCC);
	/* root reg files only */
	ASSERT_ERRFUNC(test_nosubdir_reg, r.count == 3);
}

static void
test_func_match_dname(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_REG, match_txt, NULL);
	ASSERT_ERRFUNC(test_func_match_dname, ret == JSTR_RET_SUCC);
	/* only .txt reg files: file_a.txt, file_c.txt, file_d.txt */
	ASSERT_ERRFUNC(test_func_match_dname, r.count == 3);
}

static void
test_func_match_path(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH, match_txt, NULL);
	ASSERT_ERRFUNC(test_func_match_path, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_func_match_path, r.count == 3);
}

static void
test_func_match_noflags(void)
{
	/* func_match without REG or DIR: filters reg files only */
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		collect_cb, &r,
		0, match_bin, NULL);
	ASSERT_ERRFUNC(test_func_match_noflags, ret == JSTR_RET_SUCC);
	/* dirs pass through, only file_b.bin passes match among reg files */
	size_t ndirs = 0, nfiles = 0;
	for (size_t i = 0; i < r.count; ++i)
		if (r.is_dirs[i])
			++ndirs;
		else
			++nfiles;
	ASSERT_ERRFUNC(test_func_match_noflags, ndirs == 4);
	ASSERT_ERRFUNC(test_func_match_noflags, nfiles == 1);
	ASSERT_ERRFUNC(test_func_match_noflags, r.count == 5);
}

static void
test_actionretval_stop(void)
{
	int ret = jstr_io_ftw_len(
		tmpdir_root, (jstr_io_path_size_ty)tmpdir_root_len,
		skip_cb, NULL,
		JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	/* stop from root callback → converted to jstr_ret_ty error in outer handler */
	ASSERT_ERRFUNC(test_actionretval_stop, jstr_chk(ret));
}

static void
test_non_dir_path(void)
{
	char path[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(path, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(path, plen, "/file_a.txt", 11);
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		path, (jstr_io_path_size_ty)plen,
		collect_cb, &r,
		0, NULL, NULL);
	ASSERT_ERRFUNC(test_non_dir_path, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_non_dir_path, r.count == 1);
	ASSERT_ERRFUNC(test_non_dir_path, !r.is_dirs[0]);
}

static void
test_reg_only_on_nonreg(void)
{
	char path[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(path, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(path, plen, "/subdir", 7);
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		path, (jstr_io_path_size_ty)plen,
		collect_cb, &r,
		JSTR_IO_FTW_REG, NULL, NULL);
	ASSERT_ERRFUNC(test_reg_only_on_nonreg, ret == JSTR_RET_SUCC);
	/* subdir is a dir, REG-only skips dir callback but still recurses
	 * into subdirs to find regular files: file_c.txt + file_d.txt = 2 */
	ASSERT_ERRFUNC(test_reg_only_on_nonreg, r.count == 2);
}

static void
test_nonfatal_err(void)
{
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	/* open(2) on a non-existent path fails → returns error */
	int ret = jstr_io_ftw_len(
		"/nonexistent_path_xyzzy", 23,
		collect_cb, &r,
		0, NULL, NULL);
	ASSERT_ERRFUNC(test_nonfatal_err, ret != JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_nonfatal_err, r.count == 0);
}

static void
test_empty_dir(void)
{
	char path[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(path, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(path, plen, "/empty_subdir", 13);
	struct ftw_result r;
	memset(&r, 0, sizeof(r));
	int ret = jstr_io_ftw_len(
		path, (jstr_io_path_size_ty)plen,
		collect_cb, &r,
		0, NULL, NULL);
	ASSERT_ERRFUNC(test_empty_dir, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_empty_dir, r.count == 1);
	ASSERT_ERRFUNC(test_empty_dir, r.is_dirs[0]);
}

/* ---------- readfile / writefile / expandtilde tests ---------- */

static void
test_writefile_readfile(void)
{
	char *r = NULL;
	size_t rsz = 0, rcap = 0;
	const char *data = "hello world\n";
	size_t dlen = 12;

	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/write_test.txt", 15);

	int ret = jstr_io_writefile_len(data, dlen, fpath, O_CREAT | O_TRUNC, 0644);
	ASSERT_ERRFUNC(test_writefile_readfile, ret == JSTR_RET_SUCC);

	ret = jstr_io_readfile_len(&r, &rsz, &rcap, fpath, O_RDONLY, dlen);
	ASSERT_ERRFUNC(test_writefile_readfile, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_writefile_readfile, rsz == dlen);
	ASSERT_ERRFUNC(test_writefile_readfile, memcmp(r, data, dlen) == 0);

	jstr_free(&r, &rsz, &rcap);
}

static void
test_writefile_append_newline(void)
{
	char *r = NULL;
	size_t rsz = 0, rcap = 0;
	const char *data = "no newline";
	size_t dlen = 10;

	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/write_nonl.txt", 15);

	int ret = jstr_io_writefile_len(data, dlen, fpath, O_CREAT | O_TRUNC, 0644);
	ASSERT_ERRFUNC(test_writefile_append_newline, ret == JSTR_RET_SUCC);

	ret = jstr_io_readfile_len(&r, &rsz, &rcap, fpath, O_RDONLY, dlen + 1);
	ASSERT_ERRFUNC(test_writefile_append_newline, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_writefile_append_newline, rsz == dlen + 1);
	ASSERT_ERRFUNC(test_writefile_append_newline, memcmp(r, data, dlen) == 0);
	ASSERT_ERRFUNC(test_writefile_append_newline, r[dlen] == '\n');

	jstr_free(&r, &rsz, &rcap);
}

static void
test_expandtilde(void)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;

	int ret = jstr_assign_len(&s, &sz, &cap, "~/file.txt", 10);
	ASSERT_ERRFUNC(test_expandtilde, ret == JSTR_RET_SUCC);

	ret = jstr_io_expandtildefirst(&s, &sz, &cap);
	ASSERT_ERRFUNC(test_expandtilde, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_expandtilde, sz > 0 && s[0] == '/');
	ASSERT_ERRFUNC(test_expandtilde, strstr(s, "/file.txt") != NULL);

	jstr_free(&s, &sz, &cap);
}

static void
test_expandtilde_no_tilde(void)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;

	int ret = jstr_assign_len(&s, &sz, &cap, "/no/tilde", 9);
	ASSERT_ERRFUNC(test_expandtilde_no_tilde, ret == JSTR_RET_SUCC);

	ret = jstr_io_expandtildefirst(&s, &sz, &cap);
	ASSERT_ERRFUNC(test_expandtilde_no_tilde, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_expandtilde_no_tilde, sz == 9);
	ASSERT_ERRFUNC(test_expandtilde_no_tilde, strcmp(s, "/no/tilde") == 0);

	jstr_free(&s, &sz, &cap);
}

static void
test_appendpath(void)
{
	char *s = NULL;
	size_t sz = 0, cap = 0;

	int ret = jstr_assign_len(&s, &sz, &cap, "/base", 5);
	ASSERT_ERRFUNC(test_appendpath, ret == JSTR_RET_SUCC);

	ret = jstr_io_appendpath_len(&s, &sz, &cap, "subdir", 6);
	ASSERT_ERRFUNC(test_appendpath, ret == JSTR_RET_SUCC);
	ASSERT_ERRFUNC(test_appendpath, sz == 12);
	ASSERT_ERRFUNC(test_appendpath, strcmp(s, "/base/subdir") == 0);
	ASSERT_ERRFUNC(test_appendpath, s[sz] == '\0');

	jstr_free(&s, &sz, &cap);
}

/* ---------- main ---------- */

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;

	if (setup_tree()) {
		PRINTERR("test-io-ftw: failed to set up temp tree\n");
		return EXIT_FAILURE;
	}

	TESTING(test_basic_traversal);
	test_basic_traversal();
	TESTING(test_nohidden);
	test_nohidden();
	TESTING(test_reg_only);
	test_reg_only();
	TESTING(test_dir_only);
	test_dir_only();
	TESTING(test_nosubdir);
	test_nosubdir();
	TESTING(test_nosubdir_reg);
	test_nosubdir_reg();
	TESTING(test_func_match_dname);
	test_func_match_dname();
	TESTING(test_func_match_path);
	test_func_match_path();
	TESTING(test_func_match_noflags);
	test_func_match_noflags();
	TESTING(test_actionretval_stop);
	test_actionretval_stop();
	TESTING(test_non_dir_path);
	test_non_dir_path();
	TESTING(test_reg_only_on_nonreg);
	test_reg_only_on_nonreg();
	TESTING(test_empty_dir);
	test_empty_dir();
	TESTING(test_writefile_readfile);
	test_writefile_readfile();
	TESTING(test_writefile_append_newline);
	test_writefile_append_newline();
	TESTING(test_expandtilde);
	test_expandtilde();
	TESTING(test_expandtilde_no_tilde);
	test_expandtilde_no_tilde();
	TESTING(test_appendpath);
	test_appendpath();
	TESTING(test_nonfatal_err);
	test_nonfatal_err();

	teardown_tree();
	SUCCESS();
	return EXIT_SUCCESS;
}
