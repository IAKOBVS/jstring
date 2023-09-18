#ifndef JSTR_IO_H
#define JSTR_IO_H 1

#include "jstr-macros.h"

P_JSTR_BEGIN_DECLS
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
P_JSTR_END_DECLS

#include "jstr-builder.h"
#include "jstr-io-table.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

P_JSTR_BEGIN_DECLS

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
JSTR_FUNC_PURE
JSTR_NOINLINE
static jstr_io_ext_ty
p_jstr_io_ext_type(const char *R ext) JSTR_NOEXCEPT
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
   Return jstr_io_ext_ty based on the filename extension;
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_io_ext_ty
jstr_io_ext_type_len(const char *R filename,
		     const size_t sz) JSTR_NOEXCEPT
{
	filename = (char *)jstr_memrchr(filename, '.', sz);
	if (filename == NULL)
		return JSTR_IO_UNKNOWN;
	return p_jstr_io_ext_type(filename + 1);
}

/*
   Return jstr_io_ext_ty based on the filename extension;
*/
JSTR_INLINE
JSTR_FUNC_PURE
static jstr_io_ext_ty
jstr_io_ext_type(const char *R filename) JSTR_NOEXCEPT
{
	filename = strrchr(filename, '.');
	if (filename == NULL)
		return JSTR_IO_UNKNOWN;
	return p_jstr_io_ext_type(filename + 1);
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
		if (p_jstr_io_reject_table[*s++])
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
		if (p_jstr_io_reject_table[*s++])
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

JSTR_INLINE
JSTR_FUNC
static int
p_jstr_io_alloc_file(const int alloc_exact,
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
	       ? JSTR_ALIGN_UP_STR(JSTR_MIN_ALLOCEXACT(st->st_size + 1))
	       : JSTR_ALIGN_UP_STR(JSTR_MIN_ALLOC(st->st_size));
	*s = (char *)malloc(*cap);
	P_JSTR_MALLOC_ERR(*s, goto err_close);
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
JSTR_FUNC
static int
jstr_io_alloc_file(char *R *R const s,
		   size_t *R const sz,
		   size_t *R const cap,
		   const char *R const fname,
		   struct stat *R const st) JSTR_NOEXCEPT
{
	return p_jstr_io_alloc_file(0, s, sz, cap, fname, st);
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
	return p_jstr_io_alloc_file(1, s, sz, cap, fname, st);
}

/*
   Return value:
   0 if no errors.
*/
JSTR_FUNC
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
JSTR_FUNC
static int
jstr_io_allocexact_file_j(jstr_ty *R const j,
			  const char *R const fname,
			  struct stat *R const st) JSTR_NOEXCEPT
{
	return jstr_io_allocexact_file(&j->data, &j->size, &j->capacity, fname, st);
}

enum {
	JSTR_IO_MAX_FNAME = 4906,
};

/*
   Calls FUNC on all regular files found recursively.
   ARG is a ptr to struct which contains additional arguments to be passed to FUNC.
   FUNC therefore must be a helper function which correctly interprets ARG.
*/
JSTR_FUNC_VOID
static void
jstr_io_files_do(const char *R const dir,
		 const size_t dlen,
		 void (*func)(const char *R const fname, const void *R const arg),
		 const void *R const arg)
{
	DIR *R dp = opendir(dir);
	if (jstr_unlikely(dp == NULL))
		return;
	char fulpath[JSTR_IO_MAX_FNAME];
	fulpath[0] = '\0';
#if !defined _GNU_SOURCE || !defined _DIRENT_HAVE_D_TYPE
	struct stat st;
#endif
	for (struct dirent *R ep; (ep = readdir(dp));) {
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_TYPE
		switch (ep->d_type) {
		case DT_REG: goto do_reg;
		case DT_DIR: goto do_dir;
		}
#else
		if (jstr_unlikely(stat(dir, &st)))
			continue;
		if (S_ISREG(st.st_mode))
			goto do_reg;
		else if (S_ISDIR(st.st_mode))
			goto do_dir;
#endif /* HAVE_D_TYPE */
do_reg:
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_NAMLEN
		memcpy(fulpath + dlen, ep->d_name, ep->d_namlen);
		fulpath[dlen + ep->d_namlen] = '\0';
#else
		strcpy(fulpath + dlen, ep->d_name);
#endif
		func(fulpath, arg);
		continue;
do_dir:
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
		jstr_io_files_do(fulpath,
				 jstr_stpcpy(fulpath + dlen, ep->d_name) - dir,
				 func,
				 arg);
		continue;
	}
	closedir(dp);
}

/*
   Equivalent to jstr_io_files_do for files that match the fnmatch glob pattern.
*/
JSTR_FUNC_VOID
static void
jstr_io_files_do_matched(const char *R const dir,
			 const size_t dlen,
			 const char *R const fn_glob,
			 const int fn_flags,
			 void (*func)(const char *R const fname, const void *R const arg),
			 const void *R const arg)
{
	DIR *R dp = opendir(dir);
	if (jstr_unlikely(dp == NULL))
		return;
	char fulpath[JSTR_IO_MAX_FNAME];
	fulpath[0] = '\0';
#if !defined _GNU_SOURCE || !defined _DIRENT_HAVE_D_TYPE
	struct stat st;
#endif
	for (struct dirent *R ep; (ep = readdir(dp));) {
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_TYPE
		switch (ep->d_type) {
		case DT_REG: goto do_reg;
		case DT_DIR: goto do_dir;
		}
#else
		if (jstr_unlikely(stat(dir, &st)))
			continue;
		if (S_ISREG(st.st_mode))
			goto do_reg;
		else if (S_ISDIR(st.st_mode))
			goto do_dir;
#endif /* HAVE_D_TYPE */
do_reg:
		if (fnmatch(fn_glob, ep->d_name, fn_flags))
			continue;
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_NAMLEN
		memcpy(fulpath + dlen, ep->d_name, ep->d_namlen);
		fulpath[dlen + ep->d_namlen] = '\0';
#else
		strcpy(fulpath + dlen, ep->d_name);
#endif
		func(fulpath, arg);
		continue;
do_dir:
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
		jstr_io_files_do_matched(fulpath,
					 jstr_stpcpy(fulpath + dlen, ep->d_name) - dir,
					 fn_glob,
					 fn_flags,
					 func,
					 arg);
		continue;
	}
	closedir(dp);
}

/*
   Equivalent to jstr_io_files_do_matched where the pattern matches the whole pathname.
*/
JSTR_FUNC_VOID
static void
jstr_io_files_do_matched_path(const char *R const dir,
			      const size_t dlen,
			      const char *R const fn_glob,
			      const int fn_flags,
			      void (*func)(const char *R const fname, const void *R const arg),
			      const void *R const arg)
{
	DIR *R dp = opendir(dir);
	if (jstr_unlikely(dp == NULL))
		return;
	char fulpath[JSTR_IO_MAX_FNAME];
	fulpath[0] = '\0';
#if !defined _GNU_SOURCE || !defined _DIRENT_HAVE_D_TYPE
	struct stat st;
#endif
	for (struct dirent *R ep; (ep = readdir(dp));) {
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_TYPE
		switch (ep->d_type) {
		case DT_REG: goto do_reg;
		case DT_DIR: goto do_dir;
		}
#else
		if (jstr_unlikely(stat(dir, &st)))
			continue;
		if (S_ISREG(st.st_mode))
			goto do_reg;
		else if (S_ISDIR(st.st_mode))
			goto do_dir;
#endif /* HAVE_D_TYPE */
do_reg:
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
		if (fnmatch(fn_glob, fulpath, fn_flags))
			continue;
#if defined _GNU_SOURCE && defined _DIRENT_HAVE_D_NAMLEN
		memcpy(fulpath + dlen, ep->d_name, ep->d_namlen);
		fulpath[dlen + ep->d_namlen] = '\0';
#else
		strcpy(fulpath + dlen, ep->d_name);
#endif
		func(fulpath, arg);
		continue;
do_dir:
		if (jstr_unlikely(fulpath[0] == '\0'))
			memcpy(fulpath, dir, dlen);
		jstr_io_files_do_matched(fulpath,
					 jstr_stpcpy(fulpath + dlen, ep->d_name) - dir,
					 fn_glob,
					 fn_flags,
					 func,
					 arg);
		continue;
	}
	closedir(dp);
}

P_JSTR_END_DECLS

#undef R

#endif /* JSTR_IO_H */
