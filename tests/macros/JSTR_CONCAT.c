#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int JSTR_CONCAT(foo, bar) = 7;
	assert(foobar == 7);
	return 0;
}
