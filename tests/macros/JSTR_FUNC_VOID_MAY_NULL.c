#include <assert.h>
#include "jstr.h"

JSTR_FUNC_VOID_MAY_NULL static void helper(void) { }

int
main(void)
{
	helper();
	return 0;
}
