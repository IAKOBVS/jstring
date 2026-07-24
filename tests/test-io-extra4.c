#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
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
static int ftw_cb_counter(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return 0;
}

static int ftw_cb_action_stop(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return JSTR_IO_FTW_RET_STOP;
}

static int ftw_cb_action_stop_deep(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	if (ftw_counter > 2)
		return JSTR_IO_FTW_RET_STOP;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_cb_fail(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	return -1;
}

static int ftw_match_name_only(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name_len;
	(void)args;
	return strcmp(name, "keep") == 0 ? 0 : 1;
}

int main(void) {
	/* ── 1. jstr_io_readsystem — basic popen ─────────── */
	{
		char *s = NULL;
		size_t sz = 0, cap = 0;
		int r = jstr_io_readsystem(&s, &sz, &cap, "echo hello");
		assert(r == 0);
		assert(sz > 0);
		assert(strstr(s, "hello") != NULL);
		free(s);
	}
	/* readstdin test via pipe+fork */
	{
		int pfd[2];
		assert(pipe(pfd) == 0);
		pid_t pid = fork();
		assert(pid >= 0);
		if (pid == 0) {
			close(pfd[0]);
			const char *msg = "stdin data\n";
			write(pfd[1], msg, strlen(msg));
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
		assert(sz > 0);
		assert(strstr(s, "stdin data") != NULL);
		free(s);
		assert(dup2(old_stdin, STDIN_FILENO) >= 0);
		close(old_stdin);
		waitpid(pid, NULL, 0);
	}
	/* ── 3. FTW: symlink entry with REG|DIR filter ──── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_sym_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/reg", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/link", tmpdir);
		assert(symlink("reg", p) == 0);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* stat follows symlinks, so link appears as S_IFREG.
		 * REG|DIR: root(dir) + reg(S_IFREG) + link(S_IFREG) = 3 */
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 3); /* root + reg + link */
		/* REG only: link + reg = 2 (root skipped) */
		ftw_counter = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 2);
		/* No flags: root + reg + link = 3 (root counted) */
		ftw_counter = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    0, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 3);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 4. FTW: NONFATAL_ERR — inaccessible dir ───── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_deny_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0) == 0); /* no permissions */
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* REG only — inaccessible dir skipped silently, no files */
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 0);
		/* REG|DIR — inaccessible dir triggers func callback for dir */
		ftw_counter = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 2); /* root + sub via NONFATAL_ERR */
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 5. FTW: non-directory path + func_match (no MATCHPATH) ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_funcm_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/subdir", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/keep", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/skip", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* func_match without MATCHPATH — match on filename */
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_counter, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR,
		    ftw_match_name_only, NULL);
		assert(ret == 0);
		assert(ftw_counter == 3); /* root + subdir (as dir) + keep */
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 6. FTW: ACTIONRETVAL recursive STOP from subdir ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_rstop_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/deep/x", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_stop_deep, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == JSTR_IO_FTW_RET_STOP);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 7. FTW: recursive error (non-ACTIONRETVAL) from subdir ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_rerr_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/deep/x", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_fail, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret != 0);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 8. FTW: dir callback with ACTIONRETVAL STOP ────── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_dstop_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_stop, NULL,
		    JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == JSTR_IO_FTW_RET_STOP);
		assert(ftw_counter == 1);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	/* ── 9. FTW: dir callback error (non-ACTIONRETVAL) ──── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_derr_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_fail, NULL,
		    JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret != 0);
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}
	return 0;
}
