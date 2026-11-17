#include <assert.h>
#include "jstr.h"

#include <stddef.h>

static size_t
fail(void)
{
	JSTR_RETURN_ERR_ZU((size_t)42);
}

int
main(void)
{
	assert(fail() == 42);
	return 0;
}
