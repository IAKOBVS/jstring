#ifndef JSTR_IO_H
#define JSTR_IO_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
P_JSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-io-table.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

typedef enum {
	JSTR_IO_FT_UNKNOWN = 0,
	JSTR_IO_FT_TEXT,
	JSTR_IO_FT_BINARY
} jstr_io_ext_ty;

#define S switch (*ext++)
#define T  \
case '\0': \
	return JSTR_IO_FT_TEXT
#define B  \
case '\0': \
	return JSTR_IO_FT_BINARY
JSTR_FUNC_PURE
JSTR_NOINLINE
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
	return JSTR_IO_FT_UNKNOWN;
}

#undef S
#undef T
#undef B

/*
   Return jstr_io_ext_ty based on the FNAME extension;
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_io_ext_ty
jstr_io_ext_type_len(const char *R fname,
		     const size_t sz) JSTR_NOEXCEPT
{
	fname = (char *)jstr_memrchr(fname, '.', sz);
	return fname ? pjstr_io_ext_type(fname + 1) : JSTR_IO_FT_UNKNOWN;
}

/*
   Return jstr_io_ext_ty based on the FNAME extension;
*/
JSTR_INLINE
JSTR_FUNC_PURE
static jstr_io_ext_ty
jstr_io_ext_type(const char *R fname) JSTR_NOEXCEPT
{
	fname = strrchr(fname, '.');
	return fname ? pjstr_io_ext_type(fname + 1) : JSTR_IO_FT_UNKNOWN;
}

/*
   Check MIN(N, SZ) bytes for any unprintable character.
*/
JSTR_FUNC
static int
jstr_isbinary(const char *R const buf,
	      const size_t n,
	      const size_t sz) JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const unsigned char *const end = (unsigned char *)buf + JSTR_MIN(n, sz);
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (pjstr_io_reject_table[*s++])
			return 1;
	return 0;
}

#define P_JSTR_ELF    "\x7f\ELF"
#define P_JSTR_ELF_SZ (sizeof("\x7f\ELF") - 1)
#define P_JSTR_UTF    "\xEF\xBB\xBF"
#define P_JSTR_UTF_SZ (sizeof("\xEF\xBB\xBF") - 1)

/*
   Check if the first 64 bytes or fewer contain any unprintable character.
*/
JSTR_FUNC
static int
jstr_io_isbinary_maybe(const char *R const buf,
		       const size_t sz) JSTR_NOEXCEPT
{
#define JSTR_BINARY_CHECK()                                                         \
	do {                                                                        \
		if (jstr_likely(sz > P_JSTR_ELF_SZ - 1)) {                          \
			if (jstr_unlikely(!memcmp(buf, P_JSTR_ELF, P_JSTR_ELF_SZ))) \
				return 1;                                           \
check_utf:;                                                                         \
			if (!memcmp(buf, P_JSTR_UTF, P_JSTR_UTF_SZ))                \
				return 0;                                           \
		} else if (jstr_likely(sz == P_JSTR_UTF_SZ)) {                      \
			goto check_utf;                                             \
		}                                                                   \
	} while (0)
	if (jstr_unlikely(sz == 0))
		return 0;
	JSTR_BINARY_CHECK();
	const unsigned char *const end = (unsigned char *)buf + JSTR_MIN(sz, 64) + 1;
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (pjstr_io_reject_table[*s++])
			return 1;
	return 0;
}

/*
   Check if the first 32 bytes or fewer contain any unprintable character.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_io_isbinary_maybe_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	return jstr_io_isbinary_maybe(j->data, j->size);
}

/*
   Check the whole file for a NUL byte.
   File must be nul terminated.
*/
JSTR_FUNC_PURE
static int
jstr_io_isbinary(const char *R const buf,
		 const size_t sz) JSTR_NOEXCEPT
{
	JSTR_BINARY_CHECK();
	return strlen(buf) != sz;
}

