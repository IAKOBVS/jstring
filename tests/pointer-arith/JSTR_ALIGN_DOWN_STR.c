#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

int
main(void)
{
	const size_t align = JSTR_MALLOC_ALIGNMENT;

	/* Exhaustive sweep over offsets around the alignment boundary. */
	for (size_t off = 0; off < 2 * align; ++off) {
		const size_t down = JSTR_ALIGN_DOWN_STR(off);
		assert(down == JSTR_ALIGN_DOWN(off, align));
		assert(down == off - off % align);
		assert(down <= off);
		assert(off - down < align);
		assert(down % align == 0);
		/* Aligned values are fixed points. */
		assert(JSTR_ALIGN_DOWN_STR(down) == down);
	}

	/* Larger values and exact multiples. */
	for (size_t k = 0; k <= 1000; ++k) {
		const size_t v = k * align;
		assert(JSTR_ALIGN_DOWN_STR(v) == v);
		assert(JSTR_ALIGN_DOWN_STR(v + 1) == v);
		assert(JSTR_ALIGN_DOWN_STR(v + align - 1) == v);
	}

	assert(JSTR_ALIGN_DOWN_STR(100) == JSTR_ALIGN_DOWN(100, align));
	assert(JSTR_ALIGN_DOWN_STR(100) <= 100);
	assert(JSTR_ALIGN_DOWN_STR(0) == 0);
	assert(JSTR_ALIGN_DOWN_STR(1) == 0);

	return 0;
}
