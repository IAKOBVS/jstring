#ifndef JSTR_PARSER_H
#define JSTR_PARSER_H 1

#include "jstr-builder.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

JSTR_FUNC_VOID
static int
jstr_match_func_name(const char *R s,
		     const char **fn,
		     const char **fn_e)
{
	int flag = 0;
	enum { IN_ALPHA = (0 << 1) };
	for (;; ++s) {
		switch (*s) {
			JSTR_CASE_ALNUM
		case '_':
			if (flag == 0) {
				*fn = s;
				flag &= IN_ALPHA;
			}
			break;
		case '\t':
		case ' ':
			if (flag & IN_ALPHA)
				return 0;
			break;
		case ')':
		case ',':
			goto end;
		default:
		case '\0':
			return 0;
		}
	}
end:
	*fn_e = s;
	return 1;
}

JSTR_FUNC_PURE
static int
jstr_match_func_len_maybe(const char *R s,
			  const size_t sz)
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const char *const end = s + sz;
	s = (char *)memchr(s, '(', sz);
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(++s != end))
		return 0;
	s = (char *)memchr(s, ')', end - s);
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(++s != end))
		return 0;
	s = (char *)memchr(s, '{', end - s);
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(++s != end))
		return 0;
	s = (char *)memchr(s, '}', end - s);
	if (jstr_unlikely(s == NULL))
		return 0;
	return 1;
}

JSTR_FUNC_PURE
static int
jstr_match_func_maybe(const char *R s)
{
	if (jstr_unlikely(*s == '\0'))
		return 0;
	s = strchr(s, '(');
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(*++s == '\0'))
		return 0;
	s = strchr(s, ')');
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(*++s == '\0'))
		return 0;
	s = strchr(s, '{');
	if (jstr_unlikely(s == NULL))
		return 0;
	if (jstr_unlikely(*++s == '\0'))
		return 0;
	s = strchr(s, '}');
	if (jstr_unlikely(s == NULL))
		return 0;
	return 1;
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_PARSER_H */
