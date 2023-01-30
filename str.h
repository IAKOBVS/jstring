#ifndef STR_H_DEF
#define STR_H_DEF
struct sizeOfPtr {
	void *ptr;
	int size;
};

/* joinStr(char &dest, "hello", var, ..., "\0") *1/ */
int addStrings(char **outStr, ...);
/* addStr(src, 0, &dest, 0) */
int addStr(char *src, int srcLen, char **dest, int destLen);
#endif
