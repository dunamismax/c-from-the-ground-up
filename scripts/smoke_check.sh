#!/bin/sh

set -eu

CC=${CC:-cc}
BASE_CFLAGS=${CFLAGS:--Wall -Wextra -Wpedantic -Wstrict-prototypes -Wshadow -Wconversion -Wsign-conversion -Wmissing-prototypes -Werror}

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_DIR=$(mktemp -d "${TMPDIR:-/tmp}/cftgu-smoke.XXXXXX")
SOURCE_LIST="$BUILD_DIR/sources.txt"
LESSON31_DIR="$ROOT_DIR/Part 5 - Expert Systems & Application Development/31_make_files_for_multi_file_projects"
SOCKET_SERVER_PID=
STANDARD_FLAG=
EFFECTIVE_CFLAGS=
# The lessons teach C17 (BUILD.md decision D1), so that is the standard every
# check below uses. When the compiler also accepts C23, every lesson is
# compiled a second time under it, so the course keeps building on the newest
# standard too.
TAUGHT_STANDARD=-std=c17
NEWER_STANDARD=-std=c23

cleanup() {
    if [ -n "${SOCKET_SERVER_PID}" ]; then
        kill "$SOCKET_SERVER_PID" >/dev/null 2>&1 || true
        wait "$SOCKET_SERVER_PID" >/dev/null 2>&1 || true
    fi
    rm -rf "$BUILD_DIR"
    make -C "$LESSON31_DIR" clean >/dev/null 2>&1 || true
}

trap cleanup EXIT INT TERM HUP

standard_supported() {
    cat > "$BUILD_DIR/standard_probe.c" <<'EOF'
int main(void)
{
    return 0;
}
EOF

    "$CC" $BASE_CFLAGS "$BUILD_DIR/standard_probe.c" -o "$BUILD_DIR/standard_probe" "$1" >/dev/null 2>&1
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
    lesson_cflags=${3:-$EFFECTIVE_CFLAGS}
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
    "$CC" $lesson_cflags "$ROOT_DIR/$lesson_path" -o "$output_path" $extra_flags
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

    # Commands read from a regular file (seekable stdin), ending at EOF, with
    # unknown commands: a child that calls exit() instead of _exit() after a
    # failed execvp rewinds the shared input, and when the parent refills its
    # buffer at the end it reads the same commands again, forever.
    run_lesson_with_input "$shell_bin" 'no_such_command_cftgu\nno_such_command_cftgu\n'
    expect_status 0 "TinyShell did not exit cleanly after unknown commands read from a file."
    failures=$(printf '%s\n' "$RUN_OUTPUT" | grep -c 'execvp failed' || true)
    if [ "$failures" -ne 2 ]; then
        fail_with_output "TinyShell ran 2 unknown commands from a file but reported $failures failures." "$RUN_OUTPUT"
    fi
}

