#include "../src/jarr.h"

#define ASZ(arr) (sizeof(arr) / sizeof(arr[0]))
#define T(func, ...)                                              \
	do {                                                      \
		int expected[] = { __VA_ARGS__ };                 \
		func;                                             \
		assert(j.data);                                   \
		assert(!memcmp(j.data, expected, ASZ(expected))); \
	} while (0)

int
main()
{
	jarr_ty(int, j) = JARR_INIT;
	T(jarr_cat(&j, 1, 2, 3, 4, 5), 1, 2, 3, 4, 5);
	T(jarr_push_back(&j, 6), 1, 2, 3, 4, 5, 6);
	T(jarr_pop_back(&j), 1, 2, 3, 4, 5);
	T(jarr_pop_front(&j), 2, 3, 4, 5);
	jarr_free(&j);
	return 0;
}
