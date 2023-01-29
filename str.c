#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "str.h"

#define MAX_MALLOC 1024

/* Put "\0" (with quotes) as the last parameter
int addStr(char **outStr, ..., "\0") */
int joinStr(char **outStr, ...)
{
	int mallocSize = 512;
	va_list argp;
	va_start(argp, outStr);
	*outStr = malloc(mallocSize);
	if (!*outStr)
		goto ERR;
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		for (int i=0 ; strArgv[i]; ++argLen, ++i)
			*outStr[argLen] = strArgv[i];
		if ((argLen * 2) > mallocSize) {
			mallocSize = argLen * 2;
			char *tmp = realloc(*outStr, mallocSize);
			if (!*tmp)
				goto ERR;
			*outStr = tmp;
			free(tmp);
		}
	}
	if (!argLen)
		goto ERR;
	if (mallocSize > (2 * argLen)) {
		mallocSize = argLen * 2;
		char *tmp = realloc(*outStr, mallocSize);
		if (!*outStr)
			goto ERR;
		*outStr = tmp;
		free(tmp);
	}
	va_end(argp);
	return mallocSize;
ERR:
	fprintf(stderr, "joinStr:(char **outStr) ...");
	perror("");
	free(*outStr);
	va_end(argp);
	return 0;
}

int joinStrLarge(char **outStr, ...)
{
	va_list argp;
	va_start(argp, outStr);
	int mallocSize = 1024;
	*outStr = malloc(mallocSize);
	if (!*outStr)
		goto ERR;
	int argLen=0;
	for (int mallocSize = MAX_MALLOC;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		for (int i=0 ; strArgv[i]; ++argLen, ++i)
			*outStr[argLen] = strArgv[i];
		if ((argLen * 2) > mallocSize) {
			mallocSize = argLen * 2;
			char *tmp = realloc(*outStr, mallocSize);
			if (!tmp)
				goto ERR;
			*outStr = tmp;
			free(tmp);
		}
	}
	if (!argLen)
		goto ERR;
	if (mallocSize > (2 * argLen)) {
		mallocSize = argLen * 2;
		char *tmp = realloc(*outStr, mallocSize);
		if (!*outStr)
			goto ERR;
		*outStr = tmp;
		free(tmp);
	}
	va_end(argp);
	return mallocSize;
ERR:
	fprintf(stderr, "joinStrLarge:(char **outStr) ...");
	perror("");
	free(*outStr);
	va_end(argp);
	return 0;
}
