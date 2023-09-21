# jstring

## Features:

- works with any C string type: functions take a pointer to string, size, and capacity.
- in-place string replacement
- in-place regex string replacement and backreferences
- reverse standard string functions: strrstr, strrspn, strrcspn, etc.

## Error handling: 
Functions returning int as error codes return 0 as error. The programmer is expected to check the return value. Use jstr\_err\(\) to print the error message or jstr\_err\_exit() to also exit. For more comprehensive error messages, use the \*\_debug\(\) functions or define JSTR\_DEBUG 1; these will make it print the filename, line number, and function in which the error occured.

## Installation:

```
sudo ./install
```
```
#include <jstr/jstr.h>
```
or
```
./build && cp -rf ./jstr ./path/to/library
```
```
#include "./jstr/jstr.h"
```

## Function affixes: 

- \*\_len(): takes the length of the string
- \*\_p(): returns a pointer to the end of the string
- \*\_f(): assumes that there be enough space in the string: it will not grow it with realloc
- \*\_now(): compiles regular expression now
- \*\_bref(): enables backreferences for the replacement string.<br>
\\0 is interpreted as referring to the whole matched string
- P_\*() or p_\*(): private functions or macros used internally

## Scripts:

- build: generates functions
- install: builds and copies to /usr/local/jstr (requires sudo)
- uninstall: removes /usr/local/jstr (requires sudo)
- clean: removes files generated by build
- fmt: formats code
- renamespace: renames the namespace<br>
For example, ./renamespace j will rename jstr\_\*() to j\_\*().<br>
The result will be placed in another directory called jstr\_\<name\_of\_namespace\>
- renamespace\_jarray: the same as renamespace but for jarray.h.<br>
Instead of placing the result in another directory, it will modify the original files
