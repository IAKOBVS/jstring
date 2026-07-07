import re
import sys
from typing import NoReturn


JL_FN_RE: re.Pattern[str] = re.compile(
    r'^([^{}]*(?:^|\W))'          # 1 — pre-return attrs
    r'(\w+\s*[* \t\n]*)'          # 2 — return type
    r'\s+'
    r'(\w+)'                      # 3 — function name
    r'\s*\(([^{}]*)\)'            # 4 — args
    r'[^{}}]*'                    # post-args attrs (no braces)
    r'\{((?:.|\n)*)\}'            # 5 — body
)


def jl_file_get_str(fname: str) -> str:
    with open(fname) as fh:
        return fh.read()


def jl_file_namespace_macros(file_str: str, prefix: str, ignore_prefix: list[str]) -> str:
    """
    Find all macros that appear in `#undef` directives and,
    unless they already match an `ignore_prefix`, rename every
    bare occurrence to `prefix + macro`.
    """
    lines = file_str.split('\n')
    macros: set[str] = set()
    for line in lines:
        m = re.match(r'^[ \t]*#[ \t]*undef[ \t]+([_A-Z0-9]+)', line)
        if m:
            macro = m.group(1)
            if not any(macro.startswith(p) for p in ignore_prefix):
                macros.add(macro)
    for macro in macros:
        file_str = re.sub(
            r'([^\'_\"\w]|^)' + re.escape(macro) + r'([^\'_\"\w]|$)',
            r'\1' + prefix + macro + r'\2',
            file_str,
        )
    return file_str


def jl_file_tidy_newlines(file_str: str) -> str:
    file_str = re.sub(r'(\n[ \t].*)\n\n\n*', r'\1\n', file_str)
    file_str = re.sub(r'\n\n\n*\t', '\n\t', file_str)
    file_str = re.sub(r'\n\n\n* ', '\n ', file_str)
    return file_str


def jl_arg_to_array(arg_str: str) -> list[str]:
    return [x.strip() for x in arg_str.split(',')]


def jl_arg_to_string(arg_arr: list[str]) -> str:
    return ', '.join(arg_arr)


def jl_arg_push(arg_arr: list[str], add: str) -> None:
    arg_arr.append(add)


def jl_arg_index(arg_arr: list[str], find: str) -> int:
    for i, s in enumerate(arg_arr):
        if re.search(re.escape(find) + '$', s):
            return i
    return -1


def jl_arg_insert(arg_arr: list[str], insert: str, i: int) -> None:
    arg_arr.insert(i, insert)


def jl_arg_insert_after(arg_arr: list[str], insert: str, after: str) -> None:
    i = jl_arg_index(arg_arr, after)
    if i != -1:
        arg_arr.insert(i + 1, insert)


def jl_arg_replace(arg_arr: list[str], find: str, replace: str) -> None:
    i = jl_arg_index(arg_arr, find)
    if i != -1:
        arg_arr[i] = replace


def jl_arg_remove(arg_arr: list[str], remove: str) -> None:
    i = jl_arg_index(arg_arr, remove)
    if i != -1:
        del arg_arr[i:]


def jl_arg_is_const(arg_str: str) -> bool:
    return bool(re.match(r'^\s*const', arg_str)) or bool(re.match(r'^\s*\w*\s*const', arg_str))


def jl_arg_get_rettype(arg_str: str) -> str:
    s = re.sub(r'const', '', arg_str)
    m = re.match(r'^\s*(\w*)', s)
    return m.group(1) if m else ''


def jl_arg_get_var(arg_str: str) -> str:
    m = re.search(r'(\w+)\s*$', arg_str)
    return m.group(1) if m else ''


def jl_arg_is_ptr_ptr(arg_str: str) -> bool:
    return arg_str.count('*') >= 2


def jl_arg_is_ptr(arg_str: str) -> bool:
    return '*' in arg_str


def jl_fn_get(block_str: str, want_body: bool = False) -> tuple[str, str, str, list[str], str | None] | None:
    m = JL_FN_RE.match(block_str)
    if not m:
        return None
    attr = m.group(1)
    rettype = m.group(2).strip()
    name = m.group(3)
    arg_arr = jl_arg_to_array(m.group(4))
    body = m.group(5) if want_body else None

    kws = {'if', 'else if', 'switch', 'for', 'while'}
    if name in kws:
        return None

    return (attr, rettype, name, arg_arr, body)


def jl_fn_to_string(attr: str, rettype: str, name: str, arg_arr: list[str], body: str | None = None) -> str:
    arg = jl_arg_to_string(arg_arr)
    s = f"{attr}\n{rettype}\n{name}({arg})"
    if body is not None:
        s += f" {{ {body} }}"
    return s


def jl_name_prepend(name: str, prefix: str) -> str:
    return prefix + name


def jl_name_append(name: str, suffix: str) -> str:
    return name + suffix


def jl_file_to_blocks(file_str: str) -> list[str]:
    return file_str.split('\n\n')
