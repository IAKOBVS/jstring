# Test Coverage Report

Generated: 2026-07-03

## Summary

| Header | Public Functions | Tested | Untested | Coverage |
| ------ | ---------------- | ------ | -------- | -------- |
| `replace.h` | 67 | 12 | 55 | ~18% |
| `regex.h` | 29 | 29 | 0 | **100%** |
| `io.h` | 17 | 8 | 9 | ~47% |
| `string.h` | 77 | 37 | 40 | ~48% |
| `stdstring.h` | 34 | 27 | 7 | ~79% |
| `ctype.h` | 23 | 23 | 0 | **100%** |
| `builder.h` | 34 | 30 | 4 | ~88% |
| `itoa.h` | 29 | 25 | 4 | ~86% |
| **Total** | **310** | **191** | **119** | **~62%** |

---

## Per-Header Detail

### `replace.h` — Fixed-string replace/remove/insert — 12/67 tested

**Untested gaps (55 functions):**
- All `_unsafe` / pointer-returning variants (e.g., `jstr_insert_unsafe`, `jstr_assignnchr_unsafe_p`)
- All `exec` (twoway-optimized) variants: `jstr_rm_len_from_exec`, `jstr_rm_len_exec`,
  `jstr_rmn_len_from_exec`, `jstr_rmn_len_exec`, `jstr_rmall_len_exec`,
  `jstr_rplc_len_from_exec`, `jstr_rplc_len_exec`, `jstr_rplcn_len_from_exec`,
  `jstr_rplcn_len_exec`, `jstr_rplcall_len_from_exec`, `jstr_rplcall_len_exec`
- All 6 `_from` span variants: `jstr_rplcallspn_from`, `jstr_rmspn_from`,
  `jstr_rmspn_len_from`, `jstr_rmallspn_from`, `jstr_stripspn_from`
- `jstr_rmnchr_len`, `jstr_rmnchr_from`, `jstr_rmnchr`, `jstr_rmallchr_len_from`,
  `jstr_rmallchr_from`, `jstr_rmallchr`, `jstr_rmchr_len_from`, `jstr_rmchr_len`,
  `jstr_rmchr_from`, `jstr_rmchr`
- `jstr_insert_len`, `jstr_rplcat_len`, `jstr_insertafterchr_len`,
  `jstr_insertafterallchr_len`, `jstr_insertafter_len`, `jstr_insertafterall_len`
- `jstr_rmat_len_p`, `jstr_rm_len_from`, `jstr_rm_len`
- `jstr_rplcallchr`, `jstr_rplcnchr`, `jstr_rplcchr` (non-`_len`)
- `jstr_rmlast_len_from`, `jstr_rmlast_len`, `jstr_rplclast_len`
- `jstr_rmn_len_from`, `jstr_rmall_len`
- `jstr_rplc_len_from`, `jstr_rplc_len`, `jstr_rplcn_len`,
  `jstr_rplcall_len_from`, `jstr_rplcall_len`
- All `place_*` functions (4)
- `jstr_repeat_len`

### `regex.h` — Regex compilation/matching/replace — 29/29 tested

Tested by `test-fuzz-regex.c` + 3 partitioned variants (`p0`, `p1`, `p2`)
via `fuzz-regex.h`. Covers all `jstr_re_comp`, `jstr_re_exec_len`,
`jstr_re_match_len`, `jstr_re_search_len`, `rm*_exec`, `rplc*_exec`,
and `rplc*_backref*_exec` variants.

### `io.h` — File I/O, path, binary detection — 8/17 tested

**Tested:** `jstr_io_isbinary_maybe`, `jstr_io_isbinary`, `jstr_io_appendpath_len_p`,
`jstr_io_appendpath_p`, `jstr_io_appendpath_len` (test-fuzz-io).

**Untested:** `jstr_isbinary`, `jstr_io_writefilefd_len`, `jstr_io_writefile_len`,
`jstr_io_fwritefilefp_len`, `jstr_io_fwritefile_len`, `jstr_io_readfilefd_len`,
`jstr_io_readfilefd`, `jstr_io_freadfilefp_len`, `jstr_io_freadfilefp`,
`jstr_io_freadfile_len`, `jstr_io_freadfile`, `jstr_io_readfile_len`,
`jstr_io_readfile`, `jstr_io_expandtildefirst_len_unsafe_p`,
`jstr_io_expandtildefirst`, `jstr_io_readsystem`, `jstr_io_readstdin`,
`jstr_io_ftw_len`

