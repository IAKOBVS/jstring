#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	const char *data = "abc";
	FILE *fp = tmpfile();
	assert(fp);
	assert(fwrite(data, 1, 3, fp) == 3);
	rewind(fp);
	jstr_ty j = JSTR_INIT;
	assert(jstr_io_freadfilefp_len_j(&j, fp, 3) == JSTR_RET_SUCC);
	assert(j.size == 3);
	assert(memcmp(j.data, data, 3) == 0);
	fclose(fp);
	free(j.data);
	return 0;
}
