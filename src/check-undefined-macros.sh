#!/bin/sh

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
