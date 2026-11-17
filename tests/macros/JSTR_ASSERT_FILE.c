#include <assert.h>
#include "jstr.h"

#include <string.h>

int
main(void)
{
	assert(strlen(JSTR_ASSERT_FILE) > 0);
	return 0;
}
