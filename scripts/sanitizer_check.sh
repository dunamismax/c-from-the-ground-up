#!/bin/sh
#
# Run the lessons under the sanitizers, to catch what the smoke check's output
# assertions cannot see:
#
#   address  AddressSanitizer + UndefinedBehaviorSanitizer + LeakSanitizer on
#            every lesson that runs without a terminal, each fed a canned input
#            (valid input, and bad input for the interactive lessons). Lesson
#            35's loader runs through a harness, since the game itself needs a
#            terminal.
#   memory   MemorySanitizer (Clang on Linux) on the same runs: reads of
#            uninitialized memory. A garbage value can happen to print the
#            right answer, which is how the old lesson 13 passed the smoke
#            check.
#   thread   ThreadSanitizer on lesson 30, the only multithreaded lesson.
#
# A sanitizer the compiler or platform does not support is skipped with a
# message, unless SANITIZERS_REQUIRED=1 (CI on Linux), which makes that an
# error. LeakSanitizer is part of the address run where the platform has it.
#
# Usage:
#   sh scripts/sanitizer_check.sh                 # all three, with $CC (clang)
#   SANITIZERS=thread sh scripts/sanitizer_check.sh
#   CC=clang-20 SANITIZERS_REQUIRED=1 sh scripts/sanitizer_check.sh
#
# Lessons 26 (sockets) and 33 (ncurses) are not run here: 26 is covered by the
# smoke check's exchange test, and 33 needs a terminal.

set -eu

CC=${CC:-clang}
SANITIZERS=${SANITIZERS:-address memory thread}
SANITIZERS_REQUIRED=${SANITIZERS_REQUIRED:-0}
CFLAGS="-Wall -Wextra -Werror -g -O1 -fno-omit-frame-pointer"

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=$(mktemp -d "${TMPDIR:-/tmp}/cftgu-sanitize.XXXXXX")
trap 'rm -rf "$BUILD_DIR"' EXIT INT TERM HUP

P1="$ROOT_DIR/Part 1 - The Beginner Path_ Core Concepts"
P2="$ROOT_DIR/Part 2 - The Intermediate Path_ Deeper into C"
P3="$ROOT_DIR/Part 3 - The Advanced Path_ Towards Mastery"
P4="$ROOT_DIR/Part 4 - The Expert Path_ Systems and Concurrency"
P5="$ROOT_DIR/Part 5 - Expert Systems & Application Development"

# Every sanitizer exits with this status when it reports something, so a
# lesson's own exit status (1 for bad input) is never mistaken for a finding.
FOUND=86
export ASAN_OPTIONS="exitcode=$FOUND:detect_leaks=1:abort_on_error=0"
export LSAN_OPTIONS="exitcode=$FOUND"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:exitcode=$FOUND"
export MSAN_OPTIONS="exitcode=$FOUND"
export TSAN_OPTIONS="exitcode=$FOUND:halt_on_error=1"

STANDARD=-std=c23
probe=$BUILD_DIR/probe.c
printf 'int main(void)\n{\n    return 0;\n}\n' > "$probe"
"$CC" $STANDARD "$probe" -o "$BUILD_DIR/probe" >/dev/null 2>&1 || STANDARD=-std=c17

NCURSES_FLAG=
for flag in -lncursesw -lncurses; do
    printf '#include <ncurses.h>\nint main(void)\n{\n    return 0;\n}\n' > "$BUILD_DIR/curses.c"
    if "$CC" "$BUILD_DIR/curses.c" -o "$BUILD_DIR/curses" "$flag" >/dev/null 2>&1; then
        NCURSES_FLAG=$flag
        break
    fi
done

fail() {
    echo "$1" >&2
    if [ -n "${2:-}" ]; then
        printf '%s\n' "$2" | tail -n 60 >&2
    fi
    exit 1
}

skip_or_fail() {
    if [ "$SANITIZERS_REQUIRED" = 1 ]; then
        fail "$1 (SANITIZERS_REQUIRED=1)"
    fi
    printf 'Skipping: %s\n' "$1"
}

# supported <flags>: does this compiler build and run a program with them?
supported() {
    "$CC" $CFLAGS $STANDARD "$@" "$probe" -o "$BUILD_DIR/probe_san" >/dev/null 2>&1 &&
        "$BUILD_DIR/probe_san" >/dev/null 2>&1
}

# Does LeakSanitizer work here? A program that leaks must be reported.
leaks_detected() {
    cat > "$BUILD_DIR/leak.c" <<'EOF'
#include <stdlib.h>

void *volatile kept;

int main(void)
{
    kept = malloc(64);
    kept = NULL;
    return 0;
}
EOF
    "$CC" $CFLAGS $STANDARD "$@" "$BUILD_DIR/leak.c" -o "$BUILD_DIR/leak" >/dev/null 2>&1 || return 1
    status=0
    "$BUILD_DIR/leak" >/dev/null 2>&1 || status=$?
    [ "$status" = "$FOUND" ]
}

