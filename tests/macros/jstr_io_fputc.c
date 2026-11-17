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
	assert(jstr_io_fputc('A', f) == 'A');
	assert(jstr_io_fflush(f) == 0);
	rewind(f);
	assert(jstr_io_fgetc(f) == 'A');
	fclose(f);
	return 0;
}
