#define __STRICT_ANSI__

#include "test.h"

#define TOLOWER(c) (unsigned char)(((c) >= 'A' && (c) <= 'Z') ? ((c) - 'A' + 'a') : (c))
#define TOUPPER(c) (unsigned char)(((c) >= 'a' && (c) <= 'z') ? ((c) - 'a' + 'A') : (c))

char *
simple_strrstr_len(const char *h,
                   const size_t hl,
                   const char *n,
                   const size_t nl)
{
	if (hl < nl)
		return NULL;
	const char *p = h + hl - nl;
	for (; p >= h; --p)
		if (!memcmp(p, n, nl))
			return (char *)p;
	return NULL;
}

char *
simple_strrstr(const char *h,
               const char *n)
{
	return simple_strrstr_len(h, strlen(h), n, strlen(n));
}

char *
simple_memmem(const char *h,
              const size_t hl,
              const char *n,
              const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const char *const end = h + hl - nl;
	for (; h <= end; ++h)
		if (!memcmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strstr(const char *h,
              const char *n)
{
	return simple_memmem(h, strlen(h), n, strlen(n));
}

int
simple_strncasecmp(const char *s1,
                   const char *s2,
                   size_t n)
{
	if (n == 0)
		return 0;
	int ret;
	for (; (ret = (TOLOWER(*s1) - TOLOWER(*s2))) == 0 && *s1 && --n; ++s1, ++s2)
		;
	return ret;
}

char *
simple_strcasestr_len(const char *h,
                      const size_t hl,
                      const char *n,
                      const size_t nl)
{
	if (nl == 0)
		return (char *)h;
	if (hl == 0)
		return NULL;
	if (hl < nl)
		return NULL;
	const char *end = h + hl - nl;
	for (; h <= end; ++h)
		if (!simple_strncasecmp(h, n, nl))
			return (char *)h;
	return NULL;
}

char *
simple_strcasestr(const char *h,
                  const char *n)
{
	return simple_strcasestr_len(h, strlen(h), n, strlen(n));
}

char *
jstr_strrstr(const char *h, const char *n)
{
	return jstr_strrstr_len(h, strlen(h), n, strlen(n));
}

#define T_STRSTR(fn, simple_fn)                                                           \
	do {                                                                              \
		for (size_t i = 0; i < JSTR_ARRAY_SIZE(hs_ne); ++i) {                     \
			const char *const result = fn(hs_ne[i].hs, hs_ne[i].ne);          \
			const char *const expected = simple_fn(hs_ne[i].hs, hs_ne[i].ne); \
			ASSERT(result == expected, result, expected);                     \
		}                                                                         \
	} while (0)

#define T_STRSTR_LEN(fn, simple_fn)                                                                                                 \
	do {                                                                                                                        \
		for (size_t i = 0; i < JSTR_ARRAY_SIZE(hs_ne); ++i) {                                                               \
			const char *const result = fn(hs_ne[i].hs, strlen(hs_ne[i].hs), hs_ne[i].ne, strlen(hs_ne[i].ne));          \
			const char *const expected = simple_fn(hs_ne[i].hs, strlen(hs_ne[i].hs), hs_ne[i].ne, strlen(hs_ne[i].ne)); \
			ASSERT(result == expected, result, expected);                                                               \
		}                                                                                                                   \
	} while (0)

int
main(int argc, char **argv)
{
	struct hs_ne_ty {
		const char *hs;
		const char *ne;
	} hs_ne[] = {
		{"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "xxyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyxx",
                 "yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy"},
		{ "yxxyyyyxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yyyyyxxxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yxyyyxxxyxxxxyyyy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yxxyyyyxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yyyyyxxxyxxxxy",
                 "yyyyy"		                                                                                                                                                                                                                                                    },
		{ "yxxxyxxxxy",
                 "xxxx"		                                                                                                                                                                                                                                                     },
		{ "yxxxyxxxxy",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "yxxxyxxy",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 "x"		                                                                                                                                                                                                                                                        },
		{ "xxx",
                 "yyy"		                                                                                                                                                                                                                                                      },
		{ "x",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "xxx",
                 ""		                                                                                                                                                                                                                                                         },
		{ "",
                 "xxx"		                                                                                                                                                                                                                                                      },
		{ "",
                 ""		                                                                                                                                                                                                                                                         }
	};
	T_STRSTR(jstr_strcasestr, simple_strcasestr);
	T_STRSTR_LEN(jstr_strcasestr_len, simple_strcasestr_len);
	T_STRSTR_LEN(jstr_memmem, simple_memmem);
	T_STRSTR_LEN(jstr_strrstr_len, simple_strrstr_len);
	SUCCESS();
	return EXIT_SUCCESS;
}
