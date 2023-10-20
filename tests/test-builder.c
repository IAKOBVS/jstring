#define JSTR_DEBUG 1

#include "test.h"

#define T_APPEND(fn, ...)                                                            \
	do {                                                                         \
		assert(expected);                                                    \
		PRINT("Testing %s.\n", JSTR_STRINGIFY(fn));                          \
		jstr_free_j(&j);                                                     \
		ret = fn(__VA_ARGS__);                                               \
		if (ret == 0) {                                                      \
			jstr_err("");                                                \
			assert(fn(__VA_ARGS__) != 0);                                \
		}                                                                    \
		jstr_debug(&j);\
		assert(strcmp(j.data, expected) == 0);                               \
		assert(j.size == strlen(expected));                                  \
		assert(memcmp(j.data, expected, j.size) == 0);                       \
		assert(strlen(expected) != 0 && j.size != 0 && j.capacity > j.size); \
	} while (0)

jstr_ty j = { 0 };

char buf[4096] = {0};

int
main(int argc, char **argv)
{
	int ret;
	char *expected;
	expected = "hello world";
	T_APPEND(jstr_cat, &j.data, &j.size, &j.capacity, "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(jstr_append_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));
	expected = "hello world";
	T_APPEND(jstr_prepend_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));
	expected = "hello world";
	T_APPEND(jstr_assign_len, &j.data, &j.size, &j.capacity, expected, strlen(expected));
	SUCCESS();
	return 0;
}
