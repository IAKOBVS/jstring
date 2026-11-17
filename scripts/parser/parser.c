#include "jstrgen.h"

#include <string.h>

#define JP_NPOS ((size_t)-1)

#define JP_WORD_EQ(s, n, lit) jp_word_eq_n((s), (n), JP_LIT(lit))
#define JP_STARTS(s, n, lit) jp_starts_with_n((s), (n), JP_LIT(lit))

static int
jp_word_eq_n(const char *s, size_t n, const char *word, size_t word_len)
{
	return n == word_len && memcmp(s, word, n) == 0;
}

static int
jp_starts_with_n(const char *s, size_t n, const char *prefix, size_t prefix_len)
{
	return n >= prefix_len && memcmp(s, prefix, prefix_len) == 0;
}

static int
jp_tok_punct(const char *src, const jp_token_ty *t, char c)
{
	return t->kind == JP_TK_PUNCT && t->end - t->start == 1 && src[t->start] == c;
}

static int
jp_tok_dots(const jp_token_ty *t)
{
	return t->kind == JP_TK_PUNCT && t->end - t->start == 3;
}

static int
jp_is_storage(const char *s, size_t n)
{
	return JP_WORD_EQ(s, n, "static") || JP_WORD_EQ(s, n, "inline") ||
	       JP_WORD_EQ(s, n, "extern") || JP_WORD_EQ(s, n, "_Noreturn") ||
	       JP_WORD_EQ(s, n, "register") || JP_WORD_EQ(s, n, "auto") ||
	       JP_WORD_EQ(s, n, "_Thread_local");
}

static int
jp_is_attr_macro(const char *s, size_t n)
{
	return JP_STARTS(s, n, "JSTR_FUNC") || JP_STARTS(s, n, "JSTR_ATTR_") ||
	       JP_STARTS(s, n, "JSTR_NONNULL");
}

static int
jp_is_internal_macro(const char *s, size_t n)
{
	return JP_STARTS(s, n, "JSTR_INTERNAL_");
}

static int
jp_is_type_keyword(const char *s, size_t n)
{
	static const jp_str_ty kws[] = {
		JP_STR_LIT("typedef"),   JP_STR_LIT("struct"),   JP_STR_LIT("union"),
		JP_STR_LIT("enum"),      JP_STR_LIT("void"),     JP_STR_LIT("char"),
		JP_STR_LIT("short"),     JP_STR_LIT("int"),      JP_STR_LIT("long"),
		JP_STR_LIT("float"),     JP_STR_LIT("double"),   JP_STR_LIT("signed"),
		JP_STR_LIT("unsigned"),  JP_STR_LIT("_Bool"),    JP_STR_LIT("const"),
		JP_STR_LIT("volatile"),  JP_STR_LIT("restrict"), JP_STR_LIT("size_t"),
		JP_STR_LIT("ssize_t"),   JP_STR_LIT("ptrdiff_t"), JP_STR_LIT("FILE"),
		JP_STR_LIT("va_list"),   JP_STR_LIT("R"),        JP_STR_LIT("JSTR_RESTRICT"),
	};
	size_t i;

	for (i = 0; i < sizeof(kws) / sizeof(kws[0]); i++) {
		if (jp_word_eq_n(s, n, kws[i].data, kws[i].len))
			return 1;
	}
	return 0;
}

static size_t
jp_next_sig(const jp_tokens_ty *toks, size_t i)
{
	while (i < toks->len) {
		jp_tok_kind_ty k = toks->data[i].kind;
		if (k == JP_TK_COMMENT || k == JP_TK_PP) {
			i++;
			continue;
		}
		break;
	}
	return i;
}

static size_t
jp_prev_sig(const jp_tokens_ty *toks, size_t i)
{
	while (i > 0) {
		jp_tok_kind_ty k = toks->data[i - 1].kind;
		i--;
		if (k == JP_TK_COMMENT || k == JP_TK_PP)
			continue;
		return i;
	}
	return JP_NPOS;
}

static size_t
jp_match(const char *src, const jp_tokens_ty *toks, size_t open_idx, char open, char close)
{
	size_t i;
	int depth = 0;

	for (i = open_idx; i < toks->len; i++) {
		const jp_token_ty *t = &toks->data[i];
		char c;

		if (t->kind != JP_TK_PUNCT || t->end - t->start != 1)
			continue;
		c = src[t->start];
		if (c == open) {
			depth++;
		} else if (c == close) {
			depth--;
			if (depth == 0)
				return i;
		}
	}
	return JP_NPOS;
}

