import { describe, expect, it } from 'vitest';
import { readFile } from 'node:fs/promises';
import path from 'node:path';
import { courseRoot } from './paths.js';
import { parseSummary } from './summary.js';
import { loadMarkdownPage, renderMarkdownPage, siteHref } from './page.js';

describe('siteHref', () => {
	it('turns chapter links into lesson links', () => {
		expect(siteHref('chapters/01-hello-world.md')).toBe('/lessons/01-hello-world');
		expect(siteHref('chapters/26-simple-sockets.md#what-is-a-socket')).toBe(
			'/lessons/26-simple-sockets#what-is-a-socket'
		);
	});

	it('leaves other links alone', () => {
		expect(siteHref('https://example.com/a.md')).toBe('https://example.com/a.md');
		expect(siteHref('setup.md')).toBe('setup.md');
	});
});

describe('renderMarkdownPage', () => {
	it('splits off the title and lists the h2 headings', async () => {
		const page = await renderMarkdownPage(
			'# Glossary\n\nIntro text.\n\n## A\n\n- **Array**: see [Lesson 8](chapters/08-arrays.md)\n',
			'test.md'
		);
		expect(page.title).toBe('Glossary');
		expect(page.description).toBe('Intro text.');
		expect(page.toc).toEqual([{ depth: 2, id: 'a', text: 'A' }]);
		expect(page.html).not.toContain('<h1');
		expect(page.html).toContain('href="/lessons/08-arrays"');
	});

	it('rejects a page without a title', async () => {
		await expect(renderMarkdownPage('Just text.\n', 'bad.md')).rejects.toThrow(/# Title/);
	});
});

describe('the glossary', () => {
	it('links only to lessons that exist', async () => {
		// The slugs from SUMMARY.md, without loading (and highlighting) the course.
		const summary = parseSummary(await readFile(path.join(courseRoot(), 'src/SUMMARY.md'), 'utf8'));
		const slugs = new Set(summary.flatMap((part) => part.chapters.map((chapter) => chapter.slug)));
		const page = await loadMarkdownPage('glossary.md');
		const links = [...page.html.matchAll(/href="\/lessons\/([^"#]+)/g)].map((match) => match[1]);
		expect(links.length).toBeGreaterThan(50);
		for (const slug of links) expect(slugs, slug).toContain(slug);
	});
});
