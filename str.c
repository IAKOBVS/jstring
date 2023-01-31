#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "str.h"

/* end with "\0" or "" since the \0 is implicit */
/* addStrings(&struct, &dest, ..., "") */
int addStrings(struct ptrSize *ptrInfo, char **dest, ...)
{
	/* **dest must be initialized with 0 if empty */
	va_list ap;
	va_start(ap, dest);
	int argLen=0;
	for (;;) {
		char *strArgv = va_arg(ap, char*);
		if (!strArgv[0])
			break;
		argLen += strlen(strArgv);
	}
	va_end(ap);
	if (!argLen)
		goto ERR;
	int mallocSize;
	int i;
	if (!*dest) {
		mallocSize = 2 * argLen;
		if (!(*dest = malloc(mallocSize)))
			goto ERR;
		i=0;
		ptrInfo->len = argLen;
	} else {
		if (!ptrInfo->len)
			ptrInfo->len = strlen(*dest);
		i = ptrInfo->len;
		ptrInfo->len += argLen;
		mallocSize = 2 * (argLen + ptrInfo->len);
		switch (ptrInfo->size) {
		case 0:
			{
			char *tmp = *dest;
			if (!(*dest = malloc(mallocSize)))
				goto ERR;
			memcpy(*dest, tmp, ptrInfo->len);
			}
			break;
		default:
			if (mallocSize > ptrInfo->size && !(*dest = realloc(*dest, mallocSize)))
				goto ERR;
		}
	}
	va_start(ap, dest);
	for (;;) {
		char *strArgv = va_arg(ap, char*);
		if (!strArgv[0])
			break;
		int j=0;
		do {
			(*dest)[i++] = strArgv[j++];
		} while (strArgv[j]);
	}
	va_end(ap);
	(*dest)[ptrInfo->len + 1] = '\0';
	ptrInfo->size = mallocSize;
	return ptrInfo->size;

ERR:
	fprintf(stderr, "addStringsPtr(char **dest) ...): ");
	perror("");
	return 0;
}

/* DO NOT USE */
int addStr(char **dest, int destLen, char *src, int srcLen, struct ptrSize *ptrInfo)
{
	if ((!destLen && !(destLen = strlen(*dest))) || (!srcLen && !(srcLen = strlen(src))))
		goto ERR;
	ptrInfo->len = srcLen + destLen;
	char *tmp = *dest;
	ptrInfo->size = ptrInfo->len * 2;
	if (!(*dest = malloc(ptrInfo->size)))
		goto ERR;
	memcpy(*dest, tmp, srcLen);
	srcLen=0;
	do {
		(*dest)[++destLen] = src[srcLen++];
	} while (src[srcLen]);
	(*dest)[ptrInfo->len + 1] = '\0';
	return srcLen;
ERR:;
	fprintf(stderr, "joinStr(char **dest) ...):");
	perror("");
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
		case 10:
			continue;
		default:
			return 0;
		}
	return 1;
}

int structIsZero(struct ptrSize *ptrStruct)
{
	if (!*((unsigned char *)&*ptrStruct))
		return 1;
	return 0;
}
