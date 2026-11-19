#define _POSIX_C_SOURCE 200809L
#include "../../tests/test.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(void)
{
	jstr_ty j = JSTR_INIT;
	j.data = malloc(8);
	strcpy(j.data, "hello");
	j.size = 5;
	j.capacity = 8;
	FILE *fp = tmpfile();
	assert(fp);
	assert(jstr_io_fwritefilefp_len_j(&j, fp) == JSTR_RET_SUCC);
	rewind(fp);
	char buf[64];
	size_t r = fread(buf, 1, 64, fp);
	buf[r] = '\0';
	assert(r == 6);
	assert(strcmp(buf, "hello\n") == 0);
	fclose(fp);
	free(j.data);
	return 0;
}
