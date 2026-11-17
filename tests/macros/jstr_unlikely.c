#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(jstr_unlikely(1) != 0);
	assert(jstr_unlikely(0) == 0);
	return 0;
}
