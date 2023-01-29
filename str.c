#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_MALLOC 1024

/* Put "\0" (with quotes) as the last parameter
int addStr(char **outStr, ..., "\0") */
int joinStr(char **outStr, ...)
{
	char *tmp;
	va_list argp;
	va_start(argp, outStr);
	tmp = malloc(MAX_MALLOC);
	if (!tmp)
		goto ERR;
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		for (int i=0 ; strArgv[i]; ++argLen, ++i)
			tmp[argLen] = strArgv[i];
	}
	if (!argLen)
		goto ERR;
	*outStr = realloc(tmp, argLen);
	if (!*outStr)
		goto ERR;
	va_end(argp);
	return argLen;
ERR:
	fprintf(stderr, "joinStr:(char **outStr) ...");
	perror("");
	free(tmp);
	va_end(argp);
	return 0;
}

int joinStrLarge(char **outStr, ...)
{
	char *tmp;
	va_list argp;
	va_start(argp, outStr);
	tmp = malloc(MAX_MALLOC);
	if (!tmp)
		goto ERR;
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(argp, char*);
		if (!strArgv[0])
			break;
		for (int i=0 ; strArgv[i]; ++argLen, ++i)
			tmp[argLen] = strArgv[i];
		if (MAX_MALLOC < argLen * 4) {
			char* tmp2 = realloc(tmp, (argLen * 8));
			if (!tmp2)
				goto ERR;
			tmp = tmp2;
		}
	}
	if (!argLen)
		goto ERR;
	*outStr = realloc(tmp, argLen);
	if (!*outStr)
		goto ERR;
	va_end(argp);
	return argLen;
ERR:
	fprintf(stderr, "joinStrLarge:(char **outStr) ...");
	perror("");
	free(tmp);
	va_end(argp);
	return 0;
}
