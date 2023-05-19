#ifndef JSTDIO_DEF_H__
#define JSTDIO_DEF_H__

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

JSTR_INLINE__
JSTR_WARN_UNUSED__
size_t jstdio_sizeof_file(const char *JSTR_RESTRICT__ filename) JSTR_NOEXCEPT__
{
	struct stat st;
	return !stat(filename, &st) ? st.st_size : 0;
}

JSTR_INLINE__
JSTR_WARN_UNUSED__
int jstdio_cat(char *JSTR_RESTRICT__ buf,
	const char *JSTR_RESTRICT__ filename,
	const size_t sizeof_file) JSTR_NOEXCEPT__
{
	FILE *fp = fopen(filename, "r");
	if (unlikely(!fp))
		return 0;
#ifdef JSTR_HAS_FREAD_UNLOCKED
	fread_unlocked(JSTR_CAST__(void *) buf, 1, sizeof_file, fp);
#else
	fread(JSTR_CAST__(void *) buf, 1, sizeof_file, fp);
#endif /* JSTR_HAS_FREAD_UNLOCKED */
	fclose(fp);
	*(buf + sizeof_file) = '\0';
	return 1;
}

JSTR_INLINE__
JSTR_WARN_UNUSED__
int jstdio_dirof(char *JSTR_RESTRICT__ dirname) JSTR_NOEXCEPT__
{
	char *JSTR_RESTRICT__ const s = strrchr(dirname, '/');
	if (unlikely(!s))
		return 0;
	*s = '\0';
	return 1;
}

#ifdef __cplusplus
}
#endif

#endif // JSTDIO_DEF_H__
