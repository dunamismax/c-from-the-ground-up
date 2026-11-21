#!/bin/sh
#
# Run the lessons under the sanitizers, to catch what the smoke check's output
# assertions cannot see:
#
#   address  AddressSanitizer + UndefinedBehaviorSanitizer + LeakSanitizer on
#            every lesson, each fed a canned input (valid input, and bad input
#            for the interactive lessons). Lesson 35's loader also runs through
#            a harness on bad maps. The ncurses lessons, 33 and 35, run in a
#            pseudo-terminal with scripted keys (scripts/pty_run.py).
#   memory   MemorySanitizer (Clang on Linux) on the same runs: reads of
#            uninitialized memory. A garbage value can happen to print the
#            right answer, which is how the old lesson 13 passed the smoke
#            check. Lessons 33 and 35 are left out: ncurses is not built
#            with MemorySanitizer, so everything it returns would look
#            uninitialized.
#   thread   ThreadSanitizer on lesson 30, the only multithreaded lesson.
#
# The address and memory runs also build and run every worked exercise
# solution in exercises/.
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
# Lesson 26's server and client run as a pair over a local port.

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
P6="$ROOT_DIR/Part 6 - The Toolsmith Path_ Debugging, UB and Testing"
# The unit tests lesson 38 describes; each includes a lesson with its main renamed.
TESTS="$ROOT_DIR/tests"
# Worked solutions to the chapters' exercises, three per chapter that has them.
EXERCISES="$ROOT_DIR/exercises"

# Every sanitizer exits with this status when it reports something, so a
# lesson's own exit status (1 for bad input) is never mistaken for a finding.
FOUND=86
export ASAN_OPTIONS="exitcode=$FOUND:detect_leaks=1:abort_on_error=0"
export LSAN_OPTIONS="exitcode=$FOUND"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1:exitcode=$FOUND"
export MSAN_OPTIONS="exitcode=$FOUND"
export TSAN_OPTIONS="exitcode=$FOUND:halt_on_error=1"

# The standard the lessons teach (BUILD.md decision D1). The smoke check also
# compiles every lesson under C23.
STANDARD=-std=c17
probe=$BUILD_DIR/probe.c
printf 'int main(void)\n{\n    return 0;\n}\n' > "$probe"

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
        *33_*|*35_*|*capstone_loader.c) printf '%s' "$NCURSES_FLAG" ;;
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

# Lesson 26: start the sanitized server, send it one message with the
# sanitized client, and check both for reports. Skipped (with a message)
# where the environment cannot bind a local port, as in the smoke check.
run_socket_pair() {
    server=$1
    client=$2
    log=$BUILD_DIR/server.log
    for attempt in 1 2 3 4 5; do
        port=$((35000 + ($$ + attempt + RUNS) % 20000))
        : > "$log"
        "$server" "$port" > "$log" 2>&1 &
        server_pid=$!
        # Its output is buffered until it exits, so give it time to start
        # listening, as the smoke check does, rather than waiting for a line.
        ticks=0
        while [ "$ticks" -lt 20 ] && kill -0 "$server_pid" 2>/dev/null; do
            sleep 0.1
            ticks=$((ticks + 1))
        done
        if kill -0 "$server_pid" 2>/dev/null; then
            run_case "26_simple_socket_client" "$client" '' 127.0.0.1 "$port" "Sanitized hello"
            status=0
            wait "$server_pid" || status=$?
            output=$(cat "$log")
            case "$output" in
                *"Sanitizer"*|*"runtime error:"*)
                    fail "26_simple_socket_server: sanitizer report." "$output"
                    ;;
            esac
            if [ "$status" = "$FOUND" ] || [ "$status" -gt 128 ]; then
                fail "26_simple_socket_server: exited with status $status." "$output"
            fi
            case "$output" in
                *"Sanitized hello"*) ;;
                *) fail "26_simple_socket_server: the message did not arrive." "$output" ;;
            esac
            return 0
        fi
        kill "$server_pid" 2>/dev/null || true
        wait "$server_pid" 2>/dev/null || true
    done
    printf 'Skipping lesson 26 (could not start the server on a local port).\n'
}

