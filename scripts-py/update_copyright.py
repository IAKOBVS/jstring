#!/usr/bin/env python3
import re
import subprocess
import sys

AUTHOR: str = 'James Tirta Halim'


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit(f"Usage: {sys.argv[0]} <filename>")

    curr_year = subprocess.run(['date', '+%Y'], capture_output=True, text=True).stdout.strip()
    fname = sys.argv[1]

    with open(fname) as fh:
        lines = fh.readlines()

    out_lines: list[str] = []
    for line in lines:
        m = re.match(r'.*Copyright \(c\) ([0-9]*)(?:\-([0-9]*))? ' + re.escape(AUTHOR) + r'.*', line)
        if m:
            first_year = m.group(1)
            last_year = m.group(2)
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
