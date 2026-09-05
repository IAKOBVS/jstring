/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com>
 *
 * Tests for uncovered code paths in io.h: binary signature detection,
 * FILE* read/write functions, readfilefd, readfile, expandtilde unsafe,
 * writefilefd_len with zero size. */

#define _POSIX_C_SOURCE 200809L

#include "test.h"
#include "../include/io.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char tmpdir_root[JSTR_IO_PATH_MAX];
static size_t tmpdir_root_len;

static int
setup(void)
{
	memcpy(tmpdir_root, "/tmp/jstr_io_extra_XXXXXX", 26);
	if (!mkdtemp(tmpdir_root)) {
		perror("mkdtemp");
		return -1;
	}
	tmpdir_root_len = strlen(tmpdir_root);
	return 0;
}

static void
teardown(void)
{
	char cmd[JSTR_IO_PATH_MAX + 20];
	size_t off = 0;
	memcpy(cmd + off, "rm -rf '", 8);
	off += 8;
	memcpy(cmd + off, tmpdir_root, tmpdir_root_len);
	off += tmpdir_root_len;
	cmd[off] = '\'';
	cmd[off + 1] = '\0';
	system(cmd);
}

static size_t
pathcat(char *dst, size_t off, const char *src, size_t src_len)
{
	memcpy(dst + off, src, src_len);
	off += src_len;
	dst[off] = '\0';
	return off;
}

/* ---------- binary signature tests ---------- */

static void
test_elf_signature(void)
{
	const unsigned char uelf[] = { 0x7F, 'E', 'L', 'F', 0x00, 0x00, 0x00, 0x00 };
	const char *elf = (const char *)uelf;
	int ret = jstr_io_isbinary_maybe(elf, sizeof(uelf));
	assert(ret == 1);
	ret = jstr_io_isbinary(elf, sizeof(uelf));
	assert(ret == 1);
}

static void
test_utf8_bom_signature(void)
{
	const unsigned char ubom[] = { 0xEF, 0xBB, 0xBF, 'a', 'b', 'c' };
	const char *bom = (const char *)ubom;
	int ret = jstr_io_isbinary_maybe(bom, sizeof(ubom));
	assert(ret == 0);
	ret = jstr_io_isbinary(bom, sizeof(ubom));
	assert(ret == 0);
}

static void
test_isbinary_control_char(void)
{
	const char data[] = { 'a', 'b', '\0', 'c', 'd' };
	int ret = jstr_io_isbinary_maybe(data, sizeof(data));
	assert(ret == 1);
	ret = jstr_isbinary(data, sizeof(data), 5);
	assert(ret == 1);
	const char printable[] = "hello\tworld\n";
	ret = jstr_isbinary(printable, 12, 12);
	assert(ret == 0);
}

static void
test_isbinary_atleast(void)
{
	const char text[] = "hello world";
	size_t tlen = strlen(text);
	/* Pure text: never binary, regardless of window. */
	assert(jstr_io_isbinary_atleast(text, tlen, tlen) == 0);
	assert(jstr_io_isbinary_atleast(text, tlen, 4096) == 0);
	/* NUL inside the window. */
	const char data[] = { 'a', 'b', '\0', 'c', 'd' };
	assert(jstr_io_isbinary_atleast(data, 5, 5) == 1);
	assert(jstr_io_isbinary_atleast(data, 5, 3) == 1);
	/* NUL beyond the window: not binary within it. */
	const char late[] = { 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', '\0', 'b', 'b', 'b' };
	assert(jstr_io_isbinary_atleast(late, 12, 8) == 0);
	assert(jstr_io_isbinary_atleast(late, 12, 9) == 1);
	/* Window larger than sz is clamped to sz. */
	assert(jstr_io_isbinary_atleast(data, 5, 1024) == 1);
	/* NUL at the very first byte. */
	const char nul0[] = { '\0', 'a', 'b' };
	assert(jstr_io_isbinary_atleast(nul0, 3, 1) == 1);
	/* Empty buffer. */
	assert(jstr_io_isbinary_atleast("", 0, 5) == 0);
}

/* ---------- FILE* write / read tests ---------- */

static void
test_fwritefile(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/fw_test.txt", 12);
	const char *data = "hello fwritefile\n";
	size_t dlen = 16;
	int ret = jstr_io_fwritefile_len(data, dlen, fpath, "w");
	assert(ret == JSTR_RET_SUCC);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	ret = jstr_io_freadfile_len(&s, &sz, &cap, fpath, "r", dlen);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	jstr_free(&s, &sz, &cap);
}

static void
test_fwritefile_append_newline(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/fw_nonl.txt", 12);
	const char *data = "no newline at end";
	size_t dlen = 18;
	int ret = jstr_io_fwritefile_len(data, dlen, fpath, "w");
	assert(ret == JSTR_RET_SUCC);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	ret = jstr_io_freadfile_len(&s, &sz, &cap, fpath, "r", dlen + 1);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen + 1);
	assert(memcmp(s, data, dlen) == 0);
	assert(s[dlen] == '\n');
	jstr_free(&s, &sz, &cap);
}

