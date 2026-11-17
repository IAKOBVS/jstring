#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	FILE *f = tmpfile();
	char buf[8] = {0};
	assert(f != NULL);
	assert(fputs("hi\n", f) >= 0);
	rewind(f);
	assert(jstr_io_fgets(buf, sizeof buf, f) == buf);
	assert(strcmp(buf, "hi\n") == 0);
	fclose(f);
	return 0;
}
