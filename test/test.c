#include "../c/jstr_builder.h"
#include "../c/jstr_ctype.h"
#include "../c/jstr_macros.h"
#include "../c/jstr_replace.h"
#include <assert.h>

#define G_SSZ 4096

jstr_t j1 = { 0 };
jstr_t j2 = { 0 };
const char *g_ne = 0;
char g_s[G_SSZ];

#define xstr(s) str(s)
#define str(s)	#s

#define assert_(x)                                                                                        \
	do {                                                                                              \
		if (!(x)) {                                                                               \
			fprintf(stderr, "\n%s:%d:%s:ASSERT: %s\n", __FILE__, __LINE__, __func__, str(x)); \
			fprintf(stderr, "hs:%s\n", g_s);                                                  \
			if (g_ne) {                                                                       \
				fprintf(stderr, "ne:%s\n", g_ne);                                         \
			}                                                                                 \
			fprintf(stderr, "\nsimple:\n");                                                   \
			if (j1.data) {                                                                    \
				fprintf(stderr, "data:%s\n", j1.data);                                    \
				fprintf(stderr, "fwrite data:");                                          \
				fwrite(j1.data, 1, j1.size, stderr);                                      \
				fputc('\n', stderr);                                                      \
				fprintf(stderr, "strlen:%zu\n", strlen(j1.data));                         \
			}                                                                                 \
			if (j1.size) {                                                                    \
				fprintf(stderr, "size:%zu\n", j1.size);                                   \
			}                                                                                 \
			if (j1.cap) {                                                                     \
				fprintf(stderr, "cap:%zu\n", j1.cap);                                     \
			}                                                                                 \
			fprintf(stderr, "\nfunc:\n");                                                     \
			if (j2.data) {                                                                    \
				fprintf(stderr, "data:%s\n", j2.data);                                    \
				fprintf(stderr, "fwrite data:");                                          \
				fwrite(j2.data, 1, j2.size, stderr);                                      \
				fputc('\n', stderr);                                                      \
				fprintf(stderr, "strlen:%zu\n", strlen(j2.data));                         \
			}                                                                                 \
			if (j2.size) {                                                                    \
				fprintf(stderr, "size:%zu\n", j2.size);                                   \
			}                                                                                 \
			if (j2.cap) {                                                                     \
				fprintf(stderr, "cap:%zu\n", j2.cap);                                     \
			}                                                                                 \
			assert(x);                                                                        \
		}                                                                                         \
	} while (0)

char *strdup_(char *s)
{
	assert(s);
	char *p = malloc(strlen(s) + 1);
	assert(p);
	strcpy(p, s);
	return p;
}

char *simple_jstr_rmall_mem_p(jstr_t *j,
			      const char *JSTR_RST const ne,
			      const size_t nelen) JSTR_NOEXCEPT
{
	assert(j);
	assert(j->data);
	assert(j->size);
	assert(ne);
	assert(nelen == strlen(ne));
	g_ne = ne;
	char *dst = j->data;
	char *s = j->data;
	const char *end = s + j->size - nelen;
	while (s <= end)
		if (*s && !memcmp(s, ne, nelen))
			s += nelen;
		else
			*dst++ = *s++;
	while ((*dst++ = *s++))
		;
	return dst - 1;
}

void test_rmall_mem_p(char *s, const char *hs, char *ne)
{

#define TEST_START                    \
	do {                          \
		assert(s);            \
		assert(hs);           \
		assert(ne);           \
		strcpy(s, hs);        \
		j1.data = strdup_(s); \
		j1.size = strlen(s);  \
		j2.data = strdup_(s); \
		j2.size = strlen(s);  \
	} while (0)

#define TEST_CHECK(F1, F2)                                   \
	do {                                                 \
		size_t ret1 = F1 - j1.data;                  \
		size_t ret2 = F2 - j1.data;                  \
		j1.size = ret1;                              \
		j2.size = ret2;                              \
		assert_(ret1 == ret2);                       \
		assert_(strlen(j1.data) == strlen(j2.data)); \
		assert_(!memcmp(j1.data, j2.data, ret1));    \
		assert_(!strcmp(j1.data, j2.data));          \
	} while (0)

#define TEST_CLEANUP                 \
	do {                         \
		free(j1.data);       \
		free(j2.data);       \
		j1.data = 0;         \
		j1.size = 0;         \
		j1.cap = 0;          \
		j2.data = 0;         \
		j2.size = 0;         \
		j2.cap = 0;          \
		g_ne = 0;            \
		memset(s, 0, G_SSZ); \
	} while (0)

#define TEST(F1, F2)                \
	do {                        \
		TEST_START;         \
		TEST_CHECK(F1, F2); \
		TEST_CLEANUP;       \
	} while (0)

	TEST(simple_jstr_rmall_mem_p(&j1, ne, strlen(ne)), jstr_rmall_mem_p_j(&j2, ne, strlen(ne)));
}