#undef JSTR_BINARY_CHECK
#undef P_JSTR_ELF
#undef P_JSTR_ELF_SZ
#undef P_JSTR_UTF
#undef P_JSTR_UTF_SZ

/*
   Check the whole file for any unprintable character.
   File must be nul terminated.
*/
JSTR_FUNC_PURE
JSTR_INLINE
static int
jstr_io_isbinary_j(jstr_ty *R const j) JSTR_NOEXCEPT
{
	return jstr_io_isbinary(j->data, j->size);
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwrite(const char *R const s,
	       const size_t sz,
	       FILE *R const fp) JSTR_NOEXCEPT
{
	return fwrite(s, 1, sz, fp) != sz;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwrite_j(const jstr_ty *R const j,
		 FILE *R const fp) JSTR_NOEXCEPT
{
	return jstr_io_fwrite(j->data, j->size, fp);
}

JSTR_FUNC
static int
jstr_io_write_file(const char *R const s,
		   const size_t sz,
		   const char *R const fname,
		   const int oflag) JSTR_NOEXCEPT
{
	const int fd = open(fname, oflag | O_WRONLY);
	if (jstr_unlikely(fd == -1))
		return 0;
	if (jstr_unlikely((size_t)write(fd, s, sz) != sz))
		goto err;
	close(fd);
	return 1;
err:
	close(fd);
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_write_file_j(const jstr_ty *R const j,
		     const char *R const fname,
		     const int oflag) JSTR_NOEXCEPT
{
	return jstr_io_write_file(j->data, j->size, fname, oflag);
}

JSTR_FUNC
static int
jstr_io_fwrite_file(const char *R const s,
		    const size_t sz,
		    const char *R const fname,
		    const char *R const modes) JSTR_NOEXCEPT
{
	FILE *R const fp = fopen(fname, modes);
	if (jstr_unlikely(fp == NULL))
		return 0;
	if (jstr_unlikely(!jstr_io_fwrite(s, sz, fp)))
		goto err;
	fclose(fp);
	return 1;
err:
	fclose(fp);
	return 0;
}

JSTR_FUNC
JSTR_INLINE
static int
jstr_io_fwrite_file_j(const jstr_ty *R const j,
		      const char *R const fname,
		      const char *R const modes) JSTR_NOEXCEPT
{
	return jstr_io_fwrite_file(j->data, j->size, fname, modes);
}

#if JSTR_HAVE_POPEN

#	if 0 /* broken */

JSTR_FUNC
static int
jstr_io_alloc_popen(char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const char *R const cmd) JSTR_NOEXCEPT
{
	FILE *R const fp = popen(cmd, "r");
	if (jstr_unlikely(fp == NULL))
		goto err;
	char *p;
	enum { MINBUF = BUFSIZ };
	char buf[MINBUF];
	p = buf;
	int c;
	while (((c = getc(fp)) != EOF)
	       & (p - buf != MINBUF))
		*p++ = c;
	*cap = P_JSTR_MIN_ALLOC(p - buf);
	*cap = JSTR_ALIGN_UP_STR(*cap);
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, goto err_close);
	memcpy(*s, buf, p - buf);
	if (jstr_unlikely(p - buf == MINBUF)) {
		p = *s + (p - buf);
		const char *old;
		while ((c = getc(fp)) != EOF) {
			if (jstr_unlikely((size_t)(p - *s) == *cap)) {
				old = *s;
				P_JSTR_REALLOCEXACT(*s, *cap, (size_t)(*cap * JSTR_GROWTH), goto err_close);
				p = *s + (p - old);
			}
			*p++ = c;
		}
		*p = '\0';
		*sz = p - *s;
	} else {
		(*s)[p - buf] = '\0';
		*sz = p - buf;
	}
	pclose(fp);
	return 1;
err_close:
	pclose(fp);
err:
	return 0;
}

#	else

JSTR_FUNC
static int
jstr_io_alloc_popen(char *R *R const s,
		    size_t *R const sz,
		    size_t *R const cap,
		    const char *R const cmd) JSTR_NOEXCEPT
{
	FILE *R const fp = popen(cmd, "r");
	if (jstr_unlikely(fp == NULL))
		goto err;
	char *p;
	enum { MINBUF = JSTR_MAX_STACK_ARRAY };
	char buf[MINBUF];
	p = buf + fread(buf, 1, MINBUF, fp);
	if (jstr_unlikely(ferror(fp)))
		goto err_close;
	*cap = P_JSTR_MIN_ALLOC(p - buf);
	*cap = JSTR_ALIGN_UP_STR(*cap);
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, goto err_close);
	memcpy(*s, buf, p - buf);
	*sz = p - buf;
	if (jstr_unlikely(p - buf == MINBUF)) {
		p = *s + (p - buf);
		const char *old;
		size_t req_size;
		for (;;) {
			req_size = *cap - *sz;
			old = p;
			p += fread(p, 1, req_size, fp);
			*sz += (p - old);
			if ((size_t)(p - old) < req_size)
				break;
			if ((size_t)(p - *s) == *cap) {
				old = *s;
				P_JSTR_REALLOCEXACT(*s, *cap, (size_t)(*cap * JSTR_GROWTH), goto err_close);
				p = *s + (p - old);
			}
		}
		if (jstr_unlikely(ferror(fp)))
			goto err_close_free;
		*p = '\0';
	} else {
		(*s)[p - buf] = '\0';
	}
	pclose(fp);
	return 1;
err_close_free:
	free(*s);
err_close:
	pclose(fp);
err:
	return 0;
}

#	endif

#endif

JSTR_INLINE
JSTR_FUNC
static int
pjstr_io_alloc_file_len(const int alloc_exact,
			char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const char *R const fname,
			const size_t filesz) JSTR_NOEXCEPT
{
	const int fd = open(fname, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	if (alloc_exact)
		*cap = P_JSTR_MIN_ALLOCEXACT(filesz + 1);
	else
		*cap = P_JSTR_MIN_ALLOC(filesz);
	*cap = JSTR_ALIGN_UP_STR(*cap);
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, goto err_close);
	if (jstr_unlikely(filesz != (size_t)read(fd, *s, filesz)))
		goto err_close_free;
	close(fd);
	(*s)[filesz] = '\0';
	return 1;
err_close_free:
	free(*s);
err_close:
	close(fd);
err:
	return 0;
}

/*
   Return value:
   0 on error;
   otherwise 1.
*/
JSTR_FUNC
static int
jstr_io_alloc_file(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const fname,
		   const size_t filesz) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file_len(0, s, sz, cap, fname, filesz);
}

