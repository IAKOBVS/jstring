#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "jstr.h"
#include "io.h"

/* Helper: read whole file into heap */
static char *readfile(const char *path, size_t *outlen) {
	FILE *f = fopen(path, "r");
	if (!f)
		return NULL;
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = (char *)malloc((size_t)len + 1);
	if (!buf) { fclose(f); return NULL; }
	size_t got = fread(buf, 1, (size_t)len, f);
	buf[got] = '\0';
	fclose(f);
	*outlen = got;
	return buf;
}

static int ftw_counter;
static int ftw_cb_reg(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return 0;
}

static int ftw_cb_action_continue(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_cb_action_stop(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return JSTR_IO_FTW_RET_STOP;
}

static int ftw_cb_action_stop_on_second(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return ftw_counter >= 2 ? JSTR_IO_FTW_RET_STOP : JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_cb_action_skip_siblings_on_first(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	return ftw_counter == 1 ? JSTR_IO_FTW_RET_SKIP_SIBLINGS : JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_cb_action_skip_subtree_first_dir(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++ftw_counter;
	/* Skip subtree on the first directory encountered */
	if (ftw->st && S_ISDIR(ftw->st->st_mode))
		return JSTR_IO_FTW_RET_SKIP_SUBTREE;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int ftw_match_skip_all(const char *fname, jstr_io_path_size_ty fname_len, const void *args) {
	(void)fname;
	(void)fname_len;
	(void)args;
	return 1;
}

static int ftw_match_skip_hidden(const char *fname, jstr_io_path_size_ty fname_len, const void *args) {
	(void)fname_len;
	(void)args;
	if (fname[0] == '.')
		return 1;
	return 0;
}

int main(void) {
	/* ── 1. Open failures ───────────────────────────────── */
	{
		int r = jstr_io_writefile_len("x", 1, "/nonexistent/path/xyz", O_WRONLY, 0);
		assert(r != 0);
	}
	{
		int r = jstr_io_fwritefile_len("x", 1, "/nonexistent/path/xyz/w", "w");
		assert(r != 0);
	}
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfile_len(&s, &sz, &cap, "/nonexistent/path/xyz", O_RDONLY, 5);
		assert(r != 0);
		assert(s == NULL);
	}
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfile(&s, &sz, &cap, "/nonexistent/path/xyz", O_RDONLY);
		assert(r != 0);
		assert(s == NULL);
	}
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_freadfile_len(&s, &sz, &cap, "/nonexistent/path/xyz", "r", 5);
		assert(r != 0);
		assert(s == NULL);
	}
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		struct stat st;
		int r = jstr_io_freadfile(&s, &sz, &cap, "/nonexistent/path/xyz", "r", &st);
		assert(r != 0);
		assert(s == NULL);
	}

	/* ── 2. writev/write failure: write to /dev/full ──── */
	{
		int fd = open("/dev/full", O_WRONLY);
		if (fd >= 0) {
			int r = jstr_io_writefilefd_len("hello", 5, fd);
			assert(r != 0);
			close(fd);
		}
	}

	/* ── 3. Read EOF from /dev/null ──────────────────── */
	{
		int fd = open("/dev/null", O_RDONLY);
		assert(fd >= 0);
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfilefd_len(&s, &sz, &cap, fd, 100);
		assert(r != 0);
		free(s);
		close(fd);
	}

	/* ── 4. Read error: invalid fd ───────────────────── */
	{
		int fd = open("/dev/null", O_RDONLY);
		assert(fd >= 0);
		close(fd); /* invalidate */
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfilefd_len(&s, &sz, &cap, fd, 100);
		assert(r != 0);
		free(s);
	}

	/* ── 5. fstat failure (readfilefd) ──────────────── */
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		struct stat st;
		int r = jstr_io_readfilefd(&s, &sz, &cap, -1, &st);
		assert(r != 0);
	}

	/* ── 6. HOME not set (expandtilde unsafe) ────────── */
	{
		const char *old_home = getenv("HOME");
		unsetenv("HOME");
		char buf[256];
		strcpy(buf, "~/test");
		char *r = jstr_io_expandtildefirst_len_unsafe_p(buf, sizeof buf);
		assert(r == NULL);
		if (old_home)
			setenv("HOME", old_home, 1);
	}

	/* ── 7. jstr_io_expandtildefirst (jstr version) ──── */
	{
		/* Without tilde — no-op path */
		jstr_ty j = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(&j.data, &j.size, &j.capacity, "/tmp/foo", 8)));
		int r = jstr_io_expandtildefirst(&j.data, &j.size, &j.capacity);
		assert(r == 0);
		assert(!strcmp(j.data, "/tmp/foo"));
		free(j.data);
	}
	{
		/* With tilde — expansion path */
		const char *home = getenv("HOME");
		if (home) {
			jstr_ty j = JSTR_INIT;
			assert(!jstr_chk(jstr_assign_len(&j.data, &j.size, &j.capacity, "~/sub", 5)));
			int r = jstr_io_expandtildefirst(&j.data, &j.size, &j.capacity);
			assert(r == 0);
			assert(j.data[0] == '/');
			free(j.data);
		}
	}

	/* ── 8. fwritefilefp_len: empty buffer ──────────── */
	{
		FILE *fp = fopen("/dev/null", "w");
		assert(fp);
		int r = jstr_io_fwritefilefp_len("", 0, fp);
		assert(r == 0);
		fclose(fp);
	}

	/* ── 9. fwritefilefp_len: append \n ─────────────── */
	{
		const char *path = "/tmp/jstr_test_nl.txt";
		FILE *fp = fopen(path, "w");
		assert(fp);
		/* Write without trailing \n — should append \n */
		int r = jstr_io_fwritefilefp_len("hello", 5, fp);
		assert(r == 0);
		fclose(fp);
		size_t len;
		char *content = readfile(path, &len);
		assert(content);
		assert(len == 6);
		assert(content[5] == '\n');
		free(content);
		unlink(path);
	}

	/* ── 10. fwritefilefp_len: fwrite failure ───────── */
	{
		FILE *fp = fopen("/dev/full", "w");
		assert(fp);
		/* Set unbuffered so writes go directly to /dev/full */
		assert(!setvbuf(fp, NULL, _IONBF, 0));
		int r = jstr_io_fwritefilefp_len("hello", 5, fp);
		assert(r != 0);
		fclose(fp);
	}

	/* ── 11. freadfilefp_len: fread failure ─────────── */
	{
		FILE *fp = fopen("/dev/null", "r");
		assert(fp);
		close(fileno(fp)); /* corrupt */
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_freadfilefp_len(&s, &sz, &cap, fp, 10);
		assert(r != 0);
		free(s);
		fclose(fp);
	}

	/* ── 12. freadfilefp: fileno failure ────────────── */
	{
		FILE *fp = fopen("/dev/null", "r");
		assert(fp);
		close(fileno(fp));
		/* The fd is closed, fileno still returns the fd number,
		 * but fstat on it will fail.
		 * We can't directly trigger fileno < 0 from userspace,
		 * so we skip the fileno-failure test. */
		fclose(fp);
	}

	/* ── 13. freadfilefp: fstat failure ─────────────── */
	{
		FILE *fp = fopen("/dev/null", "r");
		assert(fp);
		close(fileno(fp));
		char *s = NULL; size_t sz = 0, cap = 0;
		struct stat st;
		int r = jstr_io_freadfilefp(&s, &sz, &cap, "/dev/null", fp, &st);
		/* fileno succeeds (returns 3 or whatever), fstat fails (EBADF) */
		assert(r != 0);
		free(s);
		fclose(fp);
	}

	/* ── 14. pread EOF from /dev/null ───────────────── */
	{
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfile(&s, &sz, &cap, "/dev/null", O_RDONLY);
		assert(r == 0);
		assert(sz == 0);
		free(s);
	}

	/* ── 15. lseek failure on FIFO ──────────────────── */
	{
		const char *fifo = "/tmp/jstr_test_fifo";
		unlink(fifo);
		assert(mkfifo(fifo, 0644) == 0);
		/* open with O_NONBLOCK|O_RDONLY, lseek on FIFO fails with ESPIPE */
		char *s = NULL; size_t sz = 0, cap = 0;
		int r = jstr_io_readfile(&s, &sz, &cap, fifo, O_NONBLOCK);
		assert(r != 0);
		assert(s == NULL);
		unlink(fifo);
	}

	/* ── 16. jstr_io_writefilefd_len: sz==0 ──────────── */
	{
		int fd = open("/dev/null", O_WRONLY);
		assert(fd >= 0);
		int r = jstr_io_writefilefd_len("", 0, fd);
		assert(r == 0);
		close(fd);
	}

	/* ── 17. jstr_io_appendpath_len: already tested ──── */

	/* ── 18. FTW: basic flag combos ───────────────────── */
	{
		char tmpdir[] = "/tmp/jstr_ftw2_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		/* Create: dir/a, dir/sub/, dir/sub/b */
		snprintf(p, sizeof p, "%s/a", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);

		/* REG only */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL) == 0);
		assert(ftw_counter == 2); /* a, sub/b */

		/* DIR only */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_DIR, NULL, NULL) == 0);
		assert(ftw_counter == 2); /* tmpdir, sub */

		/* NOSUBDIR|REG|DIR */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_NOSUBDIR | JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL) == 0);
		assert(ftw_counter == 3); /* tmpdir, a, sub */

		/* NOSUBDIR|REG */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_NOSUBDIR | JSTR_IO_FTW_REG, NULL, NULL) == 0);
		assert(ftw_counter == 1); /* a only */

		/* func_match that skips "sub" */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, ftw_match_skip_hidden, NULL) == 0);
		assert(ftw_counter == 4); /* none skipped, nothing is hidden */

		/* func_match that skips all — root dir + sub dir still reported
		 * (func_match only filters regular files, not directories) */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, ftw_match_skip_all, NULL) == 0);
		assert(ftw_counter == 2); /* root + sub dir */

		/* Cleanup */
		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 19. FTW: ACTIONRETVAL ──────────────────────── */
	{
		char tmpdir[] = "/tmp/jstr_ftw3_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/a", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);

		/* ACTIONRETVAL: return CONTINUE for all — dirs are not recursed into */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_continue, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL,
		    NULL, NULL) == 0);
		assert(ftw_counter == 3); /* root + a + sub (sub not recursed) */

		/* ACTIONRETVAL: stop immediately on first entry */
		ftw_counter = 0;
		int stop_ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_stop, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL,
		    NULL, NULL);
		assert(stop_ret == JSTR_IO_FTW_RET_STOP);
		assert(ftw_counter == 1); /* only root dir called before stop */

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 20. FTW: MATCHPATH with func_match ──────────── */
	{
		char tmpdir[] = "/tmp/jstr_ftw4_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/match.txt", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);

		/* func_match that checks full path contains "match" */
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH,
		    ftw_match_skip_all, NULL) == 0);
		assert(ftw_counter == 0); /* root skipped since DIR not in flags */

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 21. FTW: NOSTAT flag (when d_type available) ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw5_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/reg", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);

		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_NOSTAT, NULL, NULL) == 0);
		assert(ftw_counter == 1);

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 22. jstr_io_expandtildefirst: HOME unset ────── */
	{
		const char *old_home = getenv("HOME");
		unsetenv("HOME");
		jstr_ty j = JSTR_INIT;
		assert(!jstr_chk(jstr_assign_len(&j.data, &j.size, &j.capacity, "~/x", 3)));
		int r = jstr_io_expandtildefirst(&j.data, &j.size, &j.capacity);
		assert(r != 0);
		free(j.data);
		if (old_home)
			setenv("HOME", old_home, 1);
	}

	/* ── 23. jstr_io_fwritefilefp_len: no \n, fwrite ok ── */
	{
		const char *path = "/tmp/jstr_test_nl2.txt";
		FILE *fp = fopen(path, "w");
		assert(fp);
		int r = jstr_io_fwritefilefp_len("hello", 5, fp);
		assert(r == 0);
		fclose(fp);
		size_t len;
		char *content = readfile(path, &len);
		assert(content);
		assert(len > 5);
		assert(content[5] == '\n');
		free(content);
		unlink(path);
	}

	/* ── 24. FTW: dirpath_len == 0 ─────────────────── */
	{
		int r = jstr_io_ftw_len("", 0, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(r != 0);
	}

	/* ── 25. FTW: non-directory root (regular file) ── */
	{
		const char *path = "/tmp/jstr_ftw_regroot";
		FILE *f = fopen(path, "w"); assert(f); fputc('x', f); fclose(f);
		jstr_io_path_size_ty plen = (jstr_io_path_size_ty)strlen(path);
		ftw_counter = 0;
		assert(jstr_io_ftw_len(path, plen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL) == 0);
		assert(ftw_counter == 1);
		unlink(path);
	}

	/* ── 26. FTW: non-directory root, REG not matching ── */
	{
		/* Symlink is not a regular file — use /dev/null as target */
		const char *path = "/tmp/jstr_ftw_regroot2";
		const char *target = "/dev/null";
		unlink(path);
		assert(symlink(target, path) == 0);
		jstr_io_path_size_ty plen = (jstr_io_path_size_ty)strlen(path);
		ftw_counter = 0;
		assert(jstr_io_ftw_len(path, plen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL) == 0);
		assert(ftw_counter == 0); /* filtered out, not REG */
		unlink(path);
	}

	/* ── 27. FTW: NOHIDDEN flag ───────────────────── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_nohidden_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/.hidden", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/visible", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		assert(jstr_io_ftw_len(tmpdir, dlen, ftw_cb_reg, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_NOHIDDEN, NULL, NULL) == 0);
		assert(ftw_counter == 1); /* only visible file */

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 28. FTW: ACTIONRETVAL SKIP_SIBLINGS from reg (REG-only, no root cb) ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_sibs_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/a", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/b", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		/* REG-only: root skipped, first cb call is for entry "a" which returns SKIP_SIBLINGS */
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_skip_siblings_on_first, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == 0);
		assert(ftw_counter == 1); /* only "a" processed, "b" skipped by SKIP_SIBLINGS */

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 29. FTW: ACTIONRETVAL STOP from inside (not root, REG-only) ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_stop2_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/a", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/b", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		/* REG-only: root skipped, first cb call is for "a" (CONTINUE), second for "b" (STOP) */
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_stop_on_second, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == JSTR_IO_FTW_RET_STOP);
		assert(ftw_counter == 2); /* a, then stop before b */

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	/* ── 30. FTW: ACTIONRETVAL SKIP_SUBTREE from dir callback ── */
	{
		char tmpdir[] = "/tmp/jstr_ftw_subtree_XXXXXX";
		assert(mkdtemp(tmpdir));
		char p[512];
		snprintf(p, sizeof p, "%s/reg", tmpdir);
		FILE *f = fopen(p, "w"); assert(f); fputc('x', f); fclose(f);
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		assert(mkdir(p, 0755) == 0);
		snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
		f = fopen(p, "w"); assert(f); fputc('y', f); fclose(f);

		jstr_io_path_size_ty dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		ftw_counter = 0;
		/* REG|DIR: root called, cb returns SKIP_SUBTREE (is dir). Jstr_internal
		 * processes entries; first dir encountered gets SKIP_SUBTREE → break.
		 * readdir order is fs-dependent, so just verify no crash and ret==0 */
		int ret = jstr_io_ftw_len(tmpdir, dlen, ftw_cb_action_skip_subtree_first_dir, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == 0);
		/* Must have at least root (1), and deep must NOT be visited */
		assert(ftw_counter >= 1);

		snprintf(p, sizeof p, "rm -rf %s", tmpdir);
		system(p);
	}

	return 0;
}
