#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "jstr.h"
#include "io.h"

static int ftw_counter;
static int ftw_cb_count(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return 0;
}

struct match_ctx {
	const char *name;
	int found;
};

static int ftw_match_accept_all(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name;
	(void)name_len;
	(void)args;
	return 0;
}

static int ftw_cb_check_dirpath(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	if (ftw->ep == NULL)
		return 0;
	/* verify dirpath ends with our entry name */
	const char *ep_name = ftw->ep->d_name;
	size_t dlen = ftw->dirpath_len;
	size_t nlen = strlen(ep_name);
	assert(dlen >= nlen);
	assert(memcmp(ftw->dirpath + dlen - nlen, ep_name, nlen) == 0);
	return 0;
}

int main(void) {
	/* 1. readsystem loop: command output > JSTR_PAGE_SIZE (4096) */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int r = jstr_io_readsystem(&s, &sz, &cap, "/usr/bin/head -c 5000 /dev/zero");
		assert(r == 0);
		assert(sz >= 5000);
		free(s);
	}
	/* 2. readstdin loop: pipe > BUFSIZ (8192) bytes to stdin */
	{
		int pfd[2];
		assert(pipe(pfd) == 0);
		pid_t pid = fork();
		assert(pid >= 0);
		if (pid == 0) {
			close(pfd[0]);
			/* Write 9000 bytes */
			char buf[9000];
			memset(buf, 'a', sizeof(buf));
			buf[sizeof(buf) - 1] = '\n';
			ssize_t written = write(pfd[1], buf, sizeof(buf));
			(void)written;
			close(pfd[1]);
			_exit(0);
		}
		close(pfd[1]);
		int old_stdin = dup(STDIN_FILENO);
		assert(old_stdin >= 0);
		assert(dup2(pfd[0], STDIN_FILENO) >= 0);
		close(pfd[0]);
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int r = jstr_io_readstdin(&s, &sz, &cap);
		assert(r == 0);
		assert(sz >= 9000);
		free(s);
		assert(dup2(old_stdin, STDIN_FILENO) >= 0);
		close(old_stdin);
		waitpid(pid, NULL, 0);
	}
	/* 3. FTW non-dir root: pass a regular file as dirpath */
	{
		char tmpdir[] = "/tmp/jstr_nondir_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/afile", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(p);
		/* func returns 0, so ftw should return 0 */
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(p, dlen, ftw_cb_count, NULL, 0, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 1);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* 4. FTW non-dir root with func_match that accepts (no MATCHPATH) */
	{
		char tmpdir[] = "/tmp/jstr_nondir2_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/afile", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(p);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(p, dlen, ftw_cb_count, NULL,
		    JSTR_IO_FTW_REG, ftw_match_accept_all, NULL);
		assert(ret == 0);
		assert(ftw_counter == 1);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* 5. FTW non-dir root with func_match that accepts (MATCHPATH) */
	{
		char tmpdir[] = "/tmp/jstr_nondir3_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/afile", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(p);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(p, dlen, ftw_cb_count, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH, ftw_match_accept_all, NULL);
		assert(ret == 0);
		assert(ftw_counter == 1);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* 6. FTW non-dir root with func_match that rejects (MATCHPATH) */
	{
		char tmpdir[] = "/tmp/jstr_nondir4_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/afile", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		struct match_ctx mc = {"nope", 0};
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(p);
		int ret = jstr_io_ftw_len(p, dlen, ftw_cb_check_dirpath, NULL,
		    JSTR_IO_FTW_MATCHPATH, ftw_match_accept_all, NULL);
		assert(ret == 0);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* 7. FTW with non-dir root: reg-only, func_match rejects (no MATCHPATH) */
	{
		char tmpdir[] = "/tmp/jstr_nondir5_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/afile", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(p);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(p, dlen, ftw_cb_count, NULL, 0, ftw_match_accept_all, NULL);
		assert(ret == 0);
		assert(ftw_counter == 1);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	return 0;
}
