#include "jstrgen.h"

#include <stdlib.h>
#include <string.h>

/* All strings in this file carry an explicit length.  The generator never
 * calls strlen: literals use sizeof(s) - 1 and dynamic strings use jp_str_ty. */

#define JP_SLIT(s) ((jp_str_ty)JP_STR_LIT(s))

/* ── small helpers ────────────────────────────────────────────────────── */

static int
jp_isspace(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static int
jp_mem_eq(const char *a, size_t an, const char *b, size_t bn)
{
	return an == bn && (an == 0 || memcmp(a, b, an) == 0);
}

static int
jp_str_eq(jp_str_ty a, jp_str_ty b)
{
	return jp_mem_eq(a.data, a.len, b.data, b.len);
}

static int
jp_str_eq_lit(jp_str_ty a, const char *b, size_t bn)
{
	return jp_mem_eq(a.data, a.len, b, bn);
}

#define JP_EQ(a, lit) jp_str_eq_lit((a), JP_LIT(lit))

static int
jp_ends_with(jp_str_ty s, const char *suffix, size_t suffix_len)
{
	return s.len >= suffix_len &&
	       (suffix_len == 0 || memcmp(s.data + s.len - suffix_len, suffix, suffix_len) == 0);
}

#define JP_ENDS(s, lit) jp_ends_with((s), JP_LIT(lit))

static int
jp_contains(jp_str_ty hay, const char *needle, size_t needle_len)
{
	size_t i;

	if (needle_len == 0)
		return 1;
	if (needle_len > hay.len)
		return 0;
	for (i = 0; i + needle_len <= hay.len; i++) {
		if (memcmp(hay.data + i, needle, needle_len) == 0)
			return 1;
	}
	return 0;
}

#define JP_CONTAINS(s, lit) jp_contains((s), JP_LIT(lit))

static int
jp_starts_with(jp_str_ty s, const char *prefix, size_t prefix_len)
{
	return s.len >= prefix_len && (prefix_len == 0 || memcmp(s.data, prefix, prefix_len) == 0);
}

#define JP_STARTS_STR(s, lit) jp_starts_with((s), JP_LIT(lit))

static jp_str_ty
jp_trimmed_span(const char *src, jp_span_ty sp, jp_arena_ty *arena)
{
	jp_str_ty r;
	size_t a = sp.start;
	size_t b = sp.end;

	while (a < b && jp_isspace(src[a]))
		a++;
	while (b > a && jp_isspace(src[b - 1]))
		b--;
	r.data = jp_arena_strndup(arena, src + a, b - a);
	r.len = b - a;
	return r;
}

static jp_str_ty
jp_buf_commit(jp_buf_ty *b, jp_arena_ty *arena)
{
	jp_str_ty s;

	s.len = b->len;
	s.data = jp_arena_strndup(arena, b->data ? b->data : "", b->len);
	jp_buf_free(b);
	return s;
}

static int
jp_buf_contains(const jp_buf_ty *b, const char *needle, size_t needle_len)
{
	jp_str_ty s;

	s.data = b->data ? b->data : "";
	s.len = b->len;
	return jp_contains(s, needle, needle_len);
}

static int
jp_str_replace_all(jp_buf_ty *b, const char *from, size_t from_len, const char *to, size_t to_len)
{
	size_t i = 0;
	size_t last = 0;
	jp_buf_ty out;

	jp_buf_init(&out);
	while (from_len != 0 && i + from_len <= b->len) {
		if (memcmp(b->data + i, from, from_len) == 0) {
			if (jp_buf_append(&out, b->data + last, i - last) != 0)
				goto fail;
			if (jp_buf_append(&out, to, to_len) != 0)
				goto fail;
			i += from_len;
			last = i;
		} else {
			i++;
		}
	}
	if (jp_buf_append(&out, b->data + last, b->len - last) != 0)
		goto fail;
	jp_buf_free(b);
	*b = out;
	return 0;
fail:
	jp_buf_free(&out);
	return -1;
}

/* Replace bare `JSTR_FUNC` (not followed by an identifier character). */
static jp_str_ty
jp_replace_bare_func(jp_str_ty s, jp_arena_ty *arena)
{
	size_t fl = sizeof("JSTR_FUNC") - 1;
	size_t tl = sizeof("JSTR_FUNC_VOID") - 1;
	size_t i = 0;
	jp_buf_ty out;

	jp_buf_init(&out);
	while (i < s.len) {
		if (i + fl <= s.len && memcmp(s.data + i, "JSTR_FUNC", fl) == 0) {
			char n = (i + fl < s.len) ? s.data[i + fl] : '\0';
			int word = (n >= 'A' && n <= 'Z') || (n >= 'a' && n <= 'z') ||
			           (n >= '0' && n <= '9') || n == '_';
			if (!word) {
				jp_buf_append(&out, "JSTR_FUNC_VOID", tl);
				i += fl;
				continue;
			}
		}
		jp_buf_putc(&out, s.data[i]);
		i++;
	}
	return jp_buf_commit(&out, arena);
}

/* ── name set ─────────────────────────────────────────────────────────── */

typedef struct jp_names {
	jp_str_ty *data;
	size_t len;
	size_t cap;
} jp_names_ty;

static void
jp_names_init(jp_names_ty *s)
{
	s->data = NULL;
	s->len = 0;
	s->cap = 0;
}

static void
jp_names_free(jp_names_ty *s)
{
	free(s->data);
	s->data = NULL;
	s->len = 0;
	s->cap = 0;
}

static int
jp_names_has(const jp_names_ty *s, jp_str_ty name)
{
	size_t i;

	for (i = 0; i < s->len; i++) {
		if (jp_str_eq(s->data[i], name))
			return 1;
	}
	return 0;
}

static int
jp_names_add(jp_names_ty *s, jp_arena_ty *arena, jp_str_ty name)
{
	if (jp_names_has(s, name))
		return 0;
	if (s->len == s->cap) {
		size_t cap = s->cap ? s->cap * 2 : 32;
		jp_str_ty *p = realloc(s->data, cap * sizeof(*p));
		if (!p)
			return -1;
		s->data = p;
		s->cap = cap;
	}
	s->data[s->len].data = jp_arena_strndup(arena, name.data, name.len);
	s->data[s->len].len = name.len;
	if (!s->data[s->len].data)
		return -1;
	s->len++;
	return 0;
}

/* ── signatures ───────────────────────────────────────────────────────── */

typedef struct jp_sigparam {
	jp_str_ty text;
	jp_str_ty var;
	int is_ptr;
	int is_ptr_ptr;
	int is_const;
	int has_size_t;
} jp_sigparam_ty;

typedef struct jp_sig {
	jp_str_ty name;
	jp_str_ty raw_attrs;
	jp_str_ty rettype;
	jp_sigparam_ty *params;
	size_t nparams;
	int variadic;
} jp_sig_ty;

static int
jp_is_jstr_name(jp_str_ty name)
{
	size_t i;

	if (name.len < 4 || memcmp(name.data, "jstr", 4) != 0)
		return 0;
	if (name.len == 4)
		return 0;
	if (name.data[4] == '_')
		return 1;
	for (i = 4; i < name.len && name.data[i] != '_'; i++) {
		char c = name.data[i];
		if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
			return 0;
	}
	return i < name.len && name.data[i] == '_';
}

static int
jp_has_end_ptr_suffix(jp_str_ty name)
{
	return JP_ENDS(name, "_p") || JP_CONTAINS(name, "_p_");
}

static jp_str_ty
jp_remove_seg(jp_str_ty name, const char *seg, size_t seg_len, jp_arena_ty *arena)
{
	size_t i = 0;
	jp_buf_ty out;

	jp_buf_init(&out);
	while (i < name.len) {
		if (i + seg_len <= name.len && memcmp(name.data + i, seg, seg_len) == 0 &&
		    (i + seg_len == name.len || name.data[i + seg_len] == '_')) {
			i += seg_len;
			continue;
		}
		jp_buf_putc(&out, name.data[i]);
		i++;
	}
	return jp_buf_commit(&out, arena);
}

static jp_str_ty
jp_concat_lit(jp_str_ty a, const char *lit, size_t lit_len, jp_arena_ty *arena)
{
	jp_buf_ty b;

	jp_buf_init(&b);
	jp_buf_putstr(&b, a);
	jp_buf_append(&b, lit, lit_len);
	return jp_buf_commit(&b, arena);
}

static jp_str_ty
jp_join_params(const jp_sigparam_ty *p, size_t n, jp_arena_ty *arena)
{
	size_t i;
	jp_buf_ty b;

	jp_buf_init(&b);
	for (i = 0; i < n; i++) {
		if (i)
			jp_buf_append(&b, ", ", 2);
		jp_buf_putstr(&b, p[i].text);
	}
	return jp_buf_commit(&b, arena);
}

static jp_str_ty
jp_raw_attrs(const char *src, const jp_tokens_ty *toks, const jp_function_ty *fn,
             jp_arena_ty *arena)
{
	jp_buf_ty b;
	size_t i;
	unsigned int prev_line = 0;
	int first = 1;

	jp_buf_init(&b);
	for (i = fn->attrs_begin; i < fn->attrs_end; i++) {
		const jp_token_ty *t = &toks->data[i];
		size_t len = t->end - t->start;
		const char *text = src + t->start;

		if (t->kind == JP_TK_COMMENT || t->kind == JP_TK_PP)
			continue;
		if (t->kind == JP_TK_IDENT && len >= 16 &&
		    memcmp(text, "JSTR_ATTR_ACCESS", 16) == 0) {
			size_t next = i + 1;

			while (next < fn->attrs_end &&
			       (toks->data[next].kind == JP_TK_COMMENT ||
			        toks->data[next].kind == JP_TK_PP))
				next++;
			if (next < fn->attrs_end && src[toks->data[next].start] == '(') {
				int depth = 0;
				size_t j;

				for (j = next; j < fn->attrs_end; j++) {
					const jp_token_ty *tj = &toks->data[j];
					char c;

					if (tj->kind != JP_TK_PUNCT || tj->end - tj->start != 1)
						continue;
					c = src[tj->start];
					if (c == '(')
						depth++;
					else if (c == ')') {
						depth--;
						if (depth == 0) {
							i = j;
							break;
						}
					}
				}
			}
			continue;
		}
		if (!first) {
			if (t->line == prev_line)
				jp_buf_putc(&b, ' ');
			else
				jp_buf_putc(&b, '\n');
		}
		jp_buf_append(&b, text, len);
		prev_line = t->line;
		first = 0;
	}
	return jp_buf_commit(&b, arena);
}

static jp_str_ty
jp_rettype_text(const char *src, const jp_tokens_ty *toks, const jp_function_ty *fn,
                jp_arena_ty *arena)
{
	jp_buf_ty b;
	size_t i;
	int first = 1;

	jp_buf_init(&b);
	for (i = fn->ret_begin; i < fn->ret_end; i++) {
		const jp_token_ty *t = &toks->data[i];

		if (t->kind == JP_TK_COMMENT || t->kind == JP_TK_PP)
			continue;
		if (!first)
			jp_buf_putc(&b, ' ');
		jp_buf_append(&b, src + t->start, t->end - t->start);
		first = 0;
	}
	return jp_buf_commit(&b, arena);
}

static jp_str_ty
jp_clean_attr(jp_str_ty raw, int ret_nonnull_to_void, jp_arena_ty *arena)
{
	jp_buf_ty b;
	int has_inline;

	jp_buf_init(&b);
	jp_buf_append(&b, raw.data, raw.len);
	if (ret_nonnull_to_void)
		jp_str_replace_all(&b, JP_LIT("JSTR_FUNC_RET_NONNULL"), JP_LIT("JSTR_FUNC_VOID"));
	has_inline = jp_buf_contains(&b, JP_LIT("JSTR_ATTR_INLINE")) ||
	             jp_buf_contains(&b, JP_LIT("static"));
	if (!has_inline) {
		jp_str_replace_all(&b, JP_LIT("JSTR_ATTR_NOINLINE"), "", 0);
		if (b.len)
			jp_buf_putc(&b, '\n');
		jp_buf_append(&b, JP_LIT("static JSTR_ATTR_INLINE"));
	}
	return jp_buf_commit(&b, arena);
}

static void
jp_sig_from_fn(const char *src, const jp_tokens_ty *toks, const jp_function_ty *fn,
               jp_arena_ty *arena, jp_sig_ty *sig)
{
	size_t i;

	memset(sig, 0, sizeof(*sig));
	sig->name = jp_trimmed_span(src,
	                            (jp_span_ty){toks->data[fn->name_idx].start,
	                                         toks->data[fn->name_idx].end},
	                            arena);
	sig->raw_attrs = jp_raw_attrs(src, toks, fn, arena);
	sig->rettype = jp_rettype_text(src, toks, fn, arena);
	sig->variadic = fn->variadic;
	sig->params = jp_arena_alloc(arena, (fn->nparams ? fn->nparams : 1) * sizeof(*sig->params));
	if (!sig->params)
		return;
	for (i = 0; i < fn->nparams; i++) {
		jp_sigparam_ty *p = &sig->params[i];
		const jp_param_ty *q = &fn->params[i];

		p->text = jp_trimmed_span(src, q->span, arena);
		p->var = jp_trimmed_span(src, q->name, arena);
		p->is_ptr = q->is_ptr;
		p->is_ptr_ptr = q->is_ptr_ptr;
		p->is_const = q->is_const;
		p->has_size_t = q->has_size_t;
	}
	sig->nparams = fn->nparams;
}

/* ── pass 1: *_len() -> convenience wrapper ───────────────────────────── */

static void
jp_arg_append(jp_buf_ty *b, jp_str_ty s, int *first)
{
	if (!*first)
		jp_buf_append(b, ", ", 2);
	jp_buf_putstr(b, s);
	*first = 0;
}

static int
jp_pass1(const jp_sig_ty *in, jp_names_ty *declared, jp_arena_ty *arena, jp_sig_ty *out_sig,
         jp_buf_ty *text)
{
	jp_str_ty base;
	jp_sigparam_ty *remaining;
	size_t nrem = 0;
	size_t i;
	int replaced = 0;
	int first_arg = 1;
	jp_buf_ty args;
	jp_str_ty attrs;
	jp_str_ty params_text;

	if (!jp_is_jstr_name(in->name) || in->variadic || in->nparams == 0)
		return 0;
	if (!JP_CONTAINS(in->name, "_len"))
		return 0;
	if (JP_ENDS(in->name, "_j"))
		return 0;
	base = jp_remove_seg(in->name, JP_LIT("_len"), arena);
	if (jp_names_has(declared, base))
		return 0;

	remaining = jp_arena_alloc(arena, in->nparams * sizeof(*remaining));
	if (!remaining)
		return 0;
	jp_buf_init(&args);
	for (i = 0; i < in->nparams; i++) {
		const jp_sigparam_ty *p = &in->params[i];

		if (p->has_size_t && JP_ENDS(p->var, "_len")) {
			jp_str_ty base_var = jp_remove_seg(p->var, JP_LIT("_len"), arena);
			size_t j;
			size_t buf_idx = (size_t)-1;
			int found = 0;

			replaced = 1;
			for (j = 0; j < in->nparams; j++) {
				if (jp_str_eq(in->params[j].var, base_var)) {
					buf_idx = j;
					found = 1;
					break;
				}
			}
			if (!found) {
				for (j = 0; j < in->nparams; j++) {
					if (jp_ends_with(in->params[j].text, base_var.data, base_var.len)) {
						buf_idx = j;
						found = 1;
						break;
					}
				}
			}
			if (found) {
				jp_buf_ty tmp;

				jp_buf_init(&tmp);
				jp_buf_append(&tmp, JP_LIT("strlen((const char *)"));
				jp_buf_putstr(&tmp, in->params[buf_idx].var);
				jp_buf_putc(&tmp, ')');
				jp_arg_append(&args, (jp_str_ty){tmp.data, tmp.len}, &first_arg);
				jp_buf_free(&tmp);
				continue;
			}
		} else if (p->has_size_t && JP_EQ(p->var, "sz") && !p->is_ptr) {
			size_t buf_idx = 0;
			size_t j;

			/* The length comes from the data buffer, which is not
			 * necessarily the first parameter (e.g. `t, s, sz, ...`). */
			for (j = 0; j < in->nparams; j++) {
				if (JP_EQ(in->params[j].var, "s") && !in->params[j].has_size_t) {
					buf_idx = j;
					goto found_bare_buf;
				}
			}
			for (j = i; j > 0; j--) {
				if (in->params[j - 1].is_ptr &&
				    JP_CONTAINS(in->params[j - 1].text, "char")) {
					buf_idx = j - 1;
					goto found_bare_buf;
				}
			}
			for (j = i; j > 0; j--) {
				if (in->params[j - 1].is_ptr) {
					buf_idx = j - 1;
					break;
				}
			}
		found_bare_buf: {
			jp_buf_ty tmp;

			replaced = 1;
			jp_buf_init(&tmp);
			jp_buf_append(&tmp, JP_LIT("strlen((const char *)"));
			jp_buf_putstr(&tmp, in->params[buf_idx].var);
			jp_buf_putc(&tmp, ')');
			jp_arg_append(&args, (jp_str_ty){tmp.data, tmp.len}, &first_arg);
			jp_buf_free(&tmp);
			continue;
		}
		}
		remaining[nrem++] = *p;
		jp_arg_append(&args, p->var, &first_arg);
	}
	if (!replaced) {
		jp_buf_free(&args);
		return 0;
	}

	attrs = jp_clean_attr(in->raw_attrs, 0, arena);
	params_text = jp_join_params(remaining, nrem, arena);
	jp_buf_init(text);
	jp_buf_putstr(text, attrs);
	jp_buf_putc(text, '\n');
	jp_buf_putstr(text, in->rettype);
	jp_buf_putc(text, '\n');
	jp_buf_putstr(text, base);
	jp_buf_putc(text, '(');
	jp_buf_putstr(text, params_text);
	jp_buf_append(text, JP_LIT(") { "));
	if (!JP_EQ(in->rettype, "void"))
		jp_buf_append(text, JP_LIT("return "));
	jp_buf_putstr(text, in->name);
	jp_buf_putc(text, '(');
	jp_buf_append(text, args.data ? args.data : "", args.len);
	jp_buf_append(text, JP_LIT("); }"));
	jp_buf_free(&args);

	memset(out_sig, 0, sizeof(*out_sig));
	out_sig->name = base;
	out_sig->raw_attrs = in->raw_attrs;
	out_sig->rettype = in->rettype;
	out_sig->params = remaining;
	out_sig->nparams = nrem;
	out_sig->variadic = 0;
	return 1;
}

/* ── pass 2: (s, sz, cap) -> jstr_ty method ───────────────────────────── */

static int
jp_pass2(const jp_sig_ty *in, jp_names_ty *used, jp_arena_ty *arena, jp_buf_ty *text)
{
	jp_sigparam_ty *remaining;
	size_t nrem = 0;
	size_t i;
	int has_size_or_cap = 0;
	int has_data = 0;
	int returns_end_ptr = 0;
	int first_arg = 1;
	int ret_is_void;
	jp_str_ty new_ret;
	jp_str_ty j_name;
	jp_str_ty new_name;
	jp_buf_ty args;
	jp_str_ty attrs;
	jp_str_ty params_text;

	if (!jp_is_jstr_name(in->name) || JP_ENDS(in->name, "_j") || in->nparams == 0)
		return 0;
	if (JP_CONTAINS(in->name, "unsafe"))
		return 0;
	if (in->variadic)
		return 0;
	for (i = 0; i < in->nparams; i++) {
		if (JP_ENDS(in->params[i].text, "sz") || JP_ENDS(in->params[i].text, "cap")) {
			has_size_or_cap = 1;
			break;
		}
	}
	if (!has_size_or_cap)
		return 0;

	j_name = jp_concat_lit(in->name, JP_LIT("_j"), arena);
	if (jp_names_has(used, j_name))
		return 0;

	ret_is_void = JP_EQ(in->rettype, "void");
	if (!ret_is_void) {
		if (jp_has_end_ptr_suffix(in->name)) {
			returns_end_ptr = 1;
			new_ret = JP_SLIT("void");
		} else {
			jp_str_ty pname = jp_concat_lit(in->name, JP_LIT("_p"), arena);

			if (jp_names_has(used, pname))
				return 0;
			new_ret = in->rettype;
		}
	} else {
		new_ret = in->rettype;
	}

	remaining = jp_arena_alloc(arena, in->nparams * sizeof(*remaining));
	if (!remaining)
		return 0;
	jp_buf_init(&args);
	if (!ret_is_void && returns_end_ptr)
		jp_buf_append(&args, JP_LIT("j->size = JSTR_DIFF("));
	else if (!ret_is_void)
		jp_buf_append(&args, JP_LIT("return "));
	jp_buf_putstr(&args, in->name);
	jp_buf_putc(&args, '(');
	first_arg = 1;
	for (i = 0; i < in->nparams; i++) {
		const jp_sigparam_ty *p = &in->params[i];

		if (JP_EQ(p->var, "s")) {
			has_data = 1;
			if (p->is_ptr_ptr)
				jp_arg_append(&args, JP_SLIT("&j->data"), &first_arg);
			else
				jp_arg_append(&args, JP_SLIT("j->data"), &first_arg);
			remaining[nrem].text = p->is_const ? JP_SLIT("const jstr_ty *JSTR_RESTRICT j")
			                                  : JP_SLIT("jstr_ty *JSTR_RESTRICT j");
			remaining[nrem].var = JP_SLIT("j");
			remaining[nrem].is_ptr = 1;
			remaining[nrem].is_ptr_ptr = 0;
			remaining[nrem].is_const = p->is_const;
			remaining[nrem].has_size_t = 0;
			nrem++;
			continue;
		}
		if (JP_EQ(p->var, "sz") || JP_EQ(p->var, "cap")) {
			jp_buf_ty tmp;

			jp_buf_init(&tmp);
			if (p->is_ptr)
				jp_buf_append(&tmp, JP_LIT("&j->"));
			else
				jp_buf_append(&tmp, JP_LIT("j->"));
			if (JP_EQ(p->var, "sz"))
				jp_buf_append(&tmp, JP_LIT("size"));
			else
				jp_buf_append(&tmp, JP_LIT("capacity"));
			jp_arg_append(&args, (jp_str_ty){tmp.data, tmp.len}, &first_arg);
			jp_buf_free(&tmp);
			continue;
		}
		remaining[nrem++] = *p;
		jp_arg_append(&args, p->var, &first_arg);
	}
	if (!has_data) {
		jp_buf_free(&args);
		return 0;
	}
	jp_buf_putc(&args, ')');
	if (returns_end_ptr)
		jp_buf_append(&args, JP_LIT(", j->data)"));
	jp_buf_putc(&args, ';');

	attrs = jp_clean_attr(in->raw_attrs, 1, arena);
	if (returns_end_ptr)
		attrs = jp_replace_bare_func(attrs, arena);

	new_name = jp_remove_seg(j_name, JP_LIT("_p"), arena);
	params_text = jp_join_params(remaining, nrem, arena);
	jp_buf_init(text);
	jp_buf_putstr(text, attrs);
	jp_buf_putc(text, '\n');
	jp_buf_putstr(text, new_ret);
	jp_buf_putc(text, '\n');
	jp_buf_putstr(text, new_name);
	jp_buf_putc(text, '(');
	jp_buf_putstr(text, params_text);
	jp_buf_append(text, JP_LIT(") { "));
	jp_buf_append(text, args.data ? args.data : "", args.len);
	jp_buf_append(text, JP_LIT(" }"));
	jp_buf_free(&args);
	return 1;
}

/* ── generation pipeline ──────────────────────────────────────────────── */

typedef struct jp_gen_item {
	jp_function_ty fn;
	jp_sig_ty sig;
	jp_buf_ty pass1;
	jp_sig_ty sig_pass1;
	int has_pass1;
	jp_buf_ty pass2;
	int has_pass2;
	jp_buf_ty pass2w1;
	int has_pass2w1;
} jp_gen_item_ty;

int
jp_generate(const char *src, size_t len, jp_buf_ty *out)
{
	jp_tokens_ty toks;
	jp_arena_ty arena;
	jp_names_ty declared;
	jp_names_ty used;
	jp_gen_item_ty *items = NULL;
	size_t nitems = 0;
	size_t cap = 0;
	size_t i = 0;
	size_t pos = 0;

	if (jp_lex(src, len, &toks) != 0)
		return -1;
	jp_arena_init(&arena);
	jp_names_init(&declared);
	jp_names_init(&used);

	while (i < toks.len) {
		const jp_token_ty *t = &toks.data[i];
		jp_decl_result_ty res;

		if (t->kind == JP_TK_EOF)
			break;
		if (t->kind == JP_TK_COMMENT || t->kind == JP_TK_PP ||
		    (t->kind == JP_TK_PUNCT && t->end - t->start == 1 && src[t->start] == ';')) {
			i++;
			continue;
		}
		if (t->kind == JP_TK_PUNCT && t->end - t->start == 1 && src[t->start] == '{') {
			size_t j;
			int depth = 0;

			for (j = i; j < toks.len; j++) {
				const jp_token_ty *tj = &toks.data[j];
				if (tj->kind == JP_TK_PUNCT && tj->end - tj->start == 1) {
					if (src[tj->start] == '{')
						depth++;
					else if (src[tj->start] == '}') {
						depth--;
						if (depth == 0)
							break;
					}
				}
			}
			i = (j < toks.len) ? j + 1 : toks.len;
			continue;
		}
		memset(&res, 0, sizeof(res));
		(void)jp_parse_function(src, &toks, i, &arena, &res);
		if (res.kind == JP_DECL_FUNCTION) {
			jp_gen_item_ty *it;

			if (nitems == cap) {
				size_t ncap = cap ? cap * 2 : 32;
				jp_gen_item_ty *np = malloc(ncap * sizeof(*np));

				if (!np) {
					free(items);
					jp_names_free(&declared);
					jp_names_free(&used);
					jp_arena_free(&arena);
					jp_tokens_free(&toks);
					return -1;
				}
				if (nitems)
					memcpy(np, items, nitems * sizeof(*np));
				free(items);
				items = np;
				cap = ncap;
			}
			it = &items[nitems++];
			memset(it, 0, sizeof(*it));
			it->fn = res.fn;
			jp_sig_from_fn(src, &toks, &it->fn, &arena, &it->sig);
			jp_names_add(&declared, &arena, it->sig.name);
			if (jp_is_jstr_name(it->sig.name) && !it->sig.variadic && it->sig.nparams > 0)
				jp_names_add(&used, &arena, it->sig.name);
		} else if (res.kind == JP_DECL_PROTOTYPE) {
			jp_str_ty name =
				jp_trimmed_span(src,
			                        (jp_span_ty){toks.data[res.fn.name_idx].start,
			                                     toks.data[res.fn.name_idx].end},
			                        &arena);

			jp_names_add(&declared, &arena, name);
		}
		i = (res.end > i) ? res.end : i + 1;
	}

	/* Pass 1. */
	for (i = 0; i < nitems; i++) {
		if (jp_pass1(&items[i].sig, &declared, &arena, &items[i].sig_pass1, &items[i].pass1)) {
			items[i].has_pass1 = 1;
			jp_names_add(&used, &arena, items[i].sig_pass1.name);
		}
	}

	/* Pass 2, in the same order the text pipeline would visit blocks. */
	for (i = 0; i < nitems; i++) {
		jp_buf_ty text;

		jp_buf_init(&text);
		if (jp_pass2(&items[i].sig, &used, &arena, &text)) {
			jp_str_ty jname = jp_concat_lit(items[i].sig.name, JP_LIT("_j"), &arena);

			items[i].pass2 = text;
			items[i].has_pass2 = 1;
			jp_names_add(&used, &arena, jname);
		} else {
			jp_buf_free(&text);
		}
		if (items[i].has_pass1) {
			jp_buf_ty text2;

			jp_buf_init(&text2);
			if (jp_pass2(&items[i].sig_pass1, &used, &arena, &text2)) {
				jp_str_ty jname =
					jp_concat_lit(items[i].sig_pass1.name, JP_LIT("_j"), &arena);

				items[i].pass2w1 = text2;
				items[i].has_pass2w1 = 1;
				jp_names_add(&used, &arena, jname);
			} else {
				jp_buf_free(&text2);
			}
		}
	}

	/* Assemble: copy source, inserting wrappers after each function item. */
	jp_buf_init(out);
	for (i = 0; i < nitems; i++) {
		size_t end = items[i].fn.end_off;
		int inserted = 0;

		if (end < pos || end > len)
			continue;
		if (jp_buf_append(out, src + pos, end - pos) != 0)
			goto fail;
		if (items[i].has_pass2) {
			jp_buf_append(out, JP_LIT("\n\n"));
			jp_buf_append(out, items[i].pass2.data, items[i].pass2.len);
			inserted = 1;
		}
		if (items[i].has_pass1) {
			jp_buf_append(out, JP_LIT("\n\n"));
			jp_buf_append(out, items[i].pass1.data, items[i].pass1.len);
			inserted = 1;
		}
		if (items[i].has_pass2w1) {
			jp_buf_append(out, JP_LIT("\n\n"));
			jp_buf_append(out, items[i].pass2w1.data, items[i].pass2w1.len);
			inserted = 1;
		}
		if (inserted && end < len && src[end] != '\n')
			jp_buf_putc(out, '\n');
		pos = end;
	}
	if (jp_buf_append(out, src + pos, len - pos) != 0)
		goto fail;
	if (out->len == 0 || out->data[out->len - 1] != '\n')
		jp_buf_putc(out, '\n');

	for (i = 0; i < nitems; i++) {
		jp_buf_free(&items[i].pass1);
		jp_buf_free(&items[i].pass2);
		jp_buf_free(&items[i].pass2w1);
	}
	free(items);
	jp_names_free(&declared);
	jp_names_free(&used);
	jp_arena_free(&arena);
	jp_tokens_free(&toks);
	return 0;

fail:
	for (i = 0; i < nitems; i++) {
		jp_buf_free(&items[i].pass1);
		jp_buf_free(&items[i].pass2);
		jp_buf_free(&items[i].pass2w1);
	}
	free(items);
	jp_names_free(&declared);
	jp_names_free(&used);
	jp_arena_free(&arena);
	jp_tokens_free(&toks);
	jp_buf_free(out);
	return -1;
}

/* ── namespace macro renaming ─────────────────────────────────────────── */

static int
jp_ns_should_ignore(jp_str_ty s, const jp_names_ty *ignore)
{
	size_t i;

	for (i = 0; i < ignore->len; i++) {
		if (jp_starts_with(s, ignore->data[i].data, ignore->data[i].len))
			return 1;
	}
	return 0;
}

static void
jp_ns_scan(const char *src, size_t len, jp_str_ty prefix, const jp_names_ty *names,
           jp_buf_ty *out)
{
	size_t i = 0;

	while (i < len) {
		char c = src[i];

		if (c == '/' && i + 1 < len && src[i + 1] == '/') {
			size_t start = i;

			i += 2;
			while (i < len && src[i] != '\n')
				i++;
			jp_buf_append(out, src + start, i - start);
			continue;
		}
		if (c == '/' && i + 1 < len && src[i + 1] == '*') {
			size_t start = i;

			i += 2;
			while (i + 1 < len && !(src[i] == '*' && src[i + 1] == '/'))
				i++;
			if (i + 1 < len)
				i += 2;
			jp_buf_append(out, src + start, i - start);
			continue;
		}
		if (c == '"' || c == '\'') {
			char quote = c;
			size_t start = i;

			i++;
			while (i < len) {
				if (src[i] == '\\' && i + 1 < len) {
					i += 2;
					continue;
				}
				if (src[i] == quote) {
					i++;
					break;
				}
				if (src[i] == '\n')
					break;
				i++;
			}
			jp_buf_append(out, src + start, i - start);
			continue;
		}
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_') {
			size_t start = i;
			jp_str_ty word;
			int found = 0;
			size_t k;

			i++;
			while (i < len) {
				char d = src[i];
				if ((d >= 'A' && d <= 'Z') || (d >= 'a' && d <= 'z') ||
				    (d >= '0' && d <= '9') || d == '_')
					i++;
				else
					break;
			}
			word.data = src + start;
			word.len = i - start;
			for (k = 0; k < names->len; k++) {
				if (jp_str_eq(names->data[k], word)) {
					found = 1;
					break;
				}
			}
			if (found) {
				jp_buf_putstr(out, prefix);
				jp_buf_putstr(out, word);
			} else {
				jp_buf_putstr(out, word);
			}
			continue;
		}
		jp_buf_putc(out, c);
		i++;
	}
}

