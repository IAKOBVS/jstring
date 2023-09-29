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
jstr_parser_func_comp(regex_t *R preg)
{
	return jstr_reg_comp(preg, "\\([_A-Za-z][_A-Za-z0-9]*\\)[ \t]*\\((\\).*\\()\\).*{.*}", 0);
}

JSTR_FUNC
static char *
jstr_parser_tok(const char **const save_ptr,
		const char *const end)
{
	return jstr_strtok_ne_len(save_ptr, end, "\n\n", 2);
}

typedef struct jstr_parser_func_ty {
	const char *fn;
	const char *fn_e;
	const char *brk;
	const char *brk_e;
	const char **args;
	const char **args_e;
	size_t args_num;
	size_t _args_cap;
} jstr_parser_func_ty;

JSTR_FUNC
static int
jstr_parser_func_match(const regex_t *R preg,
		       const char *R s,
		       const size_t sz,
		       jstr_parser_func_ty *R p)
{
	regmatch_t pm[4];
	if (jstr_reg_exec_len(preg, s, sz, 2, pm, 0) == JSTR_REG_RET_NOERROR) {
		p->fn = s + pm[1].rm_so;
		p->fn_e = s + pm[1].rm_eo;
		p->brk = s + pm[2].rm_so;
		p->brk_e = s + pm[3].rm_so;
		return 1;
	}
	return 0;
}

JSTR_FUNC_VOID
JSTR_INLINE
static void
jstr_parser_func_free(regex_t *R preg)
{
	jstr_reg_free(preg);
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_PARSER_H */
