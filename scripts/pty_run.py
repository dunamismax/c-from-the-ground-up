#!/usr/bin/env python3
"""Run a terminal (ncurses) program under a pseudo-terminal with scripted keys.

The ncurses lessons need a real terminal: without one, initscr() exits. This
gives the program an 80x24 pseudo-terminal, types the given input into it a
piece at a time, and reports how the program ended. scripts/sanitizer_check.sh
uses it to run lessons 33 and 35 under the sanitizers.

Usage:
    python3 scripts/pty_run.py [--keys TEXT] [--timeout SECONDS] [--log FILE] -- PROGRAM [ARGS...]

--keys is split on newlines; each line (with its newline) is typed after a
short pause, so the program has time to draw and wait for input. Escapes such
as \\n in TEXT are interpreted. Everything the program draws goes to --log.

Exit status: the program's own status, 128 + N if signal N killed it, or 124
if it did not finish within the timeout.

Standard library only; Linux and macOS (POSIX pty, termios).
"""

from __future__ import annotations

import argparse
import fcntl
import os
import pty
import select
import signal
import struct
import sys
import termios
import time

ROWS, COLS = 24, 80


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--keys", default="")
    parser.add_argument("--timeout", type=float, default=30.0)
    parser.add_argument("--log", default=os.devnull)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        parser.error("no program given")

    keys = args.keys.encode().decode("unicode_escape")
    pieces = [line + "\n" for line in keys.split("\n")[:-1]] if keys else []
    if keys and not keys.endswith("\n"):
        pieces.append(keys.split("\n")[-1])

    pid, fd = pty.fork()
    if pid == 0:
        os.environ["TERM"] = "xterm"
        os.environ.pop("COLUMNS", None)
        os.environ.pop("LINES", None)
        try:
            os.execvp(command[0], command)
        finally:
            os._exit(127)

    fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack("HHHH", ROWS, COLS, 0, 0))
    deadline = time.monotonic() + args.timeout
    next_key = time.monotonic() + 0.5
    status = None

    with open(args.log, "wb") as log:
        while True:
            now = time.monotonic()
            if now > deadline:
                os.kill(pid, signal.SIGKILL)
                os.waitpid(pid, 0)
                print(f"{command[0]}: did not finish within {args.timeout:g} seconds", file=sys.stderr)
                return 124
            if pieces and now >= next_key:
                os.write(fd, pieces.pop(0).encode())
                next_key = now + 0.3
            ready, _, _ = select.select([fd], [], [], 0.05)
            if ready:
                try:
                    data = os.read(fd, 65536)
                except OSError:
                    data = b""
                if data:
                    log.write(data)
                    continue
            done, raw = os.waitpid(pid, os.WNOHANG)
            if done:
                status = raw
                # Drain what is left after the child exits.
                try:
                    while select.select([fd], [], [], 0.05)[0]:
                        data = os.read(fd, 65536)
                        if not data:
                            break
                        log.write(data)
                except OSError:
                    pass
                break

    if os.WIFSIGNALED(status):
        return 128 + os.WTERMSIG(status)
    return os.WEXITSTATUS(status)


if __name__ == "__main__":
    sys.exit(main())
