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
		const uintptr_t base = (uintptr_t)storage;

		for (uintptr_t off = 0; off < 2 * align; ++off) {
			const uintptr_t p = base + off;
			const uintptr_t got = JSTR_PTR_ALIGN_DOWN(p, align);

			assert(got == (p & ~(align - 1)));
			assert(got <= p);
			assert(p - got < align);
			assert(got % align == 0);
			/* Fixed point. */
			assert(JSTR_PTR_ALIGN_DOWN(got, align) == got);
			/* Same result through the pointer-typed form. */
			assert(JSTR_PTR_ALIGN_DOWN(storage + off, align) == got);
		}

		/* NULL / zero address is representable and stays zero. */
		assert(JSTR_PTR_ALIGN_DOWN((uintptr_t)0, align) == 0);
	}

	assert(JSTR_PTR_ALIGN_DOWN((uintptr_t)0x1234, 8) == 0x1230);
	assert(JSTR_PTR_ALIGN_DOWN((uintptr_t)0x1234, 16) == 0x1230);
	assert(JSTR_PTR_ALIGN_DOWN((uintptr_t)0x1234, 4096) == 0x1000);

	return 0;
}
