/* See LICENSE file for copyright and license details. */

#ifndef JSTR_IO_H
#define JSTR_IO_H 1

#include "jstr-macros.h"

JSTRP__BEGIN_DECLS
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
JSTRP__END_DECLS

#include "jstr-builder.h"
#include "jstr-io-table.h"
#include "jstr-string.h"

#define R JSTR_RESTRICT

JSTRP__BEGIN_DECLS

typedef enum {
	JSTR_IO_FT_UNKNOWN = 0,
	JSTR_IO_FT_TEXT,
	JSTR_IO_FT_BINARY
} jstr_io_ext_ty;

enum {
#ifdef PATH_MAX
	JSTR_IO_PATH_MAX = PATH_MAX,
#else
	JSTR_IO_PATH_MAX = 4096,
#endif
#ifdef NAME_MAX
	JSTR_IO_NAME_MAX = NAME_MAX,
#else
	JSTR_IO_NAME_MAX = 255,
#endif
	JSTR_IO_BINARY_CHECK_MAX = 64
};

JSTR_FUNC_PURE
JSTR_NOINLINE
static jstr_io_ext_ty
jstrp__io_exttype_len(const char *ext,
                     const int ext_len)
JSTR_NOEXCEPT
{
	/* From shortest to longest extension. */
	static const char *text[][4] = {
		{ NULL }, /* A */
		{ NULL }, /* B */
		{ "C" }, /* C */
		{ NULL }, /* D */
		{ NULL }, /* E */
		{ NULL }, /* F */
		{ NULL }, /* G */
		{ NULL }, /* H */
		{ NULL }, /* I */
		{ NULL }, /* J */
		{ NULL }, /* K */
		{ NULL }, /* L */
		{ NULL }, /* M */
		{ NULL }, /* N */
		{ NULL }, /* O */
		{ NULL }, /* P */
		{ NULL }, /* Q */
		{ NULL }, /* R */
		{ "S" }, /* S */
		{ NULL }, /* T */
		{ NULL }, /* U */
		{ NULL }, /* V */
		{ NULL }, /* W */
		{ NULL }, /* X */
		{ NULL }, /* Y */
		{ NULL }, /* Z */
		{ NULL }, /* a */
		{ NULL }, /* b */
		{ "c", "cc", "cs", "cpp" }, /* c */
		{ NULL }, /* d */
		{ NULL }, /* e */
		{ NULL }, /* f */
		{ NULL }, /* g */
		{ "h", "hh", "hpp", "html" }, /* h */
		{ NULL }, /* i */
		{ "js", "json" }, /* j */
		{ NULL }, /* k */
		{ NULL }, /* l */
		{ "md" }, /* m */
		{ NULL }, /* n */
		{ NULL }, /* o */
		{ "pl", "pm", "py", "pyi" }, /* p */
		{ NULL }, /* q */
		{ "rs" }, /* r */
		{ "s", "sh" }, /* s */
		{ "ts", "txt" }, /* t */
		{ NULL }, /* u */
		{ NULL }, /* v */
		{ NULL }, /* w */
		{ NULL }, /* x */
		{ NULL }, /* y */
		{ NULL }  /* z */
	};
	static const char *binary[][4] = {
		{ NULL }, /* A */
		{ NULL }, /* B */
		{ NULL }, /* C */
		{ NULL }, /* D */
		{ NULL }, /* E */
		{ NULL }, /* F */
		{ NULL }, /* G */
		{ NULL }, /* H */
		{ NULL }, /* I */
		{ NULL }, /* J */
		{ NULL }, /* K */
		{ NULL }, /* L */
		{ NULL }, /* M */
		{ NULL }, /* N */
		{ NULL }, /* O */
		{ NULL }, /* P */
		{ NULL }, /* Q */
		{ NULL }, /* R */
		{ NULL }, /* S */
		{ NULL }, /* T */
		{ NULL }, /* U */
		{ NULL }, /* V */
		{ NULL }, /* W */
		{ NULL }, /* X */
		{ NULL }, /* Y */
		{ NULL }, /* Z */
		{ "a" }, /* a */
		{ "bin" }, /* b */
		{ "c", "cc", "cs", "cpp" }, /* c */
		{ NULL }, /* d */
		{ NULL }, /* e */
		{ NULL }, /* f */
		{ "gz" }, /* g */
		{ NULL }, /* h */
		{ NULL }, /* i */
		{ "jpg", "jpeg" }, /* j */
		{ NULL }, /* k */
		{ NULL }, /* l */
		{ "mp4", "mp3", "mkv" }, /* m */
		{ NULL }, /* n */
		{ "o" }, /* o */
		{ "pdf", "png", "pyc" }, /* p */
		{ NULL }, /* q */
		{ "rar" }, /* r */
		{ "so" }, /* s */
		{ NULL }, /* t */
		{ NULL }, /* u */
		{ NULL }, /* v */
		{ "wav" }, /* w */
		{ NULL }, /* x */
		{ NULL }, /* y */
		{ "zip" }  /* z */
	};
	const int idx = *ext;
	if (jstr_unlikely(!jstr_isalpha(idx)))
		return JSTR_IO_FT_UNKNOWN;
	if (text[idx][0]) {
		for (int i = 0; i < (int)JSTR_ARRAY_SIZE(text[idx]); ++i)
			if (sizeof(text[idx][i]) - 1 == ext_len) {
				if (!memcmp(ext, text[idx][i], ext_len))
					return JSTR_IO_FT_TEXT;
			} else if (jstr_unlikely((int)sizeof(binary[idx][i]) - 1 > ext_len)) {
				break;
			}
	}
	if (binary[idx][0]) {
		for (int i = 0; i < (int)JSTR_ARRAY_SIZE(binary[idx]); ++i)
			if (sizeof(binary[idx][i]) - 1 == ext_len) {
				if (!memcmp(ext, text[idx][i], ext_len))
					return JSTR_IO_FT_BINARY;
			} else if (jstr_unlikely((int)sizeof(binary[idx][i]) - 1 > ext_len)) {
				break;
			}
	}
	return JSTR_IO_FT_BINARY;
}

