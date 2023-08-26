#ifndef JSTRIO_H_DEF
#define JSTRIO_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_builder.h"
#include "_macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

typedef enum {
	JSTRIO_UNKNOWN = 0,
	JSTRIO_TEXT,
	JSTRIO_BINARY,
} jstrio_ext_ty;

JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOINLINE
static jstrio_ext_ty
priv_jstrio_ext_type(const char *JSTR_RST _ext) JSTR_NOEXCEPT
{
	switch (*_ext++) {
	/* a : BINARY */
	case 'a':
		switch (*_ext) {
		/* a */
		case '\0': return JSTRIO_BINARY;
		}
		break;
	/* bin : BINARY */
	case 'b':
		switch (*_ext++) {
		case 'i':
			switch (*_ext++) {
			case 'n':
				switch (*_ext) {
				/* bin */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			}
			break;
		}
		break;
	/* C : TEXT */
	case 'C':
		switch (*_ext) {
		case '\0': return JSTRIO_TEXT;
		}
		break;
	/* c, cc, cpp, cs : TEXT */
	case 'c':
		switch (*_ext++) {
		/* c */
		case '\0': return JSTRIO_TEXT;
		case 'c':
			switch (*_ext) {
			/* cc */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'p':
			switch (*_ext++) {
			case 'p':
				switch (*_ext) {
				/* cpp */
				case '\0': return JSTRIO_TEXT;
				}
			}
			break;
		case 's':
			switch (*_ext) {
			/* cs */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		}
		break;
	/* md : TEXT */
	case 'm':
		switch (*_ext++) {
		case 'd':
			switch (*_ext) {
			/* md */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'k':
			switch (*_ext++) {
			case 'k':
				switch (*_ext++) {
				case 'v':
					switch (*_ext) {
					case '\0': return JSTRIO_BINARY;
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	/* gz : BINARY */
	case 'g':
		switch (*_ext++) {
		case 'z':
			switch (*_ext) {
			/* gz */
			case '\0': return JSTRIO_BINARY;
			}
			break;
		}
		break;
	/* h, hh, hpp, html : TEXT */
	case 'h':
		switch (*_ext++) {
		/* h */
		case '\0': return JSTRIO_TEXT;
		case 'h':
			switch (*_ext) {
			/* hh */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'p':
			switch (*_ext++) {
			case 'p':
				switch (*_ext) {
				/* .hpp */
				case '\0': return JSTRIO_TEXT;
				}
				break;
			}
			break;
		case 't':
			switch (*_ext++) {
			case 'm':
				switch (*_ext++) {
				case 'l':
					switch (*_ext++) {
					/* html */
					case '\0': return JSTRIO_TEXT;
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	/* jpg, jpeg : BINARY
	   js, json : TEXT */
	case 'j':
		switch (*_ext++) {
		case 'p':
			switch (*_ext++) {
			case 'g':
				switch (*_ext) {
				/* jpg */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			case 'e':
				switch (*_ext++) {
				case 'g':
					switch (*_ext) {
					/* jpeg */
					case '\0': return JSTRIO_BINARY;
					}
					break;
				}
				break;
			}
			break;
		case 's':
			switch (*_ext++) {
			/* js */
			case '\0': return JSTRIO_TEXT;
			case 'o':
				switch (*_ext++) {
				case 'n':
					switch (*_ext) {
					/* json */
					case '\0': return JSTRIO_TEXT;
					}
					break;
				}
				break;
			}
		}
		break;
	/* o : BINARY */
	case 'o':
		switch (*_ext++) {
		/* o */
		case '\0': return JSTRIO_BINARY;
		}
		break;
	/* png, pyc, pdf : BINARY
	   pl, pm, py, pyi : TEXT */
	case 'p':
		switch (*_ext++) {
		case 'l':
			switch (*_ext) {
			/* pl */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'm':
			switch (*_ext) {
			/* pm */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'y':
			switch (*_ext++) {
			case 'i':
				switch (*_ext) {
				/* pyi */
				case '\0': return JSTRIO_TEXT;
				}
				break;
			case 'c':
				switch (*_ext) {
				/* pyc */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			/* py */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'n':
			switch (*_ext++) {
			case 'g':
				switch (*_ext) {
				/* png */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			}
			break;
		case 'd':
			switch (*_ext++) {
			case 'f':
				switch (*_ext) {
				/* pdf */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			}
			break;
		}
		break;
	/* so : BINARY
	   sh, s : TEXT */
	case 's':
		switch (*_ext++) {
		case 'h':
			switch (*_ext) {
			/* .sh */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'o':
			switch (*_ext) {
			/* .so */
			case '\0': return JSTRIO_BINARY;
			}
			break;
		/* s */
		case '\0': return JSTRIO_TEXT;
		}
		break;
	/* rs : TEXT */
	case 'r':
		switch (*_ext++) {
		case 's':
			switch (*_ext) {
			/* rs */
			case '\0': return JSTRIO_TEXT;
			}
			break;
		}
		break;
	/* S : TEXT */
	case 'S':
		switch (*_ext) {
		/* S */
		case '\0': return JSTRIO_TEXT;
		}
		break;
	/* txt : TEXT */
	case 't':
		switch (*_ext++) {
		case 's':
			switch (*_ext) {
			case '\0': return JSTRIO_TEXT;
			}
			break;
		case 'x':
			switch (*_ext++) {
			case 't':
				switch (*_ext) {
				/* txt */
				case '\0': return JSTRIO_TEXT;
				}
				break;
			}
			break;
		}
		break;
	/* wav : BINARY */
	case 'w':
		switch (*_ext++) {
		case 'a':
			switch (*_ext++) {
			case 'v':
				switch (*_ext) {
				/* wav */
				case '\0': return JSTRIO_BINARY;
				}
				break;
			}
			break;
		}
		break;
	}
	return JSTRIO_UNKNOWN;
}

#if JSTR_HAVE_MEMRCHR

/*
   Returns jstrio_ext_ty based on the filename extension;
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static jstrio_ext_ty
jstrio_ext_type_mem(const char *JSTR_RST _filename,
		    const size_t _sz) JSTR_NOEXCEPT
{
	_filename = (char *)memrchr(_filename, '.', _sz);
	if (_filename == NULL)
		return JSTRIO_UNKNOWN;
	return priv_jstrio_ext_type(_filename + 1);
}

#endif /* HAVE_MEMRCHR */

/*
   Returns jstrio_ext_ty based on the filename extension;
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
static jstrio_ext_ty
jstrio_ext_type(const char *JSTR_RST _filename) JSTR_NOEXCEPT
{
	_filename = strrchr(_filename, '.');
	if (_filename == NULL)
		return JSTRIO_UNKNOWN;
	return priv_jstrio_ext_type(_filename + 1);
}

#define JSTR_ELF    "\x7ELF"
#define JSTR_ELF_SZ (sizeof("\x7ELF") - 1)
#define JSTR_UTF    "\xEF\xBB\xBF"
#define JSTR_UTF_SZ (sizeof("\xEF\xBB\xBF") - 1)

/* Exclude \r on Windows */
#ifdef _WIN32
#	define JSTR_UNPRINTABLE "\x01\x02\x03\x04\x05\x06\x07\x08\x0B\x0C\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
#else
#	define JSTR_UNPRINTABLE "\x01\x02\x03\x04\x05\x06\x07\x08\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
#endif /* _WIN32 */

/*
   Checks if the first 32 bytes or fewer contain any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static int
jstrio_is_binary_maybe(char *JSTR_RST const _buf,
		       const size_t _sz) JSTR_NOEXCEPT
{
#define JSTR_BINARY_CHECK()                                                      \
	do {                                                                     \
		if (jstr_likely(_sz > JSTR_ELF_SZ - 1)) {                        \
			if (jstr_unlikely(!memcmp(_buf, JSTR_ELF, JSTR_ELF_SZ))) \
				return 1;                                        \
CHECK_UTF:;                                                                      \
			unsigned char *JSTR_RST _s = (unsigned char *)_buf;      \
			if (!memcmp(_s, JSTR_UTF, JSTR_UTF_SZ))                  \
				return 0;                                        \
		} else if (jstr_likely(_sz == JSTR_UTF_SZ)) {                    \
			goto CHECK_UTF;                                          \
		}                                                                \
	} while (0)
	JSTR_BINARY_CHECK();
	if (jstr_likely(_sz > 32)) {
		const char old = *(_buf + _sz);
		*(_buf + _sz) = '\0';
		const int ret = strcspn(_buf, JSTR_UNPRINTABLE) != 32;
		*(_buf + _sz) = old;
		return ret;
	}
	return strcspn(_buf, JSTR_UNPRINTABLE) != _sz;
}

/*
   Checks if the first 32 bytes or fewer contain any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static int
jstrio_is_binary_maybe_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	return jstrio_is_binary_maybe(_j->data, _j->size);
}

/*
   Checks the whole file for any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static int
jstrio_is_binary(const char *JSTR_RST const _buf,
		 const size_t _sz) JSTR_NOEXCEPT
{
	JSTR_BINARY_CHECK();
	return strcspn(_buf, JSTR_UNPRINTABLE) == _sz;
}

#undef JSTR_ELF
#undef JSTR_ELF_SZ
#undef JSTR_UTF
#undef JSTR_UTF_SZ
#undef JSTR_UNPRINTABLE

/*
   Checks the whole file for any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
static int
jstrio_is_binary_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	return jstrio_is_binary(_j->data, _j->size);
}

JSTR_INLINE
JSTR_NONNULL_ALL
static int
priv_jstrio_alloc_file(const int alloc_exact,
		       char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const char *JSTR_RST const _fname,
		       struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	FILE *JSTR_RST const _fp = fopen(_fname, "r");
	if (jstr_unlikely(_fp == NULL))
		goto _ERR;
	if (jstr_unlikely(stat(_fname, _st)))
		goto _ERR_CLOSE;
	*_cap = alloc_exact ? _st->st_size : _st->st_size * 2;
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, goto _ERR_CLOSE);
	fread(*_s, 1, _st->st_size, _fp);
	fclose(_fp);
	*(*_s + _st->st_size) = '\0';
	*_sz = _st->st_size;
	return 0;
_ERR_CLOSE:
	fclose(_fp);
_ERR:
	perror("");
	return 1;
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static int
jstrio_alloc_file(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _fname,
		  struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return priv_jstrio_alloc_file(0, _s, _sz, _cap, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static int
jstrio_allocexact_file(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const char *JSTR_RST const _fname,
		       struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return priv_jstrio_alloc_file(1, _s, _sz, _cap, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static int
jstrio_alloc_file_j(jstr_ty *JSTR_RST const _j,
		    const char *JSTR_RST const _fname,
		    struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return jstrio_alloc_file(&_j->data, &_j->size, &_j->cap, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
static int
jstrio_allocexact_file_j(jstr_ty *JSTR_RST const _j,
			 const char *JSTR_RST const _fname,
			 struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return jstrio_allocexact_file(&_j->data, &_j->size, &_j->cap, _fname, _st);
}

#ifdef __cplusplus
}
#endif /* __cpluslus */

#endif /* JSTRIO_H_DEF */
