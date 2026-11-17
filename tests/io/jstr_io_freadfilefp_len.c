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
	const char *data = "hello";
	size_t dlen = 5;
	FILE *fp = tmpfile();
	assert(fp);
	assert(fwrite(data, 1, dlen, fp) == dlen);
	rewind(fp);
	char *s = NULL;
	size_t sz = 0, cap = 0;
	assert(jstr_io_freadfilefp_len(&s, &sz, &cap, fp, dlen) == JSTR_RET_SUCC);
	assert(sz == dlen);
	assert(memcmp(s, data, dlen) == 0);
	assert(s[dlen] == '\0');
	free(s);
	/* 0 */
	rewind(fp);
	/* empty stream read 0 */
	s = NULL; sz = 0; cap = 0;
	assert(jstr_io_freadfilefp_len(&s, &sz, &cap, fp, 0) == JSTR_RET_SUCC);
	assert(sz == 0);
	free(s);
	/* short read error */
	fp = tmpfile();
	assert(fp);
	assert(fwrite(data, 1, dlen, fp) == dlen);
	rewind(fp);
	s = NULL; sz = 0; cap = 0;
	assert(jstr_io_freadfilefp_len(&s, &sz, &cap, fp, dlen + 1) != JSTR_RET_SUCC);
	free(s);
	fclose(fp);
	SUCCESS();
	return 0;
}
