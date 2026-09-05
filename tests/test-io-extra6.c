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

static int count;

static int cb_count(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	++count;
	return 0;
}

static int cb_fail(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)ftw;
	(void)args;
	return -1;
}

static int cb_stop(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	if (ftw->ep && S_ISDIR(ftw->st->st_mode))
		return JSTR_IO_FTW_RET_STOP;
	return JSTR_IO_FTW_RET_CONTINUE;
}

struct depth_ctx {
	int dir_count;
	int threshold;
};

static int cb_stop_at_depth(const struct JSTR_IO_FTW *ftw, const void *args) {
	struct depth_ctx *ctx = (struct depth_ctx *)args;
	if (ftw->ep == NULL)
		return JSTR_IO_FTW_RET_CONTINUE;
	if (S_ISDIR(ftw->st->st_mode)) {
		ctx->dir_count++;
		if (ctx->dir_count >= ctx->threshold)
			return JSTR_IO_FTW_RET_STOP;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_fail_at_depth(const struct JSTR_IO_FTW *ftw, const void *args) {
	struct depth_ctx *ctx = (struct depth_ctx *)args;
	if (ftw->ep == NULL)
		return 0;
	if (S_ISDIR(ftw->st->st_mode)) {
		ctx->dir_count++;
		if (ctx->dir_count >= ctx->threshold)
			return -1;
	}
	return 0;
}

static int match_accept(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name;
	(void)name_len;
	(void)args;
	return 1;
}

static void cleanup(const char *dir) {
	char p[512];
	snprintf(p, sizeof p, "rm -rf %s", dir);
	system(p);
}

static void create_file(const char *path, char c) {
	FILE *f = fopen(path, "w");
	if (f) {
		fputc(c, f);
		fclose(f);
	}
}

int main(void) {
	char tmpdir[256];
	char p[512];
	jstr_io_path_size_ty dlen;
	int ret;

	/* ===== FTW: NONFATAL_ERR on opendir(EACCES) ===== */
	/* Create: root/a (file), root/inaccessible/ (mode 0000) */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_eacces_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/inaccessible", tmpdir);
		mkdir(p, 0000);

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* Default flags (no REG, no DIR): hits lines 936-938 (call func) */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL, 0, NULL, NULL);
		assert(ret == 0);
		assert(count >= 1);

		/* REG only (no DIR): hits lines 931-933 (return SUCC silently) */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL);
		assert(ret == 0);
		assert(count >= 1);

		/* func_match present: hits lines 934-935 (match → return SUCC) */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
		    0, match_accept, NULL);
		assert(ret == 0);
		assert(count >= 1);

		/* EACCES is fatal (non-fatal check fails): hits line 940 */
		/* chmod 0000 + non-nonfatal-errno is hard to trigger,
		 * so skip this for now */

		cleanup(tmpdir);
	}

	/* ===== FTW: Non-regular, non-directory entry (FIFO) ===== */
	/* Lines 987-989 */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_fifo_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/myfifo", tmpdir);
		mkfifo(p, 0644);

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);

		/* DIR|REG flags → FIFO skipped (line 987-988) */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret == 0);
		/* Only file 'a' counted (FIFO skipped) */
		assert(count == 2);

		/* No DIR|REG → FIFO goes to do_reg (line 989) */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL, 0, NULL, NULL);
		assert(ret == 0);
		/* Root + file 'a' + FIFO = 3 entries */
		assert(count == 3);

		cleanup(tmpdir);
	}

	/* ===== FTW: Dir callback STOP with ACTIONRETVAL (lines 1069-1071) ===== */
	/* The dir callback on the subdir returns STOP */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_stop_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* DIR|ACTIONRETVAL: cb_stop returns STOP on subdir */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_stop, NULL,
		    JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == JSTR_IO_FTW_RET_STOP);

		cleanup(tmpdir);
	}

	/* ===== FTW: Dir callback error without ACTIONRETVAL (lines 1074-1076) ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_derr_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* DIR only: cb_fail returns -1 on subdir */
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_fail, NULL,
		    JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret == -1);

		cleanup(tmpdir);
	}

	/* ===== FTW: Recursive STOP (lines 1090-1092) ===== */
	/* 3-level: root/sub/deep/ — callback STOPs on deep */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_rstop_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');
		snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/deep/c", tmpdir);
		create_file(p, 'z');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* REG|ACTIONRETVAL: skip dir callback at root level (goto skip_fn),
		 * recursive call returns SUCC (== STOP == 0) → lines 1090-1092 */
		struct depth_ctx ctx3 = {0, 2};
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_stop_at_depth, &ctx3,
		    JSTR_IO_FTW_REG | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
		assert(ret == JSTR_IO_FTW_RET_STOP);

		cleanup(tmpdir);
	}

	/* ===== FTW: Recursive error (lines 1095-1097) ===== */
	/* 3-level: root/sub/deep/ — callback FAILS on deep */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_rerr_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');
		snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/deep/c", tmpdir);
		create_file(p, 'z');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		struct depth_ctx ctx2 = {0, 2};
		/* DIR only (no ACTIONRETVAL): error at depth 2 */
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_fail_at_depth, &ctx2,
		    JSTR_IO_FTW_DIR, NULL, NULL);
		assert(ret == -1);

		cleanup(tmpdir);
	}

	/* ===== FTW: non-dir root, no MATCHPATH, func_match matches (line 1228) ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_nodir_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/a", tmpdir);
		create_file(p, 'x');

		/* Pass full path with '/', match_accept matches → return SUCC */
		dlen = (jstr_io_path_size_ty)strlen(p);
		count = 0;
		ret = jstr_io_ftw_len(p, dlen, cb_count, NULL,
		    0, match_accept, NULL);
		assert(ret == 0);
		assert(count == 0);

		cleanup(tmpdir);
	}

	/* ===== FTW: NOSUBDIR (lines 1081-1082) ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_nosub_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* DIR|NOSUBDIR: processes dirs but doesn't recurse */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
		    JSTR_IO_FTW_DIR | JSTR_IO_FTW_NOSUBDIR, NULL, NULL);
		assert(ret == 0);
		/* root dir + sub dir (callback called, just no recursion) */
		assert(count == 2);

		cleanup(tmpdir);
	}

	/* ===== FTW: REG & !DIR skip_fn (lines 1061-1062) ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_skipfn_XXXXXX");
		assert(mkdtemp(tmpdir));
		snprintf(p, sizeof p, "%s/sub", tmpdir);
		mkdir(p, 0755);
		snprintf(p, sizeof p, "%s/sub/b", tmpdir);
		create_file(p, 'y');

		dlen = (jstr_io_path_size_ty)strlen(tmpdir);
		/* REG only (no DIR): subdir's callback is skipped */
		count = 0;
		ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
		    JSTR_IO_FTW_REG, NULL, NULL);
		assert(ret == 0);
		/* Only file 'b' counted (sub dir not called) */
		assert(count == 1);

		cleanup(tmpdir);
	}

	/* ===== FTW: non-dir root, no '/', func_match (lines 1230-1231) ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_nodir2_XXXXXX");
		assert(mkdtemp(tmpdir));
		char *orig_dir = getcwd(NULL, 0);
		assert(chdir(tmpdir) == 0);

		create_file("testfile", 'x');
		dlen = (jstr_io_path_size_ty)strlen("testfile");
		count = 0;
		ret = jstr_io_ftw_len("testfile", dlen, cb_count, NULL,
		    0, match_accept, NULL);
		/* func_match matches → return SUCC, callback not called */
		assert(ret == 0);
		assert(count == 0);

		assert(chdir(orig_dir) == 0);
		free(orig_dir);
		cleanup(tmpdir);
	}

	/* ===== FTW: non-dir root, no func_match → callback called ===== */
	{
		snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw6_nodir3_XXXXXX");
		assert(mkdtemp(tmpdir));
		char *orig_dir = getcwd(NULL, 0);
		assert(chdir(tmpdir) == 0);

		create_file("testfile", 'x');
		dlen = (jstr_io_path_size_ty)strlen("testfile");
		count = 0;
		ret = jstr_io_ftw_len("testfile", dlen, cb_count, NULL,
		    0, NULL, NULL);
		/* No func_match → falls through to func(&data.ftw, func_args) */
		assert(ret == 0);
		assert(count == 1);

		assert(chdir(orig_dir) == 0);
		free(orig_dir);
		cleanup(tmpdir);
	}

	/* ===== FTW: ENAMETOOLONG (lines 963-965) ===== */
	{
		/* Create 19 levels of 200-char dirs → path length = ~30 + 19*201 = 3849 >= 3841 threshold */
		char name200[201];
		memset(name200, 'a', 200);
		name200[200] = '\0';

		/* Unique root per process: the suite runs 4 variants of this test in
		 * parallel, so a hardcoded path would let one instance's `rm -rf`
		 * clobber another's mid-walk (failing its chdir/assert). */
		char deeproot[32];
		snprintf(deeproot, sizeof deeproot, "/tmp/jstr_ftw6eo_XXXXXX");
		assert(mkdtemp(deeproot));

		char deep_path[4096];
		snprintf(deep_path, sizeof deep_path, "%s", deeproot);
		for (int i = 0; i < 19; i++) {
			strcat(deep_path, "/");
			strcat(deep_path, name200);
			mkdir(deep_path, 0755);
		}
		dlen = (jstr_io_path_size_ty)strlen(deep_path);
		assert(dlen >= 3841);
		assert(dlen < 4096);

		/* chdir inside and create a file with 255-char name */
		char *orig_dir = getcwd(NULL, 0);
		assert(chdir(deep_path) == 0);
		char fname255[256];
		memset(fname255, 'b', 255);
		fname255[255] = '\0';
		create_file(fname255, 'z');
		assert(chdir(orig_dir) == 0);
		free(orig_dir);

		/* FTW should return error (ENAMETOOLONG) */
		ret = jstr_io_ftw_len(deep_path, dlen, cb_count, NULL, 0, NULL, NULL);
		assert(ret != 0);

		/* cleanup: remove the deeply nested tree */
		snprintf(p, sizeof p, "rm -rf %s", deeproot);
		system(p);
	}

	/* ===== readstdin: read fails (line 610) ===== */
	{
		int saved_stdin = dup(STDIN_FILENO);
		close(STDIN_FILENO);
		/* STDIN_FILENO is now invalid → read returns -1 */
		char *s = NULL;
		size_t sz = 0, cap = 0;
		ret = jstr_io_readstdin(&s, &sz, &cap);
		assert(ret != 0);
		/* Restore stdin */
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
		free(s);
	}

	return 0;
}
