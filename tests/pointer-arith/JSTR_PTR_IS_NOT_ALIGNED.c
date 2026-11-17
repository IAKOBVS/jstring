#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include "pointer-arith.h"

static char storage[2 * 4096 + 64];

int
main(void)
{
	static const uintptr_t aligns[] = { 1, 2, 4, 8, 16, 32, 64, 4096 };

	for (size_t a = 0; a < sizeof aligns / sizeof aligns[0]; ++a) {
		const uintptr_t align = aligns[a];
		const uintptr_t base = JSTR_PTR_ALIGN_UP((uintptr_t)storage, align);

		for (uintptr_t off = 0; off < 2 * align; ++off) {
			const uintptr_t p = base + off;
			const int misaligned = (off % align != 0);
			assert((JSTR_PTR_IS_NOT_ALIGNED(p, align) != 0) == misaligned);
			/* Exact complement of the positive predicate. */
			assert(!JSTR_PTR_IS_NOT_ALIGNED(p, align) ==
			       (JSTR_PTR_IS_ALIGNED(p, align) != 0));
		}

		assert(JSTR_PTR_IS_NOT_ALIGNED(base, align) == 0);
		assert(JSTR_PTR_IS_NOT_ALIGNED(base + align, align) == 0);
		if (align > 1)
			assert(JSTR_PTR_IS_NOT_ALIGNED(base + 1, align) != 0);

		assert(JSTR_PTR_IS_NOT_ALIGNED((uintptr_t)0, align) == 0);
	}

	return 0;
}