/*
   Return value:
   0 on error;
   otherwise 1.
*/
JSTR_FUNC
static int
jstr_io_allocexact_file(char *R *R const s,
			size_t *R const sz,
			size_t *R const cap,
			const char *R const fname,
			const size_t filesz) JSTR_NOEXCEPT
{
	return pjstr_io_alloc_file_len(0, s, sz, cap, fname, filesz);
}

/*
   Return value:
   0 on error;
   otherwise 1.
*/
JSTR_FUNC
static int
jstr_io_alloc_file_len(char *R *R const s,
		       size_t *R const sz,
		       size_t *R const cap,
		       const char *R const fname,
		       struct stat *R st) JSTR_NOEXCEPT
{
	if (jstr_unlikely(stat(fname, st)))
		return 0;
	return jstr_io_alloc_file(s, sz, cap, fname, st->st_size);
}

/*
   Return value:
   0 on error;
   otherwise 1.
*/
JSTR_FUNC
static int
jstr_io_allocexact_file_len(char *R *R const s,
			    size_t *R const sz,
			    size_t *R const cap,
			    const char *R const fname,
			    struct stat *R st) JSTR_NOEXCEPT
{
	if (jstr_unlikely(stat(fname, st)))
		return 0;
	return jstr_io_allocexact_file(s, sz, cap, fname, st->st_size);
}

