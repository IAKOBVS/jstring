#ifndef JSTDIO_DEF_H__
#define JSTDIO_DEF_H__

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

JSTR_INLINE__
JSTR_WARN_UNUSED__
size_t jstdio_sizeof_file(const char *JSTR_RESTRICT__ const filename) JSTR_NOEXCEPT__
{
	struct stat st;
	return (!stat(filename, &st) ? st.st_size : 0);
}

JSTR_INLINE__
JSTR_WARN_UNUSED__
int jstdio_readfile(char *JSTR_RESTRICT__ buf,
		const char *JSTR_RESTRICT__ filename,
		const size_t sizeof_file) JSTR_NOEXCEPT__
{
	FILE *fp = fopen(filename, "r");
	if (unlikely(!fp))
		return 0;
	fread(JSTR_CAST__(void *)buf, 1, sizeof_file, fp);
	fclose(fp);
	return 1;
}

#ifdef __cplusplus
}
#endif

#endif // JSTDIO_DEF_H__
