#ifndef JSTR_PARSER_H
#define JSTR_PARSER_H 1

#include "jstr-builder.h"
#include "jstr-ctype.h"
#include "jstr-macros.h"
#include "jstr-regex.h"

P_JSTR_BEGIN_DECLS
#include <stdlib.h>
#include <string.h>
P_JSTR_END_DECLS

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_parser_fn_decl_comp(regex_t *R preg)
{
	return jstr_reg_comp(preg, "\\([^(){}]*[_0-9A-Za-z]*[ \n\t]*[_A-Za-z][_0-9A-Za-z]*[ \n\t]*([^(){}}]*)\\)[^(){}]*{", 0);
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_parser_fn_free(regex_t *R preg)
{
	jstr_reg_free(preg);
}

JSTR_FUNC_VOID
static void
jstr_parser_fn_decl_gen(const regex_t *R preg,
			char *R s,
			const char *sep,
			const size_t sz,
			const size_t seplen)
{
	char *tok;
	char *savep = s;
	char *savepp;
	const char *const end = s + sz;
	const char *tmp;
	int c;
	regmatch_t pm[2];
	while ((tok = jstr_strtok_ne_len((const char **)&savep, end, sep, seplen))) {
		if (savep == end) {
			savepp = savep;
			c = *savepp;
			*savepp = '\0';
		} else {
			savepp = savep - seplen;
			c = *savepp;
			*savepp = '\0';
		}
		if (jstr_reg_exec_len(preg, tok, savepp - tok, 2, pm, 0) == JSTR_REG_RET_NOERROR) {
			/* Check if tok is a macro. */
			tmp = jstr_skip_blanks(tok);
			if (*tmp == '#') {
				tmp = jstr_skip_blanks(tmp + 1);
				if (jstr_starts_len(tmp, savepp - tmp, "define", strlen("define")))
					/* tok is a macro. */
					goto not_fn;
			}
			fwrite(tok + pm[1].rm_so,
			       1,
			       pm[1].rm_eo - pm[1].rm_so,
			       stdout);
			fwrite(";\n", 1, 2, stdout);
		}
not_fn:
		*savepp = c;
	}
}

#if 0 /* unfinished */

JSTR_FUNC
JSTR_INLINE
static jstr_reg_errcode_ty
jstr_parser_fn_comp(regex_t *R preg)
{
	return jstr_reg_comp(preg, "\\([_A-Za-z][_A-Za-z0-9]*\\)[ \n\t]*\\((\\)[^)]*\\()\\)[^{)]*{[^}]*}", 0);
}

JSTR_FUNC
static char *
jstr_parser_tok(const char **const save_ptr,
		const char *const end)
{
	return jstr_strtok_ne_len(save_ptr, end, "\n\n", 2);
}

typedef struct jstr_parser_fn_ty {
	const char *fn;
	const char *fn_e;
	const char *brc;
	const char *brc_e;
	const char **args;
	const char **args_e;
	size_t args_num;
	size_t _args_cap;
} jstr_parser_fn_ty;

JSTR_FUNC
static int
jstr_parser_fn_match(const regex_t *R preg,
		       const char *R const s,
		       const size_t sz,
		       jstr_parser_fn_ty *R p)
{
	regmatch_t pm[4];
	if (jstr_reg_exec_len(preg, s, sz, 2, pm, 0) == JSTR_REG_RET_NOERROR) {
		p->fn = s + pm[1].rm_so;
		p->fn_e = s + pm[1].rm_eo;
		p->brc = s + pm[2].rm_so;
		p->brc_e = s + pm[3].rm_so;
		return 1;
	}
	return 0;
}

JSTR_FUNC_VOID
static void
jstr_parser_fn_fill_args(jstr_parser_fn_ty *R p)
{
	const char *s = p->brc;
	const char *const e = p->brc_e;
	p->args_num = 0;
	for (; s >= e; ++s) {
		if (jstr_isspace(*s))
			continue;
		if (jstr_isctype(*s, JSTR_ISWORD))
			p->args[p->args_num] = s;
		else
			break;
		while (s >= e
		       && jstr_isctype(*s, JSTR_ISWORD))
			++s;
		p->args_e[p->args_num++] = s;
		if (*s == ',')
			continue;
		if (*s == ')')
			break;
	}
}

#endif

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_PARSER_H */
