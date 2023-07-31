#include "../c/jstr_builder.h"
#include "../c/jstr_ctype.h"
#include "../c/jstr_macros.h"
#include "../c/jstr_replace.h"
#include <assert.h>

jstr_t j1 = { 0 };
jstr_t j2 = { 0 };
const char *g_searc = 0;
char g_s[4096];

#define xstr(s) str(s)
#define str(s)	#s

#define assert_(x)                                                                                        \
	do {                                                                                              \
		if (!(x)) {                                                                               \
			fprintf(stderr, "\n%s:%d:%s:ASSERT: %s\n", __FILE__, __LINE__, __func__, str(x)); \
			fprintf(stderr, "hs:%s\n", g_s);                                             \
			if (g_searc) {                                                                    \
				fprintf(stderr, "searc:%s\n", g_searc);                                   \
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
			      const char *JSTR_RST const searc,
			      const size_t searclen) JSTR_NOEXCEPT
{
	assert(j);
	assert(j->data);
	assert(j->size);
	assert(searc);
	assert(searclen == strlen(searc));
	g_searc = searc;
	char *dst = j->data;
	char *s = j->data;
	const char *end = s + j->size - searclen;
	while (s <= end)
		if (*s && !memcmp(s, searc, searclen))
			s += searclen;
		else
			*dst++ = *s++;
	while ((*dst++ = *s++))
		;
	return dst - 1;
}

void test_jstr_rmall_mem_p(char *s, const char *lit_str, char *searc)
{
	assert(s);
	assert(lit_str);
	assert(searc);
	strcpy(s, lit_str);
	j1.data = strdup_(s);
	j1.size = strlen(s);
	size_t ret1 = simple_jstr_rmall_mem_p(&j1, searc, strlen(searc)) - j1.data;
	j1.size = ret1;
	j2.data = strdup_(s);
	j2.size = strlen(s);
	size_t ret2 = jstr_rmall_mem_p_j(&j2, searc, strlen(searc)) - j2.data;
	j2.size = ret2;
	assert_(ret1 == ret2);
	assert_(strlen(j1.data) == strlen(j2.data));
	assert_(!memcmp(j1.data, j2.data, ret1));
	assert_(!strcmp(j1.data, j2.data));
	free(j1.data);
	free(j2.data);
	j1.data = 0;
	j1.size = 0;
	j1.cap = 0;
	j2.data = 0;
	j2.size = 0;
	j2.cap = 0;
	g_searc = 0;
	memset(s, 0, 4096);
}

int main()
{

	const char *hs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	test_jstr_rmall_mem_p(g_s, hs, "A");
	test_jstr_rmall_mem_p(g_s, hs, "a");
	test_jstr_rmall_mem_p(g_s, hs, "AB");
	test_jstr_rmall_mem_p(g_s, hs, "ab");
	test_jstr_rmall_mem_p(g_s, hs, "ABC");
	test_jstr_rmall_mem_p(g_s, hs, "abc");
	test_jstr_rmall_mem_p(g_s, hs, "ABCD");
	test_jstr_rmall_mem_p(g_s, hs, "abcd");
	test_jstr_rmall_mem_p(g_s, hs, "ABCDE");
	test_jstr_rmall_mem_p(g_s, hs, "abcde");

	test_jstr_rmall_mem_p(g_s, hs, "z");
	test_jstr_rmall_mem_p(g_s, hs, "yz");
	test_jstr_rmall_mem_p(g_s, hs, "xyz");
	test_jstr_rmall_mem_p(g_s, hs, "wxyz");
	test_jstr_rmall_mem_p(g_s, hs, "vwxyz");

	return 0;
}
