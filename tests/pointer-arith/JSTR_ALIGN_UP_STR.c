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
		const size_t up = JSTR_ALIGN_UP_STR(off);
		assert(up == JSTR_ALIGN_UP(off, align));
		assert(up >= off);
		assert(up - off < align);
		assert(up % align == 0);
		assert(up == JSTR_ALIGN_DOWN_STR(off + align - 1));
		/* Aligned values are fixed points. */
		assert(JSTR_ALIGN_UP_STR(up) == up);
	}

	for (size_t k = 0; k <= 1000; ++k) {
		const size_t v = k * align;
		assert(JSTR_ALIGN_UP_STR(v) == v);
		assert(JSTR_ALIGN_UP_STR(v + 1) == v + align);
		assert(JSTR_ALIGN_UP_STR(v + align - 1) == v + align);
	}

	assert(JSTR_ALIGN_UP_STR(100) == JSTR_ALIGN_UP(100, align));
	assert(JSTR_ALIGN_UP_STR(100) >= 100);
	assert(JSTR_ALIGN_UP_STR(0) == 0);
	assert(JSTR_ALIGN_UP_STR(1) == align);

	return 0;
}
