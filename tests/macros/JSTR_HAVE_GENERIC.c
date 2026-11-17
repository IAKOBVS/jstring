#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_GENERIC == 0) || (JSTR_HAVE_GENERIC == 1));
	return 0;
}
