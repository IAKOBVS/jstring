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
	assert(jstr_io_putc('Q', f) == 'Q');
	rewind(f);
	assert(jstr_io_getc(f) == 'Q');
	assert(jstr_io_getc(f) == EOF);
	fclose(f);
	return 0;
}
