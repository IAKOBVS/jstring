#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	assert(JSTR_ALIGN_DOWN(100, 8) == 96);
	assert(JSTR_ALIGN_DOWN(0, 8) == 0);
	assert(JSTR_ALIGN_DOWN(8, 8) == 8);
	return 0;
}
