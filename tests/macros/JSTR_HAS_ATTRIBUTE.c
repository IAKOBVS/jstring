#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_HAS_ATTRIBUTE(const) == 0 || JSTR_HAS_ATTRIBUTE(const) == 1);
	return 0;
}
