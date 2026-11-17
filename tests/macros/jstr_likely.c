#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(jstr_likely(1) != 0);
	assert(jstr_likely(0) == 0);
	return 0;
}
