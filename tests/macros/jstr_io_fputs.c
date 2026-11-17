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
	assert(jstr_io_fputs("hi", f) >= 0);
	rewind(f);
	assert(fgets(buf, sizeof buf, f) != NULL);
	assert(strcmp(buf, "hi") == 0);
	fclose(f);
	return 0;
}
