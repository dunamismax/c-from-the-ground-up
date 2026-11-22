# Changelog

## v2.0.0 (2026-11-22): final release

The course is complete. This is the last planned release; the repository
stays open for corrections.

### The course

- **Part 6, The Toolsmith Path** (lessons 36-38): debugging with gdb and
  lldb, undefined behavior and the sanitizers, and testing in C. The course
  now has 38 lessons in six parts.
- **Every lesson reviewed for correctness.** Fixed lesson bugs and wrong
  comments, checked every `scanf` and `fgets`, explained every unsafe API a
  lesson uses, and made lesson 32 portable (`M_PI`) and lesson 29 stop
  looping on commands from a file.
- **Teaches C17** (`-std=c17`) everywhere, and every lesson also builds
  under C23. Windows is supported through WSL.
- **Learning aids:** a "What you'll learn / Before you start" box and three
  "Try It Yourself" exercises on every chapter, worked solutions for Part 1
  in `exercises/`, a glossary, and a setup chapter.

### The website

- **https://cfromthegroundup.com**, the main way to take the course: a
  page per lesson, progress tracking with no account needed, search, "Try it
  online" in Compiler Explorer, copy-ready compile commands, downloads,
  light and dark themes, keyboard navigation, and WCAG 2.2 AA checks.
- Optional GitHub sign-in with synced progress, and self-hosted Umami
  analytics; both are built and switched off in production.
- The mdBook edition stays as a mirror, and every page links to the site.

### Quality gates

- `make verify` runs the same checks as CI: listings in sync with the
  sources, clang-format, clang-tidy, the book build and link check, every
  lesson compiled with strict warnings as errors under Clang and GCC with
  behavioural checks, ASan, UBSan, LeakSanitizer, MemorySanitizer and
  ThreadSanitizer runs, libFuzzer targets for the two file parsers, and the
  site's type checks, lint, unit and end-to-end tests.

### Licence

- MIT, as in v1.0.0. (The project was briefly GPL-3.0 in April and May
  2026.)

## v1.0.0 (2026-03-26)

- The course as an mdBook published on GitHub Pages: 35 lessons in five
  parts, from Hello World to a capstone ncurses text adventure.
