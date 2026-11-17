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
			const uintptr_t got = JSTR_PTR_ALIGN_UP(p, align);

			assert(got == ((p + align - 1) & ~(align - 1)));
			assert(got >= p);
			assert(got - p < align);
			assert(got % align == 0);
			assert(got == JSTR_PTR_ALIGN_DOWN(p + align - 1, align));
			/* Fixed point. */
			assert(JSTR_PTR_ALIGN_UP(got, align) == got);
			/* Same result through the pointer-typed form. */
			assert(JSTR_PTR_ALIGN_UP(storage + off, align) == got);
		}

		assert(JSTR_PTR_ALIGN_UP((uintptr_t)0, align) == 0);
	}

	assert(JSTR_PTR_ALIGN_UP((uintptr_t)0x1234, 8) == 0x1238);
	assert(JSTR_PTR_ALIGN_UP((uintptr_t)0x1234, 16) == 0x1240);
	assert(JSTR_PTR_ALIGN_UP((uintptr_t)0x1234, 4096) == 0x2000);
	assert(JSTR_PTR_ALIGN_UP((uintptr_t)0x1000, 4096) == 0x1000);

	return 0;
}