static void
test_freadfile_full(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/fr_full.txt", 12);
	const char *data = "full freadfile test data\n";
	size_t dlen = 24;
	int fd = open(fpath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	assert(fd >= 0);
	ssize_t wret = write(fd, data, dlen);
	assert((size_t)wret == dlen);
	assert(close(fd) == 0);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	struct stat st;
	int ret = jstr_io_freadfile(&s, &sz, &cap, fpath, "r", &st);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	jstr_free(&s, &sz, &cap);
}

/* ---------- fd-based read tests ---------- */

static void
test_readfilefd(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/rfd_test.txt", 13);
	const char *data = "readfilefd test\n";
	size_t dlen = 16;
	int fd = open(fpath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	assert(fd >= 0);
	ssize_t wret = write(fd, data, dlen);
	assert((size_t)wret == dlen);
	assert(close(fd) == 0);
	fd = open(fpath, O_RDONLY);
	assert(fd >= 0);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	struct stat st;
	int ret = jstr_io_readfilefd(&s, &sz, &cap, fd, &st);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	assert(close(fd) == 0);
	jstr_free(&s, &sz, &cap);
}

static void
test_readfile(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/rf_test.txt", 12);
	const char *data = "readfile pread test\n";
	size_t dlen = 19;
	int fd = open(fpath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	assert(fd >= 0);
	ssize_t wret = write(fd, data, dlen);
	assert((size_t)wret == dlen);
	assert(close(fd) == 0);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	int ret = jstr_io_readfile(&s, &sz, &cap, fpath, O_RDONLY);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	jstr_free(&s, &sz, &cap);
}

/* ---------- tilde expansion (unsafe) ---------- */

static void
test_expandtilde_unsafe(void)
{
	char buf1[] = "/no/tilde";
	size_t sz1 = strlen(buf1);
	char *end = jstr_io_expandtildefirst_len_unsafe_p(buf1, sz1);
	assert(end == buf1 + sz1);
	assert(strcmp(buf1, "/no/tilde") == 0);
	char *home = getenv("HOME");
	if (home) {
		size_t homelen = strlen(home);
		char buf2[JSTR_IO_PATH_MAX];
		size_t pathlen = 15;
		memcpy(buf2, "~/some_dir/sub", pathlen);
		buf2[pathlen] = '\0';
		end = jstr_io_expandtildefirst_len_unsafe_p(buf2, pathlen);
		assert(end != NULL);
		assert(memcmp(buf2, home, homelen) == 0);
		assert(buf2[homelen] == '/');
		assert(strcmp(buf2 + homelen, "/some_dir/sub") == 0);
	}
}

/* ---------- writefilefd_len with zero size ---------- */

static void
test_writefilefd_len_empty(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/empty.txt", 10);
	int fd = open(fpath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	assert(fd >= 0);
	int ret = jstr_io_writefilefd_len("", (size_t)0, fd);
	assert(ret == JSTR_RET_SUCC);
	assert(close(fd) == 0);
}

/* ---------- additional edge case coverage for include/io.h ---------- */

static void
test_io_more_edges(void)
{
	/* 1. expandtildefirst error condition when $HOME is unset/missing or empty */
	char *old_home = getenv("HOME");
	/* Keep old home to restore it later */
	char home_backup[JSTR_IO_PATH_MAX] = {0};
	if (old_home) {
		strncpy(home_backup, old_home, sizeof(home_backup) - 1);
	}
	unsetenv("HOME");
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_assign_len(&s, &sz, &cap, "~/test_tilde", 12) == JSTR_RET_SUCC);
	/* Should fail/error out when HOME is unset */
	int ret = jstr_io_expandtildefirst(&s, &sz, &cap);
	assert(ret == JSTR_RET_ERR);

	/* Restore home */
	if (old_home) {
		setenv("HOME", home_backup, 1);
	}

	/* 2. expandtildefirst with no tilde at the beginning */
	sz = 0;
	assert(jstr_assign_len(&s, &sz, &cap, "no_tilde", 8) == JSTR_RET_SUCC);
	ret = jstr_io_expandtildefirst(&s, &sz, &cap);
	assert(ret == JSTR_RET_SUCC);
	assert(strcmp(s, "no_tilde") == 0);

	/* 3. writefilefd_len failure with invalid fd */
	ret = jstr_io_writefilefd_len("test", 4, -1);
	assert(ret == JSTR_RET_ERR);

	/* 4. writefile_len failure with invalid mode or path */
	ret = jstr_io_writefile_len("test", 4, "/nonexistent_dir/nonexistent_file_xyz", O_WRONLY | O_CREAT, 0666);
	assert(ret == JSTR_RET_ERR);

	/* 5. fwritefile_len failure with invalid mode or path */
	ret = jstr_io_fwritefile_len("test", 4, "/nonexistent_dir/nonexistent_file_xyz", "w");
	assert(ret == JSTR_RET_ERR);

	/* 6. readfilefd failure with invalid fd */
	struct stat st;
	ret = jstr_io_readfilefd(&s, &sz, &cap, -1, &st);
	assert(ret == JSTR_RET_ERR);

	/* 7. freadfilefp failure. Works regardless of whether the library uses
	 * the fd-based (JSTR_HAVE_FILENO: fstat on a closed fd) or path-based
	 * (stat(fname)) branch: we both unlink the file (so stat fails on the
	 * nonexistent path) and close the fd (so fstat fails with EBADF). */
	{
		char fpath_dummy[JSTR_IO_PATH_MAX];
		size_t pld = pathcat(fpath_dummy, 0, tmpdir_root, tmpdir_root_len);
		pld = pathcat(fpath_dummy, pld, "/dummy_close.txt", 16);
		/* Create the file first so we can open it */
		int tmp_fd = open(fpath_dummy, O_CREAT | O_WRONLY, 0644);
		assert(tmp_fd >= 0);
		close(tmp_fd);

		FILE *fp_dummy = fopen(fpath_dummy, "r");
		assert(fp_dummy != NULL);
		int inner_fd = fileno(fp_dummy);
		assert(inner_fd >= 0);
		unlink(fpath_dummy);  /* remove the path (path-based branch fails) */
		close(inner_fd);      /* invalidate the fd (fd-based branch fails) */

		ret = jstr_io_freadfilefp(&s, &sz, &cap, fpath_dummy, fp_dummy, &st);
		assert(ret == JSTR_RET_ERR);
		fclose(fp_dummy);
	}

	jstr_free(&s, &sz, &cap);
}

/* ---------- fwritefilefp_len and freadfilefp tests ---------- */

static void
test_fwritefilefp_len(void)
{
	char fpath[JSTR_IO_PATH_MAX];
	size_t plen = pathcat(fpath, 0, tmpdir_root, tmpdir_root_len);
	plen = pathcat(fpath, plen, "/fpf.txt", 8);
	const char *data = "fp write test\n";
	size_t dlen = 14;
	FILE *fp = fopen(fpath, "w");
	assert(fp != NULL);
	int ret = jstr_io_fwritefilefp_len(data, dlen, fp);
	assert(ret == JSTR_RET_SUCC);
	assert(fclose(fp) == 0);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	FILE *fpr = fopen(fpath, "r");
	assert(fpr != NULL);
	struct stat st;
	ret = jstr_io_freadfilefp(&s, &sz, &cap, fpath, fpr, &st);
	assert(ret == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	assert(fclose(fpr) == 0);
	jstr_free(&s, &sz, &cap);
}

/* ---------- main ---------- */

int
main(int argc, char **argv)
{
	START();
	(void)argc;
	(void)argv;
	if (setup()) {
		PRINTERR("test-io-extra: failed to create temp dir\n");
		return EXIT_FAILURE;
	}
	TESTING(test_elf_signature);
	test_elf_signature();
	TESTING(test_utf8_bom_signature);
	test_utf8_bom_signature();
	TESTING(test_isbinary_control_char);
	test_isbinary_control_char();
	TESTING(test_isbinary_atleast);
	test_isbinary_atleast();
	TESTING(test_fwritefile);
	test_fwritefile();
	TESTING(test_fwritefile_append_newline);
	test_fwritefile_append_newline();
	TESTING(test_freadfile_full);
	test_freadfile_full();
	TESTING(test_readfilefd);
	test_readfilefd();
	TESTING(test_readfile);
	test_readfile();
	TESTING(test_expandtilde_unsafe);
	test_expandtilde_unsafe();
	TESTING(test_writefilefd_len_empty);
	test_writefilefd_len_empty();
	TESTING(test_fwritefilefp_len);
	test_fwritefilefp_len();
	TESTING(test_io_more_edges);
	test_io_more_edges();
	teardown();
	SUCCESS();
	return EXIT_SUCCESS;
}
