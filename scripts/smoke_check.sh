#!/bin/sh

set -eu

CC=${CC:-cc}
BASE_CFLAGS=${CFLAGS:--Wall -Wextra -Wpedantic -Wstrict-prototypes -Werror}

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=$(mktemp -d "${TMPDIR:-/tmp}/cftgu-smoke.XXXXXX")
SOURCE_LIST="$BUILD_DIR/sources.txt"
LESSON31_DIR="$ROOT_DIR/Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects"
SOCKET_SERVER_PID=
STANDARD_FLAG=
EFFECTIVE_CFLAGS=

cleanup() {
    if [ -n "${SOCKET_SERVER_PID}" ]; then
        kill "$SOCKET_SERVER_PID" >/dev/null 2>&1 || true
        wait "$SOCKET_SERVER_PID" >/dev/null 2>&1 || true
    fi
    rm -rf "$BUILD_DIR"
    make -C "$LESSON31_DIR" clean >/dev/null 2>&1 || true
}

trap cleanup EXIT INT TERM HUP

detect_standard_flag() {
    cat > "$BUILD_DIR/standard_probe.c" <<'EOF'
int main(void)
{
    return 0;
}
EOF

    for flag in -std=c23 -std=c17; do
        if "$CC" $BASE_CFLAGS "$BUILD_DIR/standard_probe.c" -o "$BUILD_DIR/standard_probe" "$flag" >/dev/null 2>&1; then
            printf '%s\n' "$flag"
            return 0
        fi
    done

    return 1
}

detect_ncurses_flag() {
    cat > "$BUILD_DIR/ncurses_probe.c" <<'EOF'
#include <ncurses.h>

int main(void)
{
    return 0;
}
EOF

    for flag in -lncursesw -lncurses; do
        if "$CC" $EFFECTIVE_CFLAGS "$BUILD_DIR/ncurses_probe.c" -o "$BUILD_DIR/ncurses_probe" "$flag" >/dev/null 2>&1; then
            printf '%s\n' "$flag"
            return 0
        fi
    done

    return 1
}

detect_sanitizer_flags() {
    cat > "$BUILD_DIR/sanitizer_probe.c" <<'EOF'
int main(void)
{
    return 0;
}
EOF

    if "$CC" $EFFECTIVE_CFLAGS "$BUILD_DIR/sanitizer_probe.c" -o "$BUILD_DIR/sanitizer_probe" -fsanitize=address,undefined >/dev/null 2>&1; then
        printf '%s\n' "-fsanitize=address,undefined"
        return 0
    fi

    return 1
}

compile_lesson() {
    lesson_path=$1
    output_path=$2
    extra_flags=

    case "$lesson_path" in
        *30_multithreaded_file_analyzer.c)
            extra_flags="-pthread"
            ;;
        *32_linking_external_libraries.c)
            extra_flags="-lm"
            ;;
        *33_advanced_terminal_ui.c|*35_capstone_awesome_text_adventure.c)
            extra_flags=$NCURSES_FLAG
            ;;
    esac

    printf 'Compiling %s\n' "$lesson_path"
    "$CC" $EFFECTIVE_CFLAGS "$ROOT_DIR/$lesson_path" -o "$output_path" $extra_flags
}

fail_with_output() {
    message=$1
    output=$2

    echo "$message" >&2
    printf '%s\n' "$output" >&2
    exit 1
}

expect_contains() {
    output=$1
    expected=$2
    message=$3

    case "$output" in
        *"$expected"*)
            ;;
        *)
            fail_with_output "$message" "$output"
            ;;
    esac
}

expect_not_contains() {
    output=$1
    unexpected=$2
    message=$3

    case "$output" in
        *"$unexpected"*)
            fail_with_output "$message" "$output"
            ;;
    esac
}

