#ifndef JSTRGEN_H
#define JSTRGEN_H

#include <stdarg.h>
#include <stddef.h>

/*
 * jstrgen — source-preserving C lexer + recursive-descent declaration parser
 * used to generate jstr wrappers. No regular expressions are used anywhere.
 *
 * All internal types use the repository's `_ty` typedef convention.
 */

typedef struct jp_span {
	size_t start;
	size_t end;
} jp_span_ty;

/* Borrowed or arena-owned byte string with an explicit length. */
typedef struct jp_str {
	const char *data;
	size_t len;
} jp_str_ty;

#define JP_LIT(s) (s), (sizeof(s) - 1)
#define JP_STR_LIT(s)                                                                              \
	{                                                                                              \
		(s), sizeof(s) - 1                                                                         \
	}

/* ── growable byte buffer ─────────────────────────────────────────────── */

typedef struct jp_buf {
	char *data;
	size_t len;
	size_t cap;
} jp_buf_ty;

void jp_buf_init(jp_buf_ty *b);
void jp_buf_free(jp_buf_ty *b);
int jp_buf_reserve(jp_buf_ty *b, size_t extra);
int jp_buf_append(jp_buf_ty *b, const char *s, size_t n);
int jp_buf_putc(jp_buf_ty *b, char c);
int jp_buf_putstr(jp_buf_ty *b, jp_str_ty s);
int jp_buf_printf(jp_buf_ty *b, const char *fmt, ...);

/* ── arena ────────────────────────────────────────────────────────────── */

typedef struct jp_arena_block jp_arena_block_ty;
typedef struct jp_arena {
	jp_arena_block_ty *head;
} jp_arena_ty;

void jp_arena_init(jp_arena_ty *a);
void jp_arena_free(jp_arena_ty *a);
void *jp_arena_alloc(jp_arena_ty *a, size_t n);
char *jp_arena_strndup(jp_arena_ty *a, const char *s, size_t n);

/* ── tokens ───────────────────────────────────────────────────────────── */

typedef enum jp_tok_kind {
	JP_TK_EOF,
	JP_TK_IDENT,
	JP_TK_NUMBER,
	JP_TK_STRING,
	JP_TK_CHAR,
	JP_TK_PUNCT,
	JP_TK_COMMENT,
	JP_TK_PP
} jp_tok_kind_ty;

typedef struct jp_token {
	jp_tok_kind_ty kind;
	size_t start;
	size_t end;
	unsigned int line;
	unsigned int col;
} jp_token_ty;

typedef struct jp_tokens {
	jp_token_ty *data;
	size_t len;
	size_t cap;
} jp_tokens_ty;

void jp_tokens_init(jp_tokens_ty *t);
void jp_tokens_free(jp_tokens_ty *t);
int jp_lex(const char *src, size_t len, jp_tokens_ty *out);

/* ── parsed declarations ──────────────────────────────────────────────── */

typedef struct jp_param {
	jp_span_ty span; /* full parameter declaration text */
	jp_span_ty name; /* identifier, end == start when absent */
	int is_ptr;
	int is_ptr_ptr;
	int is_const;
	int has_size_t;
	int is_variadic;
} jp_param_ty;

typedef struct jp_function {
	jp_span_ty full; /* whole top-level item */
	size_t attrs_begin;
	size_t attrs_end;
	size_t ret_begin;
	size_t ret_end;
	size_t name_idx;
	size_t lparen_idx;
	size_t rparen_idx;
	size_t lbrace_idx;
	size_t rbrace_idx;
	size_t end_off; /* insertion offset after any trailing conditionals */
	jp_param_ty *params;
	size_t nparams;
	int variadic;
} jp_function_ty;

enum {
	JP_DECL_NONE = 0,
	JP_DECL_FUNCTION = 1,
	JP_DECL_PROTOTYPE = 2
};

typedef struct jp_decl_result {
	int kind;
	jp_function_ty fn;
	size_t end;
} jp_decl_result_ty;

int jp_parse_function(const char *src, const jp_tokens_ty *toks, size_t start, jp_arena_ty *arena,
                      jp_decl_result_ty *out);

/* ── preprocessor helpers ─────────────────────────────────────────────── */

int jp_pp_is_n(const char *src, const jp_token_ty *tok, const char *word, size_t word_len);
#define jp_pp_is(src, tok, lit) jp_pp_is_n((src), (tok), JP_LIT(lit))
int jp_pp_undef_name(const char *src, const jp_token_ty *tok, jp_span_ty *name);

/* ── generators ───────────────────────────────────────────────────────── */

int jp_generate(const char *src, size_t len, jp_buf_ty *out);
int jp_namespace(const char *src, size_t len, jp_buf_ty *out);
int jp_read_file(const char *path, jp_buf_ty *out);

#endif /* JSTRGEN_H */