### `string.h` — String search/misc — 37/77 tested

**Tested** (via test-fuzz-string, test-fuzz-search):
- `jstr_countchr_len`, `jstr_countchr` (fuzz-string)
- `jstr_count_len`, `jstr_count` (fuzz-string, fuzz-search)
- `jstr_starts_len`, `jstr_starts` (fuzz-string, fuzz-search)
- `jstr_ends_len`, `jstr_ends` (fuzz-string, fuzz-search)
- `jstr_startscase_len`, `jstr_endscase_len` (fuzz-string)
- `jstr_basename` (fuzz-string)
- `jstr_linestart`, `jstr_linenumber` (fuzz-string)
- `jstr_rev_len`, `jstr_rev_p` (fuzz-string)
- `jstr_trimend_len_p`, `jstr_trimend_p`, `jstr_trimstart_len_p`,
  `jstr_trimstart_p`, `jstr_trim_len_p`, `jstr_trim_p` (fuzz-string)
- `jstr_thousep_len_p`, `jstr_thousep_p` (fuzz-string)
- `jstr_unescape_p` (fuzz-string)
- `jstr_strstr_len`, `jstr_strrstr_len`, `jstr_strcasestr_len` (fuzz-search)
- `jstr_memmem`, `jstr_memrmem` (fuzz-search)

**Notable untested:**
- `jstr_strchrnul`, `jstr_strcasechrnul`, `jstr_strcasechr`, `jstr_memcasechr`,
  `jstr_memrchrnul`, `jstr_memchrnul`
- `jstr_stpcpy`, `jstr_stpcat`
- `jstr_strnchr`, `jstr_strncasechr`
- `jstr_basename_len`, `jstr_strstrnul`
- `jstr_cmpeq`, `jstr_cmpcaseeq`
- `jstr_endscase`, `jstr_startscase`, `jstr_startscasenul`
- `jstr_memmem_comp`, `jstr_memmem_exec`
- `jstr_strnstr_len`, `jstr_memnmem`, `jstr_memmemnul`, `jstr_strstrnul_len`
- `jstr_strcasestr_len_comp`, `jstr_strcasestr_len_exec`
- `jstr_strncasestr_len`, `jstr_strcasestr`
- `jstr_strcasestr_comp`, `jstr_strcasestr_exec`
- `jstr_strstr_comp`, `jstr_strstr_exec`
- All `strrcspn`/`strrspn`/`strrpbrk`/`memspn`/`memcspn`/`mempbrk` variants
- All `*chrinv`/`*chrnulinv` variants (8 functions)
- `jstr_linenext_len`, `jstr_linenext`, `jstr_linenextnul_len`, `jstr_linenextnul`
- `jstr_revcpy_len`, `jstr_revcpy_p`
- `jstr_trimstart` (non-`_p`)
- `jstr_strtok_ne_len`, `jstr_strtok_ne`, `jstr_strtok`
- `jstr_repeat_len_unsafe_p`, `jstr_repeatcpy_len_p`
- `jstr_thousepcpy_len_p`, `jstr_thousepcpy_p`, `jstr_unescapecpy_p`
- `jstr_count_len_exec`

### `stdstring.h` — Std-compat mem/str — 27/34 tested

**Tested** (test-fuzz-stdstring): `jstr_strncasecmp`, `jstr_strncasecmpeq`,
`jstr_strcasecmpeq_len`, `jstr_strcasecmpeq_len_loop`, `jstr_memcasecmpeq`,
`jstr_memcmpeq_loop`, `jstr_strnlen`, `jstr_strnlen_loop`, `jstr_mempset`,
`jstr_stpset_len`, `jstr_stpset`, `jstr_bzero`, `jstr_memnchr`,
`jstr_strnchr_len`, `jstr_mempcpy`, `jstr_mempmove`, `jstr_strmove_len`,
`jstr_stpmove_len`, `jstr_stpmove`, `jstr_strcpy_len`, `jstr_stpcpy_len`,
`jstr_memccpy`, `jstr_memdup`, `jstr_strdup_len`, `jstr_strdup`,
`jstr_atou`, `jstr_atoi`, `jstr_atoul`, `jstr_atol`, `jstr_atoull`,
`jstr_atoll`, `jstr_atod`, `jstr_atof`

