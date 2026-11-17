#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char **argv)
{
	START();
	FILE *fp = tmpfile();
	assert(fp);
	assert(jstr_io_fwritefilefp_len(jstr_literal("hello"), fp) == JSTR_RET_SUCC);
	/* read back */
	rewind(fp);
	char buf[64];
	size_t r = fread(buf, 1, 64, fp);
	buf[r] = '\0';
	assert(r == 6);
	assert(strcmp(buf, "hello\n") == 0);
	/* sz == 0 */
	assert(jstr_io_fwritefilefp_len("x", 0, fp) == JSTR_RET_SUCC);
	fclose(fp);
	/* already newline */
	fp = tmpfile();
	assert(fp);
	assert(jstr_io_fwritefilefp_len(jstr_literal("hi\n"), fp) == JSTR_RET_SUCC);
	rewind(fp);
	r = fread(buf, 1, 64, fp);
	buf[r] = '\0';
	assert(r == 3);
	assert(strcmp(buf, "hi\n") == 0);
	fclose(fp);
	SUCCESS();
	return 0;
}
