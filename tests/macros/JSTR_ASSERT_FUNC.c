#include <assert.h>
#include "jstr.h"

#include <string.h>

static void
check_func(void)
{
	assert(strcmp(JSTR_ASSERT_FUNC, "check_func") == 0);
}

int
main(void)
{
	check_func();
	return 0;
}
