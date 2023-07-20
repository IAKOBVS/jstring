#define _GNU_SOURCE

#include "jstr.h"

#include <sys/stat.h>

#define RST    JSTR_RESTRICT
#define ERR(p) (unlikely(p) && (exit(1), 0))

void read(jstr_t *j,
	  const char *RST fname)
{
	FILE *fp = fopen(fname, "r");
	ERR(!fp);
	struct stat st;
	stat(fname, &st);
	j->size = st.st_size;
	j->cap = j->size + 1;
	j->data = (char *)malloc(j->cap);
	ERR(!j->data);
	fread(j->data, 1, j->size, fp);
	fclose(fp);
}

typedef struct fmatch_t {
	size_t bref_so;
	size_t bref_eo;
} fmatch_t;

#define lswitch  \
	for (;;) \
		switch

#define next(state_)            \
	do {                    \
		state = state_; \
		goto state_;    \
	} while (0)

int get_args(fmatch_t *RST rm,
	     const char *const sstart,
	     const char *s)
{
	enum {
		START = 0,
		BRACKET,
		NAME,
		COMMA_OR,
	};
	const char *last;
	int state = START;
	switch (state) {
	case START:
	START:
		lswitch(*s++)
		{
		case '(':
			next(BRACKET);
			break;
		case '{':
		case '\0':
			goto BAD;
		}
		break;
	case BRACKET:
	BRACKET:
		lswitch(*s++)
		{
			JSTR_CASE_ALPHANUM
		case ' ':
		case '\t':
		case '_':
			break;
		case ',':
		case ')':
			next(NAME);
		default:
		case '\0':
			goto BAD;
		}
		break;
	case NAME:
	NAME:
		last = s;
		--s;
		rm->bref_eo = s - sstart;
		while (s != sstart)
			switch (*s--) {
			case '{':
			case '\0':
				goto BAD;
				JSTR_CASE_ALPHANUM
			case '_':
				continue;
			default:
				s = last;
				next(START);
			case ' ':
			case '\n':
			case '\t':
			case '(':
				goto END;
			}
		break;
	}
END:
	rm->bref_so = s + 1 - sstart;
	return 1;
BAD:
	return 0;
}

typedef struct move_t {
	char *curr;
	char *next;
} move_t;

int main()
{
	jstr_t j;
	read(&j, "jstr.h");
	char *sv = j.data;
	fmatch_t f;
	for (char *tok;;) {
		tok = strstr(tok, "static");
		if (!tok)
			continue;
		tok += sizeof("static") - 1;
		f.bref_eo = 0;
		while (get_args(&f, tok, tok + f.bref_eo)) {
			fwrite(tok + f.bref_so, 1, f.bref_eo - f.bref_so, stdout);
		}
	}
	free(j.data);
	return 0;
}
