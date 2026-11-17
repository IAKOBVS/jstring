#include <assert.h>
#include "jstr.h"

#include <stdint.h>
#include <stdlib.h>
#include "pointer-arith.h"

static char storage[2 * 64 + 64];

int
main(void)
{
	const uintptr_t align = JSTR_MALLOC_ALIGNMENT;
	const uintptr_t base = JSTR_PTR_ALIGN_UP((uintptr_t)storage, align);

	for (uintptr_t off = 0; off < 2 * align; ++off) {
		const uintptr_t p = base + off;
		const int expected = (off % align == 0);
		assert((JSTR_PTR_IS_ALIGNED_STR(p) != 0) == expected);
		assert(JSTR_PTR_IS_ALIGNED_STR(p) ==
		       JSTR_PTR_IS_ALIGNED(p, align));
	}

	assert(JSTR_PTR_IS_ALIGNED_STR(base) != 0);
	if (align > 1)
		assert(JSTR_PTR_IS_ALIGNED_STR(base + 1) == 0);
	assert(JSTR_PTR_IS_ALIGNED_STR((uintptr_t)0) != 0);

	/* malloc is documented to return maximally aligned storage. */
	{
		void *m = malloc(1);
		assert(m != NULL);
		assert(JSTR_PTR_IS_ALIGNED_STR(m) != 0);
		free(m);
	}

	return 0;
}
