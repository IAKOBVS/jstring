#!/usr/bin/env python3
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import jl_file_get_str, jl_file_namespace_macros


def main() -> None:
    if len(sys.argv) == 1:
        file_str = sys.stdin.read()
    else:
        file_str = jl_file_get_str(sys.argv[1])
    file_str = jl_file_namespace_macros(file_str, 'JSTR_INTERNAL_', ['JSTR', 'jstr'])
    sys.stdout.write(file_str)


if __name__ == '__main__':
    main()
