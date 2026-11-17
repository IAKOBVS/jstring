#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "jstr.h"

int
main(void)
{
	FILE *f = tmpfile();
	wchar_t buf[8] = {0};
	assert(f != NULL);
	assert(jstr_io_fputws(L"hi", f) >= 0);
	rewind(f);
	assert(jstr_io_fgetws(buf, 8, f) != NULL);
	assert(wcscmp(buf, L"hi") == 0);
	fclose(f);
	return 0;
}
