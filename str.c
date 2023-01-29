#include <stdarg.h>
#include <stdlib.h>

/* Use "\0" (with quotes) as the last parameter to mark where the strings end
int addStr(char **outStr, ..., "\0") */
int joinStr(char **outStr, ...)
{
	char *tmp;
	va_list argp;
	va_start(argp, outStr);
	tmp = malloc(100);
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
	free(tmp);
	va_end(argp);
	return 0;
}

int main(void)
{
	return 0;
}
