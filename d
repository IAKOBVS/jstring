#!/bin/sh
test -e /bin/gcc && compiler=gcc || compiler=clang
$compiler test.c -o test -g -fsanitize=address
./test
