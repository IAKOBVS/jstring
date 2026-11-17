#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_FILENO == 0) || (JSTR_HAVE_FILENO == 1));
	return 0;
}
