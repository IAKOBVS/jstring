#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	char a[16];

	/* All ordered pairs, signed result, including negative direction. */
	for (size_t i = 0; i < sizeof a; ++i) {
		for (size_t j = 0; j < sizeof a; ++j) {
			const ptrdiff_t d = JSTR_PTR_DIFF(&a[i], &a[j]);
			assert(d == (ptrdiff_t)i - (ptrdiff_t)j);
			assert(-d == JSTR_PTR_DIFF(&a[j], &a[i]));
		}
		assert(JSTR_PTR_DIFF(&a[i], &a[i]) == 0);
	}

	assert(JSTR_PTR_DIFF(&a[5], &a[2]) == 3);
	assert(JSTR_PTR_DIFF(&a[2], &a[5]) == -3);
	assert(JSTR_PTR_DIFF(&a[1], &a[0]) == 1);
	assert(JSTR_PTR_DIFF(&a[0], &a[1]) == -1);

	/* Round trip: adding the signed delta to the lower pointer recovers the
	 * higher one. */
	for (size_t i = 0; i < sizeof a; ++i) {
		for (size_t j = 0; j <= i; ++j) {
			char *p = &a[j] + JSTR_PTR_DIFF(&a[i], &a[j]);
			assert(p == &a[i]);
		}
	}

	return 0;
}