run_analyzer_check() {
    sample_file=$BUILD_DIR/analyzer_sample.txt
    analyzer_bin=$BUILD_DIR/30_multithreaded_file_analyzer
    empty_file=$BUILD_DIR/analyzer_empty.txt

    awk 'BEGIN {
        for (i = 0; i < 40; i++) printf "a";
        printf " ";
        for (i = 0; i < 40; i++) printf "b";
        printf "\n";
    }' > "$sample_file"

    set -- $(wc "$sample_file")
    expected_lines=$1
    expected_words=$2
    expected_chars=$3

    analyzer_output=$("$analyzer_bin" "$sample_file")
    actual_chars=$(printf '%s\n' "$analyzer_output" | awk '/Total Characters:/ { print $3 }')
    actual_words=$(printf '%s\n' "$analyzer_output" | awk '/Total Words:/ { print $3 }')
    actual_lines=$(printf '%s\n' "$analyzer_output" | awk '/Total Lines:/ { print $3 }')

    if [ "$actual_lines" != "$expected_lines" ] || [ "$actual_words" != "$expected_words" ] || [ "$actual_chars" != "$expected_chars" ]; then
        echo "Analyzer verification failed." >&2
        echo "Expected: lines=$expected_lines words=$expected_words chars=$expected_chars" >&2
        echo "Actual:   lines=$actual_lines words=$actual_words chars=$actual_chars" >&2
        exit 1
    fi

    : > "$empty_file"
    empty_output=$("$analyzer_bin" "$empty_file")
    expect_contains "$empty_output" "File is empty. Nothing to analyze." "Analyzer did not report empty-file handling."
    expect_contains "$empty_output" "Total Characters: 0" "Analyzer empty-file character count is incorrect."
    expect_contains "$empty_output" "Total Words:      0" "Analyzer empty-file word count is incorrect."
    expect_contains "$empty_output" "Total Lines:      0" "Analyzer empty-file line count is incorrect."
}

run_socket_check() {
    server_bin=$BUILD_DIR/26_simple_socket_server
    client_bin=$BUILD_DIR/26_simple_socket_client
    server_log=$BUILD_DIR/socket_server.log
    permission_denied_bind=1

    for attempt in 1 2 3 4 5; do
        port=$((35000 + ($$ + attempt) % 20000))
        : > "$server_log"
        "$server_bin" "$port" > "$server_log" 2>&1 &
        SOCKET_SERVER_PID=$!

        for _ in 1 2 3 4 5 6 7 8 9 10; do
            if ! kill -0 "$SOCKET_SERVER_PID" >/dev/null 2>&1; then
                break
            fi

            sleep 0.2
        done

        if kill -0 "$SOCKET_SERVER_PID" >/dev/null 2>&1; then
            client_output=$("$client_bin" 127.0.0.1 "$port" "Smoke test message" 2>&1)
            wait "$SOCKET_SERVER_PID"
            SOCKET_SERVER_PID=

            expect_contains "$client_output" "Server reply: Message received. Thank you!" "Socket client/server exchange did not complete successfully."
            expect_contains "$(cat "$server_log")" "Connection accepted from" "Socket server did not accept the smoke-test client."
            return 0
        fi

        wait "$SOCKET_SERVER_PID" >/dev/null 2>&1 || true
        SOCKET_SERVER_PID=

        if ! grep -F "Bind failed: Operation not permitted" "$server_log" >/dev/null 2>&1; then
            permission_denied_bind=0
        fi
    done

    if [ "$permission_denied_bind" = 1 ]; then
        printf 'Skipping socket client/server regression check (environment does not permit binding a local listening socket).\n'
        return 0
    fi

    fail_with_output "Socket server/client regression test could not start successfully." "$(cat "$server_log")"
}

