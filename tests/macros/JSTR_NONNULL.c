#include <assert.h>
#include "jstr.h"

static int
nonnull(JSTR_NONNULL((1)) const void *p)
{
	return p != NULL;
}

int
main(void)
{
	int x = 0;
	assert(nonnull(&x));
	return 0;
}
