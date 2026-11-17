#include <assert.h>
#include "jstr.h"

static void *
fail(void)
{
	JSTR_RETURN_ERR_P(NULL);
}

int
main(void)
{
	assert(fail() == NULL);
	return 0;
}