run_student_record_checks() {
    student_bin=$BUILD_DIR/17_student_record_system
    invalid_dir=$BUILD_DIR/student_invalid
    comma_dir=$BUILD_DIR/student_comma
    save_dir=$BUILD_DIR/student_save

    mkdir -p "$invalid_dir" "$comma_dir" "$save_dir"

    invalid_output=$(cd "$invalid_dir" && printf '1\nabc\n2\n4\n' | "$student_bin")
    expect_contains "$invalid_output" "Invalid student ID. Record was not added." "Student system accepted a non-numeric ID."
    expect_contains "$invalid_output" "No records to display." "Student system added a record after invalid numeric input."

    comma_output=$(cd "$comma_dir" && printf '1\n42\nDoe, Jane\n4\n' | "$student_bin")
    expect_contains "$comma_output" "Names cannot contain commas" "Student system did not protect the on-disk format from comma-containing names."

    save_output=$(cd "$save_dir" && printf '1\n42\nJane Doe\n3.50\n3\n4\n' | "$student_bin")
    expect_contains "$save_output" "Student added successfully." "Student system failed to add a valid record."
    expect_contains "$save_output" "Successfully saved 1 record(s) to students.db." "Student system failed to save the valid record."

    load_output=$(cd "$save_dir" && printf '2\n4\n' | "$student_bin")
    expect_contains "$load_output" "Successfully loaded 1 record(s) from students.db." "Student system failed to reload a saved record."
    expect_contains "$load_output" "Jane Doe" "Student system did not print the reloaded student name."
    expect_contains "$load_output" "3.50" "Student system did not print the reloaded GPA."
}

run_tiny_shell_check() {
    shell_bin=$BUILD_DIR/29_tiny_shell

    shell_output=$(awk 'BEGIN {
        printf "echo ";
        for (i = 0; i < 120; i++) printf "a";
        printf "\nexit\n";
    }' | "$shell_bin" 2>&1)

    expect_contains "$shell_output" "Command too long. Maximum length is 78 characters." "TinyShell did not reject an overlong command line."
    expect_not_contains "$shell_output" "execvp failed" "TinyShell split an overlong command into a second command."
}

# Run a lesson binary with the given stdin text, failing if it does not exit
# within 5 seconds. Interactive lessons must never hang on bad input or EOF.
# Output is capped at 1 MiB (ulimit -f counts 512-byte blocks) so a runaway
# printing loop dies with SIGXFSZ instead of filling the disk.
# Sets RUN_OUTPUT and RUN_STATUS.
run_lesson_with_input() {
    lesson_bin=$1
    input_text=$2
    input_file=$BUILD_DIR/stdin.txt
    output_file=$BUILD_DIR/stdout.txt

    printf '%b' "$input_text" > "$input_file"
    (ulimit -f 2048 && exec "$lesson_bin" < "$input_file" > "$output_file" 2>&1) &
    lesson_pid=$!

    ticks=0
    while kill -0 "$lesson_pid" 2>/dev/null; do
        if [ "$ticks" -ge 50 ]; then
            kill "$lesson_pid" 2>/dev/null || true
            wait "$lesson_pid" 2>/dev/null || true
            fail_with_output "$(basename "$lesson_bin") did not exit within 5 seconds for input: $input_text" "$(tail -c 2000 "$output_file")"
        fi
        sleep 0.1
        ticks=$((ticks + 1))
    done

    RUN_STATUS=0
    wait "$lesson_pid" || RUN_STATUS=$?
    if [ "$RUN_STATUS" -eq 153 ]; then
        fail_with_output "$(basename "$lesson_bin") printed more than 1 MiB for input: $input_text (probably an infinite loop)" "$(tail -c 2000 "$output_file")"
    fi
    RUN_OUTPUT=$(cat "$output_file")
}

expect_status() {
    expected=$1
    message=$2

    if [ "$RUN_STATUS" -ne "$expected" ]; then
        fail_with_output "$message (exit status $RUN_STATUS, expected $expected)" "$RUN_OUTPUT"
    fi
}

