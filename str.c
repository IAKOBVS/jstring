#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#include "str.h"

/* Put "\0" (with quotes) as the last parameter
joinStr(char &dest, "hello", var, ..., "\0") */
int addStrings(char **dest, ...)
{
	/* **dest must be initialized as 0 if not terminated with '\0' */
	int argLen=0;
	va_list argp;
	va_start(argp, dest);
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		argLen += strlen(strArgv);
	}
	int j;
	va_end(argp);
	if (!argLen)
		goto ERR;
	if (!*dest) {
		j=0;
		printf("test");
		*dest = malloc(++argLen);
		if (!*dest)
			goto ERR;
	} else {
		argLen += strlen(*dest);
		j=argLen;
		char *tmp = *dest;
		*dest = malloc(++argLen);
		if (!*dest)
			goto ERR;
		strcpy(*dest, tmp);
	}
	va_start(argp, dest);
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		int i=0;
		do {
			(*dest)[j++] = strArgv[i++];
		} while (strArgv[i]);
	}
	(*dest)[argLen] = '\0';
	va_end(argp);
	return argLen;

ERR:
	fprintf(stderr, "joinStr(char **dest) ...):");
	perror("");
	return 0;
}

/* use 0 if length unknown */
/* addStr(src, 0, &dest, 0) */
int addStr(char *src, int srcLen, char **dest, int destLen)
{
	/* **dest must be initialized as 0 if not terminated with '\0' */
	if (!*dest)
		goto ERR;
	if (!destLen) {
		destLen = strlen(*dest);
		if (!destLen)
			goto ERR;
	}
	if (!srcLen) {
		srcLen = strlen(src);
		if (!srcLen)
			goto ERR;
	}
	srcLen += destLen;
	char *tmp = *dest;
	*dest = malloc(++srcLen);
	if (!*dest)
		goto ERR;
	strcpy(*dest, tmp);
	int i=0;
	do {
		(*dest)[destLen++] = src[i++];
	} while (src[i]);
	(*dest)[++destLen] = '\0';
	return srcLen;
ERR:;
	fprintf(stderr, "joinStr(char **dest) ...):");
	perror("");
	return 0;
}
