#include <assert.h>
#include "jstr.h"

int
main(void)
{
	JSTR_STATIC_ASSERT(1, "true");
	JSTR_STATIC_ASSERT(sizeof(int) >= 2, "int size");
	return 0;
}
