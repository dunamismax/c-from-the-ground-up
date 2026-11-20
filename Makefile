# Repository maintenance tasks for C From The Ground Up.
#
# This Makefile is tooling for maintainers. It is not a lesson; the Makefile
# lesson lives in Part 5, lesson 31.
#
#   make verify             full local gate (same checks CI runs)
#   make sync               copy lesson sources into the book chapters
#   make serve              live-preview the book at http://localhost:3000
#   make smoke CC=gcc-15    smoke check with a specific compiler

CC ?= cc
PYTHON ?= python3

.PHONY: help verify sync check-sync book serve smoke smoke-all clean

help:
	@sed -n '3,10p' Makefile

verify: check-sync book smoke-all

sync:
	$(PYTHON) scripts/sync_chapters.py

check-sync:
	$(PYTHON) scripts/sync_chapters.py --check

book: check-sync
	mdbook build

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

clean:
	rm -rf book
