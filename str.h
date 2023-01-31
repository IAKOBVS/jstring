#ifndef STR_H_DEF
#define STR_H_DEF
struct ptrSize {
	int size;
	int len;
};

/* int addStringsPtr(struct ptrSize *ptrInfo, char **dest, ...); */
int addStrings(struct ptrSize *ptrInfo, char **dest, ...);
int addStr(char *src, int srcLen, char **dest, int destLen);
#endif
