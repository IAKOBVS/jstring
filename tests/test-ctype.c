/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2023-2026 James Tirta Halim <tirtajames45 at gmail dot com> */

#include "test.h"
#include "../include/ctype.h"
#include <stdlib.h>

struct ctype_case {
	const char *name;
	int (*jstr_fn)(int);
	int (*ref_fn)(int);
};

static const struct ctype_case ctype_cases[] = {
	{ "isalpha",  jstr_isalpha,  isalpha  },
	{ "islower",  jstr_islower,  islower  },
	{ "isupper",  jstr_isupper,  isupper  },
	{ "isspace",  jstr_isspace,  isspace  },
	{ "isblank",  jstr_isblank,  isblank  },
	{ "isdigit",  jstr_isdigit,  isdigit  },
	{ "isxdigit", jstr_isxdigit, isxdigit },
	{ "ispunct",  jstr_ispunct,  ispunct  },
	{ "isgraph",  jstr_isgraph,  isgraph  },
	{ "isprint",  jstr_isprint,  isprint  },
	{ "iscntrl",  jstr_iscntrl,  iscntrl  },
	{ "tolower",  jstr_tolower,  tolower  },
	{ "toupper",  jstr_toupper,  toupper  },
};

static char *
simple_tolowerstr_p(char *s);
static char *
simple_toupperstr_p(char *s);

struct ctype_str_case {
	const char *name;
	char *(*jstr_fn)(char *);
	char *(*ref_fn)(char *);
};

static const struct ctype_str_case ctype_str_cases[] = {
	{ "tolowerstr_p", jstr_tolowerstr_p, simple_tolowerstr_p },
	{ "toupperstr_p", jstr_toupperstr_p, simple_toupperstr_p },
};

static char buffer_result[256];
static char buffer_expected[256];

static char *
simple_toupperstr_p(char *s)
{
	for (; *s; ++s)
		*s = toupper(*s);
	return s;
}

static char *
simple_tolowerstr_p(char *s)
{
	for (; *s; ++s)
		*s = tolower(*s);
	return s;
}

static void
check_str(const struct ctype_str_case *c, const char *str)
{
	size_t expected_len, result_len;
	memcpy(buffer_expected, str, strlen(str));
	buffer_expected[strlen(str)] = '\0';
	memcpy(buffer_result, str, strlen(str));
	buffer_result[strlen(str)] = '\0';
	(void)c->jstr_fn(buffer_result);
	(void)c->ref_fn(buffer_expected);
	expected_len = strlen(buffer_expected);
	result_len = strlen(buffer_result);
	test_expect_true(c->name, "ctype string transform", !strcmp(buffer_result, buffer_expected));
	test_expect_true(c->name, "ctype string length", result_len == expected_len);
	test_expect_true(c->name, "ctype string bytes", !memcmp(buffer_result, buffer_expected, result_len));
}

int
main(void)
{
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(ctype_cases); ++i) {
		const struct ctype_case *c = &ctype_cases[i];
		for (int ch = 0; ch < 128; ++ch)
			test_expect_true(c->name, "ctype predicate", !c->jstr_fn(ch) == !c->ref_fn(ch));
	}

	const char *inputs[] = { "", ";", "a", "aA", "A" };
	for (size_t i = 0; i < JSTR_ARRAY_COUNT(inputs); ++i) {
		check_str(&ctype_str_cases[0], inputs[i]);
		check_str(&ctype_str_cases[1], inputs[i]);
	}
	return EXIT_SUCCESS;
}
