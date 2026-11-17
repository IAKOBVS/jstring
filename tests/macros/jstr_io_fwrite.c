#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	FILE *f = tmpfile();
	char out[4] = {0};
	assert(f != NULL);
	assert(jstr_io_fwrite("abc", 1, 3, f) == 3);
	rewind(f);
	assert(fread(out, 1, 3, f) == 3);
	assert(memcmp(out, "abc", 3) == 0);
	fclose(f);
	return 0;
}
