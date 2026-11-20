#!/usr/bin/env python3
"""Check the internal links and anchors of the built book.

Run after `mdbook build`. Every page under book/ is parsed, and every link or
resource that stays inside the book (href and src values that are not
absolute URLs) must point at a file that exists. A link with a #fragment must
also name an element id (or a legacy <a name>) on the target page.

External links (http:, https:, mailto: and so on) are not fetched: they are
listed with --external so a maintainer can check them by hand.

mdBook 0.5 has no maintained link-check preprocessor, which is why this
script exists. It reads only the generated HTML, so it also catches links
that mdBook rewrites (for example .md links to .html pages).

Usage:
    python3 scripts/check_book_links.py             # check book/ (CI)
    python3 scripts/check_book_links.py --external  # also list external URLs

Standard library only; Python 3.8+.
"""

from __future__ import annotations

import sys
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit

ROOT = Path(__file__).resolve().parent.parent
BOOK = ROOT / "book"

# Pages that mdBook generates for its own machinery. Their links are
# mdBook's, not ours, and print.html repeats every chapter.
SKIP_PAGES = {"print.html", "toc.html"}

LINK_ATTRIBUTES = {
    "a": "href",
    "link": "href",
    "img": "src",
    "script": "src",
    "iframe": "src",
    "source": "src",
}


class PageParser(HTMLParser):
    def __init__(self) -> None:
        super().__init__(convert_charrefs=True)
        self.ids: set[str] = set()
        self.links: list[tuple[int, str]] = []

    def handle_starttag(self, tag: str, attrs: list[tuple[str, str | None]]) -> None:
        values = dict(attrs)
        element_id = values.get("id")
        if element_id:
            self.ids.add(element_id)
        if tag == "a" and values.get("name"):
            self.ids.add(values["name"] or "")
        attribute = LINK_ATTRIBUTES.get(tag)
        if attribute:
            value = values.get(attribute)
            if value is not None:
                self.links.append((self.getpos()[0], value))


def parse(path: Path, cache: dict[Path, PageParser]) -> PageParser:
    if path not in cache:
        parser = PageParser()
        parser.feed(path.read_text(encoding="utf-8"))
        cache[path] = parser
    return cache[path]


def main() -> int:
    show_external = "--external" in sys.argv[1:]

    if not (BOOK / "index.html").is_file():
        print("book/index.html is missing: run `mdbook build` first.", file=sys.stderr)
        return 1

    cache: dict[Path, PageParser] = {}
    errors: list[str] = []
    external: set[str] = set()
    checked = 0

    pages = sorted(p for p in BOOK.rglob("*.html") if p.name not in SKIP_PAGES)
    for page in pages:
        relative_page = page.relative_to(BOOK)
        for line, raw in parse(page, cache).links:
            url = urlsplit(raw)
            if url.scheme or raw.startswith("//"):
                if url.scheme in ("http", "https"):
                    external.add(raw)
                continue
            if raw.startswith("javascript:"):
                continue

            checked += 1
            where = f"{relative_page}:{line}: {raw}"

            if url.path:
                if url.path.startswith("/"):
                    # site-url in book.toml is /c-from-the-ground-up/; mdBook
                    # only uses root-relative links on the 404 page.
                    prefix = "/c-from-the-ground-up/"
                    if not url.path.startswith(prefix):
                        errors.append(f"{where}: root-relative link outside the book")
                        continue
                    target = BOOK / unquote(url.path[len(prefix):])
                else:
                    target = (page.parent / unquote(url.path)).resolve()
                if target.is_dir():
                    target = target / "index.html"
                try:
                    target.resolve().relative_to(BOOK)
                except ValueError:
                    errors.append(f"{where}: points outside book/")
                    continue
                if not target.is_file():
                    errors.append(f"{where}: no such file")
                    continue
            else:
                target = page

            fragment = unquote(url.fragment)
            if fragment and target.suffix == ".html":
                if fragment not in parse(target, cache).ids:
                    errors.append(f"{where}: no element with id {fragment!r}")

    if show_external:
        for link in sorted(external):
            print(f"external: {link}")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        print(f"{len(errors)} broken internal link(s).", file=sys.stderr)
        return 1

    print(
        f"Book links OK ({checked} internal links in {len(pages)} pages; "
        f"{len(external)} external URLs not fetched)."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
