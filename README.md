## Dependencies:
<https://github.com/IAKOBVS/pp_macros>
<br>
Change
```
#include "/home/james/pp_macros/pp_va_args_macros.h"
```
in jstr.h to
```
#include "/path/to/pp_macros/pp_va_args_macros.h"
```
Compile jstr.c into an object file with
```
gcc -c jstr.c
```
Include jstr.o when compiling your program
```
gcc my_program.c /path/to/jstr.o -o my_program
```
