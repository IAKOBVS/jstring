#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

/* Use "\0" (with quotes) as the last parameter to mark where the strings end */
int addStr(char **outStr, ...)
{
	char *tmp;
	va_list ap;
	va_start(ap, outStr);
	tmp = malloc(100);
	if (!tmp)
		goto ERR;
	int i=0;
	for (;;) {
		char *argv = va_arg(ap, char*);
		if (!argv[0])
			break;
		for (int j=0 ; argv[j]; ++i, ++j)
			tmp[i] = argv[j];
	}
	if (!i)
		goto ERR;
	*outStr = realloc(tmp, i);
	if (!*outStr)
		goto ERR;
	va_end(ap);
	return i;
ERR:
	free(tmp);
	va_end(ap);
	return 0;
}

int main(void)
{
	char *str;
	addStr(&str, "what ", "the ", "hell ", "\0");
	printf("%s\n", str);
	return 0;
}