/*
   Expand ~/some_dir to /home/username/some_dir.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error.
*/
JSTR_FUNC
static char *
jstr_io_expand_tilde_first(char *R s,
			   const size_t sz)
{
	if (*s != '~')
		return s + sz;
	const char *R const home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	memmove(s + len, s + 1, (s + sz) - (s + 1) + 1);
	memcpy(s, home, len);
	return s + sz + len - 1;
}

/*
   Expand every ~ to /home/username.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S;
   NULL on error.
*/
JSTR_FUNC
static char *
jstr_io_expand_tilde_p_unsafe(char *R s,
			      size_t sz)
{
	const char *R const home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	char *p = s;
	while ((p = (char *)memchr(p, '~', (s + sz) - p))) {
		memmove(p + len, p + 1, (s + sz) - (p + 1) + 1);
		memcpy(p, home, len);
		p += len;
		sz += (len - 1);
	}
	return s + sz;
}

/*
   Expand every ~ to /home/username.
   Return value:
   0 on error;
   otherwise 1.
*/
JSTR_FUNC
static int
jstr_io_expand_tilde(char *R *R s,
		     size_t *R sz,
		     size_t *R cap)
{
	const char *R const home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return 0;
	const size_t len = strlen(home);
	const char *tmp;
	char *p = *s;
	while ((p = (char *)memchr(p, '~', (*s + *sz) - p))) {
		if (jstr_unlikely(*sz + len >= *cap)) {
			tmp = *s;
			P_JSTR_REALLOC(*s, *cap, *sz + len, goto err);
			p = *s + (p - tmp);
		}
		memmove(p + len, p + 1, (*s + *sz) - (p + 1) + 1);
		memcpy(p, home, len);
		p += len;
		*sz += (len - 1);
	}
	return 1;
err:
	P_JSTR_NULLIFY_MEMBERS(*sz, *cap);
	return 0;
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstr_io_append_path_p(char *R const path_end,
		      const char *R fname) JSTR_NOEXCEPT
{
	*path_end = '/';
	return jstr_stpcpy(path_end + 1, fname);
}

JSTR_INLINE
JSTR_FUNC_NOWARN
static void
jstr_io_append_path(char *R const path_end,
		    const char *R fname) JSTR_NOEXCEPT
{
	*path_end = '/';
	strcpy(path_end + 1, fname);
}

JSTR_INLINE
JSTR_FUNC_NOWARN
static void
jstr_io_append_path_len(char *R const path_end,
			const char *R fname,
			const size_t flen) JSTR_NOEXCEPT
{
	*path_end = '/';
	jstr_strcpy_len(path_end + 1, fname, flen);
}

#ifdef _DIRENT_HAVE_D_NAMLEN
#	ifndef _D_EXACT_NAMLEN
#		define _D_EXACT_NAMLEN(d) ((d)->d_namlen)
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		define _D_ALLOC_NAMLEN(d) (_D_EXACT_NAMLEN(d) + 1)
#	endif
#else
#	ifndef _D_EXACT_NAMLEN
#		define _D_EXACT_NAMLEN(d) (strlen((d)->d_name))
#	endif
#	ifndef _D_ALLOC_NAMLEN
#		ifdef _DIRENT_HAVE_D_RECLEN
#			define _D_ALLOC_NAMLEN(d) (((char *)(d) + (d)->d_reclen) - &(d)->d_name[0])
#		else
#			define _D_ALLOC_NAMLEN(d) (sizeof(d)->d_name > 1 ? sizeof(d)->d_name \
									  : _D_EXACT_NAMLEN(d) + 1)
#		endif
#	endif
#endif

enum {
#ifdef PATH_MAX
	JSTR_IO_PATH_MAX = PATH_MAX,
#else
	JSTR_IO_PATH_MAX = 4096,
#endif
#ifdef NAME_MAX
	JSTR_IO_NAME_MAX = NAME_MAX
#else
	JSTR_IO_NAME_MAX = 255
#endif
};

