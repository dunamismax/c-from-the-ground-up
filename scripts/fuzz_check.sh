#!/bin/sh
#
# Fuzz the two parsers that read files a learner writes by hand: lesson 34's
# data-file parser and lesson 35's world-map loader. Each target in fuzz/ is
# built with libFuzzer, AddressSanitizer and UndefinedBehaviorSanitizer and
# run for a fixed number of inputs, starting from the seed files in
# fuzz/corpus/<target>/ (the sample files the lessons tell the reader to
# create, plus the bad maps the other checks use).
#
# A crash, a sanitizer report or a leak fails the check. libFuzzer saves the
# input that caused it as crash-*, leak-* or timeout-* in $FUZZ_ARTIFACTS
# (default fuzz/artifacts/, git-ignored). To see the report again:
#
#   FUZZ_REPRODUCE=fuzz/artifacts/crash-... sh scripts/fuzz_check.sh world_map
#
# libFuzzer ships with Clang but not with Apple's Clang or with GCC. Where it
# is missing the check is skipped with a message, unless FUZZ_REQUIRED=1 (CI),
# which makes that an error.
#
# Usage:
#   sh scripts/fuzz_check.sh                    # both targets, 10000 runs each
#   FUZZ_RUNS=1000000 sh scripts/fuzz_check.sh  # a longer hunt
#   sh scripts/fuzz_check.sh data_parser        # one target

set -eu

CC=${CC:-clang}
FUZZ_RUNS=${FUZZ_RUNS:-10000}
FUZZ_REQUIRED=${FUZZ_REQUIRED:-0}
FUZZ_REPRODUCE=${FUZZ_REPRODUCE:-}
# Longer than any line buffer in the two lessons (256 and 1024 bytes), so the
# overlong-line paths are reached.
FUZZ_MAX_LEN=${FUZZ_MAX_LEN:-4096}
CFLAGS="-std=c17 -Wall -Wextra -Werror -g -O1 -fno-omit-frame-pointer"
FUZZ_FLAGS="-fsanitize=fuzzer,address,undefined -fno-sanitize-recover=undefined"

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FUZZ_DIR=$ROOT_DIR/fuzz
FUZZ_ARTIFACTS=${FUZZ_ARTIFACTS:-$FUZZ_DIR/artifacts}
BUILD_DIR=$(mktemp -d "${TMPDIR:-/tmp}/cftgu-fuzz.XXXXXX")
trap 'rm -rf "$BUILD_DIR"' EXIT INT TERM HUP

if [ "$#" -gt 0 ]; then
    TARGETS=$*
else
    TARGETS="data_parser world_map"
fi

fail() {
    echo "$1" >&2
    if [ -n "${2:-}" ]; then
        printf '%s\n' "$2" | tail -n 60 >&2
    fi
    exit 1
}

# Does this compiler have libFuzzer? Build and run the smallest target.
probe=$BUILD_DIR/probe.c
cat > "$probe" <<'PROBE'
#include <stddef.h>
#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    (void)data;
    (void)size;
    return 0;
}
PROBE
if ! "$CC" $CFLAGS $FUZZ_FLAGS "$probe" -o "$BUILD_DIR/probe" >/dev/null 2>&1 ||
    ! "$BUILD_DIR/probe" -runs=1 >/dev/null 2>&1; then
    if [ "$FUZZ_REQUIRED" = 1 ]; then
        fail "$CC cannot build a libFuzzer target (FUZZ_REQUIRED=1)."
    fi
    echo "Skipping the fuzz check: $CC has no libFuzzer (Apple's Clang and GCC do not ship it)."
    exit 0
fi

# Lesson 35 includes ncurses.h, so its target links ncurses although nothing
# in the loader calls it.
NCURSES_FLAG=
for flag in -lncursesw -lncurses; do
    printf '#include <ncurses.h>\nint main(void)\n{\n    return 0;\n}\n' > "$BUILD_DIR/curses.c"
    if "$CC" "$BUILD_DIR/curses.c" -o "$BUILD_DIR/curses" "$flag" >/dev/null 2>&1; then
        NCURSES_FLAG=$flag
        break
    fi
done

# LeakSanitizer does not run on every platform (not on macOS on Apple
# silicon); asking for it where it is missing stops the program at startup.
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"
if ASAN_OPTIONS=detect_leaks=1 "$BUILD_DIR/probe" -runs=1 >/dev/null 2>&1; then
    export ASAN_OPTIONS="detect_leaks=1"
else
    export ASAN_OPTIONS="detect_leaks=0"
fi

# The world-map target writes each input to this file and loads it.
export FUZZ_MAP_FILE="$BUILD_DIR/scratch.map"

for target in $TARGETS; do
    source=$FUZZ_DIR/fuzz_$target.c
    seeds=$FUZZ_DIR/corpus/$target
    binary=$BUILD_DIR/fuzz_$target
    [ -f "$source" ] || fail "No fuzz target named '$target' (expected $source)."
    [ -d "$seeds" ] || fail "No seed corpus for '$target' (expected $seeds/)."

    link_flags=
    case "$target" in
        world_map)
            [ -n "$NCURSES_FLAG" ] || fail "Lesson 35's fuzz target needs the ncurses headers."
            link_flags=$NCURSES_FLAG
            ;;
    esac

    output=$("$CC" $CFLAGS $FUZZ_FLAGS "$source" -o "$binary" $link_flags 2>&1) ||
        fail "fuzz_$target.c did not compile." "$output"

    if [ -n "$FUZZ_REPRODUCE" ]; then
        # Run one saved input and show everything it prints.
        "$binary" "$FUZZ_REPRODUCE"
        continue
    fi

    # New inputs the fuzzer finds go in a scratch corpus, so the committed
    # seeds stay as they are. -close_fd_mask=3 silences the lesson's own
    # printing (thousands of "Malformed line" warnings), not the fuzzer's.
    # -rss_limit_mb=0 stops libFuzzer starting its memory-watching thread:
    # when a short run ends while that thread is still starting,
    # LeakSanitizer reports the thread's own bookkeeping as a leak (seen on
    # macOS, in 3 runs of 20), and these parsers allocate almost nothing.
    mkdir -p "$BUILD_DIR/corpus_$target" "$FUZZ_ARTIFACTS"
    status=0
    output=$("$binary" -runs="$FUZZ_RUNS" -max_len="$FUZZ_MAX_LEN" -timeout=10 \
        -close_fd_mask=3 -rss_limit_mb=0 -print_final_stats=1 -artifact_prefix="$FUZZ_ARTIFACTS/" \
        "$BUILD_DIR/corpus_$target" "$seeds" 2>&1) || status=$?
    if [ "$status" -ne 0 ]; then
        fail "fuzz_$target found a failing input (saved under $FUZZ_ARTIFACTS/)." "$output"
    fi

    # A run that executed nothing would also "pass"; make sure it did the work.
    executed=$(printf '%s\n' "$output" | sed -n 's/^stat::number_of_executed_units:[[:space:]]*//p')
    case "$executed" in
        ''|*[!0-9]*)
            fail "fuzz_$target did not report how many inputs it ran." "$output"
            ;;
    esac
    if [ "$executed" -lt "$FUZZ_RUNS" ]; then
        fail "fuzz_$target ran only $executed of $FUZZ_RUNS inputs." "$output"
    fi
    printf 'fuzz_%s: %s inputs, no findings\n' "$target" "$executed"
done

if [ -z "$FUZZ_REPRODUCE" ]; then
    # Leave no empty directory behind when nothing was found.
    rmdir "$FUZZ_ARTIFACTS" 2>/dev/null || true
    echo "Fuzz check passed."
fi
