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
	assert(jstr_io_fputwc(L'A', f) == L'A');
	rewind(f);
	assert(jstr_io_getwc(f) == L'A');
	fclose(f);
	return 0;
}
