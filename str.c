#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "str.h"

/* Put "\0" (with quotes) as the last parameter
int joinStr(char **outStr, ..., "\0") */
int joinStr(char **outStr, ...)
{
	/* *outStr must be freed */
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
		int i=0;
		do {
			(*outStr)[argLen++] = strArgv[i++];
		} while (strArgv[i]);
		if ((argLen * 2) > mallocSize) {
			mallocSize = argLen * 2;
			*outStr = realloc(*outStr, mallocSize);
			if (!*outStr)
				goto ERR;
		}
	}
	if (!argLen)
		goto ERR;
	(*outStr)[argLen + 1] = '\0';
	if (mallocSize > (2 * argLen)) {
		mallocSize = argLen * 2;
		*outStr = realloc(*outStr, mallocSize);
		if (!*outStr)
			goto ERR;
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

/* int joinStrLarge(char **outStr, ..., "\0") */
int joinStrLarge(char **outStr, ...)
{
	/* *outStr must be freed */
	int mallocSize = 1024;
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
		int i=0;
		do {
			(*outStr)[argLen++] = strArgv[i++];
		} while (strArgv[i]);
		if ((argLen * 2) > mallocSize) {
			mallocSize = argLen * 2;
			*outStr = realloc(*outStr, mallocSize);
			if (!*outStr)
				goto ERR;
		}
	}
	if (!argLen)
		goto ERR;
	(*outStr)[argLen + 1] = '\0';
	if (mallocSize > (2 * argLen)) {
		mallocSize = argLen * 2;
		*outStr = realloc(*outStr, mallocSize);
		if (!*outStr)
			goto ERR;
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

int joinStrProto(char **outStr, ...)
{
	/* *outStr must be freed */
	va_list argp;
	va_start(argp, outStr);
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		argLen += strlen(strArgv);
	}
	if (!argLen)
		goto ERR;
	*outStr = malloc(argLen + 1);
	if (!*outStr)
		goto ERR;
	va_end(argp);
	va_start(argp, outStr);
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		int i=0;
		do {
			(*outStr)[argLen++] = strArgv[i++];
		} while (strArgv[i]);
	}
	(*outStr)[argLen + 1] = '\0';
	va_end(argp);
	return (argLen + 1);

ERR:
	fprintf(stderr, "joinStr:(char **outStr) ...");
	perror("");
	free(*outStr);
	va_end(argp);
	return 0;
}
