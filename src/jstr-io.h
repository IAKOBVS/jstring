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
#include "jstr-io-table.h"

#define R JSTR_RESTRICT

PJSTR_BEGIN_DECLS

typedef enum {
	JSTR_IO_UNKNOWN = 0,
	JSTR_IO_TEXT,
	JSTR_IO_BINARY,
} jstr_io_ext_ty;

#define S switch (*ext++)
#define T  \
case '\0': \
	return JSTR_IO_TEXT
#define B  \
case '\0': \
	return JSTR_IO_BINARY
JSTR_WARN_UNUSED
JSTR_NONNULL_ALL
JSTR_NOINLINE
JSTR_PURE
JSTR_NOTHROW
static jstr_io_ext_ty
pjstr_io_ext_type(const char *R ext) JSTR_NOEXCEPT
{
	S {
	case 'a':
		S {
			B; /* a */
		}
		break;
	case 'b':
		S {
		case 'i':
			S {
			case 'n':
				S {
					B; /* bin */
				}
				break;
			}
			break;
		}
		break;
	case 'C':
		S {
			T; /* C */
		}
		break;
	case 'c':
		S {
			T; /* c */
		case 'c':
			S {
				T; /* cc */
			}
			break;
		case 'p':
			S {
			case 'p':
				S {
					T; /* cpp */
				}
			}
			break;
		case 's':
			S {
				T; /* cs */
			}
			break;
		}
		break;
	case 'm':
		S {
		case 'd':
			S {
				T; /* md */
			}
			break;
		case 'k':
			S {
			case 'v':
				S {
					B; /* mkv */
				}
				break;
			}
			break;
		}
		break;
	case 'g':
		S {
		case 'z':
			S {
				B; /* gz */
			}
			break;
		}
		break;
	case 'h':
		S {
			T; /* h */
		case 'h':
			S {
				T; /* hh */
			}
			break;
		case 'p':
			S {
			case 'p':
				S {
					T; /* hpp */
				}
				break;
			}
			break;
		case 't':
			S {
			case 'm':
				S {
				case 'l':
					S {
						T; /* html */
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 'j':
		S {
		case 'p':
			S {
			case 'g':
				S {
					B; /* jpg */
				}
				break;
			case 'e':
				S {
				case 'g':
					S {
						B; /* jpeg */
					}
					break;
				}
				break;
			}
			break;
		case 's':
			S {
				T; /* js */
			case 'o':
				S {
				case 'n':
					S {
						T; /* json */
					}
					break;
				}
				break;
			}
			break;
		}
		break;
	case 'o':
		S {
			B; /* o */
		}
		break;
	case 'p':
		S {
		case 'l':
			S {
				T; /* pl */
			}
			break;
		case 'm':
			S {
				T; /* pm */
			}
			break;
		case 'y':
			S {
				T; /* py */
			case 'i':
				S {
					T; /* pyi */
				}
				break;
			case 'c':
				S {
					B; /* pyc */
				}
				break;
			}
			break;
		case 'n':
			S {
			case 'g':
				S {
					B; /* png */
				}
				break;
			}
			break;
		case 'd':
			S {
			case 'f':
				S {
					B; /* pdf */
				}
				break;
			}
			break;
		}
		break;
	case 's':
		S {
		case 'h':
			S {
				T; /* sh */
			}
			break;
		case 'o':
			T; /* s */
			S {
				B; /* so */
			}
			break;
		}
		break;
	case 'r':
		S {
		case 's':
			S {
				T; /* rs */
			}
			break;
		}
		break;
	case 'S':
		S {
			T; /* S */
		}
		break;
	case 't':
		S {
		case 's':
			S {
				T; /* ts */
			}
			break;
		case 'x':
			S {
			case 't':
				S {
					T; /* txt */
				}
				break;
			}
			break;
		}
		break;
	case 'w':
		S {
		case 'a':
			S {
			case 'v':
				S {
					B; /* wav */
				}
				break;
			}
			break;
		}
		break;
	}
	return JSTR_IO_UNKNOWN;
}

#undef S
#undef T
#undef B

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
jstr_io_ext_type_len(const char *R filename,
		     const size_t sz) JSTR_NOEXCEPT
{
	filename = (char *)jstr_memrchr(filename, '.', sz);
	if (filename == NULL)
		return JSTR_IO_UNKNOWN;
	return pjstr_io_ext_type(filename + 1);
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
jstr_io_ext_type(const char *R filename) JSTR_NOEXCEPT
{
	filename = strrchr(filename, '.');
	if (filename == NULL)
		return JSTR_IO_UNKNOWN;
	return pjstr_io_ext_type(filename + 1);
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
jstr_io_is_binary_maybe(const char *R buf,
			const size_t sz) JSTR_NOEXCEPT
{
#define JSTR_BINARY_CHECK()                                                       \
	do {                                                                      \
		if (jstr_likely(sz > PJSTR_ELF_SZ - 1)) {                         \
			if (jstr_unlikely(!memcmp(buf, PJSTR_ELF, PJSTR_ELF_SZ))) \
				return 1;                                         \
check_utf:;                                                                       \
			unsigned char *R s = (unsigned char *)buf;                \
			if (!memcmp(s, PJSTR_UTF, PJSTR_UTF_SZ))                  \
				return 0;                                         \
		} else if (jstr_likely(sz == PJSTR_UTF_SZ)) {                     \
			goto check_utf;                                           \
		}                                                                 \
	} while (0)
	JSTR_BINARY_CHECK();
	return strcspn(buf, JSTR_UNPRINTABLE) != sz;
	const unsigned char *const end = (unsigned char *)buf + PJSTR_MAX(sz, 64) + 1;
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (pjstr_io_reject_table[*s++])
			return 1;
	return 0;
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
jstr_io_is_binary_maybe_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	return jstr_io_is_binary_maybe(j->data, j->size);
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
jstr_io_is_binary(const char *R const buf,
		  const size_t sz) JSTR_NOEXCEPT
{
	JSTR_BINARY_CHECK();
	return strlen(buf) != sz;
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
jstr_io_is_binary_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	return jstr_io_is_binary(j->data, j->size);
}

JSTR_INLINE
JSTR_NONNULL_ALL
JSTR_NOTHROW
static int
pjstr_io_alloc_file(const int alloc_exact,
		    char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const char *R const fname,
		    struct stat *R const st) JSTR_NOEXCEPT
{
	FILE *R const fp = fopen(fname, "r");
	if (jstr_unlikely(fp == NULL))
		goto err;
	if (jstr_unlikely(stat(fname, st)))
		goto err_close;
	*cap = alloc_exact
	       ? PJSTR_ALIGN_UP_STR(PJSTR_MIN_ALLOCEXACT(st->st_size + 1))
	       : PJSTR_ALIGN_UP_STR(PJSTR_MIN_ALLOC(st->st_size));
	*s = (char *)malloc(*cap);
	PJSTR_MALLOC_ERR(*s, goto err_close);
	fread(*s, 1, st->st_size, fp);
	fclose(fp);
	(*s)[st->st_size] = '\0';
	*sz = st->st_size;
	return 0;
err_close:
	fclose(fp);
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
jstr_io_alloc_file(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const fname,
		   struct stat *R const st) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file(0, s, sz, cap, fname, st);
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
jstr_io_allocexact_file(char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const char *R const fname,
			struct stat *R const st) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file(1, s, sz, cap, fname, st);
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
jstr_io_alloc_file_j(jstr_ty *R const j,
		     const char *R const fname,
		     struct stat *R const st) JSTR_NOEXCEPT
{
	return jstr_io_alloc_file(&j->data, &j->size, &j->capacity, fname, st);
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
jstr_io_allocexact_file_j(jstr_ty *R const j,
			  const char *R const fname,
			  struct stat *R const st) JSTR_NOEXCEPT
{
	return jstr_io_allocexact_file(&j->data, &j->size, &j->capacity, fname, st);
}

PJSTR_END_DECLS

#undef R

#endif /* JSTR_IO_H */
