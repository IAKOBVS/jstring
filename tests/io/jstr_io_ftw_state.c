/* SPDX-License-Identifier: MIT */
/* Behavior pins for the explicit FTW result-state dispatch. */

#define _POSIX_C_SOURCE 200809L

#include <assert.h>
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
	char late[160];
	int calls;
};

static void
fixture_make(struct fixture *f)
{
	char tpl[] = "/tmp/jstr_ftw_state_XXXXXX";
	assert(mkdtemp(tpl) != NULL);
	assert(strlen(tpl) < sizeof(f->root));
	strcpy(f->root, tpl);
	assert(snprintf(f->sub, sizeof(f->sub), "%s/sub", f->root) > 0);
	assert(snprintf(f->deep, sizeof(f->deep), "%s/deep", f->sub) > 0);
	assert(snprintf(f->file, sizeof(f->file), "%s/x", f->deep) > 0);
	assert(snprintf(f->late, sizeof(f->late), "%s/z-late", f->root) > 0);
	assert(mkdir(f->sub, 0755) == 0);
	assert(mkdir(f->deep, 0755) == 0);
	FILE *fp = fopen(f->file, "w");
	assert(fp != NULL);
	fclose(fp);
	fp = fopen(f->late, "w");
	assert(fp != NULL);
	fclose(fp);
}

static void
fixture_free(struct fixture *f)
{
	assert(unlink(f->file) == 0);
	assert(unlink(f->late) == 0);
	assert(rmdir(f->deep) == 0);
	assert(rmdir(f->sub) == 0);
	assert(rmdir(f->root) == 0);
}

static int
is_name(const struct JSTR_IO_FTW *ftw, const char *name)
{
	return ftw->ep != NULL && strcmp(ftw->ep->d_name, name) == 0;
}

static int
cb_stop_on_x(const struct JSTR_IO_FTW *ftw, const void *args)
{
	struct fixture *f = (struct fixture *)args;
	++f->calls;
	return is_name(ftw, "x") ? JSTR_IO_FTW_RET_STOP : JSTR_IO_FTW_RET_CONTINUE;
}

static int
cb_invalid_42(const struct JSTR_IO_FTW *ftw, const void *args)
{
	struct fixture *f = (struct fixture *)args;
	++f->calls;
	return is_name(ftw, "x") ? 42 : JSTR_IO_FTW_RET_CONTINUE;
}

static int
cb_error_minus1(const struct JSTR_IO_FTW *ftw, const void *args)
{
	struct fixture *f = (struct fixture *)args;
	++f->calls;
	return is_name(ftw, "x") ? -1 : 0;
}

static int
cb_minus2(const struct JSTR_IO_FTW *ftw, const void *args)
{
	struct fixture *f = (struct fixture *)args;
	++f->calls;
	return is_name(ftw, "x") ? -2 : 0;
}

static int
cb_return_seven(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)ftw;
	(void)args;
	return 7;
}

static int root_slash_checked;

static int
cb_root_slash(const struct JSTR_IO_FTW *ftw, const void *args)
{
	(void)args;
	if (ftw->ep == NULL) {
		root_slash_checked = 1;
		assert(ftw->dirpath_len == 0);
		assert(ftw->st != NULL);
		assert(strcmp(ftw->dirpath, "/") == 0);
	}
	return JSTR_IO_FTW_RET_STOP;
}

static int
walk(struct fixture *f, jstr_io_ftw_func_ty cb, int flags)
{
	f->calls = 0;
	return jstr_io_ftw_len(f->root, (jstr_io_path_size_ty)strlen(f->root), cb, f, flags, NULL, NULL);
}

int
main(void)
{
	struct fixture f;
	fixture_make(&f);

	/* STOP from a deeply nested file aborts the whole walk and returns 0. */
	int flags = JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR | JSTR_IO_FTW_ACTIONRETVAL;
	int ret = walk(&f, cb_stop_on_x, flags);
	assert(ret == JSTR_RET_SUCC);
	assert(f.calls >= 4);

	/* Nested invalid ACTIONRETVAL value is treated as STOP (0). */
	ret = walk(&f, cb_invalid_42, flags);
	assert(ret == JSTR_RET_SUCC);

	/* Non-ACTIONRETVAL: -1 propagates through two ancestor frames. */
	flags = JSTR_IO_FTW_REG | JSTR_IO_FTW_DIR;
	ret = walk(&f, cb_error_minus1, flags);
	assert(ret == JSTR_RET_ERR);

	/* jstr_chk only recognises -1; -2 is not an error. */
	ret = walk(&f, cb_minus2, flags);
	assert(ret == JSTR_RET_SUCC);

	/* A non-directory root returns the callback value verbatim. */
	ret = jstr_io_ftw_len(f.file, (jstr_io_path_size_ty)strlen(f.file), cb_return_seven, NULL, 0, NULL, NULL);
	assert(ret == 7);

	/* Root "/" passes a zero-length dirpath and a usable stat buffer. */
	root_slash_checked = 0;
	ret = jstr_io_ftw_len("/", 1, cb_root_slash, NULL, JSTR_IO_FTW_DIR | JSTR_IO_FTW_NOSUBDIR | JSTR_IO_FTW_ACTIONRETVAL, NULL, NULL);
	assert(ret == JSTR_RET_SUCC);
	assert(root_slash_checked);

	fixture_free(&f);
	return 0;
}
