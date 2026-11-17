#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert((JSTR_HAVE_PUTC_UNLOCKED == 0) || (JSTR_HAVE_PUTC_UNLOCKED == 1));
	return 0;
}