char *simple_jstr_replaceall_mem(jstr_t *j, const char *searc, const char *rplc)
{
	const size_t searclen = strlen(searc);
	const size_t rplclen = strlen(rplc);
	char *tmp;
	char **s = &j->data;
	char *dst = j->data;
	size_t *cap = &j->cap;
	size_t *sz = &j->size;
	while ((dst = (char *)jstr_memmem(dst, (*s + *sz) - dst, searc, searclen))) {
		if (*cap > *sz + rplclen - searclen + 1) {
			memmove(dst + rplclen,
				dst + searclen,
				(*s + *sz + 1) - dst + searclen);
			memcpy(dst, rplc, rplclen);
			dst += rplclen;
		} else {
			JSTR_GROW(*cap, *sz + rplclen + 1);
			tmp = (char *)malloc(*cap);
			JSTR_MALLOC_ERR(tmp, return NULL);
			memcpy(tmp, *s, dst - *s);
			memcpy(tmp + (dst - *s), rplc, rplclen);
			memcpy(tmp + (dst - *s) + rplclen,
			       dst + rplclen,
			       (*s + *sz + 1) - (dst + rplclen));
			dst = tmp + (dst - *s) + rplclen;
			free(*s);
			*s = tmp;
		}
		*sz += rplclen - searclen;
	}
	return j->data + j->size;
}

void test_replaceall_mem_p(char *s, const char *hs, const char *ne, const char *rplc)
{
	TEST_START;
	TEST(simple_jstr_replaceall_mem(&j1, ne, rplc), (jstr_replaceall_j(&j2, ne, rplc), j2.data + j2.size));
}

int main()
{

	const char *hs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	test_rmall_mem_p(g_s, hs, "A");
	test_rmall_mem_p(g_s, hs, "a");
	test_rmall_mem_p(g_s, hs, "AB");
	test_rmall_mem_p(g_s, hs, "ab");
	test_rmall_mem_p(g_s, hs, "ABC");
	test_rmall_mem_p(g_s, hs, "abc");
	test_rmall_mem_p(g_s, hs, "ABCD");
	test_rmall_mem_p(g_s, hs, "abcd");
	test_rmall_mem_p(g_s, hs, "ABCDE");
	test_rmall_mem_p(g_s, hs, "abcde");
	test_rmall_mem_p(g_s, hs, "z");
	test_rmall_mem_p(g_s, hs, "yz");
	test_rmall_mem_p(g_s, hs, "xyz");
	test_rmall_mem_p(g_s, hs, "wxyz");
	test_rmall_mem_p(g_s, hs, "vwxyz");


	test_replaceall_mem_p(g_s, hs, "A", "____");
	test_replaceall_mem_p(g_s, hs, "a", "____");
	test_replaceall_mem_p(g_s, hs, "AB", "____");
	test_replaceall_mem_p(g_s, hs, "ab", "____");
	test_replaceall_mem_p(g_s, hs, "ABC", "____");
	test_replaceall_mem_p(g_s, hs, "abc", "____");
	test_replaceall_mem_p(g_s, hs, "ABCD", "____");
	test_replaceall_mem_p(g_s, hs, "abcd", "____");
	test_replaceall_mem_p(g_s, hs, "ABCDE", "____");
	test_replaceall_mem_p(g_s, hs, "abcde", "____");
	test_replaceall_mem_p(g_s, hs, "z", "____");
	test_replaceall_mem_p(g_s, hs, "yz", "____");
	test_replaceall_mem_p(g_s, hs, "xyz", "____");
	test_replaceall_mem_p(g_s, hs, "wxyz", "____");
	test_replaceall_mem_p(g_s, hs, "vwxyz", "____");

	return 0;
}
