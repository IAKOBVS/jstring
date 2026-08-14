#!/usr/bin/env python3
"""Shared helpers for the jstring header generators.

This is a Python port of the original `scripts/jlib.pl`. Keep it
byte-for-byte compatible: `scripts/check-py-parity` asserts that generated
output matches the Perl engine re-extracted from git history. Where a
function deliberately differs from Perl, it carries a comment explaining
the divergence.
"""
import re
from typing import TypeAlias

# Match a C function definition block, capturing:
#   1 — attributes before the return type
#   2 — return type (may include pointer asterisks and trailing whitespace)
#   3 — function name
#   4 — argument list
#   5 — function body
JL_FN_RE: re.Pattern[str] = re.compile(
    r'^([^{}]*(?:^|\W))'          # 1 — pre-return attrs
    r'(\w+\s*[* \t\n]*)'          # 2 — return type
    r'\s+'
    r'(\w+)'                      # 3 — function name
    r'\s*\(([^{}]*)\)'            # 4 — args
    r'[^{}}]*'                    # post-args attrs (no braces)
    r'\{((?:.|\n)*)\}'            # 5 — body
)

# The parsed parts of a function block returned by `jl_fn_get`.
FnParts: TypeAlias = tuple[str, str, str, list[str], str | None]


def jl_file_get_str(fname: str) -> str:
    """Read an entire file into a single string."""
    with open(fname) as fh:
        return fh.read()


def jl_file_namespace_macros(file_str: str, prefix: str, ignore_prefix: list[str]) -> str:
    """
    Find all macros that appear in `#undef` directives and, unless they
    already match an `ignore_prefix`, rename every bare occurrence to
    `prefix + macro`.

    Used to namespace internal musl macros as `JSTR_INTERNAL_*` so they
    cannot collide with the user's own macros.
    """
    lines: list[str] = file_str.split('\n')
    macros: set[str] = set()
    for line in lines:  # line: str
        m: re.Match[str] | None = re.match(r'^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)', line)
        if m:
            macro: str = m.group(1)
            if not any(macro.startswith(p) for p in ignore_prefix):
                macros.add(macro)
    for macro in macros:  # macro: str
        # Replace only bare occurrences (not inside identifiers, strings,
        # or single quotes), mirroring the Perl boundary class.
        file_str = re.sub(
            r'([^\'_\"\w]|^)' + re.escape(macro) + r'([^\'_\"\w]|$)',
            r'\1' + prefix + macro + r'\2',
            file_str,
        )
    return file_str


def jl_file_tidy_newlines(file_str: str) -> str:
    """Collapse runs of three or more consecutive newlines to two."""
    file_str = re.sub(r'(\n[ \t].*)\n\n\n*', r'\1\n', file_str)
    file_str = re.sub(r'\n\n\n*\t', '\n\t', file_str)
    file_str = re.sub(r'\n\n\n* ', '\n ', file_str)
    return file_str


def jl_arg_to_array(arg_str: str) -> list[str]:
    """Split a C argument list on commas and trim each element.

    An empty string yields an empty list, matching Perl's
    `split(/,/, '')`.
    """
    if not arg_str:
        return []
    return [x.strip() for x in arg_str.split(',')]


def jl_arg_to_string(arg_arr: list[str]) -> str:
    """Join argument elements back into one comma-separated string."""
    return ', '.join(arg_arr)


def jl_arg_push(arg_arr: list[str], add: str) -> None:
    arg_arr.append(add)


def jl_arg_index(arg_arr: list[str], find: str) -> int:
    # Intentional difference from the Perl original (jlib.pl): Perl's loop
    # runs `$i < $#$arg_arr_ref`, so it never examines the last element.
    # This version checks every element and escapes `find`. No current header
    # exercises the discrepancy; keep the correct behavior.
    for i, s in enumerate(arg_arr):  # i: int, s: str
        if re.search(re.escape(find) + '$', s):
            return i
    return -1


def jl_arg_insert(arg_arr: list[str], insert: str, i: int) -> None:
    arg_arr.insert(i, insert)


def jl_arg_insert_after(arg_arr: list[str], insert: str, after: str) -> None:
    i: int = jl_arg_index(arg_arr, after)
    if i != -1:
        arg_arr.insert(i + 1, insert)


def jl_arg_replace(arg_arr: list[str], find: str, replace: str) -> None:
    i: int = jl_arg_index(arg_arr, find)
    if i != -1:
        arg_arr[i] = replace


def jl_arg_remove(arg_arr: list[str], remove: str) -> None:
    i: int = jl_arg_index(arg_arr, remove)
    if i != -1:
        del arg_arr[i:]


def jl_arg_is_const(arg_str: str) -> bool:
    """True if the argument declaration begins with a `const` qualifier."""
    return bool(re.match(r'^\s*const', arg_str)) or bool(re.match(r'^\s*\w*\s*const', arg_str))


def jl_arg_get_rettype(arg_str: str) -> str:
    # Intentional difference from jlib.pl: Perl removes only the first
    # `const`; this removes all. Equivalent on every current header, and this
    # helper is not used by any ported generator.
    s: str = re.sub(r'const', '', arg_str)
    m: re.Match[str] | None = re.match(r'^\s*(\w*)', s)
    if m:
        return m.group(1)
    return ''


def jl_arg_get_var(arg_str: str) -> str:
    """Extract the trailing variable name of an argument declaration."""
    m: re.Match[str] | None = re.search(r'(\w+)\s*$', arg_str)
    if m:
        return m.group(1)
    return ''


def jl_arg_is_ptr_ptr(arg_str: str) -> bool:
    # Intentional difference from jlib.pl (`\*(?:.|\n)*\*`): a star count is
    # equivalent for every real argument list in the current headers.
    return arg_str.count('*') >= 2


def jl_arg_is_ptr(arg_str: str) -> bool:
    return '*' in arg_str


def jl_fn_get(block_str: str, want_body: bool = False) -> FnParts | None:
    """Parse one function block.

    Returns `(attrs, rettype, name, arg_list[, body])`, or None when the
    block is not a function definition (e.g. control-flow statements).
    """
    m: re.Match[str] | None = JL_FN_RE.match(block_str)
    if not m:
        return None
    attr: str = m.group(1)
    rettype: str = m.group(2)
    name: str = m.group(3)
    arg_arr: list[str] = jl_arg_to_array(m.group(4))
    body: str | None
    if want_body:
        body = m.group(5)
    else:
        body = None

    kws: set[str] = {'if', 'else if', 'switch', 'for', 'while'}
    if name in kws:
        return None

    return (attr, rettype, name, arg_arr, body)


def jl_fn_to_string(attr: str, rettype: str, name: str, arg_arr: list[str], body: str | None = None) -> str:
    """Reassemble a parsed function into its textual form."""
    arg: str = jl_arg_to_string(arg_arr)
    s: str = f"{attr}\n{rettype}\n{name}({arg})"
    if body is not None:
        s += f" {{ {body} }}"
    return s


def jl_name_prepend(name: str, prefix: str) -> str:
    return prefix + name


def jl_name_append(name: str, suffix: str) -> str:
    return name + suffix


def jl_file_to_blocks(file_str: str) -> list[str]:
    """Split a header file into blocks separated by blank lines."""
    return file_str.split('\n\n')