int
jp_namespace(const char *src, size_t len, jp_buf_ty *out)
{
	jp_tokens_ty toks;
	jp_arena_ty arena;
	jp_names_ty names;
	jp_names_ty ignore;
	size_t i;

	if (jp_lex(src, len, &toks) != 0)
		return -1;
	jp_arena_init(&arena);
	jp_names_init(&names);
	jp_names_init(&ignore);
	jp_names_add(&ignore, &arena, JP_SLIT("JSTR"));
	jp_names_add(&ignore, &arena, JP_SLIT("jstr"));

	for (i = 0; i < toks.len; i++) {
		jp_span_ty name;

		if (toks.data[i].kind != JP_TK_PP)
			continue;
		if (!jp_pp_undef_name(src, &toks.data[i], &name))
			continue;
		{
			jp_str_ty macro;
			size_t k;
			int valid = 1;

			macro.data = src + name.start;
			macro.len = name.end - name.start;
			for (k = 0; k < macro.len; k++) {
				char c = macro.data[k];
				if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
					valid = 0;
					break;
				}
			}
			if (!valid || macro.len == 0)
				continue;
			if (!jp_ns_should_ignore(macro, &ignore))
				jp_names_add(&names, &arena, macro);
		}
	}

	jp_buf_init(out);
	jp_ns_scan(src, len, JP_SLIT("JSTR_INTERNAL_"), &names, out);
	jp_names_free(&names);
	jp_names_free(&ignore);
	jp_arena_free(&arena);
	jp_tokens_free(&toks);
	return 0;
}
