#ifndef JSTR_IO_H_DEF
#define JSTR_IO_H_DEF

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef __cplusplus
}
#endif /* __cpluslus */

#include "_jstr_builder.h"
#include "_jstr_macros.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cpluslus */

/*
   Return value:
   0 if no errors.
*/
JSTR_NONNULL_ALL
static int
jstr_alloc_file(char **JSTR_RST const s,
		size_t *JSTR_RST sz,
		size_t *JSTR_RST cap,
		const char *JSTR_RST const _fname,
		struct stat *JSTR_RST const _st)
{
	FILE *JSTR_RST _fp = fopen(_fname, "r");
	if (unlikely(!_fp))
		goto _ERR;
	if (unlikely(stat(_fname, _st)))
		goto _ERR_CLOSE;
	*cap = _st->st_size * 2;
	*s = (char *)malloc(*cap);
	JSTR_MALLOC_ERR(*s, goto _ERR_CLOSE);
	fread(*s, 1, _st->st_size, _fp);
	*(*s + _st->st_size) = '\0';
	*sz = _st->st_size;
	fclose(_fp);
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
JSTR_INLINE
JSTR_NONNULL_ALL
static int
jstr_alloc_file_j(Jstring *JSTR_RST const j,
		  const char *JSTR_RST const _fname,
		  struct stat *JSTR_RST const _st)
{
	return jstr_alloc_file(&j->data, &j->size, &j->cap, _fname, _st);
}

#ifdef __cplusplus
}
#endif /* __cpluslus */

#endif /* JSTR_IO_H_DEF */
