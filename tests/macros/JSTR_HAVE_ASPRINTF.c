#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_ASPRINTF == 0) || (JSTR_HAVE_ASPRINTF == 1));
	return 0;
}
