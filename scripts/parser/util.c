#include "jstrgen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── growable buffer ──────────────────────────────────────────────────── */

void
jp_buf_init(jp_buf_ty *b)
{
	b->data = NULL;
	b->len = 0;
	b->cap = 0;
}

void
jp_buf_free(jp_buf_ty *b)
{
	free(b->data);
	b->data = NULL;
	b->len = 0;
	b->cap = 0;
}

int
jp_buf_reserve(jp_buf_ty *b, size_t extra)
{
	size_t need;
	size_t cap;
	char *p;

	if (extra > (size_t)-1 - b->len - 1)
		return -1;
	need = b->len + extra + 1;
	if (need <= b->cap)
		return 0;
	cap = b->cap ? b->cap : 256;
	while (cap < need) {
		if (cap > ((size_t)-1) / 2) {
			cap = need;
			break;
		}
		cap *= 2;
	}
	p = realloc(b->data, cap);
	if (!p)
		return -1;
	b->data = p;
	b->cap = cap;
	return 0;
}

int
jp_buf_append(jp_buf_ty *b, const char *s, size_t n)
{
	if (jp_buf_reserve(b, n) != 0)
		return -1;
	if (!b->data)
		return -1;
	if (n)
		memcpy(b->data + b->len, s, n);
	b->len += n;
	b->data[b->len] = '\0';
	return 0;
}

int
jp_buf_putc(jp_buf_ty *b, char c)
{
	return jp_buf_append(b, &c, 1);
}

int
jp_buf_putstr(jp_buf_ty *b, jp_str_ty s)
{
	return jp_buf_append(b, s.data, s.len);
}

int
jp_buf_printf(jp_buf_ty *b, const char *fmt, ...)
{
	va_list ap;
	va_list cp;
	int n;

	va_start(ap, fmt);
	va_copy(cp, ap);
	n = vsnprintf(NULL, 0, fmt, cp);
	va_end(cp);
	if (n < 0) {
		va_end(ap);
		return -1;
	}
	if (jp_buf_reserve(b, (size_t)n) != 0) {
		va_end(ap);
		return -1;
	}
	(void)vsnprintf(b->data + b->len, (size_t)n + 1, fmt, ap);
	va_end(ap);
	b->len += (size_t)n;
	return 0;
}

/* ── arena ────────────────────────────────────────────────────────────── */

#define JP_ARENA_BLOCK ((size_t)65536)

struct jp_arena_block {
	jp_arena_block_ty *next;
	size_t used;
	size_t cap;
	unsigned char data[];
};

void
jp_arena_init(jp_arena_ty *a)
{
	a->head = NULL;
}

void
jp_arena_free(jp_arena_ty *a)
{
	jp_arena_block_ty *p = a->head;

	while (p) {
		jp_arena_block_ty *next = p->next;
		free(p);
		p = next;
	}
	a->head = NULL;
}

static jp_arena_block_ty *
jp_arena_new_block(size_t cap)
{
	jp_arena_block_ty *p = malloc(sizeof(*p) + cap);

	if (!p)
		return NULL;
	p->next = NULL;
	p->used = 0;
	p->cap = cap;
	return p;
}