static jp_span_ty
jp_span_empty(void)
{
	jp_span_ty s;

	s.start = 0;
	s.end = 0;
	return s;
}

static jp_span_ty
jp_find_param_name(const char *src, const jp_tokens_ty *toks, size_t a, size_t b)
{
	jp_span_ty r = jp_span_empty();
	size_t i;
	size_t idx;

	/* Function-pointer parameter: the declarator name follows the first `*`
	 * inside parentheses, e.g. `int (*cb)(int)`. */
	for (i = a; i < b; i++) {
		const jp_token_ty *t = &toks->data[i];
		size_t j;
		size_t k;
		size_t m;

		if (!jp_tok_punct(src, t, '('))
			continue;
		j = jp_next_sig(toks, i + 1);
		if (j >= b || !jp_tok_punct(src, &toks->data[j], '*'))
			continue;
		k = jp_next_sig(toks, j + 1);
		if (k >= b || toks->data[k].kind != JP_TK_IDENT)
			continue;
		if (jp_is_type_keyword(src + toks->data[k].start, toks->data[k].end - toks->data[k].start))
			continue;
		m = jp_next_sig(toks, k + 1);
		if (m < b && jp_tok_punct(src, &toks->data[m], ')')) {
			r.start = toks->data[k].start;
			r.end = toks->data[k].end;
			return r;
		}
	}

	/* Named parameter: the last identifier that is not a type keyword,
	 * skipping an optional array suffix. */
	idx = jp_prev_sig(toks, b);
	if (idx != JP_NPOS && jp_tok_punct(src, &toks->data[idx], ']')) {
		int depth = 0;
		for (i = idx + 1; i > a; i--) {
			const jp_token_ty *t = &toks->data[i - 1];

			if (jp_tok_punct(src, t, ']'))
				depth++;
			else if (jp_tok_punct(src, t, '[')) {
				depth--;
				if (depth == 0) {
					idx = jp_prev_sig(toks, i - 1);
					break;
				}
			}
		}
	}
	while (idx != JP_NPOS && idx >= a) {
		const jp_token_ty *t = &toks->data[idx];

		if (t->kind == JP_TK_IDENT &&
		    !jp_is_type_keyword(src + t->start, t->end - t->start)) {
			r.start = t->start;
			r.end = t->end;
			return r;
		}
		if (idx == 0)
			break;
		idx = jp_prev_sig(toks, idx);
	}
	return r;
}

static void
jp_add_param(const char *src, const jp_tokens_ty *toks, size_t a, size_t b, jp_arena_ty *arena,
             jp_function_ty *fn, size_t *cap)
{
	size_t first;
	size_t last;
	size_t i;
	jp_param_ty p;

	first = a;
	while (first < b && (toks->data[first].kind == JP_TK_COMMENT))
		first++;
	last = b;
	while (last > first && (toks->data[last - 1].kind == JP_TK_COMMENT))
		last--;
	if (first >= last)
		return;

	/* variadic marker */
	if (last - first == 1 && jp_tok_dots(&toks->data[first])) {
		fn->variadic = 1;
		return;
	}

	memset(&p, 0, sizeof(p));
	p.span.start = toks->data[first].start;
	p.span.end = toks->data[last - 1].end;
	p.name = jp_find_param_name(src, toks, first, last);

	for (i = first; i < last; i++) {
		const jp_token_ty *t = &toks->data[i];

		if (t->kind == JP_TK_PUNCT && t->end - t->start == 1 && src[t->start] == '*')
			p.is_ptr++;
		if (t->kind == JP_TK_IDENT) {
			size_t tl = t->end - t->start;
			const char *ts = src + t->start;

			if (JP_WORD_EQ(ts, tl, "size_t") ||
			    (tl >= 7 && memcmp(ts + tl - 7, "size_ty", 7) == 0))
				p.has_size_t = 1;
		}
	}
	if (p.is_ptr >= 2)
		p.is_ptr_ptr = 1;

	/* `const` only qualifies the data when it precedes the first `*`. */
	for (i = first; i < last; i++) {
		const jp_token_ty *t = &toks->data[i];

		if (t->kind == JP_TK_PUNCT && t->end - t->start == 1 && src[t->start] == '*')
			break;
		if (t->kind == JP_TK_IDENT &&
		    JP_WORD_EQ(src + t->start, t->end - t->start, "const")) {
			p.is_const = 1;
			break;
		}
	}

	if (fn->nparams == *cap) {
		size_t ncap = *cap ? *cap * 2 : 8;
		jp_param_ty *np = jp_arena_alloc(arena, ncap * sizeof(*np));
		if (!np)
			return;
		if (fn->params && fn->nparams)
			memcpy(np, fn->params, fn->nparams * sizeof(*np));
		fn->params = np;
		*cap = ncap;
	}
	fn->params[fn->nparams++] = p;
}

