#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_PANIC == 0 || JSTR_PANIC == 1);
	return 0;
}
