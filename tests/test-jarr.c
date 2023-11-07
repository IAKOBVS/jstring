#include "../src/jarr.h"
#include "test.h"

#define ASZ(arr) (sizeof(arr) / sizeof(arr[0]))
#define T(func, ...)                                              \
	do {                                                      \
		int expected[] = { __VA_ARGS__ };                 \
		func;                                             \
		assert(j.data);                                   \
		assert(!memcmp(j.data, expected, ASZ(expected))); \
	} while (0)

int
main(int argc, char **argv)
{
	jarr_ty(int, j) = JARR_INIT;
	TESTING(jarr_cat);
	T(jarr_cat(&j, 1, 2, 3, 4, 5),
	  1,
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_pushback);
	T(jarr_pushback(&j, 6),
	  1,
	  2,
	  3,
	  4,
	  5,
	  6);
	TESTING(jarr_popback);
	T(jarr_popback(&j),
	  1,
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_popfront);
	T(jarr_popfront(&j),
	  2,
	  3,
	  4,
	  5);
	TESTING(jarr_pushfront);
	T(jarr_pushfront(&j, 1),
	  1,
	  2,
	  3,
	  4,
	  5);
	jarr_free(&j);
	SUCCESS();
	return 0;
}