# Run a lesson binary with the given stdin text (and any further arguments as
# its command-line arguments), failing if it does not exit within 5 seconds.
# Interactive lessons must never hang on bad input or EOF.
# Output is capped at 1 MiB (ulimit -f counts 512-byte blocks) so a runaway
# printing loop dies with SIGXFSZ instead of filling the disk.
# Sets RUN_OUTPUT and RUN_STATUS.
run_lesson_with_input() {
    lesson_bin=$1
    input_text=$2
    shift 2
    input_file=$BUILD_DIR/stdin.txt
    output_file=$BUILD_DIR/stdout.txt

    printf '%b' "$input_text" > "$input_file"
    (ulimit -f 2048 && exec "$lesson_bin" "$@" < "$input_file" > "$output_file" 2>&1) &
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

# Run a lesson with command-line arguments and no input. Sets RUN_OUTPUT and
# RUN_STATUS like run_lesson_with_input.
run_lesson_with_args() {
    lesson_bin=$1
    shift
    run_lesson_with_input "$lesson_bin" '' "$@"
}

run_number_argument_checks() {
    args_bin=$BUILD_DIR/15_command_line_arguments
    run_lesson_with_args "$args_bin" Bob 42
    expect_status 0 "Lesson 15 failed on valid arguments."
    expect_contains "$RUN_OUTPUT" "In 10 years, you will be 52." "Lesson 15 did not convert the age."
    for bad_age in forty 30x '' 99999999999999999999 -1; do
        run_lesson_with_args "$args_bin" Dana "$bad_age"
        expect_status 1 "Lesson 15 accepted the age '$bad_age'."
        expect_contains "$RUN_OUTPUT" "The age must be a whole number from 0 to 150" "Lesson 15 did not explain a bad age."
    done

    calculator_bin=$BUILD_DIR/16_simple_calculator
    run_lesson_with_args "$calculator_bin" 10.5 / 2
    expect_status 0 "Lesson 16 failed on valid arguments."
    expect_contains "$RUN_OUTPUT" "10.50 / 2.00 = 5.25" "Lesson 16 computed the wrong result."
    for bad_number in ten 5x ''; do
        run_lesson_with_args "$calculator_bin" "$bad_number" + 5
        expect_status 1 "Lesson 16 accepted the number '$bad_number'."
        expect_contains "$RUN_OUTPUT" "must both be numbers" "Lesson 16 did not explain a bad number."
    done
    run_lesson_with_args "$calculator_bin" 10 / 0
    expect_status 1 "Lesson 16 allowed division by zero."
    expect_contains "$RUN_OUTPUT" "Division by zero is not allowed." "Lesson 16 did not report division by zero."

    client_bin=$BUILD_DIR/26_simple_socket_client
    run_lesson_with_args "$client_bin" 999.1.1.1 8888 hello
    expect_status 1 "Lesson 26 client accepted an invalid IP address."
    expect_contains "$RUN_OUTPUT" "is not a valid IPv4 address" "Lesson 26 client did not reject an invalid IP address."
    run_lesson_with_args "$client_bin" 127.0.0.1 88a8 hello
    expect_status 1 "Lesson 26 client accepted an invalid port."
    expect_contains "$RUN_OUTPUT" "Port must be a whole number between 1 and 65535." "Lesson 26 client did not reject an invalid port."
}

run_text_editor_check() {
    editor_bin=$BUILD_DIR/25_simple_text_editor
    editor_dir=$BUILD_DIR/editor
    document=$editor_dir/doc.txt
    mkdir -p "$editor_dir"

    # An empty line and a bare "p" once made the editor parse bytes past the
    # end of the text; "pq" and "d abc" were accepted as commands.
    run_lesson_with_input "$editor_bin" 'a first\na second\n\npq\nd abc\ni 2 middle\np\ns\nq\n' "$document"
    expect_status 0 "Lesson 25 did not exit cleanly."
    expect_contains "$RUN_OUTPUT" "Unknown command. Type 'h' for help." "Lesson 25 accepted 'pq' as a command."
    expect_contains "$RUN_OUTPUT" "Error: 'abc' is not a valid line number." "Lesson 25 accepted 'abc' as a line number."
    expect_contains "$RUN_OUTPUT" "   2: middle" "Lesson 25 did not insert a line."
    expect_contains "$RUN_OUTPUT" "File '$document' saved." "Lesson 25 did not save the file."
    if [ "$(cat "$document")" != "$(printf 'first\nmiddle\nsecond')" ]; then
        fail_with_output "Lesson 25 saved the wrong text." "$(cat "$document")"
    fi

    # The rest of an overlong line must not run as a second command.
    long_input=$(awk 'BEGIN { printf "a "; for (i = 0; i < 1500; i++) printf "d"; printf "\\nq\\n" }')
    run_lesson_with_input "$editor_bin" "$long_input" "$document"
    expect_status 0 "Lesson 25 did not exit cleanly after an overlong line."
    expect_contains "$RUN_OUTPUT" "Command too long." "Lesson 25 did not reject an overlong line."
    expect_not_contains "$RUN_OUTPUT" "Unknown command" "Lesson 25 ran the rest of an overlong line as a command."

    # A line longer than any fixed buffer must load as one line, so opening
    # and saving a file leaves it unchanged (the loader once split it).
    awk 'BEGIN { printf "short\n"; for (i = 0; i < 3000; i++) printf "x"; printf "\nend\n" }' > "$document"
    cp "$document" "$editor_dir/original.txt"
    run_lesson_with_input "$editor_bin" 's\nq\n' "$document"
    expect_status 0 "Lesson 25 did not exit cleanly after loading a long line."
    if ! cmp -s "$document" "$editor_dir/original.txt"; then
        fail_with_output "Lesson 25 changed a file with a long line just by loading and saving it." "$(wc -l < "$document") lines after saving, 3 before"
    fi
}

# Lesson 34: the sample file its comments give, its limits, and a line longer
# than the line buffer whose tail looks like a record (the tail was once
# parsed as a user of its own).
run_data_parser_check() {
    parser_bin=$BUILD_DIR/34_parsing_data_files
    parser_dir=$BUILD_DIR/parser
    mkdir -p "$parser_dir"

    printf '101,alpha_coder,15,1\n102,bit_wizard,22,1\n205,kernel_hacker,45,0\nthis is a bad line and should be skipped\n310,logic_lord,33,1\n404,syntax_sorcerer,99,1\n' > "$parser_dir/users.dat"
    run_lesson_with_args "$parser_bin" "$parser_dir/users.dat"
    expect_status 0 "Lesson 34 failed on its own sample file."
    expect_contains "$RUN_OUTPUT" "Successfully parsed 5 user records:" "Lesson 34 did not parse its 5 sample records."
    expect_contains "$RUN_OUTPUT" "Warning: Malformed line skipped: this is a bad line and should be skipped" "Lesson 34 did not warn about the bad line."
    expect_contains "$RUN_OUTPUT" "  User ID: 205   | Username: kernel_hacker   | Level: 45  | Active: No" "Lesson 34 printed a record wrongly."

    # 255 characters fill the buffer exactly, so the next read starts at "7".
    # The last line has no newline and must still count.
    awk 'BEGIN { printf "1,first,1,1\n"; for (i = 0; i < 255; i++) printf "x"; printf "7,ghost,3,1\n2,last,2,0" }' > "$parser_dir/long.dat"
    run_lesson_with_args "$parser_bin" "$parser_dir/long.dat"
    expect_status 0 "Lesson 34 failed on a file with an overlong line."
    expect_contains "$RUN_OUTPUT" "Warning: Overlong line skipped." "Lesson 34 did not warn about an overlong line."
    expect_not_contains "$RUN_OUTPUT" "ghost" "Lesson 34 parsed the tail of an overlong line as a record."
    expect_contains "$RUN_OUTPUT" "Successfully parsed 2 user records:" "Lesson 34 miscounted around an overlong line."
    expect_contains "$RUN_OUTPUT" "Username: last " "Lesson 34 dropped a last line that has no newline."

    # The lesson says it keeps at most MAX_USERS (10) records.
    awk 'BEGIN { for (i = 1; i <= 12; i++) printf "%d,user%d,1,1\n", i, i }' > "$parser_dir/many.dat"
    run_lesson_with_args "$parser_bin" "$parser_dir/many.dat"
    expect_status 0 "Lesson 34 failed on a file with 12 records."
    expect_contains "$RUN_OUTPUT" "Successfully parsed 10 user records:" "Lesson 34 did not stop at 10 records."

    run_lesson_with_args "$parser_bin" "$parser_dir/missing.dat"
    expect_status 1 "Lesson 34 did not fail on a missing file."
    expect_contains "$RUN_OUTPUT" "Error opening file" "Lesson 34 did not explain a missing file."
    run_lesson_with_args "$parser_bin"
    expect_status 1 "Lesson 34 ran without a file name."
    expect_contains "$RUN_OUTPUT" "Usage:" "Lesson 34 did not print its usage."
}

run_hash_table_check() {
    run_lesson_with_input "$BUILD_DIR/28_hash_table_implementation" ''
    expect_status 0 "Lesson 28 failed."
    # The lesson's comment says "age" and "city" collide in bucket 5.
    expect_contains "$RUN_OUTPUT" 'Bucket[5]:  -> ["city": "New York"] -> ["age": "30"]' "Lesson 28's collision is not where its comment says."
    expect_contains "$RUN_OUTPUT" "Value for 'name': John Doe" "Lesson 28 did not find an existing key."
    expect_contains "$RUN_OUTPUT" "Value for 'job': Not Found" "Lesson 28 found a missing key."
    expect_contains "$RUN_OUTPUT" '["city": "Los Angeles"]' "Lesson 28 did not update a key."
}

# run_exercise <chapter>/<name> <stdin text>: run one worked solution.
# Sets RUN_OUTPUT and RUN_STATUS.
run_exercise() {
    run_lesson_with_input "$BUILD_DIR/exercise_$(printf '%s' "$1" | tr '/' '_')" "$2"
}

# expect_exercise <chapter>/<name> <stdin text> <status> <expected output>
expect_exercise() {
    run_exercise "$1" "$2"
    expect_status "$3" "Exercise solution $1 exited with the wrong status for input: $2"
    expect_contains "$RUN_OUTPUT" "$4" "Exercise solution $1 did not print \"$4\" for input: $2"
}

# The worked solutions in exercises/<chapter>/ meet the same bar as the
# lessons: each compiles warning-free under the taught standard and the newer
# one, none hangs or crashes on end of input or garbage, and each prints the
# answer its comments promise.
run_exercise_checks() {
    exercise_count=0
    for solution in "$ROOT_DIR"/exercises/*/*.c; do
        [ -f "$solution" ] || continue
        chapter=$(basename "$(dirname "$solution")")
        name=$(basename "$solution" .c)
        binary=$BUILD_DIR/exercise_${chapter}_$name
        extra_flags=
        case "$chapter/$name" in
            07-functions/3_definition_order)
                # The exercise is to delete a prototype and see that a
                # definition above main is enough, so this one file cannot
                # satisfy -Wmissing-prototypes. Its comments say so.
                extra_flags=-Wno-missing-prototypes
                ;;
        esac

        "$CC" $EFFECTIVE_CFLAGS $extra_flags "$solution" -o "$binary"
        if [ "$newer_count" -gt 0 ]; then
            "$CC" $BASE_CFLAGS $NEWER_STANDARD $extra_flags "$solution" -o "$binary.newer"
        fi

        # Status 0 or 1 is the solution's own answer; anything else is a
        # crash. run_lesson_with_input fails on a hang.
        for input in '' 'abc\n' '\n\n\n' '-1\n0\n'; do
            run_lesson_with_input "$binary" "$input"
            if [ "$RUN_STATUS" -gt 1 ]; then
                fail_with_output "Exercise solution $chapter/$name crashed (status $RUN_STATUS) for input: $input" "$RUN_OUTPUT"
            fi
        done
        exercise_count=$((exercise_count + 1))
    done
    [ "$exercise_count" -gt 0 ] || return 0

    run_exercise 01-hello-world/2_three_lines ''
    three_lines=$(printf 'Roses are red,\nViolets are blue,\nC is a language.')
    if [ "$RUN_OUTPUT" != "$(printf '%s\n%s' "$three_lines" "$three_lines")" ]; then
        fail_with_output "Exercise solution 01-hello-world/2_three_lines did not print the same three lines twice." "$RUN_OUTPUT"
    fi
    expect_exercise 02-variables-and-data-types/1_player_health '' 0 "Player health: 87.5"
    expect_exercise 02-variables-and-data-types/2_decimal_places '' 0 "With %.0f: 20"
    expect_contains "$RUN_OUTPUT" "With %.1f: 20.0" "Exercise solution 2_decimal_places printed %.1f wrongly."
    expect_contains "$RUN_OUTPUT" "With %.3f: 19.990" "Exercise solution 2_decimal_places printed %.3f wrongly."
    expect_exercise 02-variables-and-data-types/3_character_codes '' 0 "As a number:    65"
    expect_contains "$RUN_OUTPUT" "The character '7' has the code 55." "Exercise solution 3_character_codes printed the wrong code."

    expect_exercise 03-user-input/1_age_this_year '2026\n1990\n' 0 "This year you turn 36."
    expect_exercise 03-user-input/2_what_scanf_returns '42\n' 0 "scanf returned 1."
    expect_exercise 03-user-input/2_what_scanf_returns 'abc\n' 1 "scanf returned 0."
    expect_exercise 03-user-input/2_what_scanf_returns '' 1 "scanf returned -1."
    expect_exercise 03-user-input/3_leftover_newline '3.8\nx\n' 0 "Its character code is 10."

    expect_exercise 04-basic-operators/1_five_results '17\n5\n' 0 "17 / 5 = 3"
    expect_contains "$RUN_OUTPUT" "17 % 5 = 2" "Exercise solution 1_five_results printed the wrong remainder."
    expect_exercise 04-basic-operators/1_five_results '7\n0\n' 1 "Cannot divide by zero"
    expect_exercise 04-basic-operators/2_three_divisions '' 0 "7 / 2         = 3"
    expect_contains "$RUN_OUTPUT" "7.0 / 2       = 3.500000" "Exercise solution 2_three_divisions divided 7.0 / 2 wrongly."
    expect_contains "$RUN_OUTPUT" "(double)7 / 2 = 3.500000" "Exercise solution 2_three_divisions divided (double)7 / 2 wrongly."
    expect_exercise 04-basic-operators/3_leap_year '' 0 "Is 1900 a leap year? 0"
    expect_contains "$RUN_OUTPUT" "Is 2000 a leap year? 1" "Exercise solution 3_leap_year is wrong about 2000."
    expect_contains "$RUN_OUTPUT" "Is 2024 a leap year? 1" "Exercise solution 3_leap_year is wrong about 2024."

    expect_exercise 05-conditional-statements/1_temperature '-0.5\n' 0 "is freezing."
    expect_exercise 05-conditional-statements/1_temperature '0\n' 0 "is cold."
    expect_exercise 05-conditional-statements/1_temperature '10\n' 0 "is mild."
    expect_exercise 05-conditional-statements/1_temperature '25\n' 0 "is hot."
    expect_exercise 05-conditional-statements/2_grade_range '101\n' 1 "Error: 101 is not a grade between 0 and 100."
    expect_exercise 05-conditional-statements/2_grade_range '-1\n' 1 "Error: -1 is not a grade between 0 and 100."
    expect_exercise 05-conditional-statements/2_grade_range '100\n' 0 "Your letter grade is A."
    expect_exercise 05-conditional-statements/2_grade_range '0\n' 0 "Your letter grade is F."
    expect_exercise 05-conditional-statements/3_largest_of_three '5\n5\n3\n' 0 "The largest is 5."
    expect_exercise 05-conditional-statements/3_largest_of_three '3\n5\n5\n' 0 "The largest is 5."
    expect_exercise 05-conditional-statements/3_largest_of_three '5\n3\n5\n' 0 "The largest is 5."
    expect_exercise 05-conditional-statements/3_largest_of_three '4\n4\n4\n' 0 "The largest is 4."
    expect_exercise 05-conditional-statements/3_largest_of_three '-3\n-1\n-2\n' 0 "The largest is -1."

    expect_exercise 06-loops/1_multiplication_table '7\n' 0 " 1 x 7 = 7"
    expect_contains "$RUN_OUTPUT" "10 x 7 = 70" "Exercise solution 1_multiplication_table stopped before 10 times."
    expect_not_contains "$RUN_OUTPUT" "11 x 7" "Exercise solution 1_multiplication_table went past 10 times."
    expect_exercise 06-loops/2_running_total '5\n10\n-3\n0\n' 0 "The total is 12."
    expect_exercise 06-loops/2_running_total '5\nabc\n10\n0\n' 0 "The total is 15."
    expect_exercise 06-loops/2_running_total '5\n' 0 "The total is 5."
    expect_exercise 06-loops/3_guessing_game '3\nseven\n7\n' 0 "Correct! You needed 2 guesses."
    expect_exercise 06-loops/3_guessing_game '3\n' 1 "No more input."

    expect_exercise 07-functions/1_square '' 0 " 1 squared is   1"
    expect_contains "$RUN_OUTPUT" "10 squared is 100" "Exercise solution 1_square did not reach 10."
    expect_exercise 07-functions/2_max_of '' 0 "The largest of 12, 45 and 30 is 45."
    expect_contains "$RUN_OUTPUT" "The largest of 7, 7 and 2 is 7." "Exercise solution 2_max_of is wrong for equal numbers."
    expect_exercise 07-functions/3_definition_order '' 0 "Total calculated damage is: 36"

    expect_exercise 08-arrays/1_largest_score '' 0 "The largest score is: 980"
    expect_exercise 08-arrays/2_reverse_order '1\n2\n3\n4\n5\n' 0 "In reverse order: 5 4 3 2 1"
    expect_exercise 08-arrays/2_reverse_order '1\n2\nx\n' 1 "That was not a whole number."
    expect_exercise 08-arrays/3_average_temperature '' 0 "Average temperature: 76.5"

    expect_exercise 09-strings/1_count_characters '' 0 "Counted by hand: 15 characters"
    expect_contains "$RUN_OUTPUT" "According to strlen: 15 characters" "Exercise solution 1_count_characters disagrees with strlen."
    expect_contains "$RUN_OUTPUT" "An empty string has 0 characters." "Exercise solution 1_count_characters miscounted an empty string."
    expect_exercise 09-strings/2_count_a_letter '' 0 "The letter 's' appears 3 times."
    expect_exercise 09-strings/3_room_to_append '' 0 "the joined string needs 20."
    expect_contains "$RUN_OUTPUT" "Not enough room, so strcat was not called." "Exercise solution 3_room_to_append called strcat without room."

    printf 'Checked %d exercise solutions.\n' "$exercise_count"
}

# Part 6: the lessons' own output, the bugs they switch on with -DSHOW_BUG and
# -DSHOW_UB (the sanitizers must report them exactly as the comments say), and
# the unit tests in tests/ that lesson 38 describes.
run_toolsmith_checks() {
    part6="$ROOT_DIR/Part 6 - The Toolsmith Path_ Debugging, UB and Testing"

    run_lesson_with_input "$BUILD_DIR/36_debugging" ''
    expect_status 0 "Lesson 36 did not exit cleanly."
    expect_contains "$RUN_OUTPUT" "Total:   141" "Lesson 36 computed the wrong total."
    expect_contains "$RUN_OUTPUT" "Highest: 24" "Lesson 36 found the wrong highest reading."
    expect_contains "$RUN_OUTPUT" "Average: 20.14" "Lesson 36 computed the wrong average."

    run_lesson_with_input "$BUILD_DIR/37_undefined_behavior" ''
    expect_status 0 "Lesson 37 did not exit cleanly."
    expect_contains "$RUN_OUTPUT" "2000000000 + 100 = 2000000100" "Lesson 37's checked_add refused a sum that fits."
    expect_contains "$RUN_OUTPUT" "2000000000 + 2000000000 would overflow an int, so it was refused." "Lesson 37's checked_add accepted an overflow."
    expect_contains "$RUN_OUTPUT" "values[2] = 30" "Lesson 37's element_at refused a valid index."
    expect_contains "$RUN_OUTPUT" "Index 5 is outside an array of 4 elements, so it was refused." "Lesson 37's element_at accepted a bad index."
    expect_contains "$RUN_OUTPUT" "bit_mask(3) = 8" "Lesson 37's bit_mask gave the wrong mask."
    expect_contains "$RUN_OUTPUT" "bit_mask(40) was refused" "Lesson 37's bit_mask accepted a shift that is too wide."
    expect_contains "$RUN_OUTPUT" "The bits of 1.0f are 0x3F800000." "Lesson 37 printed the wrong bits for 1.0f."
    expect_not_contains "$RUN_OUTPUT" "INT_MAX + 1" "Lesson 37 ran its -DSHOW_UB overflow in a normal build."

    run_lesson_with_input "$BUILD_DIR/38_testing" ''
    expect_status 0 "Lesson 38's own tests failed."
    expect_contains "$RUN_OUTPUT" "All 18 checks passed." "Lesson 38 did not run all of its checks."

    for test_name in test_linked_list test_hash_table; do
        printf 'Compiling tests/%s.c\n' "$test_name"
        "$CC" $EFFECTIVE_CFLAGS "$ROOT_DIR/tests/$test_name.c" -o "$BUILD_DIR/$test_name" ${SANITIZER_FLAGS:-}
        run_lesson_with_input "$BUILD_DIR/$test_name" ''
        expect_status 0 "tests/$test_name.c failed."
        expect_contains "$RUN_OUTPUT" "checks passed." "tests/$test_name.c did not report its result."
    done

    if [ -z "${SANITIZER_FLAGS:-}" ]; then
        printf 'Skipping the Part 6 sanitizer demonstrations (no -fsanitize support).\n'
        return 0
    fi
    "$CC" $EFFECTIVE_CFLAGS -g -DSHOW_BUG "$part6/36_debugging.c" -o "$BUILD_DIR/36_debugging_bug" $SANITIZER_FLAGS
    # symbolize=0: GCC's AddressSanitizer on macOS can hang while it turns the
    # backtrace into function names. The error kind and the variable name do
    # not need the symbolizer.
    saved_asan_options=${ASAN_OPTIONS-}
    ASAN_OPTIONS=symbolize=0
    export ASAN_OPTIONS
    run_lesson_with_input "$BUILD_DIR/36_debugging_bug" ''
    ASAN_OPTIONS=$saved_asan_options
    expect_contains "$RUN_OUTPUT" "stack-buffer-overflow" "Lesson 36's -DSHOW_BUG build did not produce the AddressSanitizer report the lesson describes."
    expect_contains "$RUN_OUTPUT" "'readings'" "Lesson 36's AddressSanitizer report does not name the readings array."
    "$CC" $EFFECTIVE_CFLAGS -g -DSHOW_UB "$part6/37_undefined_behavior.c" -o "$BUILD_DIR/37_undefined_behavior_ub" $SANITIZER_FLAGS
    run_lesson_with_input "$BUILD_DIR/37_undefined_behavior_ub" ''
    expect_contains "$RUN_OUTPUT" "signed integer overflow" "Lesson 37's -DSHOW_UB build did not produce the UBSan report the lesson describes."
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
int capstone_lesson_main(int argc, char *argv[]);
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
        standard_supported "$TAUGHT_STANDARD" || {
            echo "$CC does not accept $TAUGHT_STANDARD, the standard the lessons teach." >&2
            exit 1
        }
        STANDARD_FLAG=$TAUGHT_STANDARD
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

# Lessons live only in the Part directories. Searching the whole repository
# would also pick up copies elsewhere, such as the lesson downloads the
# website build writes under site/.
(cd "$ROOT_DIR" && find ./Part* -type f -name '*.c' | sort > "$SOURCE_LIST")

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

newer_count=0
if [ "$STANDARD_FLAG" = "$TAUGHT_STANDARD" ] && standard_supported "$NEWER_STANDARD"; then
    newer_cflags="$BASE_CFLAGS $NEWER_STANDARD"
    printf 'Compiling every lesson again with %s\n' "$NEWER_STANDARD"
    while IFS= read -r lesson_path; do
        lesson_path=${lesson_path#./}
        lesson_name=$(basename "${lesson_path%.c}")
        case "$lesson_path" in
            */31_make_files_for_multi_file_projects/*)
                # Lesson 31 is several files; -c compiles each without linking.
                "$CC" $newer_cflags -c "$ROOT_DIR/$lesson_path" -o "$BUILD_DIR/newer_$lesson_name.o"
                ;;
            *)
                compile_lesson "$lesson_path" "$BUILD_DIR/newer_$lesson_name" "$newer_cflags" >/dev/null
                ;;
        esac
        newer_count=$((newer_count + 1))
    done < "$SOURCE_LIST"
else
    printf 'Skipping the %s pass: %s does not accept it.\n' "$NEWER_STANDARD" "$CC"
fi

run_analyzer_check
run_socket_check
run_student_record_checks
run_tiny_shell_check
run_input_checks
run_number_argument_checks
run_text_editor_check
run_hash_table_check
run_data_parser_check
run_exercise_checks
run_toolsmith_checks
run_sanitizer_regressions

printf 'Smoke check passed. Compiled %d single-file lessons plus lesson 31 and %d exercise solutions with %s' "$compiled_count" "$exercise_count" "$STANDARD_FLAG"
if [ "$newer_count" -gt 0 ]; then
    printf ', and %d source files again with %s' "$newer_count" "$NEWER_STANDARD"
fi
printf '.\n'
