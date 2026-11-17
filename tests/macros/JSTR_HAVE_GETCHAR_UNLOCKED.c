#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_GETCHAR_UNLOCKED == 0) || (JSTR_HAVE_GETCHAR_UNLOCKED == 1));
	return 0;
}
