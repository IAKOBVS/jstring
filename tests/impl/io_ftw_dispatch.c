/* SPDX-License-Identifier: MIT */
/* Direct unit tests for the internal FTW dispatch helpers documented by the
 * @anchor spec_jstr_internal_io_ftw_map_callback and
 * @anchor spec_jstr_internal_io_ftw_map_child spec comments in include/io.h.
 * These statics are only visible when JSTR_IMPLEMENTATION is defined. */

#include <assert.h>

#include "jstr.h"
#include "io.h"

#define ST_CNTX    JSTR_INTERNAL_IO_FTW_NEXT
#define ST_RECURSE JSTR_INTERNAL_IO_FTW_RECURSE
#define ST_SKIP    JSTR_INTERNAL_IO_FTW_SKIP_SIBLINGS
#define ST_STOP    JSTR_INTERNAL_IO_FTW_STOP
#define ST_ERROR   JSTR_INTERNAL_IO_FTW_ERROR
#define ST_FINISH  JSTR_INTERNAL_IO_FTW_FINISH

static void
test_map_callback_plain(void)
{
	/* actionretval == 0: only -1 is an error; everything else continues. */
	for (int at_root = 0; at_root <= 1; ++at_root) {
		jstr_internal_io_ftw_state_ty on_continue = at_root ? ST_RECURSE : ST_CNTX;
		assert(jstr_internal_io_ftw_map_callback(-1, 0, at_root, on_continue) == ST_ERROR);
		assert(jstr_internal_io_ftw_map_callback(-2, 0, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_STOP, 0, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_CONTINUE, 0, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_SKIP_SIBLINGS, 0, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_SKIP_SUBTREE, 0, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(42, 0, at_root, on_continue) == on_continue);
	}
}

static void
test_map_callback_actionretval(void)
{
	/* ACTIONRETVAL: CONTINUE/SKIP_* are normalized, STOP aborts, and an
	 * invalid value is ERROR at the root but STOP in a nested frame. */
	for (int at_root = 0; at_root <= 1; ++at_root) {
		jstr_internal_io_ftw_state_ty on_continue = at_root ? ST_RECURSE : ST_CNTX;
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_CONTINUE, 1, at_root, on_continue) == on_continue);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_SKIP_SUBTREE, 1, at_root, on_continue) == (at_root ? on_continue : ST_CNTX));
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_SKIP_SIBLINGS, 1, at_root, on_continue) == (at_root ? on_continue : ST_SKIP));
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_STOP, 1, at_root, on_continue) == ST_STOP);
		assert(jstr_internal_io_ftw_map_callback(42, 1, at_root, on_continue) == (at_root ? ST_ERROR : ST_STOP));
		assert(jstr_internal_io_ftw_map_callback(-1, 1, at_root, on_continue) == (at_root ? ST_ERROR : ST_STOP));
	}
}

static void
test_map_callback_is_pure(void)
{
	/* Repeated calls with identical inputs must produce identical results;
	 * this pins the documented "writes nothing except its return value". */
	for (int i = 0; i < 64; ++i) {
		assert(jstr_internal_io_ftw_map_callback(42, 1, 0, ST_CNTX) == ST_STOP);
		assert(jstr_internal_io_ftw_map_callback(42, 1, 1, ST_RECURSE) == ST_ERROR);
		assert(jstr_internal_io_ftw_map_callback(JSTR_IO_FTW_RET_SKIP_SIBLINGS, 1, 0, ST_CNTX) == ST_SKIP);
		assert(jstr_internal_io_ftw_map_callback(-1, 0, 1, ST_RECURSE) == ST_ERROR);
	}
}

static void
test_map_child(void)
{
	/* STOP and ERROR propagate unchanged; every other state becomes NEXT. */
	assert(jstr_internal_io_ftw_map_child(ST_STOP) == ST_STOP);
	assert(jstr_internal_io_ftw_map_child(ST_ERROR) == ST_ERROR);
	assert(jstr_internal_io_ftw_map_child(ST_CNTX) == ST_CNTX);
	assert(jstr_internal_io_ftw_map_child(ST_RECURSE) == ST_CNTX);
	assert(jstr_internal_io_ftw_map_child(ST_SKIP) == ST_CNTX);
	assert(jstr_internal_io_ftw_map_child(ST_FINISH) == ST_CNTX);

	for (int i = 0; i < 64; ++i)
		assert(jstr_internal_io_ftw_map_child(ST_RECURSE) == ST_CNTX);
}

int
main(void)
{
	test_map_callback_plain();
	test_map_callback_actionretval();
	test_map_callback_is_pure();
	test_map_child();
	return 0;
}
