# jstring — agent guidance

## Build system

Custom shell+Perl scripts (no Makefile, no CMake). Run from repo root:

```sh
./build                 # generate code into jstr/ (gitignored)
./test                  # run all tests (4 variants each, via scripts/test)
./clean                 # rm -rf jstr/*.h
./fmt                   # clang-format (skips _jstr* and *macros* files)
./install               # sudo cp jstr/ → /usr/local/include/jstr
./install-to <dir>      # install to custom prefix
```

`./build` runs `scripts/setup` first (checks page size, undefined macros, scoped macros), then generates headers via Perl (`gen-func.pl` + `namespace-macros.pl`).

## Code generation

- `include/*.h` are the source-of-truth headers; `jstr/*.h` are generated.
- `gen-func.pl` converts `JSTR_FUNC`/`JSTR_FUNC_VOID` annotations into `static inline` functions and generates `jstr_*` wrappers from annotated function blocks.
- **Do not put blank lines inside function bodies.** The Perl codegen splits blocks by blank lines.
- `namespace-macros.pl` renames `NAMESPACE_INTERNAL_*` → `jstr_internal_*` etc.

## Testing

```sh
./test                  # all tests, 4 combos: {normal, -march=native} × {fast, -DJSTR_TEST_SLOW=1}
scripts/test1 <tmpdir> <test.c> [cflags]  # compile & run a single test
```

- Every test in `tests/*.c` is compiled and run from `/tmp`.
- Default CFLAGS: `-std=c99 -Wall -Wextra -Wpedantic -O2 -g -fsanitize=address`.
- All 4 variants run in parallel; tests can take a while.
- Order: `./build && ./test`.

## Language & toolchain

- **C99** (`-std=c99`). Flags: `-Wall -Wextra -Wpedantic -Wsign-conversion`.
- Format: `.clang-format` (WebKit brace style, 8-wide tab indentation, Never sort includes).
- If `tcc` is available, `./build` uses it for a syntax check instead of `cc`.
- libc requirement: musl's twoway `memmem` is vendored. POSIX headers (`io.h`, `regex.h`) must be included explicitly by the user.

## Key conventions

| Pattern | Meaning |
|---|---|---|
| `_len` | takes explicit length parameter |
| `_p` / `*stp*` | returns pointer to end of processed string (like stpcpy) |
| `_unsafe` | assumes buffer has enough space (no realloc) |
| `_from` | operates starting from a given index (in-bounds assumed) |
| `_mem*` | string need not be NUL-terminated (takes void pointer) |
| `_backref` | replacement string supports `\0` backreferences |
| `_at` | indexed access (bounds-checked when `JSTR_DEBUG=1`) |
| `_chk*` | evaluates to true if value is an error |
| Functions without `_len` on the `str` arg | string must be NUL-terminated; size param only optimizes strlen |

**`char *` convention**: parameters of type `char *` imply no embedded NUL bytes
(`strlen(str) ≤ *sz`). Functions that take `char *` + `*sz` may use `strlen`,
`strcspn`, `strspn`, `memchr`, etc. internally and can stop at any NUL they
encounter. For data that may contain embedded NULs, use `_mem*` variants or cast
to `void *` and use the corresponding `_mem*` / `void *` overload when available.

**Always zero-initialize**: `jstr_ty j = JSTR_INIT;`

**Error handling**: functions return negative on error. Check return values. Use `jstr_err()` to print or `jstr_errdie()` to print and exit. Set `JSTR_PANIC=1` to auto-abort on errors.

**Namespacing**: everything uses `[Jj][Ss][Tt][Rr]_` prefix.

## Caveats

- `jstr_tolower()` / `jstr_toupper()` misbehave on `EOF`.
- Do not pass strings with embedded NULs to functions taking `char *` (use `void *` ones instead).
- Functions annotated `_len` that take `char *` + `*sz` may still stop at
  embedded NULs via `strlen`, `strcspn`, `strspn`, or `memchr` (see `char *`
  convention above). Use `_mem*` variants for binary-safe operations.
- `internal/*.h` are internal headers.
- Config macros go in `config.h` before including `jstr.h` (e.g., `JSTR_USE_UNLOCKED_IO`, `JSTR_DEBUG`, `JSTR_PANIC`).

## License

MIT © 2023–2026 James Tirta Halim.
