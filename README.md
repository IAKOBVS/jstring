## Dependencies:
<https://github.com/IAKOBVS/pp_macros>
<br>
Change #include "/home/james/pp_macros/pp_va_args_macros.h" in jstr.h to "/path/to/pp_macros/pp_va_args_macros.h"
<br>
Compile the jstr.c file into an object file with
```
gcc -c jstr.c
```
and include the jstr.o file when compiling your program
```
gcc my_program.c /path/to/jstr.o -o my_program
```
