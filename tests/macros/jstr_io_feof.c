#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	FILE *f = tmpfile();
	assert(f != NULL);
	assert(jstr_io_feof(f) == 0);
	assert(jstr_io_fgetc(f) == EOF);
	assert(jstr_io_feof(f) != 0);
	fclose(f);
	return 0;
}
