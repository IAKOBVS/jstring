#include <assert.h>
#include "jstr.h"

#include <string.h>

int
main(void)
{
	assert(strcmp(JSTR_STRING(hello), "hello") == 0);
	assert(strcmp(JSTR_STRING(a + b), "a + b") == 0);
	return 0;
}