# Lessons 33 and 35 need a terminal: run them in a pseudo-terminal, typing
# their input, and check for sanitizer reports in what they drew.
run_terminal_lessons() {
    if [ -z "$NCURSES_FLAG" ] || ! command -v python3 >/dev/null 2>&1; then
        skip_or_fail "lessons 33 and 35 need ncurses and python3 to run in a pseudo-terminal"
        return 0
    fi
    for name in 33_advanced_terminal_ui 35_capstone_awesome_text_adventure; do
        build "$P5/$name.c" "$BUILD_DIR/bin/$name" "$@"
    done
    run_pty_case 33_advanced_terminal_ui 'x' ""
    run_pty_case 35_capstone_awesome_text_adventure \
        'help\nlook\ngo north\ngo up\ngo east\nnonsense\ngo south\nquit\n' "Thank you for playing!" world.map
    # The player must see what they type: the game once read commands with
    # echo off, so "xyzzy" never appeared on the screen.
    run_pty_case 35_capstone_awesome_text_adventure 'xyzzy\nquit\n' "xyzzy" world.map
}

# run_pty_case <lesson> <keys> <expected text, or ""> [args...]
run_pty_case() {
    name=$1
    keys=$2
    expected=$3
    shift 3
    work=$BUILD_DIR/work.$$.$RUNS
    RUNS=$((RUNS + 1))
    mkdir -p "$work"
    cp "$BUILD_DIR"/fixtures/* "$work"/
    status=0
    (cd "$work" && python3 "$ROOT_DIR/scripts/pty_run.py" --keys "$keys" --log screen.txt -- "$BUILD_DIR/bin/$name" "$@") || status=$?
    output=$(LC_ALL=C tr -d '\000' < "$work/screen.txt")
    case "$output" in
        *"Sanitizer"*|*"runtime error:"*)
            fail "$name (terminal): sanitizer report." "$output"
            ;;
    esac
    if [ "$status" != 0 ]; then
        fail "$name (terminal): exited with status $status." "$output"
    fi
    if [ -n "$expected" ]; then
        case "$output" in
            *"$expected"*) ;;
            *) fail "$name (terminal): did not print \"$expected\"." "$output" ;;
        esac
    fi
    rm -rf "$work"
}

# run_lessons <sanitizer flags...>: build and run every case.
run_lessons() {
    bin=$BUILD_DIR/bin
    rm -rf "$bin"
    mkdir -p "$bin"

    for source in "$P1"/*.c "$P2"/*.c "$P3"/*.c "$P4"/*.c "$P5"/*.c "$P6"/*.c "$TESTS"/*.c; do
        case "$source" in
            *33_advanced_terminal_ui.c|*35_*) continue ;;
        esac
        name=$(basename "$source" .c)
        build "$source" "$bin/$name" "$@"
    done
    build "$BUILD_DIR/capstone_loader.c" "$bin/capstone_loader" "$@"

    for source in "$P1"/*.c "$P2"/*.c "$P3"/*.c "$P4"/*.c "$P5"/*.c "$P6"/*.c "$TESTS"/*.c; do
        name=$(basename "$source" .c)
        b=$bin/$name
        case "$name" in
            26_simple_socket_server)
                run_socket_pair "$b" "$bin/26_simple_socket_client"
                ;;
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

    # The worked exercise solutions (exercises/<chapter>/): with no input,
    # and with a line of numbers that answers every prompt they have.
    for source in "$EXERCISES"/*/*.c; do
        [ -f "$source" ] || continue
        name=$(basename "$(dirname "$source")")_$(basename "$source" .c)
        build "$source" "$bin/exercise_$name" "$@"
        run_case "exercise $name (no input)" "$bin/exercise_$name" ''
        run_case "exercise $name" "$bin/exercise_$name" '7\n3\n5\n2\n1\n0\n7\n'
    done
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
            run_terminal_lessons $flags
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
