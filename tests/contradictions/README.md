# Contradiction reproductions

These are regression tests for the contradictions documented in
STATE_MACHINES.md at the repository root. They failed before the corresponding
library fixes and now pass. Run them with `./test contradictions`.

| Test | Doc ID | Contradiction reproduced | Observation before fix |
| --- | --- | --- | --- |
| builder-reserve-failure.c | BU-2 / RP-7 | reserveexactalways writes `*cap` before `realloc` | `cap == SIZE_MAX` after failed allocation; under ASan the huge realloc aborts earlier |
| jstr-at-debug.c | BU-5 | `jstr_at` comma-operator precedence discards the index; `#ifdef JSTR_DEBUG` is true because config.h defines it to 0 | returns `data` instead of `data + 4` |
| ftw-basename-len.c | IO-2 | basename `end` computed from the slash pointer, not the path base | `fname_len == 23` for a 1-byte basename |
| readstdin-short-read.c | IO-8 | short `read()` treated as EOF | second pipe write is lost (`size == 5`, expected 10) |
| regex-nmatch-clamp.c | RE-6 | caller-controlled `nmatch` forwarded to a fixed `regmatch_t rm[10]` | `nmatch = 1000` is not clamped (stack corruption) |
| readstdin-empty.c | IO-8 | empty input policy differs from `readsystem` | empty stdin returns success instead of error |

## Retracted reports

The following were initially reported but are not bugs under the documented
contracts; their repro tests were removed:

- `nmatch` > 10 and exec-after-free: POSIX regex callers own the slot-count and
  lifecycle contracts. `nmatch` is now defensively clamped, but passing an
  invalid value remains caller error.
- RE-3 (`sz == 0` on a non-terminated buffer): the `char *` convention requires
  `strlen(s) <= *sz`, so `sz == 0` implies `s[0] == '\0'`. The zero-length branch
  is an intentional REG_STARTEND workaround; only the doc comment needed
  clarification.
- FTW STOP returning 0: glibc `nftw(3)` returns `FTW_STOP` (which is 0) on STOP;
  jstr matches that behavior.

## Not reproduced here

- In-library memory errors (RP-4 `start_idx` underflow, SS-9 SIMD memmem
  precondition, RE-6 pointer rebase after `realloc`): `build/lib/libjstr.so`
  is built without `-fsanitize=address`, so out-of-bounds access inside the
  library is invisible to the test binary.
- Compile-time/configuration issues (RE-3 missing `JSTR_RE_EF_STARTEND`
  fallback, IO-1 `STAT_ALWAYS` under `JSTR_HAVE_DIRENT_D_TYPE=1`, SS-8 AVX-512
  case-conversion gap): these need cross-platform or cross-ISA builds.
- Generator-state issues (PG-1..PG-14): these concern `scripts/parser/jstrgen`
  and need fixture-driven generator tests.
- Counting NUL with `jstr_countchr` (SS-14) was investigated but the SIMD path
  stops at the terminator correctly in this build.
