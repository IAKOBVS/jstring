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
	assert(jstr_io_fputws(L"hi\n", f) >= 0);
	rewind(f);
	assert(jstr_io_fgetws(buf, 8, f) == buf);
	assert(wcscmp(buf, L"hi\n") == 0);
	fclose(f);
	return 0;
}
