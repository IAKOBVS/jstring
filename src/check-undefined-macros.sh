#!/bin/sh

# Copyright (c) 2023 James Tirta Halim <tirtajames45 at gmail dot com>
# This file is part of the jstring library.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# MIT License (Expat)

prefix=$1
file=$2
macros=$(grep "${prefix}_.*1" "$file" | sed 's/^[ \t]*#[ \t]*define[ \t]*JSTR_//; s/[ \t][ \t]*1//g' | sort | uniq)
for macro in $macros; do
	if ! grep -q "${macro}[ \t]*0" "$file"; then
		echo "$macro is not defined in $file."
		exit 1
	fi &
done
wait
