#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "jstr.h"

#define newJstr(_NAME_OF_STRING, _STRING) jstr _NAME_OF_STRING = {.str = _STRING, .size = 0}

/* end with "\0" or "" since the \0 is implicit */
/* addStrings(&struct, &dest, ..., "") */
int catJstr(jstr *dest, ...)
{
	/* *dest->string must be initialized with 0 if empty */
	va_list ap;
	va_start(ap, dest);
	int argLen=0;
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		argLen += strlen(argvStr);
	}
	va_end(ap);
	if (!argLen)
		goto ERR;
	if (!dest->len)
		dest->len = strlen(dest->str);
	if (!dest->size) {
		char *tmp = dest->str;
		dest->size = 2 * (dest->len + argLen);
		if (!(dest->str = malloc(dest->size)))
			goto ERR;
		memcpy(dest->str, tmp, dest->len);
	} else if (dest->size < 2 * dest->len) {
		dest->size = 2 * (dest->len + argLen);
		if (!(dest->str = realloc(dest->str, dest->size)))
			goto ERR;
	}
	int i = dest->len;
	va_start(ap, dest);
	for (;;) {
		char *argvStr = va_arg(ap, char*);
		if (!argvStr[0])
			break;
		do {
			(dest->str)[i++] = *argvStr++;
		} while (*argvStr);
	}
	va_end(ap);
	dest->len += argLen;
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("catJstr(char *dest->string, ...): ");
	return 0;
}

int addJstr(jstr *dest, jstr *src)
{
	if ((!dest->len && !(dest->len = strlen(dest->str))) || (!src->size && !(src->size = strlen(src->str))))
		goto ERR;
	dest->len = src->size + dest->len;
	char *tmp = dest->str;
	dest->size = 2 * (dest->len + src->len);
	if (!(dest->str = malloc(dest->size)))
		goto ERR;
	memcpy(dest->str, tmp, src->size);
	int i=0;
	do {
		(dest->str)[dest->len++] = (src->str)[i++];
	} while (src->size--);
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("int addjtr(jstr *dest, jstr *src): ");
	return 0;
}

/* use 0 if length unknown */
int addStr(jstr *dest, char *src)
{
	int srcLen;
	if ((!dest->len && !(dest->len = strlen(dest->str))) || (!(srcLen = strlen(src))))
		goto ERR;
	dest->len += srcLen;
	char *tmp = dest->str;
	dest->size = dest->len * 2;
	if (!(dest->str = malloc(dest->size)))
		goto ERR;
	memcpy(dest->str, tmp, srcLen);
	int i=0;
	do {
		(dest->str)[dest->len++] = src[i++];
	} while (srcLen--);
	(dest->str)[dest->len + 1] = '\0';
	return dest->size;

ERR:
	perror("int addStr(jstr *dest, char *src): ");
	return 0;
}

int areDigits(char* src)
{
	while (*src)
		switch (*src++){
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			continue;
		default:
			return 0;
		}
	return 1;
}

int isJstr(jstr *structPtr)
{
	if (!*((unsigned char *)&*structPtr))
		return 0;
	return 1;
}