run_input_checks() {
    input_bin=$BUILD_DIR/3_user_input
    run_lesson_with_input "$input_bin" '30\n3.5\nQ\n'
    expect_status 0 "Lesson 3 failed on valid input."
    expect_contains "$RUN_OUTPUT" "You entered 30." "Lesson 3 did not echo the age."
    expect_contains "$RUN_OUTPUT" "Your GPA is 3.50." "Lesson 3 did not echo the GPA."
    expect_contains "$RUN_OUTPUT" "Your favorite letter is 'Q'." "Lesson 3 did not echo the letter."
    for bad_input in 'abc\n' '' '30\nx\n' '30\n3.5\n'; do
        run_lesson_with_input "$input_bin" "$bad_input"
        expect_status 1 "Lesson 3 did not reject bad or missing input."
        expect_contains "$RUN_OUTPUT" "Exiting." "Lesson 3 did not explain why it stopped."
    done

    conditional_bin=$BUILD_DIR/5_conditional_statements
    run_lesson_with_input "$conditional_bin" '7\n85\n'
    expect_status 0 "Lesson 5 failed on valid input."
    expect_contains "$RUN_OUTPUT" "7 is an odd number." "Lesson 5 did not classify 7 as odd."
    expect_contains "$RUN_OUTPUT" "Your letter grade is B." "Lesson 5 did not grade 85 as a B."
    for bad_input in 'abc\n' '' '7\nabc\n'; do
        run_lesson_with_input "$conditional_bin" "$bad_input"
        expect_status 1 "Lesson 5 did not reject bad or missing input."
        expect_contains "$RUN_OUTPUT" "That was not a whole number." "Lesson 5 did not explain why it stopped."
    done

    loops_bin=$BUILD_DIR/6_loops
    run_lesson_with_input "$loops_bin" 'abc\n2\n4\nxyz\n0\n7\n'
    expect_status 0 "Lesson 6 failed to recover from bad input."
    expect_contains "$RUN_OUTPUT" "That was not a number. Please try again." "Lesson 6 did not reject a non-numeric menu choice."
    expect_contains "$RUN_OUTPUT" "You chose: 2" "Lesson 6 did not accept a menu choice after bad input."
    expect_contains "$RUN_OUTPUT" "You successfully entered the number 7." "Lesson 6 did not recover in the do-while loop."
    run_lesson_with_input "$loops_bin" ''
    expect_status 1 "Lesson 6 did not stop cleanly on EOF."
    expect_contains "$RUN_OUTPUT" "No more input." "Lesson 6 did not report EOF."

    memory_bin=$BUILD_DIR/13_dynamic_memory_allocation
    run_lesson_with_input "$memory_bin" '3\n'
    expect_status 0 "Lesson 13 failed on valid input."
    expect_contains "$RUN_OUTPUT" "Student 3 GPA: 2.90" "Lesson 13 did not store and print the GPAs."
    for bad_input in 'abc\n' '' '-3\n' '0\n'; do
        run_lesson_with_input "$memory_bin" "$bad_input"
        expect_status 1 "Lesson 13 did not reject bad or missing input."
        expect_contains "$RUN_OUTPUT" "Invalid number of students." "Lesson 13 did not explain why it stopped."
    done
}

