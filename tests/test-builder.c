#define JSTR_DEBUG 1

#include "../src/jstr-regex.h"
#include "test.h"

#define T_APPEND(ret, fn, ...)                                                                         \
	do {                                                                                           \
		TESTING(fn);                                                                           \
		if (ret != fn(__VA_ARGS__)) {                                                          \
			fprintf(stderr, "%s\n", JSTR_STRING(fn) "()");                                 \
			jstr_debug(&j);                                                                \
			jstr_err_exit("");                                                             \
		}                                                                                      \
		ASSERT(strcmp(j.data, expected) == 0, j.data, expected);                               \
		ASSERT(j.size == strlen(expected), j.data, expected);                                  \
		ASSERT(memcmp(j.data, expected, j.size) == 0, j.data, expected);                       \
		ASSERT(strlen(expected) != 0 && j.size != 0 && j.capacity > j.size, j.data, expected); \
		jstr_free_j(&j);                                                                       \
	} while (0)

jstr_ty j = { 0 };

char buf[4096] = { 0 };

int
main(int argc, char **argv)
{
	const char *expected;
	const char *find;
	const char *rplc;
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_cat, JSTR_STRUCT(&j), "hello", " ", "world", NULL);
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_append_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_prepend_len, JSTR_STRUCT(&j), expected, strlen(expected));
	expected = "hello world";
	T_APPEND(JSTR_RET_SUCC, jstr_assign_len, JSTR_STRUCT(&j), expected, strlen(expected));

	const char *hello4 = "hello hello hello hello";
	assert(JSTR_RET_SUCC == jstr_assign_len(JSTR_STRUCT(&j), hello4, strlen(hello4)));
	find = "hello";
	rplc = "world";
	expected = "world hello hello hello";
	T_APPEND(JSTR_RET_SUCC, jstr_rplc_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));

	assert(JSTR_RET_SUCC == jstr_assign_len(JSTR_STRUCT(&j), hello4, strlen(hello4)));
	find = "hello";
	rplc = "world";
	expected = "world world world world";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));

	find = "world";
	rplc = "";
	expected = "   ";
	T_APPEND(JSTR_RET_SUCC, jstr_rplcall_len, JSTR_STRUCT(&j), find, strlen(find), rplc, strlen(rplc));

	SUCCESS();
	return 0;
}