# The lessons' extra link flags, as in compile_lesson() in smoke_check.sh.
link_flags() {
    case "$1" in
        *30_multithreaded_file_analyzer.c) printf '%s' -pthread ;;
        *32_linking_external_libraries.c) printf '%s' -lm ;;
        *35_*|*capstone_loader.c) printf '%s' "$NCURSES_FLAG" ;;
    esac
}

# build <source> <output> <sanitizer flags...>
build() {
    source=$1
    output=$2
    shift 2
    "$CC" $CFLAGS $STANDARD "$@" "$source" -o "$output" $(link_flags "$source") ||
        fail "Could not build $source with $*."
}

# run_case <label> <binary> <stdin text> [args...]
# Runs the binary in a fresh working directory (some lessons write files
# there), with a time limit, and fails on any sanitizer report.
run_case() {
    label=$1
    binary=$2
    input=$3
    shift 3
    work=$BUILD_DIR/work.$$.$RUNS
    RUNS=$((RUNS + 1))
    mkdir -p "$work"
    cp "$BUILD_DIR"/fixtures/* "$work"/
    printf '%b' "$input" > "$work/stdin.txt"

    (cd "$work" && exec "$binary" "$@" < stdin.txt > out.txt 2>&1) &
    pid=$!
    ticks=0
    while kill -0 "$pid" 2>/dev/null; do
        if [ "$ticks" -ge 300 ]; then
            kill "$pid" 2>/dev/null || true
            wait "$pid" 2>/dev/null || true
            fail "$label: did not finish within 30 seconds." "$(cat "$work/out.txt")"
        fi
        sleep 0.1
        ticks=$((ticks + 1))
    done
    status=0
    wait "$pid" || status=$?
    output=$(cat "$work/out.txt")

    case "$output" in
        *"Sanitizer"*|*"runtime error:"*)
            fail "$label: sanitizer report." "$output"
            ;;
    esac
    if [ "$status" = "$FOUND" ]; then
        fail "$label: exited with the sanitizer status $FOUND." "$output"
    fi
    if [ "$status" -gt 128 ]; then
        fail "$label: killed by signal $((status - 128))." "$output"
    fi
    rm -rf "$work"
}

# Input files some lessons read, copied into each working directory.
make_fixtures() {
    mkdir -p "$BUILD_DIR/fixtures"
    cp "$P4/27_build_your_own_grep.c" "$BUILD_DIR/fixtures/grep_input.c"
    awk 'BEGIN {
        for (line = 0; line < 400; line++) {
            for (word = 0; word < 12; word++) printf "word%d ", word;
            printf "\n";
        }
    }' > "$BUILD_DIR/fixtures/analyzer.txt"
    : > "$BUILD_DIR/fixtures/empty.txt"
    cat > "$BUILD_DIR/fixtures/users.dat" <<'EOF'
101,alpha_coder,15,1
102,bit_wizard,22,1
this is a bad line and should be skipped
310,logic_lord,33,1
EOF
    cat > "$BUILD_DIR/fixtures/world.map" <<'EOF'
room 0 "A dusty library. A door stands to the north."
room 1 "A cold cavern. A passage leads east."
room 2 "A cramped tunnel."
link 0 n 1
link 1 s 0
link 1 e 2
link 2 w 1
EOF
    printf 'room 0 "Fine."\nroom 1 broken\n' > "$BUILD_DIR/fixtures/bad_room.map"
    printf 'room 0 "Fine."\nlink 0 n 7\n' > "$BUILD_DIR/fixtures/bad_link.map"
}

# Lesson 35 needs a terminal for the game, so this harness runs just its
# world loader and cleanup (the code that allocates and frees).
make_capstone_harness() {
    cat > "$BUILD_DIR/capstone_loader.c" <<EOF
#include <stdio.h>

#define main capstone_lesson_main
int capstone_lesson_main(int argc, char *argv[]);
#include "$P5/35_capstone_awesome_text_adventure.c"
#undef main

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        GameState *game = load_world(argv[i]);
        if (game != NULL)
        {
            printf("%s: %d rooms\n", argv[i], game->num_rooms);
            cleanup(game);
        }
    }
    return 0;
}
EOF
}

# run_lessons <sanitizer flags...>: build and run every case.
run_lessons() {
    bin=$BUILD_DIR/bin
    rm -rf "$bin"
    mkdir -p "$bin"

    for source in "$P1"/*.c "$P2"/*.c "$P3"/*.c "$P4"/*.c "$P5"/*.c; do
        case "$source" in
            *26_simple_socket_*|*33_advanced_terminal_ui.c|*35_*) continue ;;
        esac
        name=$(basename "$source" .c)
        build "$source" "$bin/$name" "$@"
    done
    build "$BUILD_DIR/capstone_loader.c" "$bin/capstone_loader" "$@"

    for source in "$P1"/*.c "$P2"/*.c "$P3"/*.c "$P4"/*.c "$P5"/*.c; do
        name=$(basename "$source" .c)
        b=$bin/$name
        case "$name" in
            26_*|33_*|35_*)
                ;;
            3_user_input)
                run_case "$name" "$b" '30\n3.5\nQ\n'
                run_case "$name (garbage)" "$b" 'abc\n'
                run_case "$name (EOF)" "$b" ''
                ;;
            5_conditional_statements)
                run_case "$name" "$b" '7\n85\n'
                run_case "$name (garbage)" "$b" '7\nabc\n'
                ;;
            6_loops)
                run_case "$name" "$b" 'abc\n2\n4\nxyz\n0\n7\n'
                run_case "$name (EOF)" "$b" ''
                ;;
            13_dynamic_memory_allocation)
                run_case "$name" "$b" '3\n'
                run_case "$name (garbage)" "$b" 'abc\n'
                ;;
            15_command_line_arguments)
                run_case "$name" "$b" '' Bob 42
                run_case "$name (bad age)" "$b" '' Bob 30x
                ;;
            16_simple_calculator)
                run_case "$name" "$b" '' 10.5 / 2
                run_case "$name (bad number)" "$b" '' ten + 5
                ;;
            17_student_record_system)
                run_case "$name" "$b" '1\n42\nJane Doe\n3.50\n3\n2\n4\n'
                run_case "$name (garbage)" "$b" '1\nabc\n9\n\n'
                ;;
            25_simple_text_editor)
                run_case "$name" "$b" 'a first\na second\ni 2 middle\nd 1\np\ns\nq\n' doc.txt
                run_case "$name (bad commands)" "$b" '\npq\nd abc\nd 9\n' doc.txt
                ;;
            27_build_your_own_grep)
                run_case "$name" "$b" '' main grep_input.c
                ;;
            29_tiny_shell)
                run_case "$name" "$b" 'echo hello\nno_such_command_cftgu\nexit\n'
                ;;
            30_multithreaded_file_analyzer)
                run_case "$name" "$b" '' analyzer.txt
                run_case "$name (empty file)" "$b" '' empty.txt
                ;;
            34_parsing_data_files)
                run_case "$name" "$b" '' users.dat
                ;;
            *)
                run_case "$name" "$b" ''
                ;;
        esac
    done

    run_case "35 world loader" "$bin/capstone_loader" '' world.map bad_room.map bad_link.map missing.map
}

RUNS=0
make_fixtures
make_capstone_harness
printf 'Sanitizer check compiler: %s %s\n' "$CC" "$STANDARD"

for sanitizer in $SANITIZERS; do
    case "$sanitizer" in
        address)
            flags="-fsanitize=address,undefined -fno-sanitize-recover=undefined"
            if ! supported $flags; then
                skip_or_fail "AddressSanitizer and UBSan cannot build or run with $CC here"
                continue
            fi
            if leaks_detected $flags; then
                printf 'Running every lesson under ASan, UBSan and LeakSanitizer...\n'
            else
                if [ "$(uname -s)" = Linux ]; then
                    skip_or_fail "LeakSanitizer did not report a known leak"
                fi
                printf 'Running every lesson under ASan and UBSan (no LeakSanitizer here)...\n'
                ASAN_OPTIONS="exitcode=$FOUND:detect_leaks=0"
            fi
            run_lessons $flags
            ASAN_OPTIONS="exitcode=$FOUND:detect_leaks=1:abort_on_error=0"
            ;;
        memory)
            flags="-fsanitize=memory -fsanitize-memory-track-origins=2"
            if ! supported $flags; then
                skip_or_fail "MemorySanitizer cannot build or run with $CC here (it needs Clang on Linux; in Docker, also --security-opt seccomp=unconfined)"
                continue
            fi
            printf 'Running every lesson under MemorySanitizer...\n'
            run_lessons $flags
            ;;
        thread)
            flags="-fsanitize=thread"
            if ! supported $flags; then
                skip_or_fail "ThreadSanitizer cannot build or run with $CC here"
                continue
            fi
            printf 'Running lesson 30 under ThreadSanitizer...\n'
            build "$P4/30_multithreaded_file_analyzer.c" "$BUILD_DIR/30_tsan" $flags
            run_case "30_multithreaded_file_analyzer (tsan)" "$BUILD_DIR/30_tsan" '' analyzer.txt
            run_case "30_multithreaded_file_analyzer (tsan, empty file)" "$BUILD_DIR/30_tsan" '' empty.txt
            ;;
        *)
            fail "Unknown sanitizer '$sanitizer' (use address, memory or thread)."
            ;;
    esac
done

printf 'Sanitizer check passed (%d runs).\n' "$RUNS"
