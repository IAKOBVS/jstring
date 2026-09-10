#!/usr/bin/env python3
"""Generate `jstr_*` wrapper functions from annotated function blocks.

Port of `scripts-perl/gen-func.pl`, invoked by the Makefile and
`scripts/compile`. Two rewrite passes over the source header:

  Pass 1 — for every `foo_len(...)` function that takes a `size_t` argument
           named `*_len` (or a bare `*sz`), emit a convenience `foo(...)`
           wrapper that computes the length with `strlen`.
  Pass 2 — for every `foo(s, sz[, cap])`-style function, emit a `foo_j(...)`
           wrapper that operates directly on a `jstr_ty` argument `j`.

The passes are pure string transforms; emitted layout is preserved exactly
so output stays byte-identical to the Perl engine. Run
`scripts-perl/check-py-parity` after any change.
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from jlib import (
    jl_file_get_str, jl_file_to_blocks, jl_fn_get, jl_fn_to_string,
    jl_arg_get_var, jl_arg_index, jl_arg_is_ptr_ptr, jl_arg_is_ptr,
    jl_arg_is_const, FnParts,
)

# Attributes and type names used in the generated wrappers.
ATTR_NOINLINE: str     = 'JSTR_ATTR_NOINLINE'
ATTR_INLINE: str       = 'JSTR_ATTR_INLINE'
ATTR_ACCESS: str       = 'JSTR_ATTR_ACCESS'
ATTR_RESTRICT: str     = 'JSTR_RESTRICT'
ATTR_DEFAULT: str      = 'JSTR_FUNC'
ATTR_DEFAULT_VOID: str = 'JSTR_FUNC_VOID'
ATTR_RET_NONNULL: str  = 'JSTR_FUNC_RET_NONNULL'
JSTRING: str           = 'jstr_ty'
VAR_JSTRING: str       = 'j'
DATA: str              = 'data'
SIZE: str              = 'size'
CAP: str               = 'capacity'
VAR_DATA: str          = 's'
VAR_SIZE: str          = 'sz'
VAR_CAP: str           = 'cap'
PREFIX_J: str          = '_j'
PREFIX_LEN: str        = '_len'


def add_inline(attr_str: str) -> str:
    """Turn a non-static attribute block into a static inline one."""
    if 'static' not in attr_str and ATTR_INLINE not in attr_str:
        attr_str = re.sub(re.escape(ATTR_NOINLINE), '', attr_str)
        attr_str += '\nstatic ' + ATTR_INLINE
    return attr_str


def clean_attr(attr: str, ret_nonnull_to_void: bool = False) -> str:
    """Normalize a wrapper's attribute block, then force static inline.

    Both passes drop preprocessor lines and `JSTR_ATTR_ACCESS` and collapse
    blank lines; pass 2 first rewrites `JSTR_FUNC_RET_NONNULL` to
    `JSTR_FUNC_VOID` so `_j()` wrappers do not warn on unused results.
    """
    if ret_nonnull_to_void:
        attr = re.sub(re.escape(ATTR_RET_NONNULL), ATTR_DEFAULT_VOID, attr)
    attr = re.sub(r'^[ \t]*#.*$', '', attr, flags=re.MULTILINE)
    attr = re.sub(r'\s*' + re.escape(ATTR_ACCESS) + r'\(\(.*?\)\)', '', attr)
    attr = re.sub(r'\n\n', '', attr)
    attr = re.sub(r'\n$', '', attr)
    return add_inline(attr)


def contains_substr(arr: list[str], s: str) -> bool:
    """True if any element of `arr` contains the substring `s`."""
    return any(s in elem for elem in arr)


def pass1_len_wrappers(file_str1: str, used_names: list[str]) -> str:
    """Emit non-_len() wrappers for *_len() functions; return the result."""
    out: str = ''
    for block in jl_file_to_blocks(file_str1):  # block: str
        out += block + '\n\n'
        result: FnParts | None = jl_fn_get(block)
        if not result:
            continue
        attr: str
        rettype: str
        name: str
        arg_list: list[str]
        attr, rettype, name, arg_list, _ = result
        # Only jstr-prefixed names and non-variadic, non-empty arg lists.
        if not re.match(r'^jstr\w*_', name):
            continue
        if contains_substr(arg_list, '..'):   # variadic
            continue
        if len(arg_list) == 0:
            continue
        used_names.append(name)
        # Only *_len() functions get a wrapper.
        if PREFIX_LEN not in name:
            continue
        if name.endswith(PREFIX_J):
            continue
        base_name: str = re.sub(re.escape(PREFIX_LEN) + r'(_|$)', r'\1', name)
        # Skip if a wrapper already exists in the source.
        if base_name + '(' in file_str1:
            continue
        # Build the non-_len() function body, replacing size params with
        # strlen() calls.
        body: str = '' if rettype == 'void' else 'return '
        body += name + '('
        name = base_name
        replaced_size: bool = False

        i: int = 0
        while i < len(arg_list):
            arg: str = arg_list[i]
            var: str = jl_arg_get_var(arg)
            if 'size_t' in arg:
                # size_t var named `*_len`: derive length from the buffer
                # argument whose name is `var` without the `_len` suffix.
                if re.search(re.escape(PREFIX_LEN) + '$', var):
                    replaced_size = True
                    base_var: str = re.sub(re.escape(PREFIX_LEN) + r'(_|$)', r'\1', var)
                    buf_idx: int = jl_arg_index(arg_list, base_var)
                    if buf_idx != -1:
                        var_str: str = jl_arg_get_var(arg_list[buf_idx])
                        body += "strlen((const char *)" + var_str + ")"
                        arg_list.pop(i)
                        body += ', '
                        continue
                # Bare `*sz` size param: length comes from the first arg.
                elif re.match(r'^[^*]' + re.escape(VAR_SIZE) + '$', var):
                    replaced_size = True
                    var_str = jl_arg_get_var(arg_list[0])
                    body += "strlen((const char *)" + var_str + ")"
                    arg_list.pop(i)
                    body += ', '
                    continue
            body += var
            body += ', '
            i += 1

        # No size param was replaced — nothing to wrap.
        if not replaced_size:
            continue
        body = re.sub(r', $', '', body)
        body += ');'
        out += jl_fn_to_string(clean_attr(attr), rettype, name, arg_list, body) + '\n\n'
        used_names.append(name)
    return out


def pass2_j_wrappers(file_str2: str, used_names: list[str]) -> str:
    """Emit `_j()` wrappers that operate on a `jstr_ty`; return the result."""
    out: str = ''
    for block in jl_file_to_blocks(file_str2):  # block: str
        out += block + '\n\n'
        result: FnParts | None = jl_fn_get(block)
        if not result:
            continue
        attr, rettype, name, arg_list, _ = result
        if not re.match(r'^jstr\w*_', name):
            continue
        if name.endswith(PREFIX_J):
            continue
        if len(arg_list) == 0:
            continue
        if 'unsafe' in name:
            continue
        # Only functions taking a size and/or capacity param qualify.
        has_size_or_cap: bool = False
        has_variadic: bool = False
        for a in arg_list:  # a: str
            if a.endswith(VAR_SIZE) or a.endswith(VAR_CAP):
                has_size_or_cap = True
            elif '...' in a:
                has_variadic = True
                break
        if not has_size_or_cap or has_variadic:
            continue
        j_name: str = name + PREFIX_J
        # Skip if the _j() name already exists.
        if contains_substr(used_names, j_name):
            continue
        returns_end_ptr: bool = False
        body: str
        if rettype == 'void':
            body = ''
        else:
            # *_p() functions return an end pointer; convert to a jstr_ty
            # method that stores the offset in j->size.
            if re.search(r'_p(?:_|$)', name) and rettype != 'void':
                returns_end_ptr = True
                body = VAR_JSTRING + '->' + SIZE + ' = JSTR_DIFF('
                rettype = 'void'
                attr = re.sub(re.escape(ATTR_DEFAULT) + r'(\W|$)', ATTR_DEFAULT_VOID + r'\1', attr)
            elif contains_substr(used_names, name + '_p'):
                continue
            else:
                body = 'return '
        body += name + '('
        name = j_name
        name = re.sub(r'_p(_|$)', r'\1', name)
        # Rewrite arguments: the `data`/`size`/`capacity` params become
        # j->data / j->size / j->capacity expressions.
        has_data: bool = False
        j: int = 0
        while j < len(arg_list):
            arg = arg_list[j]
            var = jl_arg_get_var(arg)
            if var == VAR_DATA:
                has_data = True
                deref: str
                if jl_arg_is_ptr_ptr(arg):
                    deref = '&'
                else:
                    deref = ''
                body += deref + VAR_JSTRING + '->' + DATA
                const_str: str = ''
                if jl_arg_is_const(arg):
                    const_str = 'const '
                arg_list[j] = const_str + JSTRING + ' *' + ATTR_RESTRICT + ' j'
            elif var == VAR_SIZE:
                deref = ''
                if jl_arg_is_ptr(arg):
                    deref = '&'
                body += deref + VAR_JSTRING + '->' + SIZE
                arg_list.pop(j)
                body += ', '
                continue
            elif var == VAR_CAP:
                deref = ''
                if jl_arg_is_ptr(arg):
                    deref = '&'
                body += deref + VAR_JSTRING + '->' + CAP
                arg_list.pop(j)
                body += ', '
                continue
            else:
                body += var
            body += ', '
            j += 1
        # No data argument — not a jstr-style function.
        if not has_data:
            continue
        body = re.sub(r', $', '', body)
        body += ')'
        if returns_end_ptr:
            body += ', ' + VAR_JSTRING + '->' + DATA + ')'
        body += ';'
        out += jl_fn_to_string(clean_attr(attr, ret_nonnull_to_void=True), rettype, name, arg_list, body) + '\n\n'
        used_names.append(name)
    return out


def generate(file_str1: str) -> str:
    """Run both passes and collapse the trailing newlines to a single one."""
    used_names: list[str] = []
    file_str2: str = pass1_len_wrappers(file_str1, used_names)
    file_str3: str = pass2_j_wrappers(file_str2, used_names)
    return re.sub(r'\n\n*$', '\n', file_str3)


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit('Usage: ./' + sys.argv[0] + ' <file>')
    sys.stdout.write(generate(jl_file_get_str(sys.argv[1])))


if __name__ == '__main__':
    main()