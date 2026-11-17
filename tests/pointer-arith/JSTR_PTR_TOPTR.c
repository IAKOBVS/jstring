#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	char a[16];

	for (size_t i = 0; i < sizeof a; ++i) {
		const uintptr_t u = JSTR_PTR_TOPTR(&a[i]);
		assert(u == (uintptr_t)&a[i]);
		/* Round trip back to a pointer. */
		assert((char *)u == &a[i]);
		assert(JSTR_PTR_DIFF((void *)JSTR_PTR_TOPTR(&a[i]), &a[0]) ==
		       (ptrdiff_t)i);
	}

	assert(JSTR_PTR_TOPTR(&a[0]) == (uintptr_t)&a[0]);
	assert(JSTR_PTR_TOPTR((void *)0) == 0);

	/* TOPT R + DIFF round trips in every direction. */
	for (size_t i = 0; i < sizeof a; ++i) {
		for (size_t j = 0; j < sizeof a; ++j) {
			const ptrdiff_t d = JSTR_PTR_DIFF(&a[i], &a[j]);
			assert((char *)&a[j] + d == &a[i]);
			assert((uintptr_t)&a[j] + JSTR_DIFF(&a[i], &a[j]) ==
			       (uintptr_t)&a[i]);
			assert(JSTR_PTR_DIFF((void *)JSTR_PTR_TOPTR(&a[i]), &a[j]) ==
			       d);
		}
	}

	return 0;
}
