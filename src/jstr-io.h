#ifndef JSTR_IO_H
#define JSTR_IO_H 1

#include "jstr-macros.h"

PJSTR_BEGIN_DECLS
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
PJSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-string.h"

PJSTR_BEGIN_DECLS

typedef enum {
	JSTR_IO_UNKNOWN = 0,
	JSTR_IO_TEXT,
	JSTR_IO_BINARY,
} jstr_io_ext_ty;

#define PJSTR_S switch (*_ext++)
#define PJSTR_T  \
case '\0': \
	return JSTR_IO_TEXT
#define PJSTR_B  \
case '\0': \
	return JSTR_IO_BINARY
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOINLINE
JSTR_PURE
JSTR_NOTHROW
static jstr_io_ext_ty
pjstr_io_ext_type(const char *JSTR_RST _ext) JSTR_NOEXCEPT
{
	PJSTR_S {
	case 'a':
		PJSTR_S {
			PJSTR_B; /* a */
		}
		break;
	case 'b':
		PJSTR_S {
		case 'i':
			PJSTR_S {
			case 'n':
				PJSTR_S {
					PJSTR_B; /* bin */
				}
				break;
			}
			break;
		}
		break;
	case 'C':
		PJSTR_S {
			PJSTR_T; /* C */
		}
		break;
	case 'c':
		PJSTR_S {
			PJSTR_T; /* c */
		case 'c':
			PJSTR_S {
				PJSTR_T; /* cc */
			}
			break;
		case 'p':
			PJSTR_S {
			case 'p':
				PJSTR_S {
					PJSTR_T; /* cpp */
				}
			}
			break;
		case 's':
			PJSTR_S {
				PJSTR_T; /* cs */
			}
			break;
		}
		break;
	case 'm':
		PJSTR_S {
		case 'd':
			PJSTR_S {
				PJSTR_T; /* md */
			}
			break;
		case 'k':
			PJSTR_S {
			case 'v':
				PJSTR_S {
					PJSTR_B; /* mkv */
				}
				break;
			}
			break;
		}
		break;
	case 'g':
		PJSTR_S {
		case 'z':
			PJSTR_S {
				PJSTR_B; /* gz */
			}
			break;
		}
		break;
	case 'h':
		PJSTR_S {
			PJSTR_T; /* h */
		case 'h':
			PJSTR_S {
				PJSTR_T; /* hh */
			}
			break;
		case 'p':
			PJSTR_S {
			case 'p':
				PJSTR_S {
					PJSTR_T; /* hpp */
				}
				break;
			}
			break;
		case 't':
			PJSTR_S {
			case 'm':
				PJSTR_S {
				case 'l':
					PJSTR_S {
						PJSTR_T; /* html */
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 'j':
		PJSTR_S {
		case 'p':
			PJSTR_S {
			case 'g':
				PJSTR_S {
					PJSTR_B; /* jpg */
				}
				break;
			case 'e':
				PJSTR_S {
				case 'g':
					PJSTR_S {
						PJSTR_B; /* jpeg */
					}
					break;
				}
				break;
			}
			break;
		case 's':
			PJSTR_S {
				PJSTR_T; /* js */
			case 'o':
				PJSTR_S {
				case 'n':
					PJSTR_S {
						PJSTR_T; /* json */
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 'o':
		PJSTR_S {
			PJSTR_B; /* o */
		}
		break;
	case 'p':
		PJSTR_S {
		case 'l':
			PJSTR_S {
				PJSTR_T; /* pl */
			}
			break;
		case 'm':
			PJSTR_S {
				PJSTR_T; /* pm */
			}
			break;
		case 'y':
			PJSTR_S {
				PJSTR_T; /* py */
			case 'i':
				PJSTR_S {
					PJSTR_T; /* pyi */
				}
				break;
			case 'c':
				PJSTR_S {
					PJSTR_B; /* pyc */
				}
				break;
			}
			break;
		case 'n':
			PJSTR_S {
			case 'g':
				PJSTR_S {
					PJSTR_B; /* png */
				}
				break;
			}
			break;
		case 'd':
			PJSTR_S {
			case 'f':
				PJSTR_S {
					PJSTR_B; /* pdf */
				}
				break;
			}
			break;
		}
		break;
	case 's':
		PJSTR_S {
		case 'h':
			PJSTR_S {
				PJSTR_T; /* sh */
			}
			break;
		case 'o':
			PJSTR_T; /* s */
			PJSTR_S {
				PJSTR_B; /* so */
			}
			break;
		}
		break;
	case 'r':
		PJSTR_S {
		case 's':
			PJSTR_S {
				PJSTR_T; /* rs */
			}
			break;
		}
		break;
	case 'PJSTR_S':
		PJSTR_S {
			PJSTR_T; /* PJSTR_S */
		}
		break;
	case 't':
		PJSTR_S {
		case 's':
			PJSTR_S {
				PJSTR_T; /* ts */
			}
			break;
		case 'x':
			PJSTR_S {
			case 't':
				PJSTR_S {
					PJSTR_T; /* txt */
				}
				break;
			}
			break;
		}
		break;
	case 'w':
		PJSTR_S {
		case 'a':
			PJSTR_S {
			case 'v':
				PJSTR_S {
					PJSTR_B; /* wav */
				}
				break;
			}
			break;
		}
		break;
	}
	return JSTR_IO_UNKNOWN;
}

#undef PJSTR_S
#undef PJSTR_T
#undef PJSTR_B

/*
   Returns jstr_io_ext_ty based on the filename extension;
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_NOTHROW
static jstr_io_ext_ty
jstr_io_ext_type_len(const char *JSTR_RST _filename,
		    const size_t _sz) JSTR_NOEXCEPT
{
	_filename = (char *)jstr_memrchr(_filename, '.', _sz);
	if (_filename == NULL)
		return JSTR_IO_UNKNOWN;
	return pjstr_io_ext_type(_filename + 1);
}

/*
   Returns jstr_io_ext_ty based on the filename extension;
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_INLINE
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_NOTHROW
static jstr_io_ext_ty
jstr_io_ext_type(const char *JSTR_RST _filename) JSTR_NOEXCEPT
{
	_filename = strrchr(_filename, '.');
	if (_filename == NULL)
		return JSTR_IO_UNKNOWN;
	return pjstr_io_ext_type(_filename + 1);
}

#define PJSTR_ELF    "\x7f\ELF"
#define PJSTR_ELF_SZ (sizeof("\x7f\ELF") - 1)
#define PJSTR_UTF    "\xEF\xBB\xBF"
#define PJSTR_UTF_SZ (sizeof("\xEF\xBB\xBF") - 1)

#define JSTR_UNPRINTABLE "\x01\x02\x03\x04\x05\x06\x07\x08\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x7F"

/*
   Checks if the first 32 bytes or fewer contain any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_io_is_binary_maybe(char *JSTR_RST const _buf,
		       const size_t _sz) JSTR_NOEXCEPT
{
#define JSTR_BINARY_CHECK()                                                        \
	do {                                                                       \
		if (jstr_likely(_sz > PJSTR_ELF_SZ - 1)) {                         \
			if (jstr_unlikely(!memcmp(_buf, PJSTR_ELF, PJSTR_ELF_SZ))) \
				return 1;                                          \
check_utf:;                                                                        \
			unsigned char *JSTR_RST _s = (unsigned char *)_buf;        \
			if (!memcmp(_s, PJSTR_UTF, PJSTR_UTF_SZ))                  \
				return 0;                                          \
		} else if (jstr_likely(_sz == PJSTR_UTF_SZ)) {                     \
			goto check_utf;                                            \
		}                                                                  \
	} while (0)
	JSTR_BINARY_CHECK();
	if (jstr_likely(_sz > 32)) {
		const char old = *(_buf + _sz);
		*(_buf + 32) = '\0';
		const int ret = strcspn(_buf, JSTR_UNPRINTABLE) != 32;
		*(_buf + 32) = old;
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
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_io_is_binary_maybe_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	return jstr_io_is_binary_maybe(_j->data, _j->size);
}

/*
   Checks the whole file for a NUL byte.
*/
JSTR_NONNULL_ALL
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_PURE
JSTR_NOTHROW
static int
jstr_io_is_binary(const char *JSTR_RST const _buf,
		 const size_t _sz) JSTR_NOEXCEPT
{
	JSTR_BINARY_CHECK();
	return strlen(_buf) != _sz;
}

#undef JSTR_BINARY_CHECK
#undef PJSTR_ELF
#undef PJSTR_ELF_SZ
#undef PJSTR_UTF
#undef PJSTR_UTF_SZ
#undef JSTR_UNPRINTABLE

/*
   Checks the whole file for any unprintable character.
   File must be nul terminated.
*/
JSTR_NONNULL_ALL
JSTR_INLINE
JSTR_PURE
JSTR_WARN_UNUSED
JSTR_NOTHROW
static int
jstr_io_is_binary_j(jstr_ty *JSTR_RST const _j) JSTR_NOEXCEPT
{
	return jstr_io_is_binary(_j->data, _j->size);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
pjstr_io_alloc_file(const int alloc_exact,
		   char **JSTR_RST const _s,
		   size_t *JSTR_RST const _sz,
		   size_t *JSTR_RST const _cap,
		   const char *JSTR_RST const _fname,
		   struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	FILE *JSTR_RST const _fp = fopen(_fname, "r");
	if (jstr_unlikely(_fp == NULL))
		goto err;
	if (jstr_unlikely(stat(_fname, _st)))
		goto err_close;
	*_cap = alloc_exact
		? PJSTR_ALIGN_UP_STR(PJSTR_MIN_ALLOCEXACT(_st->st_size + 1))
		: PJSTR_ALIGN_UP_STR(PJSTR_MIN_ALLOC(_st->st_size));
	*_s = (char *)malloc(*_cap);
	PJSTR_MALLOC_ERR(*_s, goto err_close);
	fread(*_s, 1, _st->st_size, _fp);
	fclose(_fp);
	*(*_s + _st->st_size) = '\0';
	*_sz = _st->st_size;
	return 0;
err_close:
	fclose(_fp);
err:
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
JSTR_NOTHROW
static int
jstr_io_alloc_file(char **JSTR_RST const _s,
		  size_t *JSTR_RST const _sz,
		  size_t *JSTR_RST const _cap,
		  const char *JSTR_RST const _fname,
		  struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file(0, _s, _sz, _cap, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
jstr_io_allocexact_file(char **JSTR_RST const _s,
		       size_t *JSTR_RST const _sz,
		       size_t *JSTR_RST const _cap,
		       const char *JSTR_RST const _fname,
		       struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file(1, _s, _sz, _cap, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
jstr_io_alloc_file_j(jstr_ty *JSTR_RST const _j,
		    const char *JSTR_RST const _fname,
		    struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return jstr_io_alloc_file(&_j->data, &_j->size, &_j->capacity, _fname, _st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_MAYBE_UNUSED
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
jstr_io_allocexact_file_j(jstr_ty *JSTR_RST const _j,
			 const char *JSTR_RST const _fname,
			 struct stat *JSTR_RST const _st) JSTR_NOEXCEPT
{
	return jstr_io_allocexact_file(&_j->data, &_j->size, &_j->capacity, _fname, _st);
}

PJSTR_END_DECLS

#endif /* JSTR_IO_H */
