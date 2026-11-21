# Repository maintenance tasks for C From The Ground Up.
#
# This Makefile is tooling for maintainers. It is not a lesson; the Makefile
# lesson lives in Part 5, lesson 31.
#
#   make verify             full local gate (same checks CI runs)
#   make sync               copy lesson sources into the book chapters
#   make serve              live-preview the book at http://localhost:3000
#   make smoke CC=gcc-15    smoke check with a specific compiler
#   make smoke-linux        smoke check on Linux (Docker), with GCC and Clang
#   make sanitize           lessons under ASan/UBSan/LSan, MSan and TSan
#   make sanitize-linux     the same on Linux (Docker), where MSan works
#   make format             apply .clang-format to the lesson sources
#   make tidy               clang-tidy with .clang-tidy (static analysis)
#   make analyze GCC=gcc-15 GCC's static analyzer (-fanalyzer)
#   make site-verify        website gate: npm ci, check, lint, test, build
#   make site-e2e           website Playwright tests (needs the site database)

CC ?= cc
PYTHON ?= python3

.PHONY: help verify sync check-sync book serve smoke smoke-all smoke-linux sanitize sanitize-linux format check-format tidy analyze site-verify site-e2e clean

help:
	@sed -n '3,18p' Makefile

verify: check-sync check-format tidy book smoke-all sanitize site-verify

sync:
	$(PYTHON) scripts/sync_chapters.py

check-sync:
	$(PYTHON) scripts/sync_chapters.py --check

book: check-sync
	mdbook build
	$(PYTHON) scripts/check_book_links.py

serve:
	mdbook serve --open

smoke:
	CC=$(CC) sh scripts/smoke_check.sh

# Clang and GCC both, when both exist. On macOS `gcc` is usually Clang in
# disguise; set GCC=gcc-15 (or similar) to exercise real GCC.
GCC ?= gcc
smoke-all:
	CC=clang sh scripts/smoke_check.sh
	CC=$(GCC) sh scripts/smoke_check.sh

# The smoke check under glibc, as CI runs it. macOS headers expose names that
# glibc hides in strict -std= modes (lesson 32's M_PI once passed locally and
# failed CI), so run this before pushing lesson changes from a Mac. Needs
# Docker. The sources are piped in with tar because bind mounts of some
# directories do not work with Docker Desktop.
LINUX_IMAGE ?= gcc:15
smoke-linux:
	COPYFILE_DISABLE=1 tar --no-xattrs -cf - Part* scripts tests | \
		docker run --rm -i $(LINUX_IMAGE) sh -c '\
			set -e; mkdir /r; cd /r; tar xf -; \
			apt-get update -qq >/dev/null; \
			apt-get install -y -qq libncurses-dev clang >/dev/null 2>&1; \
			CC=gcc sh scripts/smoke_check.sh; \
			CC=clang sh scripts/smoke_check.sh'

# Lesson sources in the house style (.clang-format). After `make format`, run
# `make sync` so the chapters embed the reformatted files.
CLANG_FORMAT ?= clang-format
LESSON_SOURCES = find ./Part* -type f \( -name '*.c' -o -name '*.h' \) -print0
format:
	$(LESSON_SOURCES) | xargs -0 $(CLANG_FORMAT) -i
check-format:
	$(LESSON_SOURCES) | xargs -0 $(CLANG_FORMAT) --dry-run --Werror

# Static analysis. clang-tidy's checks and the reasons some are off are in
# .clang-tidy; CI pins clang-tidy 23. GCC's analyzer needs real GCC (on macOS
# `gcc` is Clang), so it is not part of verify; CI runs it without failing
# the build while it proves itself (BUILD.md).
CLANG_TIDY ?= clang-tidy
LESSON_C_SOURCES = find ./Part* -type f -name '*.c' -print0
tidy:
	$(LESSON_C_SOURCES) | xargs -0 sh -c '$(CLANG_TIDY) --quiet "$$@" -- -std=c17' sh
analyze:
	$(LESSON_C_SOURCES) | xargs -0 -n 1 sh -c '$(GCC) -std=c17 -fanalyzer -Werror -c -o /dev/null "$$0"'

# Sanitizer runs of every lesson (scripts/sanitizer_check.sh). Needs Clang;
# MemorySanitizer is skipped where it cannot run (macOS). CI runs it on Linux
# with SANITIZERS_REQUIRED=1, so nothing is skipped there.
SANITIZE_CC ?= clang
sanitize:
	CC=$(SANITIZE_CC) sh scripts/sanitizer_check.sh

# MemorySanitizer needs the personality() syscall, which Docker's default
# seccomp profile blocks.
sanitize-linux:
	COPYFILE_DISABLE=1 tar --no-xattrs -cf - Part* scripts tests | \
		docker run --rm -i --security-opt seccomp=unconfined $(LINUX_IMAGE) sh -c '\
			set -e; mkdir /r; cd /r; tar xf -; \
			apt-get update -qq >/dev/null; \
			apt-get install -y -qq libncurses-dev clang >/dev/null 2>&1; \
			CC=clang SANITIZERS_REQUIRED=1 sh scripts/sanitizer_check.sh'

# The course website in site/ (SvelteKit). Needs Node 24+ and npm.
site-verify:
	cd site && npm ci && npm run check && npm run lint && npm test && npm run build

# Start the database first: cd site && npm run db:start
site-e2e:
	cd site && npm run test:e2e

clean:
	rm -rf book site/build site/.svelte-kit