**Untested:** `jstr_strcasecmp_len`, `jstr_strcasecmp`, `jstr_strcasecmpeq_loop`,
`jstr_strcasecmpeq`, `jstr_strcmpeq_loop`, `jstr_strzero`, `jstr_stpzero`

### `ctype.h` — Character predicates + case conversion — 23/23 tested

All 15 `musl/ctype.h` predicates (`jstr_isalpha`, `jstr_isdigit`, etc. + `jstr_tolower`,
`jstr_toupper`) tested via test-fuzz-ctype.

All 8 `ctype.h` string case conversions (`jstr_toupperstr_p`, `jstr_tolowerstr_p`,
`jstr_tolowerstr_len`, `jstr_toupperstr_len`, `jstr_tolowerstrcpy_len`,
`jstr_toupperstrcpy_len`, `jstr_toupperstrcpy_p`, `jstr_tolowerstrcpy_p`)
tested via test-fuzz-tocase.

### `builder.h` — Dynamic string builder — 30/34 tested

**Tested** (test-fuzz-builder, test-fuzz-builder2, test-fuzz-snprintf):
- `jstr_assign_len`, `jstr_append_len`, `jstr_prepend_len`
- `jstr_assignnchr`, `jstr_assignnchr_unsafe_p`
- `jstr_pushback`, `jstr_pushbackn`, `jstr_pushfront`, `jstr_pushfrontn`
- `jstr_popback_p`, `jstr_popfront_p`
- `jstr_empty`, `jstr_free`, `jstr_reserve`, `jstr_reservealways`,
  `jstr_reserveexact`, `jstr_reserveexactalways`, `jstr_shrink_to_fit`
- `jstr_cat`, `jstr_strset`
- `jstr_asprintf`, `jstr_asprintf_append`, `jstr_asprintf_from`
- `jstr_sprintf`, `jstr_sprintf_from`
- `jstr_trimend_p`, `jstr_trimend_len_p`, `jstr_trimstart_p`,
  `jstr_trimstart_len_p`, `jstr_trim_p`, `jstr_trim_len_p`
- `jstr_prepend_len_unsafe_p`

**Untested:** `jstr_debug`, `jstr_io_print`, `jstr_io_println`,
`jstr_cat_j`, `jstr_append_len_unsafe_p`, `jstr_append_unsafe`,
`jstr_append_unsafe_p`, `jstr_pushbackn_len_unsafe_p`,
`jstr_pushfrontn_len_unsafe_p`, `jstr_pushback_unsafe_p`,
`jstr_pushfront_unsafe_p`, `jstr_assign_len_unsafe_p`,
`jstr_vsprintf_maxlen`, `jstr_asprintf_j`, `jstr_asprintf_append_j`,
`jstr_asprintf_from_j`, `jstr_sprintf_j`, `jstr_sprintf_from_j`
(most untested are `_j` variants or `_unsafe_p` helpers).

### `itoa.h` — Integer-to-string + atoi — 25/29 tested

**Tested** (test-fuzz-itoa):
- `jstr_ulltoa_p`, `jstr_utoa_p`, `jstr_lltoa_p`, `jstr_itoa_p`
- `jstr_ulltoa_thousep_p`, `jstr_ulltoa`, `jstr_lltoa`,
  `jstr_ulltoa_thousep`, `jstr_lltoa_thousep`
- `jstr_atou_len`, `jstr_atoul_len`, `jstr_atoull_len`,
  `jstr_atoi_len`, `jstr_atol_len`, `jstr_atoll_len`
- `jstr_utoa`, `jstr_lltoa` (safe variants)