static void
jp_parse_params(const char *src, const jp_tokens_ty *toks, size_t lp, size_t rp, jp_arena_ty *arena,
                jp_function_ty *fn)
{
	size_t a = lp + 1;
	size_t i;
	int depth = 0;
	size_t cap = 0;

	fn->params = NULL;
	fn->nparams = 0;
	fn->variadic = 0;
	for (i = lp + 1; i < rp; i++) {
		const jp_token_ty *t = &toks->data[i];

		if (jp_tok_punct(src, t, '(') || jp_tok_punct(src, t, '[') || jp_tok_punct(src, t, '{'))
			depth++;
		else if (jp_tok_punct(src, t, ')') || jp_tok_punct(src, t, ']') ||
		         jp_tok_punct(src, t, '}'))
			depth--;
		else if (depth == 0 && jp_tok_punct(src, t, ',')) {
			jp_add_param(src, toks, a, i, arena, fn, &cap);
			a = i + 1;
		}
	}
	jp_add_param(src, toks, a, rp, arena, fn, &cap);
}

static int
jp_is_type_decl_start(const char *src, const jp_token_ty *t)
{
	if (t->kind != JP_TK_IDENT)
		return 0;
	return JP_WORD_EQ(src + t->start, t->end - t->start, "typedef") ||
	       JP_WORD_EQ(src + t->start, t->end - t->start, "struct") ||
	       JP_WORD_EQ(src + t->start, t->end - t->start, "union") ||
	       JP_WORD_EQ(src + t->start, t->end - t->start, "enum");
}

