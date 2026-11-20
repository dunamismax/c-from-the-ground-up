#!/usr/bin/env python3
"""Keep the book's embedded source listings identical to the lesson files.

The lesson files under "Part N - ..." are the single source of truth for code.
Each chapter in src/chapters/ embeds one or more of them in a fenced code block
preceded by a marker line:

    <!-- source: Part 1 - The Beginner Path_ Core Concepts/1_hello_world.c -->
    ```c
    ...file contents, copied verbatim by this script...
    ```

mdBook's {{#include}} cannot be used because the lesson directory names contain
spaces, so this script does the copying instead.

Usage:
    python3 scripts/sync_chapters.py          # rewrite stale listings in place
    python3 scripts/sync_chapters.py --check  # report drift and exit 1 (CI)

Besides refreshing listings, the script fails if:
  - a marker points at a file that does not exist
  - a lesson source file (*.c, *.h, Makefile under Part */) has no marker,
    or has more than one
  - a chapter file in src/chapters/ is not listed in src/SUMMARY.md

Standard library only; Python 3.8+.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CHAPTERS_DIR = ROOT / "src" / "chapters"
SUMMARY = ROOT / "src" / "SUMMARY.md"

MARKER_RE = re.compile(r"^<!-- source: (?P<path>.+?) -->$")
FENCE_RE = re.compile(r"^(?P<fence>`{3,})(?P<lang>[\w+-]*)$")


def lesson_sources() -> list[Path]:
    """Every file that belongs to a lesson and must appear in the book."""
    found = []
    for part in sorted(ROOT.glob("Part */")):
        for path in sorted(part.rglob("*")):
            if path.is_file() and (path.suffix in {".c", ".h"} or path.name == "Makefile"):
                found.append(path)
    return found


def language_for(path: Path) -> str:
    return "makefile" if path.name == "Makefile" else "c"


def fence_for(text: str) -> str:
    """A backtick fence longer than any backtick run inside the source."""
    longest = max((len(m) for m in re.findall(r"`+", text)), default=0)
    return "`" * max(3, longest + 1)


def sync_chapter(chapter: Path, errors: list[str], seen: dict[Path, list[str]]) -> str:
    """Return the chapter text with every marked listing refreshed."""
    lines = chapter.read_text(encoding="utf-8").split("\n")
    out: list[str] = []
    i = 0
    rel_chapter = chapter.relative_to(ROOT).as_posix()

    while i < len(lines):
        line = lines[i]
        out.append(line)
        marker = MARKER_RE.match(line)
        i += 1
        if not marker:
            continue

        source = ROOT / marker.group("path")
        seen.setdefault(source.resolve(), []).append(rel_chapter)
        if not source.is_file():
            errors.append(f"{rel_chapter}: marker points at missing file {marker.group('path')!r}")
            continue

        opening = FENCE_RE.match(lines[i]) if i < len(lines) else None
        if not opening:
            errors.append(f"{rel_chapter}: marker for {marker.group('path')!r} is not followed by a code fence")
            continue

        # Skip the existing listing up to its matching closing fence.
        close = opening.group("fence")
        j = i + 1
        while j < len(lines) and lines[j] != close:
            j += 1
        if j == len(lines):
            errors.append(f"{rel_chapter}: unterminated code fence after marker for {marker.group('path')!r}")
            out.extend(lines[i:])
            break

        body = source.read_text(encoding="utf-8")
        if body.endswith("\n"):
            body = body[:-1]
        fence = fence_for(body)
        out.append(f"{fence}{language_for(source)}")
        out.extend(body.split("\n"))
        out.append(fence)
        i = j + 1

    return "\n".join(out)


def main(argv: list[str]) -> int:
    check = "--check" in argv[1:]
    errors: list[str] = []
    seen: dict[Path, list[str]] = {}
    stale: list[str] = []

    for chapter in sorted(CHAPTERS_DIR.glob("*.md")):
        original = chapter.read_text(encoding="utf-8")
        updated = sync_chapter(chapter, errors, seen)
        if updated != original:
            stale.append(chapter.relative_to(ROOT).as_posix())
            if not check:
                chapter.write_text(updated, encoding="utf-8")

    for source in lesson_sources():
        rel = source.relative_to(ROOT).as_posix()
        refs = seen.get(source.resolve(), [])
        if not refs:
            errors.append(f"{rel}: lesson source has no '<!-- source: ... -->' marker in any chapter")
        elif len(refs) > 1:
            errors.append(f"{rel}: lesson source is embedded more than once ({', '.join(refs)})")

    summary = SUMMARY.read_text(encoding="utf-8")
    for chapter in sorted(CHAPTERS_DIR.glob("*.md")):
        if f"(chapters/{chapter.name})" not in summary:
            errors.append(f"src/chapters/{chapter.name}: not listed in src/SUMMARY.md")

    for error in errors:
        print(f"error: {error}", file=sys.stderr)

    if check and stale:
        for name in stale:
            print(f"stale: {name}", file=sys.stderr)
        print("Chapter listings are out of date. Run: python3 scripts/sync_chapters.py", file=sys.stderr)
        return 1
    if not check:
        for name in stale:
            print(f"updated: {name}")

    if errors:
        return 1
    print(f"Chapter listings in sync ({sum(len(v) for v in seen.values())} embedded sources).")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
