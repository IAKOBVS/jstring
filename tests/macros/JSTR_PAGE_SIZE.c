#include <assert.h>
#include "jstr.h"

int
main(void)
{
	assert(JSTR_PAGE_SIZE == 4096);
	return 0;
}
