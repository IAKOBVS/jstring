#include <assert.h>
#include "jstr.h"

#include "config.h"

int
main(void)
{
	assert(JSTR_DEBUG == 0 || JSTR_DEBUG == 1);
	if (JSTR_DEBUG)
		assert(JSTR_PANIC == 1);
	return 0;
}