int
jp_parse_function(const char *src, const jp_tokens_ty *toks, size_t start, jp_arena_ty *arena,
                  jp_decl_result_ty *out)
{
	size_t n = toks->len;
	size_t j = start;
	size_t attrs_begin = JP_NPOS;
	size_t attrs_end = JP_NPOS;
	size_t k;

	memset(out, 0, sizeof(*out));
	out->kind = JP_DECL_NONE;
	out->end = start;

	/* Leading declaration attributes/storage. */
	while (j < n) {
		const jp_token_ty *t = &toks->data[j];
		const char *text;
		size_t len;

		if (t->kind == JP_TK_EOF)
			break;
		if (t->kind == JP_TK_COMMENT || t->kind == JP_TK_PP) {
			j++;
			continue;
		}
		if (t->kind != JP_TK_IDENT)
			break;
		text = src + t->start;
		len = t->end - t->start;
		if (jp_is_internal_macro(text, len)) {
			size_t next = jp_next_sig(toks, j + 1);

			if (next < n && jp_tok_punct(src, &toks->data[next], '(')) {
				size_t rp = jp_match(src, toks, next, '(', ')');
				out->end = (rp == JP_NPOS) ? n : rp + 1;
			} else {
				out->end = j + 1;
			}
			return 0;
		}
		if (jp_is_storage(text, len) || jp_is_attr_macro(text, len)) {
			size_t next;

			if (attrs_begin == JP_NPOS)
				attrs_begin = j;
			j++;
			next = jp_next_sig(toks, j);
			if (next < n && jp_tok_punct(src, &toks->data[next], '(')) {
				size_t rp = jp_match(src, toks, next, '(', ')');
				if (rp == JP_NPOS) {
					out->end = n;
					return 0;
				}
				j = rp + 1;
			}
			attrs_end = j;
			continue;
		}
		break;
	}

	if (j >= n) {
		out->end = n;
		return 0;
	}
	if (toks->data[j].kind == JP_TK_EOF) {
		out->end = n;
		return 0;
	}
	if (jp_tok_punct(src, &toks->data[j], ';')) {
		out->end = j + 1;
		return 0;
	}
	if (jp_is_type_decl_start(src, &toks->data[j])) {
		/* Skip the whole typedef/aggregate declaration. */
		size_t i;
		int depth = 0;
		for (i = j; i < n; i++) {
			const jp_token_ty *t = &toks->data[i];
			if (t->kind == JP_TK_EOF)
				break;
			if (jp_tok_punct(src, t, '{'))
				depth++;
			else if (jp_tok_punct(src, t, '}'))
				depth--;
			else if (depth == 0 && jp_tok_punct(src, t, ';')) {
				out->end = i + 1;
				return 0;
			}
		}
		out->end = n;
		return 0;
	}

	/* Find `IDENT (` whose closing `)` is followed by a body or `;`. */
	for (k = j; k < n; k++) {
		const jp_token_ty *t = &toks->data[k];
		size_t nk;
		size_t lp;
		size_t rp;
		size_t m;
		int found_body = 0;
		int found_proto = 0;

		if (t->kind == JP_TK_EOF) {
			out->end = n;
			return 0;
		}
		if (t->kind == JP_TK_COMMENT || t->kind == JP_TK_PP)
			continue;
		if (jp_tok_punct(src, t, ';')) {
			out->end = k + 1;
			return 0;
		}
		if (jp_tok_punct(src, t, '{')) {
			size_t rb = jp_match(src, toks, k, '{', '}');
			out->end = (rb == JP_NPOS) ? n : rb + 1;
			return 0;
		}
		if (t->kind != JP_TK_IDENT)
			continue;
		nk = jp_next_sig(toks, k + 1);
		if (nk >= n || !jp_tok_punct(src, &toks->data[nk], '('))
			continue;
		lp = nk;
		rp = jp_match(src, toks, lp, '(', ')');
		if (rp == JP_NPOS) {
			out->end = n;
			return 0;
		}
		for (m = rp + 1; m < n; m++) {
			const jp_token_ty *tm = &toks->data[m];

			if (tm->kind == JP_TK_EOF)
				break;
			if (tm->kind == JP_TK_COMMENT || tm->kind == JP_TK_PP)
				continue;
			if (jp_tok_punct(src, tm, '{')) {
				found_body = 1;
				break;
			}
			if (jp_tok_punct(src, tm, ';')) {
				found_proto = 1;
				break;
			}
			if (jp_tok_punct(src, tm, '('))
				break;
		}
		if (found_body) {
			size_t rb = jp_match(src, toks, m, '{', '}');
			size_t e;
			int saw_pp = 0;

			if (rb == JP_NPOS) {
				out->end = n;
				return 0;
			}
			memset(&out->fn, 0, sizeof(out->fn));
			out->fn.full.start = toks->data[start].start;
			out->fn.attrs_begin = (attrs_begin == JP_NPOS) ? j : attrs_begin;
			out->fn.attrs_end = (attrs_begin == JP_NPOS) ? j : attrs_end;
			out->fn.ret_begin = j;
			out->fn.ret_end = k;
			out->fn.name_idx = k;
			out->fn.lparen_idx = lp;
			out->fn.rparen_idx = rp;
			out->fn.lbrace_idx = m;
			out->fn.rbrace_idx = rb;
			jp_parse_params(src, toks, lp, rp, arena, &out->fn);
			e = rb + 1;
			while (e < n) {
				const jp_token_ty *te = &toks->data[e];

				if (te->kind == JP_TK_EOF)
					break;
				if (te->kind == JP_TK_COMMENT) {
					e++;
					continue;
				}
				if (te->kind == JP_TK_PP) {
					if (jp_pp_is(src, te, "else") || jp_pp_is(src, te, "endif") ||
					    jp_pp_is(src, te, "elif")) {
						saw_pp = 1;
						e++;
						continue;
					}
					break;
				}
				if (saw_pp && jp_tok_punct(src, te, ';')) {
					e++;
					continue;
				}
				break;
			}
			out->fn.end_off = (e > rb + 1) ? toks->data[e - 1].end : toks->data[rb].end;
			out->fn.full.end = out->fn.end_off;
			out->end = e;
			out->kind = JP_DECL_FUNCTION;
			return 1;
		}
		if (found_proto) {
			out->kind = JP_DECL_PROTOTYPE;
			out->fn.name_idx = k;
			out->fn.full.start = toks->data[start].start;
			out->fn.full.end = toks->data[m].end;
			out->end = m + 1;
			return 0;
		}
		/* Not a declarator name (e.g. a macro call); resume after its `)`. */
		k = rp;
	}

	out->end = n;
	return 0;
}
