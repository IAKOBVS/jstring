#include <assert.h>
#include "jstr.h"

int
main(void)
{
	JSTR_ASSERT_DEBUG_PRINTF(1, "true %d\n", 1);
	return 0;
}
