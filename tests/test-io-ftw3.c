/* Correctness tests for jstr_io_ftw / jstr_io_ftw_len.
 *
 * Focus areas (closes REVIEW.md I3/I4 and general test gaps):
 *  - ACTIONRETVAL directory recursion (glibc FTW semantics):
 *      CONTINUE -> recurse into dir, continue siblings
 *      SKIP_SUBTREE -> do not recurse, continue siblings
 *      SKIP_SIBLINGS -> do not recurse, skip remaining siblings
 *      RET_STOP -> stop the whole walk (ret value 0 == JSTR_RET_SUCC)
 *  - Recursive RET_STOP propagation from a nested callback.
 *  - NOHIDDEN, NOSTAT, STATREG flag behavior.
 *  - MATCHPATH (full path) vs basename func_match argument.
 *  - Empty / oversized dirpath errors; trailing-slash handling.
 *  - jstr_io_ftw (non-_len) wrapper.
 */
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "jstr.h"
#include "io.h"

static int n_visits;
static int n_reg;
static int n_dir;

static int cb_count(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (S_ISREG(ftw->st->st_mode))
		++n_reg;
	if (S_ISDIR(ftw->st->st_mode))
		++n_dir;
	return 0;
}

static int cb_continue(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (S_ISREG(ftw->st->st_mode))
		++n_reg;
	if (S_ISDIR(ftw->st->st_mode))
		++n_dir;
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_dir_skip_subtree(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISDIR(ftw->st->st_mode)) {
		++n_dir;
		return JSTR_IO_FTW_RET_SKIP_SUBTREE;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_dir_skip_siblings(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISDIR(ftw->st->st_mode)) {
		++n_dir;
		return JSTR_IO_FTW_RET_SKIP_SIBLINGS;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_file_skip_subtree(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISREG(ftw->st->st_mode)) {
		++n_reg;
		return JSTR_IO_FTW_RET_SKIP_SUBTREE;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_file_skip_siblings(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISREG(ftw->st->st_mode)) {
		++n_reg;
		return JSTR_IO_FTW_RET_SKIP_SIBLINGS;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_stop_file(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISREG(ftw->st->st_mode)) {
		++n_reg;
		return JSTR_IO_FTW_RET_STOP;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_stop_dir(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (ftw->ep != NULL && S_ISDIR(ftw->st->st_mode)) {
		++n_dir;
		return JSTR_IO_FTW_RET_STOP;
	}
	return JSTR_IO_FTW_RET_CONTINUE;
}

static int cb_statreg(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	++n_visits;
	if (S_ISREG(ftw->st->st_mode)) {
		/* STATREG must have done a full stat so st_size is real. */
		assert(ftw->st->st_size == (off_t)1234);
		++n_reg;
	}
	return 0;
}

static int cb_nostat(const struct JSTR_IO_FTW *ftw, const void *args) {
	(void)args;
	assert(S_ISREG(ftw->st->st_mode));
	++n_visits;
	++n_reg;
	return 0;
}

static char last_match[512];
static int m_accept(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name_len;
	(void)args;
	snprintf(last_match, sizeof last_match, "%s", name);
	return 0;
}

static int m_skip_x(const char *name, jstr_io_path_size_ty name_len, const void *args) {
	(void)name_len;
	(void)args;
	return name[0] == 'x' ? 1 : 0;
}

static void create(const char *path, char c) {
	FILE *f = fopen(path, "w");
	assert(f != NULL);
	fputc(c, f);
	fclose(f);
}

static void cleanup(const char *dir) {
	char p[512];
	snprintf(p, sizeof p, "rm -rf %s", dir);
	system(p);
}

static void build_tree6(const char *root) {
	/* root/{a, sub1/{w}, sub2/{z}} — 6 entries in full DFS. */
	char p[512];
	snprintf(p, sizeof p, "%s/a", root);
	create(p, 'a');
	snprintf(p, sizeof p, "%s/sub1", root);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub1/w", root);
	create(p, 'w');
	snprintf(p, sizeof p, "%s/sub2", root);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub2/z", root);
	create(p, 'z');
}

static void build_tree7(const char *root) {
	/* root/{a, sub/{b, subdeep/{c}}, d} — 7 entries in full DFS. */
	char p[512];
	snprintf(p, sizeof p, "%s/a", root);
	create(p, 'a');
	snprintf(p, sizeof p, "%s/sub", root);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/b", root);
	create(p, 'b');
	snprintf(p, sizeof p, "%s/sub/subdeep", root);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/subdeep/c", root);
	create(p, 'c');
	snprintf(p, sizeof p, "%s/d", root);
	create(p, 'd');
}

int main(void) {
	char tmpdir[256];
	char p[512];
	jstr_io_path_size_ty dlen;
	int ret;

	/* ── 1. Full recursion counts across flag combos ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_basic_XXXXXX");
	assert(mkdtemp(tmpdir));
	build_tree7(tmpdir);
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);

	/* Default (no flags): every entry visited. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL, 0, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);

	/* REG|DIR: every entry visited. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);

	/* REG|DIR|ACTIONRETVAL with CONTINUE-returning callback: recursion must
	 * still descend (I3 regression: dir CONTINUE used to skip recursion). */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_continue, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);
	assert(n_reg == 4);
	assert(n_dir == 3);

	/* REG only: regular files visited, directories recursed but not called. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 4);
	assert(n_reg == 4);

	/* DIR only: directories only. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 3);
	assert(n_dir == 3);

	cleanup(tmpdir);

	/* ── 2. jstr_io_ftw (non-_len) wrapper ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_len_XXXXXX");
	assert(mkdtemp(tmpdir));
	build_tree7(tmpdir);
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw(tmpdir, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);
	cleanup(tmpdir);

	/* ── 3. Trailing-slash stripping (line 1153) ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_slash_XXXXXX");
	assert(mkdtemp(tmpdir));
	build_tree7(tmpdir);
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);
	snprintf(p, sizeof p, "%s/", tmpdir);
	n_visits = 0;
	ret = jstr_io_ftw_len(p, (jstr_io_path_size_ty)strlen(p), cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);
	snprintf(p, sizeof p, "%s//", tmpdir);
	n_visits = 0;
	ret = jstr_io_ftw_len(p, (jstr_io_path_size_ty)strlen(p), cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 7);
	cleanup(tmpdir);

	/* ── 4. NOHIDDEN ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_hid_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/.hid", tmpdir);
	create(p, 'h');
	snprintf(p, sizeof p, "%s/vis", tmpdir);
	create(p, 'v');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_NOHIDDEN, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 2); /* root + vis */
	n_visits = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 3); /* root + .hid + vis */
	cleanup(tmpdir);

	/* ── 5. NOSTAT: st_mode still usable, no stat syscalls needed ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_nostat_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/f", tmpdir);
	create(p, 'f');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = n_reg = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_nostat, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_NOSTAT, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 1);
	assert(n_reg == 1);
	cleanup(tmpdir);

	/* ── 6. STATREG: full stat (st_size) guaranteed for regular files ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_statreg_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/big", tmpdir);
	{
		FILE *f = fopen(p, "w");
		assert(f != NULL);
		for (int i = 0; i < 1234; i++)
			fputc('x', f);
		fclose(f);
	}
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = n_reg = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_statreg, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_STATREG, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 3);
	assert(n_reg == 1);
	cleanup(tmpdir);

	/* ── 7. func_match: MATCHPATH gives full path, default gives basename ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_match_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/xfile", tmpdir);
	create(p, 'x');
	snprintf(p, sizeof p, "%s/yfile", tmpdir);
	create(p, 'y');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	/* basename mode: matcher sees the entry name only. */
	last_match[0] = '\0';
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG, m_accept, NULL);
	assert(ret == 0);
	assert(strcmp(last_match, "yfile") == 0 || strcmp(last_match, "xfile") == 0);
	/* MATCHPATH mode: matcher sees the full path. */
	last_match[0] = '\0';
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_MATCHPATH, m_accept, NULL);
	assert(ret == 0);
	assert(strstr(last_match, "/xfile") != NULL || strstr(last_match, "/yfile") != NULL);
	/* matcher that skips 'x'-named files: xfile must not be visited. */
	n_visits = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_count, NULL,
	    JSTR_IO_FTW_REG, m_skip_x, NULL);
	assert(ret == 0);
	assert(n_visits == 1); /* only yfile */
	cleanup(tmpdir);

	/* ── 8. Error paths: empty and oversized dirpath ── */
	ret = jstr_io_ftw_len("", 0, cb_count, NULL, 0, NULL, NULL);
	assert(ret != 0);
	{
		size_t biglen = JSTR_IO_PATH_MAX + 4;
		char *big = malloc(biglen);
		assert(big != NULL);
		memset(big, 'a', biglen);
		ret = jstr_io_ftw_len(big, (jstr_io_path_size_ty)JSTR_IO_PATH_MAX,
		    cb_count, NULL, 0, NULL, NULL);
		assert(ret != 0);
		free(big);
	}

	/* ── 9. ACTIONRETVAL: dir SKIP_SUBTREE skips recursion, keeps siblings ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_skpst_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/sub1", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub1/f1", tmpdir);
	create(p, '1');
	snprintf(p, sizeof p, "%s/sub2", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub2/f2", tmpdir);
	create(p, '2');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	/* Both dirs visited, neither subtree recursed (I3 fix). */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_dir_skip_subtree, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 3);
	assert(n_reg == 0);
	assert(n_dir == 2);
	cleanup(tmpdir);

	/* ── 10. ACTIONRETVAL: dir SKIP_SIBLINGS aborts sibling iteration ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_sksib_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/sub1", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub1/f1", tmpdir);
	create(p, '1');
	snprintf(p, sizeof p, "%s/sub2", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub2/f2", tmpdir);
	create(p, '2');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	/* root + the single dir whose callback asked for it. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_dir_skip_siblings, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 2);
	assert(n_reg == 0);
	assert(n_dir == 1);
	cleanup(tmpdir);

	/* ── 11. ACTIONRETVAL: file SKIP_SUBTREE is a no-op, siblings continue ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_fsub_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/a", tmpdir);
	create(p, 'a');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = n_reg = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_file_skip_subtree, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 2); /* root + a */
	assert(n_reg == 1);
	cleanup(tmpdir);

	/* ── 12. ACTIONRETVAL: file SKIP_SIBLINGS aborts sibling iteration ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_fsib_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/a", tmpdir);
	create(p, 'a');
	snprintf(p, sizeof p, "%s/b", tmpdir);
	create(p, 'b');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = n_reg = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_file_skip_siblings, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 2); /* root + first file */
	assert(n_reg == 1);
	cleanup(tmpdir);

	/* ── 13. ACTIONRETVAL: STOP from a file callback in a subtree ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_stopf_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/deep", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/deep/x", tmpdir);
	create(p, 'x');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	/* The walk must descend into sub/deep (I3 regression) and stop at x. */
	n_visits = n_reg = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_stop_file, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == JSTR_IO_FTW_RET_STOP);
	assert(n_reg == 1);
	assert(n_visits == 4); /* root + sub + deep + x */
	cleanup(tmpdir);

	/* ── 14. ACTIONRETVAL: STOP from a dir callback at depth ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_stopd_XXXXXX");
	assert(mkdtemp(tmpdir));
	snprintf(p, sizeof p, "%s/sub", tmpdir);
	assert(mkdir(p, 0755) == 0);
	snprintf(p, sizeof p, "%s/sub/x", tmpdir);
	create(p, 'x');
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_stop_dir, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == JSTR_IO_FTW_RET_STOP);
	assert(n_visits == 2); /* root + sub */
	assert(n_reg == 0);
	assert(n_dir == 1);
	cleanup(tmpdir);

	/* ── 15. ACTIONRETVAL: normal complete subtree must NOT abort walk ── */
	snprintf(tmpdir, sizeof tmpdir, "/tmp/jstr_ftw3_recur_XXXXXX");
	assert(mkdtemp(tmpdir));
	build_tree6(tmpdir);
	dlen = (jstr_io_path_size_ty)strlen(tmpdir);
	/* Descend every dir normally (CONTINUE), keep going: full DFS = 6. */
	n_visits = n_reg = n_dir = 0;
	ret = jstr_io_ftw_len(tmpdir, dlen, cb_continue, NULL,
	    JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == 0);
	assert(n_visits == 6);
	assert(n_reg == 3); /* a, w, z */
	assert(n_dir == 3); /* root + sub1 + sub2 */
	cleanup(tmpdir);

	return 0;
}