JSTR_FUNC_PURE
JSTR_INLINE
static jstr_io_ext_ty
jstrp__io_exttype(const char *ext)
JSTR_NOEXCEPT
{
	return jstrp__io_exttype_len(ext, strlen(ext));
}

/*
   Return jstr_io_ext_ty based on the FNAME extension;
*/
JSTR_FUNC_PURE
JSTR_INLINE
static jstr_io_ext_ty
jstr_io_exttype_len(const char *R fname,
                    const size_t sz)
JSTR_NOEXCEPT
{
	const char *const end = fname + sz;
	fname = (char *)jstr_memrchr(fname, '.', sz);
	return fname ? jstrp__io_exttype_len(fname + 1, end - (fname + 1)) : JSTR_IO_FT_UNKNOWN;
}

/*
   Return jstr_io_ext_ty based on the FNAME extension;
*/
JSTR_INLINE
JSTR_FUNC_PURE
static jstr_io_ext_ty
jstr_io_exttype(const char *R fname)
JSTR_NOEXCEPT
{
	fname = strrchr(fname, '.');
	return fname ? jstrp__io_exttype(fname + 1) : JSTR_IO_FT_UNKNOWN;
}

/*
   Check MIN(N, SZ) bytes for any unprintable character.
*/
JSTR_FUNC
static int
jstr_isbinary(const char *R buf,
              const size_t n,
              const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(n, sz);
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (jstrp__io_reject_table[*s++])
			return 1;
	return 0;
}

#define ELF    "\x7f\ELF"
#define ELF_SZ (sizeof("\x7f\ELF") - 1)
#define UTF    "\xEF\xBB\xBF"
#define UTF_SZ (sizeof("\xEF\xBB\xBF") - 1)

