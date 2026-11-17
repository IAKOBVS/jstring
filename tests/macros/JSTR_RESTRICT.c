#include <assert.h>
#include "jstr.h"

static int
add(int *JSTR_RESTRICT a, int *JSTR_RESTRICT b)
{
	return *a + *b;
}

int
main(void)
{
	int a = 2, b = 3;
	assert(add(&a, &b) == 5);
	return 0;
}
