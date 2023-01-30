#ifndef STR_H_DEF
#define STR_H_DEF
struct sizeOfPtr {
	void *ptr;
	int size;
};

/* int joinStr(char **outStr, ..., "\0") */
int addStrings(char **outStr, ...);
int addStr(int destLen, char **dest, int srcLen, char *src);
#endif