JSTR_FUNC
JSTR_INLINE
static int
jstrp__io_isbinarysignature(const char *R buf,
                           const size_t sz)
{
	if (jstr_likely(sz > ELF_SZ - 1)) {
		if (jstr_unlikely(!memcmp(buf, ELF, ELF_SZ)))
			return 1;
check_utf:;
		if (!memcmp(buf, UTF, UTF_SZ))
			return 0;
	} else if (jstr_likely(sz == UTF_SZ)) {
		goto check_utf;
	}
	return -1;
}

#undef ELF
#undef ELF_SZ
#undef UTF
#undef UTF_SZ

/*
   Check if the first JSTR_IO_BINARY_CHECK_MAX bytes or fewer contain any unprintable character.
*/
JSTR_FUNC
static int
jstr_io_isbinary_maybe(const char *R buf,
                       const size_t sz)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(sz == 0))
		return 0;
	const int ret = jstrp__io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	const unsigned char *const end = (const unsigned char *)buf + JSTR_MIN(sz, JSTR_IO_BINARY_CHECK_MAX) + 1;
	const unsigned char *s = (unsigned char *)buf;
	while (s < end)
		if (jstrp__io_reject_table[*s++])
			return 1;
	return 0;
}

/*
   Check the whole file for a NUL byte.
   File must be nul terminated.
*/
JSTR_FUNC_PURE
static int
jstr_io_isbinary(const char *R buf,
                 const size_t sz)
JSTR_NOEXCEPT
{
	const int ret = jstrp__io_isbinarysignature(buf, sz);
	if (ret != -1)
		return ret;
	return strlen(buf) != sz;
}

JSTR_FUNC
static int
jstr_io_write_file(const char *R s,
                   const size_t sz,
                   const char *R fname,
                   const int oflag)
JSTR_NOEXCEPT
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
jstr_io_write_file_j(const jstr_ty *R j,
                     const char *R fname,
                     const int oflag)
JSTR_NOEXCEPT
{
	return jstr_io_write_file(j->data, j->size, fname, oflag);
}

JSTR_FUNC
static int
jstr_io_fwrite_file(const char *R s,
                    const size_t sz,
                    const char *R fname,
                    const char *R modes)
