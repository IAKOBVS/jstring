#include <assert.h>
#include "jstr.h"

int
main(void)
{
	int x;
	assert(jstr_nullchk(NULL));
	assert(!jstr_nullchk(&x));
	return 0;
}