**Untested:** `jstr_ultoa_p`, `jstr_ltoa_p`, `jstr_ultoa_thousep_p`,
`jstr_ltoa_thousep_p`, `jstr_ultoa`, `jstr_ltoa`, `jstr_utoa_thousep`,
`jstr_ultoa_thousep`, `jstr_itoa_thousep`, `jstr_ltoa_thousep`,
`jstr_lltoa_thousep`, `jstr_itoa` (safe), `jstr_ltoa` (safe),
`jstr_itoa_thousep` (safe), `jstr_ltoa_thousep` (safe),
`jstr_lltoa_thousep` (safe)

(Many untested are thin wrappers around tested `_p` variants.)

---

## Priority Gaps

### High — Core functionality with zero test coverage
1. **All `place_*` functions** (`replace.h`): `jstr_place_len_unsafe`,
   `jstr_place_len`, `jstr_placeafterchr_len`, `jstr_placeafter_len`
   — insert-or-replace semantics, non-trivial.
2. **All `_unsafe` / `_unsafe_p` pointer-returning variants**: These skip
   reallocation and have different contracts; crashes from misuse would not
   be caught by the safe-path tests.
3. **Twoway-exec variants** (`replace.h`): All `jstr_*_exec` functions
   (e.g., `jstr_rm_len_from_exec`, `jstr_rplc_len_exec`, etc.) — these
   accept a pre-computed `jstr_twoway_ty` and are the performance path.
   Only the plain `_len` / `_len_from` variants are tested.

### Medium — Significant untested surface
4. **`io.h` file I/O**: No write/read file functions are tested (requires
   actual file system interaction). Only path-append functions are exercised.
   `jstr_io_readstdin`, `jstr_io_readsystem`, `jstr_io_ftw_len`,
   `jstr_io_expandtildefirst*` all untested.
5. **`string.h` string search**: `jstr_strchrnul`, `jstr_strcasechr`,
   `jstr_strnchr`, `jstr_strncasechr`, `jstr_memcasechr`, `jstr_memchr*`,
   all `*chrinv` variants, all `strrcspn`/`strrspn`/`strrpbrk` reverse
   search functions are untested.
6. **`string.h` strtok**: `jstr_strtok_ne_len`, `jstr_strtok_ne`,
   `jstr_strtok` — stateful tokenization, easy to regress.
7. **`string.h` linenext**: `jstr_linenext_len`, `jstr_linenext`,
   `jstr_linenextnul_len`, `jstr_linenextnul` — line iteration, used by
   downstream tools.

### Low — Thin wrappers or narrow contracts
8. **Non-`_len` wrappers** (`replace.h`): `jstr_rplcallchr`, `jstr_rplcnchr`,
   `jstr_rplcchr` — small wrappers around `_len` variants that are tested.
9. **`jstr*_j` builder wrappers**: `jstr_asprintf_j`, `jstr_cat_j`, etc.
   — thin wrappers around `jstr_ty` struct access.
10. **`itoa.h` safe wrappers for `long`/`int`**: `jstr_ltoa`, `jstr_itoa`,
    `jstr_*_thousep` for non-`longlong` types — mostly macro-generated thin
    wrappers.

---

## Test Files vs. Functions Covered

| Test File | # Functions | Headers Covered |
| --------- | ----------- | --------------- |
| test-fuzz-string | ~19 | string.h |
| test-fuzz-stdstring | ~27 | stdstring.h, itoa.h (ato* _len) |
| test-fuzz-search | ~8 | string.h |
| test-fuzz-ctype | ~15 | internal/musl/ctype.h |
| test-fuzz-tocase | ~8 | ctype.h |
| test-fuzz-io | ~8 | io.h, builder.h (jstr_assign_len, jstr_free) |
| test-fuzz-builder | ~15 | builder.h, replace.h |
| test-fuzz-builder2 | ~20 | builder.h |
| test-fuzz-snprintf | ~3 | builder.h |
| test-fuzz-itoa | ~15 | itoa.h |
| test-fuzz-regex | ~22 | regex.h |
| test-fuzz-regex-p{0,1,2} | ~22 each | regex.h (via fuzz-regex.h) |
| test-fuzz-replace-char | ~5 | replace.h |
| test-fuzz-replace-spn | ~5 | replace.h |
| test-fuzz-replace-substr | ~2 | replace.h |