JSTR_NOEXCEPT
{
	FILE *R fp = fopen(fname, modes);
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
jstr_io_fwrite_file_j(const jstr_ty *R j,
                      const char *R fname,
                      const char *R modes)
JSTR_NOEXCEPT
{
	return jstr_io_fwrite_file(j->data, j->size, fname, modes);
}

#if JSTR_HAVE_POPEN

JSTR_FUNC
static int
jstr_io_readpopen(char *R *R s,
                  size_t *R sz,
                  size_t *R cap,
                  const char *R cmd)
JSTR_NOEXCEPT
{
	FILE *R fp = popen(cmd, "r");
	if (jstr_unlikely(fp == NULL))
		goto err;
	char *p;
	enum { MINBUF = JSTR_PAGE_SIZE };
	char buf[MINBUF];
	p = buf + fread(buf, 1, MINBUF, fp);
	if (jstr_unlikely(ferror(fp)))
		goto err_close;
	JSTRP__RESERVE(s, sz, cap, p - buf, goto err_close);
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
				JSTRP__RESERVEEXACTALWAYS(s, sz, cap, (size_t)(*cap * JSTRP__GROWTH), goto err_close)
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
	jstr_free(s, sz, cap);
err_close:
	pclose(fp);
err:
	return 0;
}

#endif

JSTR_FUNC
static int
jstr_io_readfile_len(char *R *R s,
                     size_t *R sz,
                     size_t *R cap,
                     const char *R fname,
                     const size_t file_size)
JSTR_NOEXCEPT
{
	const int fd = open(fname, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		goto err;
	JSTRP__RESERVE(s, sz, cap, file_size, goto err_close;)
	if (jstr_unlikely(file_size != (size_t)read(fd, *s, file_size)))
		goto err_close_free;
	close(fd);
	(*s)[file_size] = '\0';
	*sz = file_size;
	return 1;
err_close_free:
	jstr_free(s, sz, cap);
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
jstr_io_readfile(char *R *R s,
                 size_t *R sz,
                 size_t *R cap,
                 const char *R fname,
                 struct stat *R st)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(stat(fname, st)))
		return 0;
	return jstr_io_readfile_len(s, sz, cap, fname, st->st_size);
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
jstr_io_expandtildefirst_len_unsafe_p(char *R s,
                                      const size_t sz)
JSTR_NOEXCEPT
{
	if (*s != '~')
		return s + sz;
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	jstr_strmove_len(s + len, s + 1, (s + sz) - (s + 1));
	memcpy(s, home, len);
	return s + sz + len - 1;
}

/*
   Expand ~/some_dir to /home/username/some_dir.
   Assume that S has enough space.
   Return value:
   ptr to '\0' in S.
   NULL on error.
*/
JSTR_FUNC
static int
jstr_io_expandtildefirst_p(char *R *R s,
                           size_t *R sz,
                           size_t *R cap)
JSTR_NOEXCEPT
{
	if (**s != '~')
		return 1;
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return 0;
	const size_t len = strlen(home);
	JSTRP__RESERVE(s, sz, cap, *sz + len, return 0);
	jstr_strmove_len(*s + len, *s + 1, (*s + *sz) - (*s + 1));
	memcpy(*s, home, len);
	*sz += len;
	return 1;
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
jstr_io_expandtilde_len_unsafe_p(char *R s,
                                 size_t sz)
JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return NULL;
	const size_t len = strlen(home);
	char *p = s;
	while ((p = (char *)memchr(p, '~', (s + sz) - p))) {
		jstr_strmove_len(p + len, p + 1, (s + sz) - (p + 1));
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
jstr_io_expandtilde(char *R *R s,
                    size_t *R sz,
                    size_t *R cap)
JSTR_NOEXCEPT
{
	const char *R home = getenv("HOME");
	if (jstr_unlikely(home == NULL))
		return 0;
	const size_t len = strlen(home);
	const char *tmp;
	char *p = *s;
	while ((p = (char *)memchr(p, '~', (*s + *sz) - p))) {
		if (jstr_unlikely(*sz + len >= *cap)) {
			tmp = *s;
			JSTRP__RESERVEALWAYS(s, sz, cap, *sz + len, return 0)
			p = *s + (p - tmp);
		}
		jstr_strmove_len(p + len, p + 1, (*s + *sz) - (p + 1));
		memcpy(p, home, len);
		p += len;
		*sz += (len - 1);
	}
	return 1;
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_io_appendpath_len_p(char *R path,
                         const size_t sz,
                         const char *R fname,
                         const size_t fname_len)
{
	*path = '/';
	jstr_strcpy_len(path + sz + 1, fname, fname_len);
	return path + sz + 1 + fname_len;
}

JSTR_FUNC_RET_NONNULL
JSTR_INLINE
static char *
jstr_io_appendpath_p(char *R path,
                     const size_t sz,
                     const char *R fname)
{
	*(path + sz) = '/';
	return jstr_stpcpy(path + sz + 1, fname);
}

JSTR_INLINE
JSTR_FUNC_RET_NONNULL
static char *
jstrp__io_appendpath_p(char *R path_end,
                      const char *R fname)
JSTR_NOEXCEPT
{
	*path_end = '/';
	return jstr_stpcpy(path_end + 1, fname);
}

JSTR_INLINE
JSTR_FUNC_VOID
static void
jstrp__io_appendpath_len(char *R path_end,
                        const char *R fname,
                        const size_t fname_len)
JSTR_NOEXCEPT
{
	*path_end = '/';
	jstr_strcpy_len(path_end + 1, fname, fname_len);
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

/* If JSTR_IO_ACTION_RETVAL is passed, use these as return values of FN(). */
typedef enum jstr_io_ftw_actionretval_ty {
	JSTR_IO_FTW_RET_STOP = 0,
	JSTR_IO_FTW_RET_CONTINUE,
	/* Skip sibling entries. */
	JSTR_IO_FTW_RET_SKIP_SIBLINGS,
	/* Skip subdirectories. */
	JSTR_IO_FTW_RET_SKIP_SUBTREE,
} jstr_io_ftw_actionretval_ty;

typedef enum jstr_io_ftw_flag_ty {
	/* Match glob with FULPATH instead of d_name. */
	JSTR_IO_FTW_MATCHPATH = (1),
	/* Call FN() on regular files. */
	JSTR_IO_FTW_REG = (JSTR_IO_FTW_MATCHPATH << 1),
	/* Call FN() on directories. */
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
	/* Handle FN() return value according to jstr_io_ftw_actionretval_ty. */
	JSTR_IO_FTW_ACTIONRETVAL = (JSTR_IO_FTW_EXPTILDE << 1)
} jstr_io_ftw_flag_ty;

#if JSTR_HAVE_FDOPENDIR && JSTR_HAVE_ATFILE
#	define USE_ATFILE 1
#else
#	define USE_ATFILE 0
#endif

#define NONFATAL_ERR() jstr_likely(errno == EACCES || errno == ENOENT)

#if JSTR_HAVE_DIRENT_D_NAMLEN
#	define FILL_PATHALWAYS()                                                                \
		do {                                                                              \
			jstrp__io_appendpath_len(dirpath + dirpath_len, ep->d_name, ep->d_namlen); \
			path_len = dirpath_len + 1 + ep->d_namlen;                                \
		} while (0)
#else
#	define FILL_PATHALWAYS() ((void)(path_len = jstrp__io_appendpath_p(dirpath + dirpath_len, ep->d_name) - dirpath))
#endif

#if USE_ATFILE
#	define STATALWAYS(st)                                              \
		do {                                                         \
			if (jstr_unlikely(fstatat(fd, ep->d_name, st, 0))) { \
				if (NONFATAL_ERR())                          \
					continue;                            \
				return 0;                                    \
			}                                                    \
		} while (0)
#else
#	define STATALWAYS(st)                               \
		do {                                          \
			if (jstr_unlikely(stat(dirpath, st))) \
				if (NONFATAL_ERR())           \
					continue;             \
			return 0;                             \
		} while (0)
#endif

#if JSTR_HAVE_DIRENT_D_TYPE
#	define ISDIR()       (ep->d_type == DT_DIR)
#	define ISREG()       (ep->d_type == DT_REG)
#	define FILL_PATH()   FILL_PATHALWAYS()
#	define STAT(st)      STATALWAYS(st)
#	define STAT_MODE(st) ((void)((st).st_mode = DTTOIF(ep->d_type)))
#	define STAT_OR_MODE(st)                         \
		do {                                     \
			if (jflags & JSTR_IO_FTW_NOSTAT) \
				STAT_MODE(st);           \
			else                             \
				STAT(st);                \
		} while (0)
#else
#	define ISDIR() (S_ISDIR(st->st_mode))
#	define ISREG() (S_ISREG(st->st_mode))
#	if USE_ATFILE
#		define FILL_PATH() FILL_PATHALWAYS()
#	else
#		define FILL_PATH() \
			do {        \
			} while (0)
#	endif
#	define STAT(st) \
		do {     \
		} while (0)
#	define STAT_MODE(st) \
		do {          \
		} while (0)
#	define STAT_OR_MODE(st) \
		do {             \
		} while (0)
#endif

#if USE_ATFILE
#	define FD_PARAM , int fd
#	define FD_ARG   , fd
#else
#	define FD_PARAM
#	define FD_ARG
#endif

typedef int (*jstr_io_ftw_func_ty)(const char *dirpath,
                                   size_t dirpath_len,
                                   const struct stat *st);

JSTR_FUNC_VOID_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
JSTR_NONNULL(7)
static int
jstrp__io_ftw_len(char *R dirpath,
                 const size_t dirpath_len,
                 int (*fn)(const char *, size_t, const struct stat *),
                 const int jflags,
                 const char *R fn_glob,
                 const int fn_flags,
                 struct stat *R st
                 FD_PARAM)
JSTR_NOEXCEPT
{
	DIR *R const dp =
#if USE_ATFILE
	fdopendir(fd);
#else
	opendir(dirpath);
#endif
	if (jstr_unlikely(dp == NULL))
		return NONFATAL_ERR();
	size_t path_len = 0;
	const struct dirent *R ep;
	int ret;
	while ((ep = readdir(dp)) != NULL) {
		if (jflags & JSTR_IO_FTW_NOHIDDEN) {
			/* Ignore hidden files. */
			if (ep->d_name[0] == '.')
				continue;
		} else {
			/* Ignore "." and "..". */
			if (ep->d_name[0] == '.'
			    && (ep->d_name[1] == '\0' || (ep->d_name[1] == '.' && ep->d_name[2] == '\0')))
				continue;
		}
		/* Exit if DIRPATH is longer than PATH_MAX. */
		if (
#if JSTR_HAVE_DIRENT_D_NAMLEN
		jstr_unlikely(dirpath_len + ep->d_namlen >= JSTR_IO_PATH_MAX)
#else
		jstr_unlikely(dirpath_len >= JSTR_IO_PATH_MAX - JSTR_IO_NAME_MAX)
		&& jstr_unlikely(dirpath_len + strlen(ep->d_name) >= JSTR_IO_PATH_MAX)
#endif
		) {
			errno = ENAMETOOLONG;
			goto err_closedir;
		}
#if !JSTR_HAVE_DIRENT_D_TYPE
#	if !(USE_ATFILE)
		FILL_PATHALWAYS();
#	endif /* ATFILE */
		STATALWAYS(st);
#endif /* D_TYPE */
		if (ISREG())
			goto do_reg;
		if (ISDIR())
			goto do_dir;
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
			if (ISREG())
				STAT(st);
			else
				STAT_MODE(st);
		} else {
			STAT_OR_MODE(st);
		}
		ret = fn(dirpath, path_len, st);
		if (jflags & JSTR_IO_FTW_ACTIONRETVAL) {
			if (ret == JSTR_IO_FTW_RET_CONTINUE
			    || ret == JSTR_IO_FTW_RET_SKIP_SUBTREE)
				continue;
			else if (ret == JSTR_IO_FTW_RET_SKIP_SIBLINGS)
				break;
			else /* RET_STOP */
				goto err_closedir;
		} else {
			if (jstr_unlikely(!ret))
				goto err_closedir;
		}
		continue;
do_dir:
		if (jflags & JSTR_IO_FTW_NOSUBDIR)
			if (jflags & JSTR_IO_FTW_REG)
				if (!(jflags & JSTR_IO_FTW_DIR))
					continue;
		FILL_PATH();
		if (jflags & JSTR_IO_FTW_STATREG)
			STAT_MODE(st);
		else
			STAT_OR_MODE(st);
		if (jflags & JSTR_IO_FTW_REG)
			if (!(jflags & JSTR_IO_FTW_DIR))
				goto CONT;
		ret = fn(dirpath, dirpath_len, st);
		if (jflags & JSTR_IO_FTW_ACTIONRETVAL) {
			if (ret == JSTR_IO_FTW_RET_CONTINUE)
				continue;
			else if (ret == JSTR_IO_FTW_RET_SKIP_SUBTREE
			         || ret == JSTR_IO_FTW_RET_SKIP_SIBLINGS)
				goto CONT;
			else /* RET_STOP */
				goto err_closedir;
		} else {
			if (jstr_unlikely(!ret))
				goto err_closedir;
		}
CONT:
		if (jflags & JSTR_IO_FTW_NOSUBDIR)
			continue;
#if USE_ATFILE
		fd = openat(fd, ep->d_name, O_RDONLY);
		if (jstr_unlikely(fd == -1))
			continue;
		ret = jstrp__io_ftw_len(dirpath, path_len, fn, jflags, fn_glob, fn_flags, st, fd);
		close(fd);
		if (jstr_unlikely(!ret))
			goto err_closedir;
#else
		if (jstr_unlikely(!jstrp__io_ftw_len(dirpath, path_len, fn, jflags, fn_glob, fn_flags, st)))
			goto err_closedir;
#endif
	}
	closedir(dp);
	return 1;
err_closedir:
	closedir(dp);
	return 0;
}

#undef ISDIR
#undef ISREG
#undef FILL_PATH
#undef FILL_PATHALWAYS
#undef STAT_OR_MODE
#undef STAT_MODE
#undef STAT
#undef NONFATAL_ERR
#undef STATALWAYS
#undef FD_PARAM

/*
   Call FN() on files found recursively that matches GLOB.
   If FN() returns 0, stop processing.
   Return value:
   0 on error;
   1 on success or non-fatal errors (EACCES or ENOENT) encountered on some entries.
   If a non-fatal error is encountered, continue processing other entries.
*/
JSTR_FUNC_MAY_NULL
JSTR_NONNULL(1)
JSTR_NONNULL(3)
static int
jstr_io_ftw_len(const char *R dirpath,
                size_t dirpath_len,
                int (*fn)(const char *, size_t, const struct stat *),
                const int jstr_io_ftw_flag,
                const char *R fn_glob,
                const int fn_flags)
JSTR_NOEXCEPT
{
	if (jstr_unlikely(dirpath_len == 0)) {
		errno = ENOENT;
		return 0;
	}
	if (jstr_unlikely(dirpath_len >= JSTR_IO_PATH_MAX)) {
		errno = ENAMETOOLONG;
		return 0;
	}
	while (dirpath_len != 1
	       && dirpath[dirpath_len - 1] == '/')
		--dirpath_len;
	char fulpath[JSTR_IO_PATH_MAX];
	if (jstr_io_ftw_flag & JSTR_IO_FTW_EXPTILDE) {
		if (*dirpath == '~') {
			const char *R home = getenv("HOME");
			if (jstr_unlikely(home == NULL))
				return 0;
			const size_t homelen = jstr_stpcpy(fulpath, home) - fulpath;
			memcpy(fulpath + homelen, dirpath + 1, dirpath_len);
			dirpath_len += homelen - 1;
		}
	} else {
		jstr_strcpy_len(fulpath, dirpath, dirpath_len);
	}
#if USE_ATFILE
	const int fd = open(fulpath, O_RDONLY);
	if (jstr_unlikely(fd == -1))
		return 0;
#endif
	struct stat st;
	/* This avoids things like //usr/cache. */
	if (jstr_unlikely(dirpath_len == 1)
	    && jstr_unlikely(*dirpath == '/')) {
		dirpath_len = 0;
		goto ftw;
	}
#if USE_ATFILE
	if (jstr_unlikely(fstat(fd, &st)))
		goto err_close;
#else
	if (jstr_unlikely(stat(fulpath, &st)))
		return 0;
#endif
	if (jstr_likely(S_ISDIR(st.st_mode))) {
ftw:
		if (jstr_io_ftw_flag & JSTR_IO_FTW_REG)
			if (!(jstr_io_ftw_flag & JSTR_IO_FTW_DIR))
				goto CONT;
		int ret;
		ret = fn(dirpath, dirpath_len, &st);
		if (jstr_io_ftw_flag & JSTR_IO_FTW_ACTIONRETVAL) {
			if (jstr_unlikely(ret != JSTR_IO_FTW_RET_CONTINUE))
				goto err_close;
		} else {
			if (jstr_unlikely(!ret))
				goto err_close;
		}
CONT:
		jstrp__io_ftw_len(fulpath, dirpath_len, fn, jstr_io_ftw_flag, fn_glob, fn_flags, &st FD_ARG);
#if USE_ATFILE
		close(fd);
#endif
		return 1;
	}
#if USE_ATFILE
	close(fd);
#endif
	if (jstr_io_ftw_flag & JSTR_IO_FTW_REG)
		if (jstr_unlikely(!S_ISREG(st.st_mode)))
			return 1;
	if (fn_glob != NULL)
		if (fnmatch(fn_glob, fulpath, fn_flags))
			return 1;
	return fn(fulpath, dirpath_len, &st);
err_close:
#if USE_ATFILE
	close(fd);
#endif
	return 0;
}

#undef USE_ATFILE
#undef FD_ARG

JSTRP__END_DECLS

#undef R

#endif /* JSTR_IO_H */
