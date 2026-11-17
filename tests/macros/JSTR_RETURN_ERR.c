#include <assert.h>
#include "jstr.h"

static int
fail(void)
{
	JSTR_RETURN_ERR(-5);
}

int
main(void)
{
	assert(fail() == -5);
	return 0;
}