typedef enum jstr_io_ftw_flag_ty {
	/* Match glob with the fname instead of the whole path. */
	JSTR_IO_FTW_MATCHPATH = (1),
	/* Call FN on regular files. */
	JSTR_IO_FTW_REG = (JSTR_IO_FTW_MATCHPATH << 1),
	/* Call FN on directories. */
	JSTR_IO_FTW_DIR = (JSTR_IO_FTW_REG << 1),
	/* Do not traverse subdirectories. */
	JSTR_IO_FTW_NOSUBDIR = (JSTR_IO_FTW_DIR << 1),
	/* Do not call stat. Only st.st_mode is defined. */
	JSTR_IO_FTW_NOSTAT = (JSTR_IO_FTW_NOSUBDIR << 1),
	/* Only call stat on regular files. */
	JSTR_IO_FTW_STATREG = (JSTR_IO_FTW_NOSTAT << 1),
	/* Ignore hidden entries. */
	JSTR_IO_FTW_NOHIDDEN = (JSTR_IO_FTW_STATREG << 1),
	/* Expand ~/ to /home/username/ if the first char is '~'. */
	JSTR_IO_FTW_EXPTILDE = (JSTR_IO_FTW_NOHIDDEN << 1),
} jstr_io_ftw_flag_ty;

#if JSTR_HAVE_DIRENT_D_NAMLEN
#	define FILL_PATH_ALWAYS()                                                         \
		do {                                                                       \
			jstr_io_append_path_len(dirpath + dlen, ep->d_name, ep->d_namlen); \
			pathlen = dlen + 1 + ep->d_namlen;                                 \
		} while (0)
#else
#	define FILL_PATH_ALWAYS()                                                             \
		do {                                                                           \
			pathlen = jstr_io_append_path_p(dirpath + dlen, ep->d_name) - dirpath; \
		} while (0)
#endif

#if JSTR_HAVE_DIRENT_D_TYPE
#	define FILL_PATH() FILL_PATH_ALWAYS()
#	if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#		define STAT()                                                      \
			do {                                                        \
				if (jstr_unlikely(fstatat(fd, ep->d_name, &st, 0))) \
					continue;                                   \
			} while (0)
#	else
#		define STAT()                                         \
			do {                                           \
				if (jstr_unlikely(stat(dirpath, &st))) \
					continue;                      \
			} while (0)
#	endif
#	define STAT_MODE()                              \
		do {                                     \
			st.st_mode = DTTOIF(ep->d_type); \
		} while (0)
#	define STAT_OR_MODE()                           \
		do {                                     \
			if (jflags & JSTR_IO_FTW_NOSTAT) \
				STAT_MODE();             \
			else                             \
				STAT();                  \
		} while (0)
#else
#	if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#		define FILL_PATH() FILL_PATH_ALWAYS()
#	else
#		define FILL_PATH() \
			do {        \
			} while (0)
#	endif
#	define STAT() \
		do {   \
		} while (0)
#	define STAT_MODE() \
		do {        \
		} while (0)
#	define STAT_OR_MODE() \
		do {           \
		} while (0)
#endif

#define IS_RELATIVE(fname) ((fname[0] == '.') && ((fname[1] == '\0') || ((fname[1] == '.') && (fname[2] == '\0'))))

typedef int (*jstr_io_ftw_func_ty)(const char *dirpath,
				   size_t dlen,
				   const struct stat *st);

