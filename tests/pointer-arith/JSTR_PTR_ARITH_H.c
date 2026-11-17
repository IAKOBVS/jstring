#include <assert.h>
#include "jstr.h"

#include <stddef.h>
#include <stdint.h>
#include "pointer-arith.h"

static char storage[2 * JSTR_PAGE_SIZE + 64];

static int
ref_not_crossing(uintptr_t p, size_t obj, size_t page)
{
	const size_t rem = (size_t)(p % page);
	/* JSTR_PTR_ALIGN_UP is a fixed point on an exactly page-aligned
	 * address, so the distance there is 0, not page. */
	const size_t dist = rem ? page - rem : 0;
	return dist >= obj;
}

int
main(void)
{
	assert(JSTR_PTR_ARITH_H == 1);

	/* Numeric sanity around a page boundary (no memory is dereferenced). */
	assert(JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x0f9c, 100, JSTR_PAGE_SIZE));
	assert(!JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x0f9c, 101, JSTR_PAGE_SIZE));
	assert(!JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x1000, 1, JSTR_PAGE_SIZE));
	assert(JSTR_PTR_NOT_CROSSING_PAGE((uintptr_t)0x1000, 0, JSTR_PAGE_SIZE));

	/* Exhaustive differential check against the reference formula near a
	 * real, page-aligned object.  Only addresses are computed; no byte past
	 * the object is read. */
	{
		const uintptr_t base =
			JSTR_PTR_ALIGN_UP((uintptr_t)storage, JSTR_PAGE_SIZE);
		static const size_t objs[] = { 0, 1, 2, 7, 64,
			                       JSTR_PAGE_SIZE - 1,
			                       JSTR_PAGE_SIZE,
			                       JSTR_PAGE_SIZE + 1 };

		for (uintptr_t off = 0; off < 2 * JSTR_PAGE_SIZE; ++off) {
			const uintptr_t p = base + off;
			for (size_t k = 0; k < sizeof objs / sizeof objs[0]; ++k) {
				const int got = JSTR_PTR_NOT_CROSSING_PAGE(
					p, objs[k], JSTR_PAGE_SIZE);
				assert(got == ref_not_crossing(p, objs[k], JSTR_PAGE_SIZE));
			}
		}

		/* Last byte of a page and the first byte of the next. */
		const uintptr_t last = base + JSTR_PAGE_SIZE - 1;
		const uintptr_t next = base + JSTR_PAGE_SIZE;
		assert(JSTR_PTR_NOT_CROSSING_PAGE(last, 1, JSTR_PAGE_SIZE));
		assert(!JSTR_PTR_NOT_CROSSING_PAGE(last, 2, JSTR_PAGE_SIZE));
		assert(!JSTR_PTR_NOT_CROSSING_PAGE(next, 1, JSTR_PAGE_SIZE));
		assert(!JSTR_PTR_NOT_CROSSING_PAGE(next, JSTR_PAGE_SIZE,
		                                   JSTR_PAGE_SIZE));
	}

	/* Smaller power-of-two page sizes, using the same reference formula. */
	{
		static const size_t pages[] = { 1, 2, 4, 8, 16, 32, 64 };
		for (size_t p = 0; p < sizeof pages / sizeof pages[0]; ++p) {
			const size_t page = pages[p];
			for (uintptr_t off = 0; off < 2 * page; ++off) {
				for (size_t obj = 0; obj <= page + 1; ++obj) {
					assert(JSTR_PTR_NOT_CROSSING_PAGE(
						       (uintptr_t)0x10000 + off, obj, page) ==
					       ref_not_crossing((uintptr_t)0x10000 + off,
					                        obj,
					                        page));
				}
			}
		}
	}

	return 0;
}
