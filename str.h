#ifndef STR_H_DEF
#define STR_H_DEF
struct sizeOfPtr {
	void *ptr;
	int size;
};

/* int joinStr(char **outStr, ..., "\0") */
int addStrings(char **outStr, ...);
int addStr(char *src, int srcLen, char **dest, int destLen);
#endif