JSTR_FUNC_MAY_NULL
static int
p_jstr_io_ftw_len(char *R dirpath,
		  const size_t dlen,
		  const char *R const fn_glob,
		  const int fn_flags,
		  const jstr_io_ftw_flag_ty jflags,
		  jstr_io_ftw_func_ty fn,
		  struct stat *R const st
#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
		  ,
		  int fd
#endif
		  ) JSTR_NOEXCEPT
{
	DIR *R const dp =
#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
	fdopendir(fd);
#else
	opendir(dirpath);
#endif
	if (jstr_unlikely(dp == NULL))
		return 1;
	size_t pathlen = 0;
	const struct dirent *R ep;
	while ((ep = readdir(dp)) != NULL) {
		/* Ignore "." and "..". */
		if (IS_RELATIVE(ep->d_name)
		    || ((jflags & JSTR_IO_FTW_NOHIDDEN) && (ep->d_name)[0] == '.'))
			continue;
#if JSTR_HAVE_DIRENT_D_NAMLEN
		/* Exit if path is longer than PATH_MAX. */
		if (dlen + ep->d_namlen >= JSTR_IO_PATH_MAX) {
			errno = ENAMETOOLONG;
			goto err_closedir;
		}
#else
		if (!JSTR_HAVE_DIRENT_D_TYPE || JSTR_HAVE_DIRENT_D_RECLEN || sizeof(ep->d_name) > 1) {
			if (jstr_unlikely(dlen + _D_ALLOC_NAMLEN(ep)) > JSTR_IO_PATH_MAX) {
				if (dlen - JSTR_IO_NAME_MAX < JSTR_IO_PATH_MAX
				    && dlen + strlen(ep->d_name) >= JSTR_IO_PATH_MAX) {
					errno = ENAMETOOLONG;
					goto err_closedir;
				}
			}
		} else {
			if (jstr_unlikely(dlen >= JSTR_IO_PATH_MAX - JSTR_IO_NAME_MAX)
			    && dlen + strlen(ep->d_name) >= JSTR_IO_PATH_MAX) {
				errno = ENAMETOOLONG;
				goto err_closedir;
			}
		}
#endif
		errno = 0;
#if JSTR_HAVE_DIRENT_D_TYPE
		if (ep->d_type == DT_REG)
			goto do_reg;
		if (ep->d_type == DT_DIR)
			goto do_dir;
#else
#	if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
		if (jstr_unlikely(fstatat(fd, ep->d_name, st, 0)))
			continue;
#	else
		FILL_PATH_ALWAYS();
		if (jstr_unlikely(stat(dirpath, st)))
			continue;
#	endif /* ATFILE */
		if (S_ISREG(st->st_mode))
			goto do_reg;
		if (S_ISDIR(st->st_mode))
			goto do_dir;
#endif /* HAVE_D_TYPE */
		/* If true, ignore other types of files. */
		if (jflags & (JSTR_IO_FTW_DIR | JSTR_IO_FTW_REG))
			continue;
do_reg:
		if (jflags & JSTR_IO_FTW_DIR)
			if (!(jflags & JSTR_IO_FTW_REG))
				continue;
		if (fn_glob != NULL) {
			if (jflags & JSTR_IO_FTW_MATCHPATH) {
				FILL_PATH();
				if (fnmatch(fn_glob, dirpath, fn_flags))
					continue;
			} else {
				if (fnmatch(fn_glob, ep->d_name, fn_flags))
					continue;
				FILL_PATH();
			}
		} else {
			FILL_PATH();
		}
		if (jflags & JSTR_IO_FTW_STATREG) {
#if JSTR_HAVE_DIRENT_D_TYPE
			if (ep->d_type == DT_REG)
#else
			if (S_ISREG(st->st_mode))
#endif
				STAT();
			else
				STAT_MODE();
		} else {
			STAT_OR_MODE();
		}
		fn(dirpath, pathlen, st);
		continue;
do_dir:
		if (jflags & JSTR_IO_FTW_NOSUBDIR)
			if (jflags & JSTR_IO_FTW_REG)
				if (!(jflags & JSTR_IO_FTW_DIR))
					continue;
		FILL_PATH();
		if (jflags & JSTR_IO_FTW_STATREG)
			STAT_MODE();
		else
			STAT_OR_MODE();
		if (jflags & JSTR_IO_FTW_REG) {
			if (jflags & JSTR_IO_FTW_DIR)
				fn(dirpath, pathlen, st);
		} else {
			fn(dirpath, pathlen, st);
		}
		if (jflags & JSTR_IO_FTW_NOSUBDIR)
			continue;
#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
		const int fd_tmp = openat(fd, ep->d_name, O_RDONLY);
		if (jstr_unlikely(fd_tmp == -1))
			continue;
		if (jstr_unlikely(!p_jstr_io_ftw_len(dirpath, pathlen, fn_glob, fn_flags, jflags, fn, st, fd_tmp))) {
			close(fd_tmp);
			goto err_closedir;
		}
		close(fd_tmp);
#else
		if (jstr_unlikely(!p_jstr_io_ftw_len(dirpath, pathlen, fn_glob, fn_flags, jflags, fn, st)))
			goto err_closedir;
#endif
		continue;
	}
	closedir(dp);
	return 1;
err_closedir:
	closedir(dp);
	return 0;
}