void *
jp_arena_alloc(jp_arena_ty *a, size_t n)
{
	jp_arena_block_ty *p;
	size_t aligned;

	if (n == 0)
		n = 1;
	aligned = (n + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
	p = a->head;
	if (!p || p->cap - p->used < aligned) {
		size_t cap = aligned > JP_ARENA_BLOCK ? aligned : JP_ARENA_BLOCK;
		jp_arena_block_ty *np = jp_arena_new_block(cap);
		if (!np)
			return NULL;
		np->next = a->head;
		a->head = np;
		p = np;
	}
	{
		void *r = p->data + p->used;
		p->used += aligned;
		return r;
	}
}

char *
jp_arena_strndup(jp_arena_ty *a, const char *s, size_t n)
{
	char *p = jp_arena_alloc(a, n + 1);

	if (!p)
		return NULL;
	if (n)
		memcpy(p, s, n);
	p[n] = '\0';
	return p;
}

/* ── token vector ─────────────────────────────────────────────────────── */

void
jp_tokens_init(jp_tokens_ty *t)
{
	t->data = NULL;
	t->len = 0;
	t->cap = 0;
}

void
jp_tokens_free(jp_tokens_ty *t)
{
	free(t->data);
	t->data = NULL;
	t->len = 0;
	t->cap = 0;
}

static int
jp_tokens_push(jp_tokens_ty *t, jp_tok_kind_ty kind, size_t start, size_t end, unsigned int line,
               unsigned int col)
{
	jp_token_ty *p;

	if (t->len == t->cap) {
		size_t cap = t->cap ? t->cap * 2 : 256;
		p = realloc(t->data, cap * sizeof(*p));
		if (!p)
			return -1;
		t->data = p;
		t->cap = cap;
	}
	t->data[t->len].kind = kind;
	t->data[t->len].start = start;
	t->data[t->len].end = end;
	t->data[t->len].line = line;
	t->data[t->len].col = col;
	t->len++;
	return 0;
}

/* ── lexer ────────────────────────────────────────────────────────────── */

static int
jp_is_ident_start(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static int
jp_is_ident_char(char c)
{
	return jp_is_ident_start(c) || (c >= '0' && c <= '9');
}

static int
jp_is_digit(char c)
{
	return c >= '0' && c <= '9';
}

static int
jp_is_hspace(char c)
{
	return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}

/* Skip a backslash-newline splice at src[i].  Returns the new index or i. */
static size_t
jp_splice(const char *src, size_t len, size_t i, unsigned int *line, unsigned int *col)
{
	if (i + 1 < len && src[i] == '\\' && src[i + 1] == '\n') {
		*line += 1;
		*col = 1;
		return i + 2;
	}
	if (i + 2 < len && src[i] == '\\' && src[i + 1] == '\r' && src[i + 2] == '\n') {
		*line += 1;
		*col = 1;
		return i + 3;
	}
	return i;
}

int
jp_lex(const char *src, size_t len, jp_tokens_ty *out)
{
	size_t i = 0;
	unsigned int line = 1;
	unsigned int col = 1;
	int at_line_start = 1;

	jp_tokens_init(out);
	while (i < len) {
		char c = src[i];
		size_t start;
		unsigned int sl, sc;

		/* line splices behave like whitespace */
		{
			size_t n = jp_splice(src, len, i, &line, &col);
			if (n != i) {
				i = n;
				continue;
			}
		}
		if (c == '\n') {
			i++;
			line++;
			col = 1;
			at_line_start = 1;
			continue;
		}
		if (jp_is_hspace(c)) {
			i++;
			col++;
			continue;
		}

		/* preprocessor directive: '#' is the first token on the logical line */
		if (c == '#' && at_line_start) {
			start = i;
			sl = line;
			sc = col;
			i++;
			col++;
			while (i < len) {
				size_t n = jp_splice(src, len, i, &line, &col);
				if (n != i) {
					i = n;
					continue;
				}
				if (src[i] == '\n' || src[i] == '\r')
					break;
				i++;
				col++;
			}
			if (jp_tokens_push(out, JP_TK_PP, start, i, sl, sc) != 0)
				return -1;
			at_line_start = 0;
			continue;
		}

		/* comments */
		if (c == '/' && i + 1 < len && src[i + 1] == '/') {
			start = i;
			sl = line;
			sc = col;
			i += 2;
			col += 2;
			while (i < len && src[i] != '\n')
				i++, col++;
			if (jp_tokens_push(out, JP_TK_COMMENT, start, i, sl, sc) != 0)
				return -1;
			at_line_start = 0;
			continue;
		}
		if (c == '/' && i + 1 < len && src[i + 1] == '*') {
			start = i;
			sl = line;
			sc = col;
			i += 2;
			col += 2;
			while (i < len) {
				if (src[i] == '*' && i + 1 < len && src[i + 1] == '/') {
					i += 2;
					col += 2;
					break;
				}
				if (src[i] == '\n') {
					i++;
					line++;
					col = 1;
					continue;
				}
				i++;
				col++;
			}
			if (jp_tokens_push(out, JP_TK_COMMENT, start, i, sl, sc) != 0)
				return -1;
			/* comments are whitespace; at_line_start is unchanged */
			continue;
		}

		/* identifiers */
		if (jp_is_ident_start(c)) {
			start = i;
			sl = line;
			sc = col;
			while (i < len && jp_is_ident_char(src[i]))
				i++, col++;
			if (jp_tokens_push(out, JP_TK_IDENT, start, i, sl, sc) != 0)
				return -1;
			at_line_start = 0;
			continue;
		}

		/* numbers */
		if (jp_is_digit(c) || (c == '.' && i + 1 < len && jp_is_digit(src[i + 1]))) {
			start = i;
			sl = line;
			sc = col;
			i++;
			col++;
			while (i < len) {
				char d = src[i];
				if (jp_is_ident_char(d) || d == '.' || d == '\'') {
					i++;
					col++;
					continue;
				}
				if ((d == '+' || d == '-') && i > start &&
				    (src[i - 1] == 'e' || src[i - 1] == 'E' || src[i - 1] == 'p' || src[i - 1] == 'P')) {
					i++;
					col++;
					continue;
				}
				break;
			}
			if (jp_tokens_push(out, JP_TK_NUMBER, start, i, sl, sc) != 0)
				return -1;
			at_line_start = 0;
			continue;
		}

		/* string and character literals */
		if (c == '"' || c == '\'') {
			char quote = c;
			start = i;
			sl = line;
			sc = col;
			i++;
			col++;
			while (i < len) {
				size_t n = jp_splice(src, len, i, &line, &col);
				if (n != i) {
					i = n;
					continue;
				}
				if (src[i] == '\\' && i + 1 < len) {
					i += 2;
					col += 2;
					continue;
				}
				if (src[i] == quote) {
					i++;
					col++;
					break;
				}
				if (src[i] == '\n') {
					/* unterminated literal; stop at newline */
					break;
				}
				i++;
				col++;
			}
			if (jp_tokens_push(out, quote == '"' ? JP_TK_STRING : JP_TK_CHAR, start, i, sl, sc) != 0)
				return -1;
			at_line_start = 0;
			continue;
		}

		/* punctuators: `...` is a single token, everything else is one byte */
		start = i;
		sl = line;
		sc = col;
		if (c == '.' && i + 2 < len && src[i + 1] == '.' && src[i + 2] == '.') {
			i += 3;
			col += 3;
		} else {
			i++;
			col++;
		}
		if (jp_tokens_push(out, JP_TK_PUNCT, start, i, sl, sc) != 0)
			return -1;
		at_line_start = 0;
	}

	return jp_tokens_push(out, JP_TK_EOF, len, len, line, col);
}

/* ── preprocessor helpers ─────────────────────────────────────────────── */

static int
jp_word_eq_n(const char *s, size_t n, const char *word, size_t word_len)
{
	return n == word_len && memcmp(s, word, n) == 0;
}

int
jp_pp_is_n(const char *src, const jp_token_ty *tok, const char *word, size_t word_len)
{
	size_t i;
	size_t start;

	if (tok->kind != JP_TK_PP)
		return 0;
	i = tok->start;
	if (i >= tok->end || src[i] != '#')
		return 0;
	i++;
	while (i < tok->end && jp_is_hspace(src[i]))
		i++;
	start = i;
	while (i < tok->end && jp_is_ident_char(src[i]))
		i++;
	return jp_word_eq_n(src + start, i - start, word, word_len);
}

int
jp_pp_undef_name(const char *src, const jp_token_ty *tok, jp_span_ty *name)
{
	size_t i;
	size_t start;

	if (!jp_pp_is_n(src, tok, JP_LIT("undef")))
		return 0;
	i = tok->start;
	i++; /* '#' */
	while (i < tok->end && jp_is_hspace(src[i]))
		i++;
	while (i < tok->end && jp_is_ident_char(src[i]))
		i++; /* "undef" */
	while (i < tok->end && jp_is_hspace(src[i]))
		i++;
	start = i;
	while (i < tok->end && (jp_is_ident_char(src[i]) || src[i] == '_'))
		i++;
	if (i == start)
		return 0;
	name->start = start;
	name->end = i;
	return 1;
}

int
jp_read_file(const char *path, jp_buf_ty *out)
{
	FILE *f;
	char tmp[8192];

	jp_buf_init(out);
	f = fopen(path, "rb");
	if (!f)
		return -1;
	for (;;) {
		size_t n = fread(tmp, 1, sizeof(tmp), f);
		if (n && jp_buf_append(out, tmp, n) != 0) {
			fclose(f);
			jp_buf_free(out);
			return -1;
		}
		if (n < sizeof(tmp)) {
			if (ferror(f)) {
				fclose(f);
				jp_buf_free(out);
				return -1;
			}
			break;
		}
	}
	fclose(f);
	return 0;
}
