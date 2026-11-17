#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	char a[16];

	/* All ordered pairs within a small object, including negative deltas. */
	for (size_t i = 0; i < sizeof a; ++i) {
		for (size_t j = 0; j < sizeof a; ++j) {
			const size_t d = JSTR_DIFF(&a[i], &a[j]);
			assert(d == (size_t)(i - j));
			if (i >= j)
				assert(d == i - j);
			else
				assert(d == (size_t)-(ptrdiff_t)(j - i));
		}
		assert(JSTR_DIFF(&a[i], &a[i]) == 0);
	}

	/* Additivity over a chain of pointers. */
	assert(JSTR_DIFF(&a[7], &a[2]) + JSTR_DIFF(&a[2], &a[5]) ==
	       JSTR_DIFF(&a[7], &a[5]));
	assert(JSTR_DIFF(&a[5], &a[2]) == 3);
	assert(JSTR_DIFF(&a[2], &a[5]) == (size_t)-3);

	/* A gap of exactly one in each direction. */
	assert(JSTR_DIFF(&a[1], &a[0]) == 1);
	assert(JSTR_DIFF(&a[0], &a[1]) == (size_t)-1);

	return 0;
}
