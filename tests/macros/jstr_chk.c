#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(jstr_chk(-1));
	assert(!jstr_chk(0));
	assert(!jstr_chk(1));
	return 0;
}
