#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int a[7];
	char b[3];
	assert(JSTR_ARRAY_COUNT(a) == 7);
	assert(JSTR_ARRAY_COUNT(b) == 3);
	assert(JSTR_ARRAY_COUNT("hello") == 6);
	return 0;
}