run_sanitizer_regressions() {
    if [ -z "${SANITIZER_FLAGS:-}" ]; then
        printf 'Skipping sanitizer regressions (compiler does not support -fsanitize=address,undefined).\n'
        return 0
    fi

    analyzer_san_bin=$BUILD_DIR/30_multithreaded_file_analyzer_san
    empty_file=$BUILD_DIR/analyzer_empty_san.txt
    capstone_harness=$BUILD_DIR/capstone_parse_harness.c
    capstone_harness_bin=$BUILD_DIR/capstone_parse_harness

    "$CC" $EFFECTIVE_CFLAGS "$ROOT_DIR/Part 4 - The Expert Path_ Systems and Concurrency/30_multithreaded_file_analyzer.c" \
        -o "$analyzer_san_bin" -pthread $SANITIZER_FLAGS
    : > "$empty_file"
    ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 "$analyzer_san_bin" "$empty_file" >/dev/null 2>&1

    cat > "$capstone_harness" <<EOF
#include <stdlib.h>
#include <string.h>

#define main capstone_lesson_main
#include "$ROOT_DIR/Part 5 - Expert Systems & Application Development/35_capstone_awesome_text_adventure.c"
#undef main

int main(void)
{
    GameState game;
    char room_line[700];
    char link_line[64];
    int i;

    memset(&game, 0, sizeof(game));

    memcpy(room_line, "room 0 \\"", 8);
    for (i = 0; i < 600; i++)
    {
        room_line[8 + i] = 'A';
    }
    room_line[608] = '"';
    room_line[609] = '\0';

    if (parse_room(room_line, &game) != 0)
    {
        return 1;
    }

    game.all_rooms[0] = calloc(1, sizeof(Room));
    game.all_rooms[1] = calloc(1, sizeof(Room));
    if (!game.all_rooms[0] || !game.all_rooms[1])
    {
        free(game.all_rooms[0]);
        free(game.all_rooms[1]);
        return 1;
    }

    game.all_rooms[0]->id = 0;
    game.all_rooms[1]->id = 1;
    game.num_rooms = 2;

    strcpy(link_line, "link 0 abcdefghijk 1");
    if (parse_link(link_line, &game) != 0)
    {
        free(game.all_rooms[0]);
        free(game.all_rooms[1]);
        return 1;
    }

    free(game.all_rooms[0]);
    free(game.all_rooms[1]);
    return 0;
}
EOF

    "$CC" $EFFECTIVE_CFLAGS "$capstone_harness" -o "$capstone_harness_bin" "$NCURSES_FLAG" $SANITIZER_FLAGS
    ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 "$capstone_harness_bin" >/dev/null 2>&1
}

case "$BASE_CFLAGS" in
    *-std=*)
        EFFECTIVE_CFLAGS=$BASE_CFLAGS
        STANDARD_FLAG=$(printf '%s\n' "$BASE_CFLAGS" | awk '{
            for (i = 1; i <= NF; i++)
            {
                if ($i ~ /^-std=/)
                {
                    print $i
                    exit
                }
            }
        }')
        ;;
    *)
        STANDARD_FLAG=$(detect_standard_flag) || {
            echo "Could not find a usable language standard flag (-std=c23 or -std=c17)." >&2
            exit 1
        }
        EFFECTIVE_CFLAGS="$BASE_CFLAGS $STANDARD_FLAG"
        ;;
esac

NCURSES_FLAG=$(detect_ncurses_flag) || {
    echo "Could not find a usable ncurses library (-lncursesw or -lncurses)." >&2
    exit 1
}
SANITIZER_FLAGS=$(detect_sanitizer_flags || true)

printf 'Smoke check compiler: %s\n' "$CC"
printf 'Smoke check standard: %s\n' "$STANDARD_FLAG"
printf 'Smoke check flags: %s\n' "$EFFECTIVE_CFLAGS"

(cd "$ROOT_DIR" && find . -type f -name '*.c' | sort > "$SOURCE_LIST")

compiled_count=0

while IFS= read -r lesson_path; do
    case "$lesson_path" in
        ./Part\ 5\ -\ Expert\ Systems\ \&\ Application\ Development/31_make_files_for_multi_file_projects/*)
            continue
            ;;
    esac

    lesson_path=${lesson_path#./}
    lesson_name=$(basename "${lesson_path%.c}")
    compile_lesson "$lesson_path" "$BUILD_DIR/$lesson_name"
    compiled_count=$((compiled_count + 1))
done < "$SOURCE_LIST"

printf 'Building %s\n' "$LESSON31_DIR"
make -C "$LESSON31_DIR" clean >/dev/null
make -C "$LESSON31_DIR" CC="$CC" CFLAGS="$EFFECTIVE_CFLAGS" >/dev/null
test -x "$LESSON31_DIR/31_project_main"
make -C "$LESSON31_DIR" clean >/dev/null

run_analyzer_check
run_socket_check
run_student_record_checks
run_tiny_shell_check
run_input_checks
run_sanitizer_regressions

printf 'Smoke check passed. Compiled %d single-file lessons plus lesson 31.\n' "$compiled_count"
