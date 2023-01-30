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
	va_list argp;
	va_start(argp, outStr);
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		argLen += strlen(strArgv);
	}
	va_end(argp);
	if (!argLen)
		goto ERR;
	*outStr = malloc(++argLen);
	if (!*outStr)
		goto ERR;
	va_start(argp, outStr);
	int j=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		int i=0;
		do {
			(*outStr)[j++] = strArgv[i++];
		} while (strArgv[i]);
	}
	(*outStr)[argLen] = '\0';
	va_end(argp);
	return argLen;

ERR:
	fprintf(stderr, "joinStr(char **outStr) ...):");
	perror("");
	return 0;
}
