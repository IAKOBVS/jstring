#!/usr/bin/env python3
"""Update the copyright year range in a file.

Port of `scripts/update-copyright.pl`: rewrites
`Copyright (c) YYYY[(-YYYY)] AUTHOR` to end at the current year. Reads the
filename from argv and writes the result to stdout (the caller does the
file replacement).
"""
import re
import subprocess
import sys
from typing import TextIO

AUTHOR: str = 'James Tirta Halim'


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    curr_year: str = subprocess.run(['date', '+%Y'], capture_output=True, text=True).stdout.strip()
    fname: str = sys.argv[1]

    fh: TextIO
    with open(fname) as fh:
        lines: list[str] = fh.readlines()

    out_lines: list[str] = []
    for line in lines:  # line: str
        # Match a copyright line belonging to AUTHOR, capturing the first
        # year and an optional trailing year.
        m: re.Match[str] | None = re.match(r'.*Copyright \(c\) ([0-9]*)(?:\-([0-9]*))? ' + re.escape(AUTHOR) + r'.*', line)
        if m:
            first_year: str = m.group(1)
            last_year: str | None = m.group(2)
            # Extend the range only when the current year is newer.
            if (last_year and curr_year > last_year) or curr_year > first_year:
                line = re.sub(
                    r'(Copyright \(c\)) [-0-9]*',
                    r'\1 ' + first_year + '-' + curr_year,
                    line,
                )
        out_lines.append(line)

    sys.stdout.write(''.join(out_lines))


if __name__ == '__main__':
    main()