#undef IS_RELATIVE
#undef FILL_PATH
#undef FILL_PATH_ALWAYS
#undef STAT_OR_MODE
#undef STAT_MODE
#undef STAT

/*
   Call FN on entries found recursively that matches GLOB.
*/
JSTR_FUNC_MAY_NULL
static int
jstr_io_ftw_len(const char *R const dirpath,
		size_t dlen,
		const char *R const fn_glob,
		const int fn_flags,
		const jstr_io_ftw_flag_ty jflags,
		jstr_io_ftw_func_ty fn) JSTR_NOEXCEPT
{
	if (jstr_unlikely(dlen == 0))
		return 0;
	while (dlen != 1
	       && dirpath[dlen - 1] == '/')
		--dlen;
	char fulpath[JSTR_IO_PATH_MAX];
	if (jflags & JSTR_IO_FTW_EXPTILDE) {
		if (*dirpath == '~') {
			const char *R const home = getenv("HOME");
			if (jstr_unlikely(home == NULL))
				return 0;
			const size_t homelen = jstr_stpcpy(fulpath, home) - fulpath;
			memcpy(fulpath + homelen, dirpath + 1, dlen);
			dlen += homelen - 1;
		}
	} else {
		jstr_strcpy_len(fulpath, dirpath, dlen);
	}
	const int fd = open(fulpath, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		return 0;
	struct stat st;
	/* This avoids things like //usr/cache. */
	if (jstr_unlikely(dlen == 1)
	    && jstr_unlikely(*dirpath == '/')) {
#if !(JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE)
		close(fd);
#endif
		dlen = 0;
		goto ftw;
	}
	if (jstr_unlikely(fstat(fd, &st)))
		return 0;
#if !(JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE)
	close(fd);
#endif
	if (jstr_likely(S_ISDIR(st.st_mode))) {
ftw:
#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
		p_jstr_io_ftw_len(fulpath, dlen, fn_glob, fn_flags, jflags, fn, &st, fd);
		close(fd);
#else
		p_jstr_io_ftw_len(fulpath, dlen, fn_glob, fn_flags, jflags, fn, &st);
#endif
		return 1;
	}
#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
	close(fd);
#endif
	if (jflags & JSTR_IO_FTW_REG)
		if (!S_ISREG(st.st_mode))
			return 0;
	if (fn_glob != NULL)
		if (fnmatch(fn_glob, fulpath, fn_flags))
			return 0;
	fn(fulpath, dlen, &st);
	return 1;
}

/*
   Call FN on entries found recursively that matches GLOB.
*/
JSTR_FUNC_MAY_NULL
JSTR_INLINE
static int
jstr_io_ftw(const char *R const dirpath,
	    const char *R const fn_glob,
	    const int fn_flags,
	    const jstr_io_ftw_flag_ty jflags,
	    jstr_io_ftw_func_ty fn) JSTR_NOEXCEPT
{
	return jstr_io_ftw_len(dirpath, strlen(dirpath), fn_glob, fn_flags, jflags, fn);
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_IO_H */
